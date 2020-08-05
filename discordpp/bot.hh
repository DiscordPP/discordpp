//
// Created by aidan on 11/7/15.
//

#pragma once

#include <random>
#include <string>
#include <vector>

#include "botStruct.hh"

namespace discordpp {
using json = nlohmann::json;
using snowflake = uint64_t;

class Bot : public virtual BotStruct {
    std::unique_ptr<boost::asio::steady_timer> reconnect_;
    std::function<int()> reconnect_millis =
        [distribution =
             std::make_shared<std::uniform_int_distribution<int>>(0, 5000),
         generator = std::make_shared<std::default_random_engine>(
             std::chrono::system_clock::now().time_since_epoch().count())]() {
            return (*distribution)(*generator);
        };
    std::unique_ptr<boost::asio::steady_timer> pacemaker_;
    std::unique_ptr<std::chrono::milliseconds> heartrate_;
    std::string session_id_ = "";
    int sequence_ = -1;
    bool gotACK = true;

  public:
    bool debugUnhandled = true;

    Bot() {
        needInit["Bot"] = true;

        handlers.insert({"READY", [this](json data) {
                             session_id_ =
                                 data["session_id"].get<std::string>();
                         }});
    }

    virtual void initBot(unsigned int apiVersionIn, const std::string &tokenIn,
                         std::shared_ptr<boost::asio::io_context> aiocIn) {
        apiVersion = apiVersionIn;
        token = tokenIn;
        aioc = aiocIn;
        needInit["Bot"] = false;
    }

  protected:
    void sendHeartbeat(const boost::system::error_code e) {
        if (e.failed()) {
            return;
        }
        if (!gotACK) {
            std::cerr << "Discord Servers did not respond to heartbeat.\n";
            reconnect();
        }
        gotACK = false;
        std::cout << "Sending heartbeat..." << std::endl;
        pacemaker_ = std::make_unique<boost::asio::steady_timer>(
            *aioc, std::chrono::steady_clock::now() + *heartrate_);
        pacemaker_->async_wait(
            [this](const boost::system::error_code ec) { sendHeartbeat(ec); });
        if (sequence_ >= 0) {
            send(1, std::make_shared<json>(sequence_), nullptr);
        } else {
            send(1, nullptr, nullptr);
        }
    }

    virtual void receivePayload(json payload) override {
        // std::cerr << "Recieved Payload: " << payload.dump(4) << '\n';

        switch (payload["op"].get<int>()) {
        case 0: // Dispatch:           dispatches an event
            sequence_ = payload["s"].get<int>();
            if (handlers.find(payload["t"]) == handlers.end()) {
                if (debugUnhandled) {
                    std::cerr << "No handlers defined for " << payload["t"]
                              << "\n";
                }
            } else {
                for (auto handler = handlers.lower_bound(payload["t"]);
                     handler != handlers.upper_bound(payload["t"]); handler++) {
                    handler->second(payload["d"]);
                }
            }
            break;
        case 1: // Heartbeat:          used for ping checking
            std::cerr << "Discord Servers requested a heartbeat, which is not "
                         "implemented.\n";
            break;
        case 7: // Reconnect:          used to tell clients to reconnect to the
                // gateway
            std::cerr << "Discord Servers requested a reconnect.\n";
            reconnect();
            break;
        case 9: // Invalid Session:	used to notify client they have an
                // invalid session id
            std::cerr << "Discord Servers notified of an invalid session ID.\n";
            reconnect_ = std::make_unique<boost::asio::steady_timer>(
                *aioc, std::chrono::steady_clock::now() +
                           std::chrono::milliseconds(reconnect_millis()));
            reconnect_->async_wait(
                [this](const boost::system::error_code) { reconnect(false); });
            break;
        case 10: // Hello:              sent immediately after connecting,
                 // contains heartbeat and server debug information
            heartrate_ = std::make_unique<std::chrono::milliseconds>(
                payload["d"]["heartbeat_interval"]);
            sendHeartbeat(boost::system::errc::make_error_code(
                boost::system::errc::success));
            if (sequence_ >= 0) {
                send(6,
                     std::make_shared<json>(json({
                         {"token", token},
                         {"session_id", session_id_},
                         {"seq", sequence_},
                     })),
                     nullptr);
            } else {
                send(
                    2,
                    std::make_shared<json>(json({{"token", token},
                                                 {"properties",
                                                  {
                                                      {"$os", "linux"},
                                                      {"$browser", "discordpp"},
                                                      {"$device", "discordpp"},
                                                  }}})),
                    nullptr);
            }
            break;
        case 11: // Heartbeat ACK:      sent immediately following a client
                 // heartbeat that was received
            gotACK = true;
            std::cout << "Heartbeat Successful." << std::endl;
            break;
        default:
            std::cerr << "Unexpected opcode " << payload["op"] << "! Message:\n"
                      << payload.dump(4) << '\n';
        }
    }

    void reconnect(const bool resume = true) override {
        std::cerr << "Reconnecting...\n";
        if (!resume) {
            sequence_ = -1;
            session_id_ = "";
        }
        pacemaker_->cancel();
        disconnect();
        connect();
    }
};
} // namespace discordpp
