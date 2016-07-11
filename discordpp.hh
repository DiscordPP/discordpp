//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#include <vector>

#include "lib/curlpp/include/curlpp/cURLpp.hpp"
#include "lib/curlpp/include/curlpp/Easy.hpp"
#include "lib/curlpp/include/curlpp/Options.hpp"
#include "lib/curlpp/include/curlpp/Exception.hpp"
#define STDC_HEADERS 1
#include <string>

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
        inline json call(std::string targetURL, std::string token, json attachJSON = {}, std::string requestType = ""){
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

                if(!requestType.empty()) {
                    request.setOpt(curlpp::options::CustomRequest(requestType));
                }

                std::list<std::string> header;
                header.push_back("Content-Type: application/json");
                if(token != "") {
                    header.push_back(std::string("Authorization: ") + token);\
                }
                request.setOpt(curlpp::options::HttpHeader(header));

                if(!attachJSON.empty()) {
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

            return {};
        }

        namespace channels{
            inline json get(snowflake channelID, std::string token = data::lastToken()){
                return DiscordAPI::call("/channels/" + std::to_string(channelID), token, {}, "GET");
            }
            inline json modify(snowflake channelID, json newParams, std::string token = data::lastToken()){
                return DiscordAPI::call("/channels/" + std::to_string(channelID), token, newParams, "PATCH");
            }
            //Can't use delete as a function, I'm guessing because of C++
            inline json close(snowflake channelID, std::string token = data::lastToken()){
                return DiscordAPI::call("/channels/" + std::to_string(channelID), token, {}, "DELETE");
            }
            namespace messages{
                inline json get(snowflake channelID, snowflake before = 0, snowflake after = 0, int limit = 50, std::string token = data::lastToken()){
                    json toSend;
                    if(before != 0){
                        toSend["before"] = before;
                    }
                    if(after != 0){
                        toSend["after"] = after;
                    }
                    toSend["limit"] = limit;
                    return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/messages", token, toSend, "GET");
                }
                inline json create(snowflake channelID, std::string message, std::string nonce = "", bool isTTS = false, std::string token = data::lastToken()) {
                    std::string callURL = "/channels/" + std::to_string(channelID) + "/messages";

                    json toSend;
                    toSend["content"] = message;
                    if(nonce != ""){
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
                    return DiscordAPI::call(callURL, token, {}, "DELETE");
                }
                inline json acknowledge(snowflake channelID, snowflake messageID, std::string token = data::lastToken()){
                    std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID) + "/ack";
                    return DiscordAPI::call(callURL, token);
                }
                //I'm not sure hw to handle the permissions. https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
                namespace invites{
                    inline json get(snowflake channelID, std::string token = data::lastToken()){
                        return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/invites", token, {}, "GET");
                    }
                    inline json create(snowflake channelID, int max_age = 0, int max_uses = 0, bool temporary = true, bool xkcdpass = false, std::string token = data::lastToken()) {
                        std::string callURL = "/channels/" + std::to_string(channelID) + "/invites";

                        json toSend;
                        if(max_age != 0)
                            toSend["max_age"] = max_age;
                        if(max_uses != 0)
                            toSend["max_uses"] = max_uses;
                        toSend["temporary"] = temporary;
                        toSend["xkcdpass"] = xkcdpass;

                        return DiscordAPI::call(callURL, token, toSend, "POST");
                    }
                }
                inline json typing(snowflake channelID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/typing", token, {}, "POST");
                }
            }
        };

        namespace guilds{
            inline json create(std::string name, std::string region, std::string icon, std::string token = data::lastToken()){
                std::string callURL = "/guilds";

                json toSend;
                toSend["name"] = name;
                toSend["region"] = region;
                toSend["icon"] = icon;

                return DiscordAPI::call(callURL, token, toSend, "POST");
            }
            inline json get(snowflake guildID, std::string token = data::lastToken()){
                return DiscordAPI::call("/guilds/" + std::to_string(guildID), token, {}, "GET");
            }
            inline json modify(snowflake guildID, json newParams, std::string token = data::lastToken()){
                return DiscordAPI::call("/guilds/" + std::to_string(guildID), token, newParams, "PATCH");
            }
            namespace channels{
                inline json createText(snowflake guildID, std::string name, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/channels";

                    json toSend;
                    toSend["name"] = name;
                    toSend["type"] = "text";

                    return DiscordAPI::call(callURL, token, toSend, "POST");
                }
                inline json createVoice(snowflake guildID, std::string name, int bitrate, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/channels";

                    json toSend;
                    toSend["name"] = name;
                    toSend["type"] = "voice";
                    toSend["bitrate"] = bitrate;

                    return DiscordAPI::call(callURL, token, toSend, "POST");
                }
                inline json get(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/channels", token, {}, "GET");
                }
                inline json modify(snowflake guildID, json newParams, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/channels", token, newParams, "PATCH");
                }
            }
            namespace members{
                inline json getInfo(snowflake guildID, snowflake userID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/members/" + std::to_string(userID) , token, {}, "GET");
                }
                inline json getList(snowflake guildID, int limit = 1, int offset = 0, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/members";

                    json toSend;
                    toSend["limit"] = limit;
                    toSend["offset"] = offset;

                    return DiscordAPI::call(callURL, token, toSend, "GET");
                }
                inline json modify(snowflake guildID, snowflake userID, json newParams, std::string token = data::lastToken()) {
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/members/" + std::to_string(userID), token, newParams, "PATCH");
                }
                inline json remove(snowflake guildID, snowflake userID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/members/" + std::to_string(userID) , token, {}, "DELETE");
                }
            }
            namespace bans{
                inline json get(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/bans", token, {}, "GET");
                }
                inline json create(snowflake guildID, snowflake userID, int deleteMessageDays = 1, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/bans/" + std::to_string(userID);

                    json toSend;
                    toSend["delete-message-days"] = deleteMessageDays;

                    return DiscordAPI::call(callURL, token, toSend, "PUT");
                }
                inline json remove(snowflake guildID, snowflake userID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/bans/" + std::to_string(userID) , token, {}, "DELETE");
                }
            }
            namespace roles{
                inline json get(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles", token, {}, "GET");
                }
                inline json create(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles", token, {}, "POST");
                }
                inline json modifyBatch(snowflake guildID, std::vector<json> newParams, std::string token = data::lastToken()) {
                    json toSend;
                    for(json role : newParams){
                        toSend.push_back(role);
                    }
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles", token, toSend, "PATCH");
                }
                inline json modify(snowflake guildID, snowflake roleID, json newParams, std::string token = data::lastToken()) {
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles/" + std::to_string(roleID), token, newParams, "PATCH");
                }
                inline json remove(snowflake guildID, snowflake roleID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/role/" + std::to_string(roleID) , token, {}, "DELETE");
                }
            }
            namespace prune{
                inline json getCount(snowflake guildID, int days, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/prune";

                    json toSend;
                    toSend["days"] = days;

                    return DiscordAPI::call(callURL, token, toSend, "GET");
                }
                inline json begin(snowflake guildID, int days, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/prune";

                    json toSend;
                    toSend["days"] = days;

                    return DiscordAPI::call(callURL, token, toSend, "POST");
                }
            }
            inline json getVoiceRegions(snowflake guildID, std::string token = data::lastToken()){
                return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/regions", token, {}, "GET");
            }
            inline json getInvites(snowflake guildID, std::string token = data::lastToken()){
                return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/invites", token, {}, "GET");
            }
            namespace integrations {
                inline json get(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/integrations", token, {}, "GET");
                }
                inline json create(snowflake guildID, std::string type, snowflake userID, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/integrations";

                    json toSend;
                    toSend["type"] = type;
                    toSend["id"] = userID;

                    return DiscordAPI::call(callURL, token, toSend, "POST");
                }
                inline json modify(snowflake guildID, snowflake integrationID, int expire_behavior, int expire_grace_period, bool enable_emoticons, std::string token = data::lastToken()){
                    std::string callURL = "/guilds/" + std::to_string(guildID) + "/integrations/" + std::to_string(integrationID);

                    json toSend;
                    toSend["expire_behavior"] = expire_behavior;
                    toSend["expire_grace_period"] = expire_grace_period;
                    toSend["enable_emoticons"] = enable_emoticons;

                    return DiscordAPI::call(callURL, token, toSend, "POST");
                }
                inline json remove(snowflake guildID, snowflake integrationID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/integrations/" + std::to_string(integrationID), token, {}, "DELETE");
                }
                inline json sync(snowflake guildID, snowflake integrationID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/integrations/" + std::to_string(integrationID) + "/sync", token, {}, "POST");
                }
            }
            namespace embed {
                inline json get(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/embed", token, {}, "GET");
                }
                inline json modify(snowflake guildID, json newParams, std::string token = data::lastToken()) {
                    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/embed", token, newParams, "PATCH");
                }
            }
        }

        /* USING THIS MAY RESULT IN A BAN
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
        */

        namespace users{
            inline json queryUsers(int limit = 25, std::string username = "", std::string token = data::lastToken()){
                std::string callURL = "/users";

                json toSend;
                if(!username.empty()) {
                    toSend["q"] = username;
                }
                toSend["limit"] = limit;

                return DiscordAPI::call(callURL, token, toSend, "POST");
            }
            inline json getUser(snowflake userID, std::string token = data::lastToken()){
                return DiscordAPI::call("/users/" + std::to_string(userID), token, {}, "GET");
            }
            namespace self{
                inline json get(std::string token = data::lastToken()){
                    return DiscordAPI::call("/users/@me", token, {}, "GET");
                }
                inline json modify(std::string username, std::string token = data::lastToken()){
                    //TODO Handle avatar data
                    return DiscordAPI::call("/users/@me", token, {{"username", username}}, "POST");
                }
                inline json getGuilds(std::string token = data::lastToken()){
                    return DiscordAPI::call("/users/@me/guilds", token, {}, "GET");
                }
                inline json leaveGuild(snowflake guildID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/users/@me" + std::to_string(guildID), token, {}, "DELETE");
                }
                inline json getDMs(std::string token = data::lastToken()){
                    return DiscordAPI::call("/users/@me/channels", token, {}, "GET");
                }
                inline json createDM(snowflake recipientID, std::string token = data::lastToken()){
                    return DiscordAPI::call("/users/@me/channels", token, {{"recipient_id", recipientID}}, "POST");
                }
                inline json getConnections(std::string token = data::lastToken()){
                    return DiscordAPI::call("/users/@me/connections", token, {}, "GET");
                }
            }
        };

        namespace invites {
            inline json get(snowflake inviteID, std::string token = data::lastToken()){
                return DiscordAPI::call("/invites/" + std::to_string(inviteID), token, {}, "GET");
            }
            inline json remove(snowflake inviteID, std::string token = data::lastToken()){
                return DiscordAPI::call("/invites/" + std::to_string(inviteID), token, {}, "DELETE");
            }
            inline json accept(snowflake inviteID, std::string token = data::lastToken()){
                return DiscordAPI::call("/invites/" + std::to_string(inviteID), token, {}, "POST");
            }
        }

        namespace voice {
            inline json listVoiceRegions(std::string token = data::lastToken()) {
                return DiscordAPI::call("/voice/regions", token, {}, "GET");
            }
        }
    };

    class Client {
        using client = websocketpp::client<websocketpp::config::asio_tls_client>;
        using message_ptr = websocketpp::config::asio_client::message_type::ptr;
    public:
        Client(asio::io_service& asio_ios, const std::string& token, std::map<std::string, std::function<void(json)>> eventResponses)//std::map <std::string, std::string> soft_responses = {})
                : asio_ios_(asio_ios)
                , token_(token)
                , keepalive_timer_(asio_ios)
                , eventResponses_(eventResponses)
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
            std::string uri = fetchGateway(token);
            std::cout << "Connecting to gateway at " << uri << "\n";
            connection_ = client_.get_connection(uri, ec);
            if (ec) {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                //TODO TBD: throw something
            } else {
                // Note that connect here only requests a connection. No network messages are
                // exchanged until the event loop starts running in the next line.
                client_.connect(connection_);
            }
        }
        ~Client() =default;
        Client(const Client&) =delete;
        Client& operator=(const Client&) =delete;
        std::map<std::string, std::function<void(json)>> eventResponses_;

    private:
        void on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
            json jmessage = json::parse(msg->get_payload());
            if(jmessage["op"].get<int>() == 0){ //Dispatch
                std::map<std::string, std::function<void(json)>>::iterator it = eventResponses_.find(jmessage["t"]);
                if(it != eventResponses_.end()){
                    asio_ios_.post(std::bind(it->second, jmessage));
                } else {
                    std::cout << "There is no function for the event " << jmessage["t"] << ".\n";
                }
                if(jmessage["t"] == "READY") {
                    uint32_t ms = jmessage["d"]["heartbeat_interval"];
                    ms *= .9;
                    keepalive(ms);
                }
            } else if(jmessage["op"].get<int>() == 1){ //Heartbeat (This isn't implemented yet, still using periodic heartbeats for now.)
                //client_.send(hdl, jmessage.dump(), websocketpp::frame::opcode::text);
            } else { //Wat
                std::cout << "Unexpected opcode received:\n\n" << jmessage.dump(4) << "\n\n\n";
            }
        }

        void keepalive(uint32_t ms){
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            json message = {
                    {"op", 1},
                    {"d", millis}
            };
            std::cout << "Sending Heartbeat:\n";
            client_.send(connection_, message.dump(), websocketpp::frame::opcode::text);
            //reset timer
            keepalive_timer_.expires_from_now(std::chrono::milliseconds(ms));
            keepalive_timer_.async_wait(std::bind(&Client::keepalive, this, ms));
        }

        void on_open(websocketpp::connection_hdl hdl) {
            std::cout << "Connection established.\n";

            json connect = {
                    {"op", 2},
                    {"d", {
                                  {"token", token_},
                                  {"v", 4},
                                  {"properties", {
                                                         {"$os", "linux"},
                                                         {"$browser", "discordpp"},
                                                         {"$device", "discordpp"},
                                                         {"$referrer",""}, {"$referring_domain",""}
                                                 }
                                  },
                                  {"compress", false},
                                  {"large_threshold", 250}
                          }
                    }
            };
            std::cout << "Client Handshake:\n" << connect.dump(1) << "\n";
            client_.send(hdl, connect.dump(), websocketpp::frame::opcode::text);
        }
        std::string fetchGateway(std::string token = data::lastToken()){
            return DiscordAPI::call("/gateway", token).at("url");
        }
        //////////////////////////////
        asio::io_service& asio_ios_;
        std::string token_;
        client client_;
        websocketpp::uri_ptr uri_ptr_;
        client::connection_ptr connection_;
        asio::steady_timer keepalive_timer_;
    };
}

#endif //DISCORDPP_DISCORDPP_HH
