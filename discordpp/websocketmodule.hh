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
    using DispatchHandler = std::function<std::vector<json>(aios_ptr, std::string, json)>;

    class WebsocketModule{
    public:
        virtual void init(std::shared_ptr<asio::io_service> asio_ios, const std::string &token, const std::string &gateway, DispatchHandler disHandler) = 0;

        std::vector<json> handleMessage(aios_ptr asio_ios, const std::string &token, DispatchHandler disHandler, json msg){
            int opcode = msg["op"];
            std::vector<json> toSend;
            //std::cout << "Opcode " << opcode << " recieved.\n";
            switch(opcode){
                case 0: // Dispatch
                    sequence_number_ = msg["s"];
                    //std::cout << "Event " << msg["t"] << " recieved.\n";
                    toSend = disHandler(asio_ios, msg["t"], msg["d"]);
                    break;
                case 10: // Hello
                    keepalive(msg["d"]["heartbeat_interval"]);
                    toSend.push_back(genHandshake(token));
                    break;
                case 11: // Heartbeat ACK
                    acknowledged = true;
                    break;
                default:
                std::cout << "Unknown opcode " << opcode << " recieved.\n";
            }
            return toSend;
        }

        json genHandshake(std::string token){
            return {
                    {"op", 2},
                    {"d",  {
                                   {"token", token},
                                   {"v", 5},
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
            };
        }

        void keepalive(uint32_t ms){
            if(acknowledged = false){
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

    protected:
        virtual void sendkeepalive(json message) = 0;

        std::shared_ptr<asio::steady_timer> keepalive_timer_;
        uint32_t sequence_number_ = 0;
    private:
        bool acknowledged = true;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKETMODULE_HH
