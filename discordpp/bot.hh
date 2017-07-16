//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#include <vector>
#include <string>


#include <lib/nlohmannjson/src/json.hpp>

#include "restmodule.hh"
#include "websocketmodule.hh"

namespace discordpp{
    class Bot;

    using json = nlohmann::json;
    using snowflake = uint64_t;
    using aios_ptr = std::shared_ptr<asio::io_service>;
    using Handler = std::function<void(Bot*, aios_ptr, json)>;

    class Bot{
    public:
        Bot(std::string token, std::shared_ptr<RestModule> rmod, std::shared_ptr<WebsocketModule> wsmod) :
            token_(token),
            rmod_(rmod),
            wsmod_(wsmod){
            handlers_["READY"] = [](Bot* bot, aios_ptr asio_ios, json jmessage) {
                std::cout << "Recieved READY payload.\n";
                std::cout << jmessage.dump(4) << "\n\n\n";
                bot->gatewayVersion_ = jmessage["d"]["v"];
                bot->me_ = jmessage["d"]["user"];
                bot->guilds_ = jmessage["d"]["guilds"];
                bot->sessionID_ = jmessage["d"]["session_id"];
            };
            handlers_["GUILD_CREATE"] = [](Bot* bot, aios_ptr asio_ios, json jmessage) {
                std::cout << "Recieved GUILD_CREATE payload.\n";
                //if(jmessage["s"].get<int>() == 4) {
                //    jmessage.erase("d");
                //}
                //std::cout << jmessage.dump(4) << "\n\n\n";

                bool replaced = false;
                for(json& guild : bot->guilds_){
                    if(guild["id"] == jmessage["d"]["id"]){
                        replaced = true;
                        guild = jmessage["d"];
                    }
                }
                if(!replaced){
                    bot->guilds_.push_back(jmessage["d"]);
                }

                //gatewayVersion = jmessage["d"]["v"];
                //me = jmessage["d"]["user"];
                //privateChannels = jmessage["d"]["private_channels"];
                //guilds = jmessage["d"]["guilds"];
                //readState = jmessage["d"]["read_state"];
                //sessionID = jmessage["d"]["session_id"];
            };
            handlers_["GUILD_UPDATE"] = handlers_["GUILD_CREATE"];
            handlers_["GUILD_DELETE"] = handlers_["GUILD_CREATE"];
        }

        json call(std::string target, json body = {}, std::string type = ""){
            rmod_->call(target, token_, body, type);
        }

        void addHandler(std::string event, Handler handler){
            //Handler bound = std::bind(handler, this, std::placeholders::_1, std::placeholders::_2);
            if(handlers_.find(event) != handlers_.end()){
                Handler old = handlers_[event];
                handlers_[event] = [old, handler](Bot* bot, aios_ptr asio_ios, json jmessage){
                    old(bot, asio_ios, jmessage);
                    handler(bot, asio_ios, jmessage);
                };
            }else{
                handlers_[event] = handler;
            }
        }

        void init(aios_ptr asio_ios){
            auto on_message_wrapper = [this, asio_ios](json msg){
                on_message(asio_ios, msg);
            };
            wsmod_->init(asio_ios, token_, rmod_->call("/gateway", token_).at("url").get<std::string>(), on_message_wrapper);
        }

        void on_message(aios_ptr asio_ios, nlohmann::json jmessage) {
            if(jmessage["op"].get<int>() == 0){ //Dispatch
                std::map<std::string, Handler>::iterator it = handlers_.find(jmessage["t"]);
                if(it != handlers_.end()){
                    asio_ios->post(std::bind(it->second, this, asio_ios, jmessage));
                } else {
                    std::cout << "There is no function for the event " << jmessage["t"] << ".\n";
                }
                if(jmessage["t"] == "READY") {
                    uint32_t ms = jmessage["d"]["heartbeat_interval"];
                    ms *= .9;
                    wsmod_->keepalive(ms);
                }
            } else if(jmessage["op"].get<int>() == 1){ //Heartbeat (This isn't implemented yet, still using periodic heartbeats for now.)
                //client_.send(hdl, jmessage.dump(), websocketpp::frame::opcode::text);
            } else { //Wat
                std::cout << "Unexpected opcode received:\n\n" << jmessage.dump(4) << "\n\n\n";
            }
        }

        json me_ = {};
        json guilds_ = {};

    private:
        const std::string token_;
        std::string sessionID_ = "";
        int gatewayVersion_ = -1;
        std::shared_ptr<RestModule> rmod_;
        std::shared_ptr<WebsocketModule> wsmod_;
        std::map<std::string, Handler> handlers_ = {};
    };
}

#endif //DISCORDPP_DISCORDPP_HH
