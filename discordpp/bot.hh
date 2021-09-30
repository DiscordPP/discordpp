//
// Created by aidan on 11/7/15.
//

#pragma once

#include <random>
#include <string>
#include <vector>

#include "botStruct.hh"
#include "intents.hh"
#include "log.hh"

#undef CALL_FIELD

namespace discordpp {

class Bot : public virtual BotStruct {
    std::unique_ptr<asio::steady_timer> reconnect_;
    std::function<int()> reconnect_millis =
        [distribution =
             std::make_shared<std::uniform_int_distribution<int>>(0, 5000),
         generator = std::make_shared<std::default_random_engine>(
             std::chrono::system_clock::now().time_since_epoch().count())]() {
            return (*distribution)(*generator);
        };
    std::unique_ptr<asio::steady_timer> pacemaker_;
    std::unique_ptr<std::chrono::milliseconds> heartrate_;
    std::string session_id_ = "";
    int sequence_ = -1;
    int needACK_ = -1;

  public:
    // This temporarily blocks sending the NONE intent on API 6 until it is
    // inactive
    bool sendNoneIntent = false;
    uint16_t intents = intents::NONE;

    unsigned int api = 9;

    // Bot sharding
    unsigned int shardID = 0;
    unsigned int numShards = 1;

    Bot() {
        needInit["Bot"] = true;

        handlers.insert({"READY", [this](json data) {
                             session_id_ =
                                 data["session_id"].get<std::string>();
                             ready_ = true;
                         }});
    }

    virtual void initBot(unsigned int apiVersionIn, const std::string &tokenIn,
                         std::shared_ptr<asio::io_context> aiocIn) {
        apiVersion = apiVersionIn;
        token = tokenIn;
        aioc = aiocIn;
        needInit["Bot"] = false;
    }

    void reconnect(const std::string &reason,
                   const bool resume = true) override {
        log::log(log::error, [reason](std::ostream *log) {
            *log << "Reconnecting because \"" << reason << "\" ...\n";
        });
        if (!resume) {
            sequence_ = -1;
            session_id_ = "";
        }
        pacemaker_->cancel();
        needACK_ = -1;
        disconnect();
        connect();
    }

  protected:
    void sendHeartbeat(const error_code e) {
        if (!connected_ || e) {
            return;
        }
        if (needACK_ > 1) {
            log::log(log::error, [this](std::ostream *log) {
                *log << "Discord Servers did not respond to " << needACK_
                     << " heartbeat" << (needACK_ == 1 ? "" : "s")
                     << "  within "
                     << std::chrono::milliseconds(*heartrate_).count()
                     << "ms; aborting connection.\n";
            });
            reconnect("Discord did not acknowledge too many heartbeats");
        } else if (needACK_ > 0) {
            log::log(log::warning, [this](std::ostream *log) {
                *log << "Discord Servers did not respond to " << needACK_
                     << " heartbeat" << (needACK_ == 1 ? "" : "s") << " within "
                     << std::chrono::milliseconds(*heartrate_).count()
                     << "ms but we're letting this one slip.\n";
            });
        }
        needACK_++;
        if (showHeartbeats) {
            std::cout << "Sending heartbeat..." << std::endl;
        }
        pacemaker_ = std::make_unique<asio::steady_timer>(
            *aioc, std::chrono::steady_clock::now() + *heartrate_);
        pacemaker_->async_wait(
            [this](const error_code ec) { sendHeartbeat(ec); });
        if (sequence_ >= 0) {
            send(1, std::make_shared<json>(sequence_), nullptr);
        } else {
            send(1, nullptr, nullptr);
        }
    }

    virtual void receivePayload(json payload) override {
        log::log(log::trace, [payload](std::ostream *log) {
            *log << "Recieved Payload: " << payload.dump(4) << '\n';
        });

        switch (payload["op"].get<int>()) {
        case 0: // Dispatch:           dispatches an event
            sequence_ = payload["s"].get<int>();
            if (handlers.find(payload["t"].get<std::string>()) ==
                handlers.end()) {
                if (debugUnhandled) {
                    std::cerr << "No handlers defined for " << payload["t"]
                              << "\n";
                }
            } else {
                for (auto handler =
                         handlers.lower_bound(payload["t"].get<std::string>());
                     handler !=
                     handlers.upper_bound(payload["t"].get<std::string>());
                     handler++) {
                    handler->second(payload["d"]);
                }
            }
            break;
        case 1: // Heartbeat:          used for ping checking
            log::log(log::info, [](std::ostream *log) {
                *log << "Discord Servers requested a heartbeat, which is not "
                        "implemented.\n";
            });
            break;
        case 7: // Reconnect:          used to tell clients to reconnect to the
                // gateway
            log::log(log::info, [](std::ostream *log) {
                *log << "Discord Servers requested a reconnect.\n";
            });
            reconnect("The gateway is restarting");
            break;
        case 9: // Invalid Session:	used to notify client they have an
                // invalid session id
            log::log(log::warning, [](std::ostream *log) {
                *log << "Discord Servers notified of an invalid session ID.\n";
            });
            reconnect_ = std::make_unique<asio::steady_timer>(
                *aioc, std::chrono::steady_clock::now() +
                           std::chrono::milliseconds(reconnect_millis()));
            reconnect_->async_wait([this](const error_code) {
                reconnect("The session is invalid", false);
            });
            break;
        case 10: // Hello:              sent immediately after connecting,
                 // contains heartbeat and server debug information
            heartrate_ = std::make_unique<std::chrono::milliseconds>(
                payload["d"]["heartbeat_interval"].get<int>());
            needACK_ = 0;
            sendHeartbeat(
#ifdef ASIO_STANDALONE
                {}
#else
                boost::system::errc::make_error_code(
                    boost::system::errc::success)
#endif
            );
            if (sequence_ >= 0) {
                send(6,
                     std::make_shared<json>(json({
                         {"token", token},
                         {"session_id", session_id_},
                         {"seq", sequence_},
                     })),
                     nullptr);
            } else {
                auto identify =
                    std::make_shared<json>(json({{"token", token},
                                                 {"properties",
                                                  {
                                                      {"$browser", "discordpp"},
                                                      {"$device", "discordpp"},
                                                  }}}));

#if defined(_WIN32) || defined(_WIN64)
                (*identify)["properties"]["$os"] = "windows";
#elif defined(__APPLE__) || defined(__MACH__)
                (*identify)["properties"]["$os"] = "osx";
#elif defined(__linux__)
                (*identify)["properties"]["$os"] = "linux";
#elif defined(__FreeBSD__)
                (*identify)["properties"]["$os"] = "freebsd";
#elif defined(__unix) || defined(__unix__)
                (*identify)["properties"]["$os"] = "unix";
#else
                (*identify)["properties"]["$os"] = "other";
#endif

                if (intents != intents::NONE || api >= 8 || sendNoneIntent) {
                    (*identify)["intents"] = intents;
                }

                if (numShards > 1) {
                    assert(shardID < numShards);
                    (*identify)["shard"] = json::array({shardID, numShards});
                }

                send(2, identify, nullptr);
            }
            break;
        case 11: // Heartbeat ACK:      sent immediately following a client
                 // heartbeat that was received
            needACK_ = false;
            if (showHeartbeats) {
                std::cout << "Heartbeat Sucessful." << std::endl;
            }
            break;
        default:
            log::log(log::warning, [payload](std::ostream *log) {
                *log << "Unexpected opcode " << payload["op"] << "! Message:\n"
                     << payload.dump(4) << '\n';
            });
        }
    }

    // REST //////////////////////
};
} // namespace discordpp
