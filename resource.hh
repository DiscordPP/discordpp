//
// Created by aidan on 5/9/16.
//

#ifndef DISCORDPP_RESOURCE_HH
#define DISCORDPP_RESOURCE_HH

#include "lib/nlohmannjson/src/json.hpp"

namespace resource {
    namespace discord {
        inline int& gatewayVersion(){
            static int v = -1;
            return v;
        }
        inline nlohmann::json& me(){
            static nlohmann::json j = {};
            return j;
        }
        inline nlohmann::json& privateChannels(){
            static nlohmann::json j = {};
            return j;
        }
        inline nlohmann::json& guilds(){
            static nlohmann::json j = {};
            return j;
        }
        inline nlohmann::json& readState(){
            static nlohmann::json j = {};
            return j;
        }
        inline std::string& sessionID(){
            static std::string s = "";
            return s;
        }
    };
}

#endif //DISCORDPP_RESOURCE_HH
