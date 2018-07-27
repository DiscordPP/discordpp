//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#include <vector>
#include <string>

#include <nlohmann/json.hpp>

#include "botStruct.hh"

namespace discordpp {
    using json = nlohmann::json;
    using snowflake = uint64_t;

    class Bot : virtual BotStruct {
    public:
        Bot(){
            needInit["Bot"] = true;
        }
        void initBot(unsigned int apiVersionIn, std::string tokenIn){
            if(needInit["Bot"]){
                apiVersion = apiVersionIn;
                token = tokenIn;
                aioc = std::make_shared<asio::io_context>();
                needInit["Bot"] = false;
            }
        }
        void initBot(unsigned int apiVersionIn, std::string tokenIn, std::shared_ptr<asio::io_context> aiocIn){
            if(needInit["Bot"]){
                apiVersion = apiVersionIn;
                token = tokenIn;
                aioc = aiocIn;
                needInit["Bot"] = false;
            }
        }
    };
}

#endif //DISCORDPP_DISCORDPP_HH
