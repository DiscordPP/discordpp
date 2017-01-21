//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#include <vector>
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
        json call(std::string targetURL, std::string token, json attachJSON = {}, std::string requestType = "");

        namespace channels{
            json get(snowflake channelID, std::string token = data::lastToken());
            json modify(snowflake channelID, json newParams, std::string token = data::lastToken());
            //Can't use delete as a function, I'm guessing because of C++
            json close(snowflake channelID, std::string token = data::lastToken());

            namespace messages{
                json get(snowflake channelID, int limit = 50, std::string token = data::lastToken());
                json getAround(snowflake channelID, snowflake target = 0, int limit = 50, std::string token = data::lastToken());
                json getBefore(snowflake channelID, snowflake target = 0, int limit = 50, std::string token = data::lastToken());
                json getAfter(snowflake channelID, snowflake target = 0, int limit = 50, std::string token = data::lastToken());
                json create(snowflake channelID, std::string message, std::string nonce = "", bool isTTS = false, std::string token = data::lastToken());
                json edit(snowflake channelID, snowflake messageID, std::string message, std::string token = data::lastToken());
                json remove(snowflake channelID, snowflake messageID, std::string token = data::lastToken());
                json acknowledge(snowflake channelID, snowflake messageID, std::string token = data::lastToken());
                //I'm not sure hw to handle the permissions. https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
                namespace invites{
                    json get(snowflake channelID, std::string token = data::lastToken());
                    json create(snowflake channelID, int max_age = 86400, int max_uses = 0, bool temporary = false, bool unique = false, std::string token = data::lastToken());
                }
                json typing(snowflake channelID, std::string token = data::lastToken());
            }
        };

        namespace guilds{
            json create(std::string name, std::string region, std::string icon, std::string token = data::lastToken());
            json get(snowflake guildID, std::string token = data::lastToken());
            json modify(snowflake guildID, json newParams, std::string token = data::lastToken());
            namespace channels{
                json createText(snowflake guildID, std::string name, std::string token = data::lastToken());
                json createVoice(snowflake guildID, std::string name, int bitrate = -1, int user_limit = -1, std::string token = data::lastToken());
                json get(snowflake guildID, std::string token = data::lastToken());
                json modify(snowflake guildID, json newParams, std::string token = data::lastToken());
            }
            namespace members{
                json getInfo(snowflake guildID, snowflake userID, std::string token = data::lastToken());
                json getList(snowflake guildID, int limit = 1, int offset = 0, std::string token = data::lastToken());
                json modify(snowflake guildID, snowflake userID, json newParams, std::string token = data::lastToken());
                json remove(snowflake guildID, snowflake userID, std::string token = data::lastToken());
            }
            namespace bans{
                json get(snowflake guildID, std::string token = data::lastToken());
                json create(snowflake guildID, snowflake userID, int deleteMessageDays = 1, std::string token = data::lastToken());
                json remove(snowflake guildID, snowflake userID, std::string token = data::lastToken());
            }
            namespace roles{
                json get(snowflake guildID, std::string token = data::lastToken());
                json create(snowflake guildID, std::string token = data::lastToken());
                json modifyBatch(snowflake guildID, std::vector<json> newParams, std::string token = data::lastToken());
                json modify(snowflake guildID, snowflake roleID, json newParams, std::string token = data::lastToken());
                json remove(snowflake guildID, snowflake roleID, std::string token = data::lastToken());
            }
            namespace prune{
                json getCount(snowflake guildID, int days, std::string token = data::lastToken());
                json begin(snowflake guildID, int days, std::string token = data::lastToken());
            }
            json getVoiceRegions(snowflake guildID, std::string token = data::lastToken());
            json getInvites(snowflake guildID, std::string token = data::lastToken());
            namespace integrations {
                json get(snowflake guildID, std::string token = data::lastToken());
                json create(snowflake guildID, std::string type, snowflake userID, std::string token = data::lastToken());
                json modify(snowflake guildID, snowflake integrationID, int expire_behavior, int expire_grace_period, bool enable_emoticons, std::string token = data::lastToken());
                json remove(snowflake guildID, snowflake integrationID, std::string token = data::lastToken());
                json sync(snowflake guildID, snowflake integrationID, std::string token = data::lastToken());
            }
            namespace embed {
                json get(snowflake guildID, std::string token = data::lastToken());
                json modify(snowflake guildID, json newParams, std::string token = data::lastToken());
            }
        }

        /* USING THIS MAY RESULT IN A BAN
        namespace auth {
            std::string login(std::string email, std::string password);
            void logout(std::string token = data::lastToken());
        };
        */

        namespace users{
            json queryUsers(int limit = 25, std::string username = "", std::string token = data::lastToken());
            json get(snowflake userID, std::string token = data::lastToken());
            namespace self{
                json get(std::string token = data::lastToken());
                json modify(std::string username, std::string token = data::lastToken());
                json getGuilds(std::string token = data::lastToken());
                json leaveGuild(snowflake guildID, std::string token = data::lastToken());
                json getDMs(std::string token = data::lastToken());
                json createDM(snowflake recipientID, std::string token = data::lastToken());
                json getConnections(std::string token = data::lastToken());
            }
        };

        namespace invites {
            json get(snowflake inviteID, std::string token = data::lastToken());
            json remove(snowflake inviteID, std::string token = data::lastToken());
            json accept(snowflake inviteID, std::string token = data::lastToken());
        }

        namespace voice {
            json listVoiceRegions(std::string token = data::lastToken());
        }
    };

    class Client {
        using client = websocketpp::client<websocketpp::config::asio_tls_client>;
        using message_ptr = websocketpp::config::asio_client::message_type::ptr;
    public:
        Client(asio::io_service& asio_ios, const std::string& token, std::map<std::string, std::function<void(json)>> eventResponses);//std::map <std::string, std::string> soft_responses = {})
        ~Client() =default;
        Client(const Client&) =delete;
        Client& operator=(const Client&) =delete;
        std::map<std::string, std::function<void(json)>> eventResponses_;

    private:
        void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

        void keepalive(uint32_t ms);

        void on_open(websocketpp::connection_hdl hdl);
        std::string fetchGateway(std::string token = data::lastToken());
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
