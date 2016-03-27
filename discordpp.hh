//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#include <string>


#include "lib/curlpp/include/curlpp/cURLpp.hpp"
#include "lib/curlpp/include/curlpp/Easy.hpp"
#include "lib/curlpp/include/curlpp/Options.hpp"
#include "lib/curlpp/include/curlpp/Exception.hpp"

#include "lib/nlohmannjson/src/json.hpp"

namespace discordpp{
    using json = nlohmann::json;

    namespace data{
        inline bool& debug(){
            static bool d = false;
            return d;
        }
        inline std::string& lastToken() {
            static std::string t;
            return t;
        }
    };

    namespace DiscordAPI {
        inline json call(std::string targetURL, std::string token, json attachJSON = nullptr){
            try
            {
                std::stringstream outstream;

                cURLpp::Cleanup clean;
                //curlpp::Cleanup clean;
                curlpp::Easy request;
                curlpp::options::WriteStream ws(&outstream);
                request.setOpt(ws);
                request.setOpt<curlpp::options::Url>("https://discordapp.com/api" + targetURL);
                request.setOpt(curlpp::options::Verbose(false));

                std::list<std::string> header;
                header.push_back("Content-Type: application/json");
                if(token != "") {
                    header.push_back(std::string("Authorization: ") + token);\
                }
                request.setOpt(curlpp::options::HttpHeader(header));

                if(attachJSON != nullptr) {
                    //std::cout << attachJSON.dump() << std::endl;
                    request.setOpt(curlpp::options::PostFields(attachJSON.dump()));
                    request.setOpt(curlpp::options::PostFieldSize(attachJSON.dump().length()));
                }

                request.perform();

                json returned = json::parse(outstream.str());

                try {
                    //std::cout << returned.dump() << std::endl;
                    std::string message = returned.at("message").get<std::string>();
                    //std::cout << returned.dump() << std::endl;
                    if(message != "") {
                        std::cout << "Discord API sent a message: \"" << message << "\"" << std::endl;
                    }
                } catch ( std::out_of_range & e) {

                } catch ( std::domain_error & e) {

                }

                return returned;
            }
            catch ( curlpp::LogicError & e ) {
                std::cout << "logic " << e.what() << std::endl;
            }
            catch ( curlpp::RuntimeError & e ) {
                std::cout << "runtime " << e.what() << std::endl;
            }

            return nullptr;
        }
        /*
        inline std::string call(std::string targetURL, std::string token){
            return call(targetURL, NULL, token);
        }
        inline std::string call(std::string targetURL, json attachJSON){
            return call(targetURL, attachJSON, data::lastToken());
        }
        inline std::string call(std::string targetURL){
            return call(targetURL, NULL, data::lastToken());
        }
         */
    };

    namespace auth {
        inline std::string login(std::string email, std::string password) {
            json loginInfo;
            loginInfo["email"]    = email;
            loginInfo["password"] = password;

            //std::cout << loginInfo.dump() << std::endl;

            json returned = DiscordAPI::call("/auth/login", "", loginInfo);

            //std::cout << returned.dump() << std::endl;
            std::string token = returned.at("token").get<std::string>();
            data::lastToken() = token;
            return token;
        };
        inline void logout(std::string token = data::lastToken()) {
            DiscordAPI::call("/auth/logout", token);
        };
    };

    namespace users{
        inline json fetchInfo(std::string userID = "@me", std::string token = data::lastToken()){
            return DiscordAPI::call("/users/" + userID, token);
        };
        inline json fetchGuilds(std::string userID = "@me", std::string token = data::lastToken()){
            return DiscordAPI::call("/users/" + userID + "/guilds", token);
        };
    };

    namespace guilds{
        inline json fetchChannels(std::string guildID, std::string token = data::lastToken()){
            return DiscordAPI::call("/guilds/" + guildID + "/channels", token);
        }
        inline json fetchInfo(std::string guildID, std::string token = data::lastToken()){
            return DiscordAPI::call("/guilds/" + guildID, token);
        }
    }

    namespace channels{
        inline json fetchInfo(std::string channelID, std::string token = data::lastToken()){
            return DiscordAPI::call("/channels/" + channelID, token);
        }
        namespace messages {
            inline json nextMessage(std::string channelID, std::string lastMessageID = 0, std::string token = data::lastToken()){
                std::string callURL = "/channels/" + channelID + "/messages?limit=1&after=" + lastMessageID;
                return DiscordAPI::call(callURL, data::lastToken());
            }
            inline json sendMessage(std::string channelID, std::string message, std::vector<std::string> mentions = {}, bool isTTS = false, std::string token = data::lastToken()) {
                std::string callURL = "/channels/" + channelID + "/messages";

                json toSend;
                toSend["content"] = message;
                toSend["mentions"] = mentions;
                toSend["tts"] = isTTS;

                return DiscordAPI::call(callURL, token, toSend);
            }
        };
    };

    namespace gateway{
        inline std::string fetchURL(std::string token = data::lastToken()){
            return DiscordAPI::call("/gateway", token).at("url");
        }
    }
}

#endif //DISCORDPP_DISCORDPP_HH
