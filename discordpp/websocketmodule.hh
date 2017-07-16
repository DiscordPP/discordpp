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

    class WebsocketModule{
    public:
        virtual void init(std::shared_ptr<asio::io_service> asio_ios, const std::string &token, const std::string &gateway, std::function<void(json)> message_handler) = 0;
        void keepalive(uint32_t ms){
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            json message = {
                    {"op", 1},
                    {"d", millis}
            };
            std::cout << "Sending Heartbeat:\n";
            sendkeepalive(message);
            //reset timer
            keepalive_timer_->expires_from_now(std::chrono::milliseconds(ms));
            keepalive_timer_->async_wait(std::bind(&WebsocketModule::keepalive, this, ms));
        }

    protected:
        virtual void sendkeepalive(json message) = 0;

        std::shared_ptr<asio::steady_timer> keepalive_timer_;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKETMODULE_HH
