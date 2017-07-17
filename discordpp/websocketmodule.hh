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
    using WSMessageHandler = std::function<void(aios_ptr, json)>;

    class WebsocketModule{
    public:
        virtual void init(std::shared_ptr<asio::io_service> asio_ios, const std::string &token, const std::string &gateway, WSMessageHandler message_handler) = 0;
        void keepalive(uint32_t ms){
            json message = {
                    {"op", 1},
                    {"d", sequence_number_}
            };
            std::cout << "Sending Heartbeat:\n";
            sendkeepalive(message);
            //reset timer
            keepalive_timer_->expires_from_now(std::chrono::milliseconds(ms));
            keepalive_timer_->async_wait(std::bind(&WebsocketModule::keepalive, this, ms));
        }

    protected:
        void keepalivewrap(aios_ptr asio_ios, json jmessage, WSMessageHandler handler){
            if(!jmessage["s"].is_null()) {
                sequence_number_ = jmessage["s"].get<int>();
            }
            handler(asio_ios, jmessage);
        }

        virtual void sendkeepalive(json message) = 0;

        std::shared_ptr<asio::steady_timer> keepalive_timer_;
        uint32_t sequence_number_ = 0;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKETMODULE_HH
