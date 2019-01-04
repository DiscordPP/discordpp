//
// Created by aidan on 7/15/17.
//

#ifndef EXAMPLE_BOT_WEBSOCKETMODULE_HH
#define EXAMPLE_BOT_WEBSOCKETMODULE_HH

#include <chrono>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

//#include <lib/nlohmannjson/src/json.hpp>
#include <nlohmann/json.hpp>

#include "colors.hh"

namespace discordpp{
    namespace asio = boost::asio;
    using boost::system::error_code;
    using json = nlohmann::json;
    using DispatchHandler = std::function<void(std::string, json)>;

    class WebsocketModule{
    public:
        WebsocketModule(std::shared_ptr<asio::io_service> asio_ios, const std::string &token):
                aios_(asio_ios),
                token_(token){}

        virtual void init(const unsigned int apiVersion, const std::string &gateway, DispatchHandler disHandler) = 0;
        virtual void send(int opcode = 0, json payload = {}) = 0;

        std::string sessionID_ = "";
    protected:
        void handleMessage(const unsigned int apiVersion, DispatchHandler disHandler, json msg){
            int opcode = msg["op"];
            //std::cout << "Opcode " << opcode << " recieved.\n";
            //std::cout << "recieved opcode " + std::to_string(msg["op"].get<int>()) + "\n";
            switch(opcode){
                case 0: // Dispatch
                    sequence_number_ = msg["s"];
                    //std::cout << "Event " << msg["t"] << " recieved.\n";
                    disHandler(msg["t"], msg["d"]);
                    break;
                case 9:
                    std::cerr << "Invalid session, dumping session ID.\n";
                    sessionID_ = "";
                    close();
                    break;
                case 10: // Hello
                    keepalive(msg["d"]["heartbeat_interval"]);
                    if(sessionID_.empty()) {
                        send(2, genHandshake(apiVersion));
                    }else{
                        hold_asio_.reset();
                        send(6, {
                                {"token", token_},
                                {"session_id", sessionID_},
                                {"seq", sequence_number_}
                        });
                    }
                    break;
                case 11: // Heartbeat ACK
                    acknowledged = true;
                    break;
                default:
                    std::cout << color("Unknown opcode ", Color.Red) 
                              << color(opcode, Color.Bold) 
                              << color(" recieved.\n", Color.Red);
            }
        }

        virtual void sendkeepalive(json message) = 0;
        virtual void connect() = 0;

        virtual void close() = 0;

        /*void resume(aios_ptr asio_ios, std::string token) {
            std::cout << "Attempting to resume.\n" << std::endl;
            if (connected) {
                send(6, {
                    {"token", token},
                    {"session_id", sessionID_},
                    {"seq", 1337}
                });
            } else {
                connect();
                //reconnect_timer_ = std::make_unique<asio::deadline_timer>(*asio_ios, boost::posix_time::seconds(5));
            }
        }*/

        std::shared_ptr<asio::io_service> aios_;
        const std::string token_;
        std::unique_ptr<asio::steady_timer> keepalive_timer_;
        std::unique_ptr<asio::deadline_timer> connect_timer_;
        std::unique_ptr<asio::io_service::work> hold_asio_;
        uint32_t sequence_number_ = 0;
        std::string gateway_;
    private:
        json genHandshake(const unsigned int apiVersion){
            return {
                    {"token", token_},
                    {"v", apiVersion},
                    {"properties", {
                                           {"$os", "linux"},
                                           {"$browser", "discordpp"},
                                           {"$device", "discordpp"},
                                           {"$referrer", ""}, {"$referring_domain", ""}
                                   }
                    },
                    {"compress", false},
                    {"large_threshold", 250}
            };
        }

        void keepalive(uint32_t ms){
            if(!acknowledged){
                //reconnect();
                std::cerr << "Discord Lost" << '\n';
                close();
            }else {
                std::cout << color("Sending Heartbeat ", Color.Green) << color(sequence_number_, Color.Bold) << ":\n";
                sendkeepalive({{"op", 1},
                               {"d",  sequence_number_}});
                acknowledged = false;
                //reset timer
                keepalive_timer_->expires_from_now(std::chrono::milliseconds(ms));
                keepalive_timer_->async_wait(std::bind(&WebsocketModule::keepalive, this, ms));
            }
        }

        bool acknowledged = true;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKETMODULE_HH
