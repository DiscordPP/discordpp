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

    class Bot {
    public:
        Bot(std::string token);

        ~Bot();

        void addResponse(std::string event, std::function<void(Bot *, json)> response);

        void start();

        int gatewayVersion = -1;
        json me = {};
        json privateChannels = {};
        json guilds = {};
        json readState = {};
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
                }}, {"GUILD_CREATE", [this](json jmessage) {
                    std::cout << "Recieved GUILD_CREATE payload.\n";
                    //if(jmessage["s"].get<int>() == 4) {
                    //    jmessage.erase("d");
                    //}
                    //std::cout << jmessage.dump(4) << "\n\n\n";

                    bool replaced = false;
                    for(json& guild : guilds){
                        if(guild["id"] == jmessage["d"]["id"]){
                            replaced = true;
                            guild = jmessage["d"];
                        }
                    }
                    if(!replaced){
                        guilds.push_back(jmessage["d"]);
                    }

                    //gatewayVersion = jmessage["d"]["v"];
                    //me = jmessage["d"]["user"];
                    //privateChannels = jmessage["d"]["private_channels"];
                    //guilds = jmessage["d"]["guilds"];
                    //readState = jmessage["d"]["read_state"];
                    //sessionID = jmessage["d"]["session_id"];
                }}
        };
    };
}


#endif //DISCORDPP_DPPBOT_H
