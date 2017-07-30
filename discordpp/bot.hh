//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#include <vector>
#include <string>


//#include <lib/nlohmannjson/src/json.hpp>
#include <nlohmann/json.hpp>

#include "restmodule.hh"
#include "websocketmodule.hh"

namespace discordpp {
    class Bot;

    using json = nlohmann::json;
    using snowflake = uint64_t;
    using Handler = std::function<void(Bot *, json)>;

    class Bot {
    public:
        Bot(std::shared_ptr<asio::io_service> aios, std::string token, std::shared_ptr<RestModule> rmod,
            std::shared_ptr<WebsocketModule> wsmod) :
                aios_(aios),
                token_(token),
                rmod_(rmod),
                wsmod_(wsmod) {
            handlers_.insert(std::pair<std::string, Handler>("READY", [this](Bot *bot, json jmessage) {
                std::cout << "Recieved READY payload.\n";
                std::cout << jmessage.dump(4) << "\n\n\n";
                bot->gatewayVersion_ = jmessage["v"];
                bot->me_ = jmessage["user"];
                for (json &guild : jmessage["guilds"]) {
                      bot->guilds_[guild["id"].get<std::string>()] = guild;
                }
                wsmod_->sessionID_ = jmessage["session_id"];
            }));
            Handler guildmod = [](Bot *bot, json jmessage) {
                std::cout << "Recieved GUILD_CREATE payload.\n";
                //if(jmessage["s"].get<int>() == 4) {
                //    jmessage.erase("d");
                //}
                //std::cout << jmessage.dump(4) << "\n\n\n";

                bot->guilds_[jmessage["id"].get<std::string>()] = jmessage;

                for (json &presence : jmessage["presences"]) {
                      bot->presences_[presence["user"]["id"].get<std::string>()] = presence;

                }

                //gatewayVersion = jmessage["v"];
                //me = jmessage["user"];
                //privateChannels = jmessage["private_channels"];
                //guilds = jmessage["guilds"];
                //readState = jmessage["read_state"];
                //sessionID = jmessage["session_id"];
            };
            Handler presence_update = [](Bot *bot, json jmessage) {
              bot->presences_[jmessage["user"]["id"].get<std::string>()] = jmessage;
            };
            handlers_.insert(std::pair<std::string, Handler>("PRESENCE_UPDATE", presence_update));
            handlers_.insert(std::pair<std::string, Handler>("GUILD_CREATE", guildmod));
            handlers_.insert(std::pair<std::string, Handler>("GUILD_UPDATE", guildmod));
            handlers_.insert(std::pair<std::string, Handler>("GUILD_DELETE", guildmod));
            /*for(auto handler : handlers_){
                std::cout << "    " << handler.first << '\n';
            }*/

            {
                DispatchHandler on_message_wrapper = [this](std::string event, json data){
                    //std::cout << "here\n";
                    return handleDispatch(event, data);
                };
                call("/gateway", /*body*/{}, /*type*/"", [this, on_message_wrapper](Bot*, json msg){
                    wsmod_->init(
                            apiVersion_,
                            msg["url"].get<std::string>() + "/?v=" + std::to_string(apiVersion_) + "&encoding=json",
                            on_message_wrapper
                    );
                });
            }
        }

        void call(std::string target, json body = {}, std::string type = "", Handler callback = [](Bot *, json) {}) {
            if (target.at(0) != '/') {
                target = '/' + target;
            }
            callback(this, rmod_->call(target, token_, body, type));
        }

        void send(int opcode, json payload = {}) {
            wsmod_->send(opcode, payload);
        }

        void addHandler(std::string event, Handler handler) {
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

        json me_ = {};
        json guilds_;
        json presences_;

    protected:
        void handleDispatch(std::string event, nlohmann::json msg) {
            //std::cout << event << '\n';
            for (auto handler : handlers_) {
                //std::cout << "    " << handler.first << '\n';
                if (handler.first == event) {
                    handler.second(this, msg);
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

        std::shared_ptr<asio::io_service> aios_;
        const std::string token_;
        int gatewayVersion_ = -1;
        std::shared_ptr<RestModule> rmod_;
        std::shared_ptr<WebsocketModule> wsmod_;
        std::multimap<std::string, Handler> handlers_ = {};
        const unsigned int apiVersion_ = 6;
    };
}

#endif //DISCORDPP_DISCORDPP_HH
