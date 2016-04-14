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

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace discordpp{
    using json = nlohmann::json;
    namespace asio = boost::asio;
    using boost::system::error_code;
    using snowflake = uint64_t;

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
        inline json call(std::string targetURL, std::string token, json attachJSON = nullptr, std::string requestType = nullptr){
            data::lastToken() = token;
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

                request.setOpt(curlpp::options::CustomRequest(requestType));

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
    };

    namespace channels{
        inline json get(snowflake channelID, std::string token = data::lastToken()){
            return DiscordAPI::call("/channels/" + channelID, token, "GET");
        }
        //Patch and Put do the same thing.
        inline json patch(snowflake channelID, json newParams, std::string token = data::lastToken()){
            return DiscordAPI::call("/channels/" + channelID, token, newParams, "PATCH");
        }
        inline json put(snowflake channelID, json newParams, std::string token = data::lastToken()){
            return DiscordAPI::call("/channels/" + channelID, token, newParams, "PUT");
        }
        //Can't use delete as a function, I'm guessing because of C++
        inline json close(snowflake channelID, std::string token = data::lastToken()){
            return DiscordAPI::call("/channels/" + channelID, token, "DELETE");
        }
        namespace messages{
            inline json get(snowflake channelID, snowflake before = nullptr, snowflake after = nullptr, int limit = 50, std::string token = data::lastToken()){
                json toSend;
                if(before != nullptr){
                    toSend["before"] = before;
                }
                if(after != nullptr){
                    toSend["after"] = after;
                }
                toSend["limit"] = limit;
                return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/messages", token, toSend, "GET");
            }
            inline json create(snowflake channelID, std::string message, std::string nonce = nullptr, bool isTTS = false, std::string token = data::lastToken()) {
                std::string callURL = "/channels/" + std::to_string(channelID) + "/messages";

                json toSend;
                toSend["content"] = message;
                if(nonce != nullptr){
                    toSend["nonce"] = nonce;
                }
                toSend["tts"] = isTTS;

                return DiscordAPI::call(callURL, token, toSend);
            }
            inline json edit(snowflake channelID, snowflake messageID, std::string message, std::string token = data::lastToken()) {
                std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID);

                json toSend;
                toSend["content"] = message;

                return DiscordAPI::call(callURL, token, toSend, "PATCH");
            }
            inline json remove(snowflake channelID, snowflake messageID, std::string token = data::lastToken()) {
                std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID);
                return DiscordAPI::call(callURL, token, "DELETE");
            }
            inline json acknowledge(snowflake channelID, snowflake messageID, std::string token = data::lastToken()){
                std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID) + "/ack";
                return DiscordAPI::call(callURL, token);
            }
            //I'm not sure hw to handle the permissions. https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
            namespace invites{
                inline json get(snowflake channelID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/invites", token, "GET");
                }
                inline json create(snowflake channelID, int max_age = nullptr, int max_uses = nullptr, bool temporary = nullptr, bool xkcdpass = nullptr, std::string token = data::lastToken()) {
                    std::string callURL = "/channels/" + std::to_string(channelID) + "/invites";

                    json toSend;
                    if(max_age != nullptr)
                        toSend["max_age"] = max_age;
                    if(max_uses != nullptr)
                        toSend["max_uses"] = max_uses;
                    if(temporary != nullptr)
                        toSend["temporary"] = temporary;
                    if(xkcdpass != nullptr)
                        toSend["xkcdpass"] = xkcdpass;

                    return DiscordAPI::call(callURL, token, toSend, "POST");
                }
            }
            inline json typing(snowflake channelID, std::string token = data::lastToken()){
                return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/typing", token, "POST");
            }
        }



        /* Old API
        inline json fetchInfo(snowflake channelID, std::string token = data::lastToken()){
            return DiscordAPI::call("/channels/" + channelID, token);
        }
        namespace messages {
                inline json nextMessage(snowflake channelID, std::string lastMessageID = 0, std::string token = data::lastToken()){
                std::string callURL = "/channels/" + channelID + "/messages?limit=1&after=" + lastMessageID;
                return DiscordAPI::call(callURL, data::lastToken());
            }
            inline json create(snowflake channelID, std::string message, std::vector<std::string> mentions = {}, bool isTTS = false, std::string token = data::lastToken()) {
                std::string callURL = "/channels/" + channelID + "/messages";

                json toSend;
                toSend["content"] = message;
                toSend["mentions"] = mentions;
                toSend["tts"] = isTTS;

                return DiscordAPI::call(callURL, token, toSend);
            }
        };
        namespace manage{
            inline json create(std::string guildID, std::string name, enum type{text, voice} = text, std::string token = data::lastToken()){
                std::string types [] = {"text", "voice"};
                json toSend;
                toSend["name"] = name;
                toSend["type"] = types[type];

                return DiscordAPI::call("/guilds/:guild_id/channels");
            }
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

    namespace gateway{
        inline std::string fetchURL(std::string token = data::lastToken()){
            return DiscordAPI::call("/gateway", token).at("url");
        }
    }

    class Client {
        using client = websocketpp::client<websocketpp::config::asio_tls_client>;
        using message_ptr = websocketpp::config::asio_client::message_type::ptr;
    public:
        Client(asio::io_service& asio_ios, const std::string& token, std::map <std::string, std::string> soft_responses = {})
                : asio_ios_(asio_ios)
                , token_(token)
                , keepalive_timer_(asio_ios)
                , soft_responses_(soft_responses)
        {
            client_.set_access_channels(websocketpp::log::alevel::all);
            client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            client_.set_tls_init_handler([this](websocketpp::connection_hdl){
                return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
            });

            client_.init_asio(&asio_ios);

            client_.set_message_handler(std::bind(&Client::on_message, this,
                                                  std::placeholders::_1, std::placeholders::_2));
            client_.set_open_handler(std::bind(&Client::on_open, this,
                                               std::placeholders::_1));

            websocketpp::lib::error_code ec;
            std::string uri = discordpp::gateway::fetchURL();
            std::cout << uri << "\n";
            connection_ = client_.get_connection(uri, ec);
            if (ec) {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                //TBD: throw something
            } else {
                // Note that connect here only requests a connection. No network messages are
                // exchanged until the event loop starts running in the next line.
                client_.connect(connection_);
            }
        }
        ~Client() =default;
        Client(const Client&) =delete;
        Client& operator=(const Client&) =delete;

    private:
        void on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
            auto jmessage = json::parse(msg->get_payload());
            if(jmessage["t"] == "READY"){
                ready_ = jmessage;
                uint32_t ms = jmessage["d"]["heartbeat_interval"];
                ms *= .9;
                keepalive(ms);
            } else if(jmessage["t"] == "MESSAGE_CREATE"){
                for(auto& val : jmessage["d"]["mentions"]){
                    if(val["id"] == ready_["d"]["user"]["id"] && jmessage["d"]["content"].get<std::string>().length() > 23){
                        std::string message = jmessage["d"]["content"].get<std::string>();
                        message = message.substr(ready_["d"]["user"]["id"].get<std::string>().length() + 4, message.length());
                        //Echo
                        /*
                        //std::string message = jmessage["d"]["content"].get<std::string>();
                        std::string target = ready_["d"]["user"]["username"].get<std::string>();
                        std::string targetID = jmessage["d"]["author"]["id"].get<std::string>();

                        //message =message.substr(ready_["d"]["user"]["id"].get<std::string>().length() + 4, message.length()); // "<@" + targetID + "> " +

                        std::cout << "Sending message " << jmessage["d"]["content"] << " to user " << jmessage["d"]["author"]["username"] << "\n";

                        discordpp::channels::messages::create(jmessage["d"]["channel_id"],  message);//, {targetID}
                        */
                        if(soft_responses_[message].empty()){
                            discordpp::channels::messages::create(jmessage["d"]["channel_id"],
                                                                  "`" + message + "` is not a command.");
                        } else {
                            discordpp::channels::messages::create(jmessage["d"]["channel_id"], soft_responses_[message]);
                        }
                    }
                }
            } else if(jmessage["t"] == "MESSAGE_UPDATE"){

            } else if(jmessage["t"] == "TYPING_START"){

            } else if(jmessage["t"] == "PRESENCE_UPDATE"){

            } else if(jmessage["t"] == "TYPING_START"){

            } else if(jmessage["t"] == "VOICE_STATE_UPDATE"){

            } else {
                std::cout << "on_message called with hdl: " << hdl.lock().get()
                << " and message: " << jmessage.dump(3)
                << "\n";
            }
        }

        void keepalive(uint32_t ms){
            //send message
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            json message = {
                    {"op", 1},
                    {"d", millis}
            };
            client_.send(connection_, message.dump(), websocketpp::frame::opcode::text);
            //reset timer
            keepalive_timer_.expires_from_now(std::chrono::milliseconds(ms));
            keepalive_timer_.async_wait(std::bind(&Client::keepalive, this, ms));
            std::cout << "Ran keepalive\n";
        }

        void on_open(websocketpp::connection_hdl hdl) {
            std::cout << "Connection established.\n";

            json connect = {
                    {"op", 2},
                    {"d", {
                                   {"token", token_},
                                   {"v", 3},
                                   {"properties", {
                                                          {"$os", ""},
                                                          {"$browser", ""},
                                                          {"$device", ""},
                                                          {"$referrer",""},
                                                          {"$referring_domain",""}
                                                  }},
                                   {"large_threshold", 250}
                           }}
            };
            std::cout << connect.dump(1) << "\n";
            client_.send(hdl, connect.dump(), websocketpp::frame::opcode::text);
        }

        //////////////////////////////
        asio::io_service& asio_ios_;
        std::string token_;
        client client_;
        websocketpp::uri_ptr uri_ptr_;
        client::connection_ptr connection_;
        asio::steady_timer keepalive_timer_;
        json ready_;
        std::map <std::string, std::string> soft_responses_;
    };
}

#endif //DISCORDPP_DISCORDPP_HH
