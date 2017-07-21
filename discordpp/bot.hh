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
            handlers_.insert(std::pair<std::string, Handler>("READY", [](Bot* bot, aios_ptr asio_ios, json jmessage) {
                std::cout << "Recieved READY payload.\n";
                std::cout << jmessage.dump(4) << "\n\n\n";
                bot->gatewayVersion_ = jmessage["v"];
                bot->me_ = jmessage["user"];
                bot->guilds_ = jmessage["guilds"];
                bot->sessionID_ = jmessage["session_id"];
            }));
            Handler guildmod = [](Bot* bot, aios_ptr asio_ios, json jmessage) {
                std::cout << "Recieved GUILD_CREATE payload.\n";
                //if(jmessage["s"].get<int>() == 4) {
                //    jmessage.erase("d");
                //}
                //std::cout << jmessage.dump(4) << "\n\n\n";

                bool replaced = false;
                for(json& guild : bot->guilds_){
                    if(guild["id"] == jmessage["id"]){
                        replaced = true;
                        guild = jmessage;
                    }
                }
                if(!replaced){
                    bot->guilds_.push_back(jmessage);
                }

                //gatewayVersion = jmessage["v"];
                //me = jmessage["user"];
                //privateChannels = jmessage["private_channels"];
                //guilds = jmessage["guilds"];
                //readState = jmessage["read_state"];
                //sessionID = jmessage["session_id"];
            };
            handlers_.insert(std::pair<std::string, Handler>("GUILD_CREATE", guildmod));
            handlers_.insert(std::pair<std::string, Handler>("GUILD_UPDATE", guildmod));
            handlers_.insert(std::pair<std::string, Handler>("GUILD_DELETE", guildmod));
            /*for(auto handler : handlers_){
                std::cout << "    " << handler.first << '\n';
            }*/
        }

        void call(aios_ptr asio_ios, std::string target, json body = {}, std::string type = "", Handler callback = [](Bot*, aios_ptr, json){}){
            if(target.at(0) != '/'){
                target = '/' + target;
            }
            callback(this, asio_ios, rmod_->call(asio_ios, target, token_, body, type));
        }

        void send(int opcode, json payload = {}){
            wsmod_->send(opcode, payload);
        }

        void addHandler(std::string event, Handler handler){
            handlers_.insert(std::pair<std::string, Handler>(event, handler));
            //Handler bound = std::bind(handler, this, std::placeholders::_1, std::placeholders::_2);
            /*if(handlers_.find(event) != handlers_.end()){
                Handler old = handlers_[event];
                handlers_[event] = [old, handler](Bot* bot, aios_ptr asio_ios, json jmessage){
                    old(bot, asio_ios, jmessage),
                    handler(bot, asio_ios, jmessage);
                };
            }else{
                handlers_[event] = handler;
            }*/
        }

        void init(aios_ptr asio_ios){
            DispatchHandler on_message_wrapper = [this](aios_ptr asio_ios, std::string event, json data){
                //std::cout << "here\n";
                return handleDispatch(asio_ios, event, data);
            };
            call(asio_ios, "/gateway", /*body*/{}, /*type*/"", [this, on_message_wrapper](Bot*, aios_ptr asio_ios, json msg){
                wsmod_->init(
                        asio_ios,
                        token_,
                        apiVersion_,
                        msg["url"].get<std::string>() + "/?v=" + std::to_string(apiVersion_) + "&encoding=json",
                        on_message_wrapper
                );
            });
        }

        json me_ = {};
        json guilds_ = {};

    protected:
        void handleDispatch(aios_ptr asio_ios, std::string event, nlohmann::json msg) {
            //std::cout << event << '\n';
            for(auto handler : handlers_){
                //std::cout << "    " << handler.first << '\n';
                if(handler.first == event){
                    handler.second(this, asio_ios, msg);
                }
            }
            /*std::map<std::string, Handler>::iterator it = handlers_.find(event);
            if (it != handlers_.end()) {
                it->second(this, asio_ios, msg);
            } else {
                std::cout << "There is no function for the event " << event << ".\n";
            }*/
            //if (jmessage["t"] == "READY") {
            //    uint32_t ms = jmessage["heartbeat_interval"];
            //    ms *= .9;
            //    wsmod_->keepalive(ms);
            //}
        }

        const std::string token_;
        std::string sessionID_ = "";
        int gatewayVersion_ = -1;
        std::shared_ptr<RestModule> rmod_;
        std::shared_ptr<WebsocketModule> wsmod_;
        std::multimap<std::string, Handler> handlers_ = {};
        const unsigned int apiVersion_ = 6;
    };
}

#endif //DISCORDPP_DISCORDPP_HH
