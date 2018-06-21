//
// Created by Aidan on 6/21/2018.
//

#ifndef EXAMPLE_BOT_BOTREQUIRED_HH
#define EXAMPLE_BOT_BOTREQUIRED_HH

#include <nlohmann/json.hpp>

namespace discordpp {
    using json = nlohmann::json;
    struct ratelimit{
        int millis;
    };
    class BotStruct {
    public:
        virtual json call(std::string targetURL, std::string token, json body = {}, std::string requestType = "") = 0;

    protected:
        unsigned int apiVersion;
    };
}

#endif //EXAMPLE_BOT_BOTREQUIRED_HH
