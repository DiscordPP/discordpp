//
// Created by aidan on 11/7/15.
//

#ifndef DISCORDPP_DISCORDPP_HH
#define DISCORDPP_DISCORDPP_HH

#ifdef _MSC_VER
	#ifdef DLL_EXPORT
		#define DLLAPI __declspec(dllexport)
	#else
		#define DLLAPI __declspec(dllimport)
	#endif
#else
	#define DLLAPI
#endif

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
        json DLLAPI call(std::string targetURL, std::string token, json attachJSON = {}, std::string requestType = "");

        namespace channels{
            json DLLAPI get(snowflake channelID, std::string token = data::lastToken());
            json DLLAPI modify(snowflake channelID, json newParams, std::string token = data::lastToken());
            //Can't use delete as a function, I'm guessing because of C++
            json DLLAPI close(snowflake channelID, std::string token = data::lastToken());

            namespace messages{
                json DLLAPI get(snowflake channelID, int limit = 50, std::string token = data::lastToken());
                json DLLAPI getAround(snowflake channelID, snowflake target = 0, int limit = 50, std::string token = data::lastToken());
                json DLLAPI getBefore(snowflake channelID, snowflake target = 0, int limit = 50, std::string token = data::lastToken());
                json DLLAPI getAfter(snowflake channelID, snowflake target = 0, int limit = 50, std::string token = data::lastToken());
                json DLLAPI create(snowflake channelID, std::string message, std::string nonce = "", bool isTTS = false, std::string token = data::lastToken());
                json DLLAPI edit(snowflake channelID, snowflake messageID, std::string message, std::string token = data::lastToken());
                json DLLAPI remove(snowflake channelID, snowflake messageID, std::string token = data::lastToken());
                json DLLAPI acknowledge(snowflake channelID, snowflake messageID, std::string token = data::lastToken());
                //I'm not sure hw to handle the permissions. https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
                namespace invites{
                    json DLLAPI get(snowflake channelID, std::string token = data::lastToken());
                    json DLLAPI create(snowflake channelID, int max_age = 86400, int max_uses = 0, bool temporary = false, bool unique = false, std::string token = data::lastToken());
                }
                json DLLAPI typing(snowflake channelID, std::string token = data::lastToken());
            }
        };

        namespace guilds{
            json DLLAPI create(std::string name, std::string region, std::string icon, std::string token = data::lastToken());
            json DLLAPI get(snowflake guildID, std::string token = data::lastToken());
            json DLLAPI modify(snowflake guildID, json newParams, std::string token = data::lastToken());
            namespace channels{
                json DLLAPI createText(snowflake guildID, std::string name, std::string token = data::lastToken());
                json DLLAPI createVoice(snowflake guildID, std::string name, int bitrate = -1, int user_limit = -1, std::string token = data::lastToken());
                json DLLAPI get(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI modify(snowflake guildID, json newParams, std::string token = data::lastToken());
            }
            namespace members{
                json DLLAPI getInfo(snowflake guildID, snowflake userID, std::string token = data::lastToken());
                json DLLAPI getList(snowflake guildID, int limit = 1, int offset = 0, std::string token = data::lastToken());
                json DLLAPI modify(snowflake guildID, snowflake userID, json newParams, std::string token = data::lastToken());
                json DLLAPI remove(snowflake guildID, snowflake userID, std::string token = data::lastToken());
            }
            namespace bans{
                json DLLAPI get(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI create(snowflake guildID, snowflake userID, int deleteMessageDays = 1, std::string token = data::lastToken());
                json DLLAPI remove(snowflake guildID, snowflake userID, std::string token = data::lastToken());
            }
            namespace roles{
                json DLLAPI get(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI create(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI modifyBatch(snowflake guildID, std::vector<json> newParams, std::string token = data::lastToken());
                json DLLAPI modify(snowflake guildID, snowflake roleID, json newParams, std::string token = data::lastToken());
                json DLLAPI remove(snowflake guildID, snowflake roleID, std::string token = data::lastToken());
            }
            namespace prune{
                json DLLAPI getCount(snowflake guildID, int days, std::string token = data::lastToken());
                json DLLAPI begin(snowflake guildID, int days, std::string token = data::lastToken());
            }
            json DLLAPI getVoiceRegions(snowflake guildID, std::string token = data::lastToken());
            json DLLAPI getInvites(snowflake guildID, std::string token = data::lastToken());
            namespace integrations {
                json DLLAPI get(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI create(snowflake guildID, std::string type, snowflake userID, std::string token = data::lastToken());
                json DLLAPI modify(snowflake guildID, snowflake integrationID, int expire_behavior, int expire_grace_period, bool enable_emoticons, std::string token = data::lastToken());
                json DLLAPI remove(snowflake guildID, snowflake integrationID, std::string token = data::lastToken());
                json DLLAPI sync(snowflake guildID, snowflake integrationID, std::string token = data::lastToken());
            }
            namespace embed {
                json DLLAPI get(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI modify(snowflake guildID, json newParams, std::string token = data::lastToken());
            }
        }

        /* USING THIS MAY RESULT IN A BAN
        namespace auth {
            std::string DLLAPI login(std::string email, std::string password);
            void DLLAPI logout(std::string token = data::lastToken());
        };
        */

        namespace users{
            json DLLAPI queryUsers(int limit = 25, std::string username = "", std::string token = data::lastToken());
            json DLLAPI get(snowflake userID, std::string token = data::lastToken());
            namespace self{
                json DLLAPI get(std::string token = data::lastToken());
                json DLLAPI modify(std::string username, std::string token = data::lastToken());
                json DLLAPI getGuilds(std::string token = data::lastToken());
                json DLLAPI leaveGuild(snowflake guildID, std::string token = data::lastToken());
                json DLLAPI getDMs(std::string token = data::lastToken());
                json DLLAPI createDM(snowflake recipientID, std::string token = data::lastToken());
                json DLLAPI getConnections(std::string token = data::lastToken());
            }
        };

        namespace invites {
            json DLLAPI get(snowflake inviteID, std::string token = data::lastToken());
            json DLLAPI remove(snowflake inviteID, std::string token = data::lastToken());
            json DLLAPI accept(snowflake inviteID, std::string token = data::lastToken());
        }

        namespace voice {
            json DLLAPI listVoiceRegions(std::string token = data::lastToken());
        }
    };

    class DLLAPI Client {
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
        uint32_t sequence_number_;
    };
}

#endif //DISCORDPP_DISCORDPP_HH
