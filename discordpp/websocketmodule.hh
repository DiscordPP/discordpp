//
// Created by aidan on 7/15/17.
//

#ifndef EXAMPLE_BOT_WEBSOCKETMODULE_HH
#define EXAMPLE_BOT_WEBSOCKETMODULE_HH

#include <chrono>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#include <lib/nlohmannjson/src/json.hpp>

namespace discordpp{
    namespace asio = boost::asio;
    using boost::system::error_code;
    using json = nlohmann::json;
    using aios_ptr = std::shared_ptr<asio::io_service>;
    using DispatchHandler = std::function<void(aios_ptr, std::string, json)>;

    class WebsocketModule{
    public:
        virtual void init(std::shared_ptr<asio::io_service> asio_ios, const std::string &token, const unsigned int apiVersion, const std::string &gateway, DispatchHandler disHandler) = 0;
        virtual void send(int opcode = 0, json payload = {}) = 0;

    protected:
        void handleMessage(aios_ptr asio_ios, const std::string &token, const unsigned int apiVersion, DispatchHandler disHandler, json msg){
            int opcode = msg["op"];
            //std::cout << "Opcode " << opcode << " recieved.\n";
            //std::cout << "recieved opcode " + std::to_string(msg["op"].get<int>()) + "\n";
            switch(opcode){
                case 0: // Dispatch
                    sequence_number_ = msg["s"];
                    //std::cout << "Event " << msg["t"] << " recieved.\n";
                    disHandler(asio_ios, msg["t"], msg["d"]);
                    break;
                case 10: // Hello
                    keepalive(msg["d"]["heartbeat_interval"]);
                    send(2, genHandshake(token, apiVersion));
                    break;
                case 11: // Heartbeat ACK
                    acknowledged = true;
                    break;
                default:
                    std::cout << "Unknown opcode " << opcode << " recieved.\n";
            }
        }

        virtual void sendkeepalive(json message) = 0;

        std::shared_ptr<asio::steady_timer> keepalive_timer_;
        uint32_t sequence_number_ = 0;
    private:
        json genHandshake(const std::string token, const unsigned int apiVersion){
            return {
                    {"token", token},
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
            /*return {
                    {"op", 2},
                    {"d",  {
                                   {"token", token},
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
                           }
                    }
            };*/
        }

        void keepalive(uint32_t ms){
            if(acknowledged == false){
                //reconnect();
                std::cerr << "Discord Lost" << '\n';
            }
            std::cout << "Sending Heartbeat " << sequence_number_ << ":\n";
            sendkeepalive({{"op", 1}, {"d", sequence_number_}});
            acknowledged = false;
            //reset timer
            keepalive_timer_->expires_from_now(std::chrono::milliseconds(ms));
            keepalive_timer_->async_wait(std::bind(&WebsocketModule::keepalive, this, ms));
        }

        bool acknowledged = true;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKETMODULE_HH
