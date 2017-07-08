//
// Created by aidan on 11/1/16.
//

#ifndef DISCORDPP_DPPBOT_H
#define DISCORDPP_DPPBOT_H

#include <string>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#include "discordpp.hh"

namespace asio = boost::asio;

namespace discordpp {
    using json = nlohmann::json;

    class DLLAPI Bot {
    public:
        Bot(std::string token);

        ~Bot();

        void addResponse(std::string event, std::function<void(Bot *, json)> response);

        void start();

        json me = {};
        json privateChannels = {};
        json guilds = {};

    private:
        //asio::io_service asio_ios_;
        std::string token_;
        std::string sessionID = "";
        int gatewayVersion = -1;

        std::map<std::string, std::function<void(json)>> eventResponses_ = {};
    };
}


#endif //DISCORDPP_DPPBOT_H
