//
// Created by aidan on 7/15/17.
//

#ifndef EXAMPLE_BOT_RESTMODULE_HH
#define EXAMPLE_BOT_RESTMODULE_HH

//#include <lib/nlohmannjson/src/json.hpp>
#include <nlohmann/json.hpp>

namespace discordpp{
    using json = nlohmann::json;

    struct ratelimit{
        int millis;
    };

    class RestModule{
    public:
        RestModule(std::shared_ptr<asio::io_service> asio_ios, const std::string &token):
            aios_(asio_ios),
            token_(token){}
        virtual json call(std::string targetURL, std::string token, json body = {}, std::string requestType = "", std::string fileName = "") = 0;

    protected:
        std::shared_ptr<asio::io_service> aios_;
        const std::string token_;
    };
}

#endif //EXAMPLE_BOT_RESTMODULE_HH
