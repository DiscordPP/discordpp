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
    };
}

#endif //DISCORDPP_DISCORDPP_HH
