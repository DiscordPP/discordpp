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
    class Bot {
    public:
        Bot(std::string token);

        ~Bot();

        void setResponse(std::string event, std::function<void(Bot*, json)> response);

        void start();

        int gatewayVersion = -1;
        nlohmann::json me = {};
        nlohmann::json privateChannels = {};
        nlohmann::json guilds = {};
        nlohmann::json readState = {};
        std::string sessionID = "";

    private:
        //asio::io_service asio_ios_;
        std::string token_;

        std::map<std::string, std::function<void(json)>> eventResponses_ = {
                {"READY", [this](json jmessage) {
                    std::cout << "Recieved READY payload.\n";
                    std::cout << jmessage.dump(4) << "\n\n\n";
                    gatewayVersion = jmessage["d"]["v"];
                    me = jmessage["d"]["user"];
                    privateChannels = jmessage["d"]["private_channels"];
                    guilds = jmessage["d"]["guilds"];
                    readState = jmessage["d"]["read_state"];
                    sessionID = jmessage["d"]["session_id"];
                }}
        };
    };
}


#endif //DISCORDPP_DPPBOT_H
