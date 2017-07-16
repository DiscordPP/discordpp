//
// Created by aidan on 7/15/17.
//

#ifndef EXAMPLE_BOT_RESTMODULE_HH
#define EXAMPLE_BOT_RESTMODULE_HH

#include <lib/nlohmannjson/src/json.hpp>

namespace discordpp{
    using json = nlohmann::json;

    struct ratelimit{
        int millis;
    };

    class RestModule{
    public:
        virtual json call(std::string targetURL, std::string token, json body = {}, std::string requestType = "") = 0;
    };
}

#endif //EXAMPLE_BOT_RESTMODULE_HH
