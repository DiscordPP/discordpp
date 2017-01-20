//
// Created by aidan on 1/2/17.
//
#include "discordpp/discordpp.hh"

#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#define STDC_HEADERS 1

#include "lib/nlohmannjson/src/json.hpp"

using namespace discordpp;

using json = nlohmann::json;
namespace asio = boost::asio;
using boost::system::error_code;
using snowflake = uint64_t;


json DiscordAPI::call(std::string targetURL, std::string token, json attachJSON, std::string requestType){
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
            if(message == "You are being rate limited."){
                std::this_thread::sleep_for(std::chrono::milliseconds(returned["retry_after"].get<int>()));
            }else if(message != "") {
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

json DiscordAPI::channels::get(snowflake channelID, std::string token){
    return DiscordAPI::call("/channels/" + std::to_string(channelID), token, "GET");
}
json DiscordAPI::channels::modify(snowflake channelID, json newParams, std::string token){
    return DiscordAPI::call("/channels/" + std::to_string(channelID), token, newParams, "PATCH");
}
json DiscordAPI::channels::close(snowflake channelID, std::string token){
    return DiscordAPI::call("/channels/" + std::to_string(channelID), token, "DELETE");
}

json DiscordAPI::channels::messages::get(snowflake channelID, snowflake before, snowflake after, int limit, std::string token){
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
json DiscordAPI::channels::messages::create(snowflake channelID, std::string message, std::string nonce, bool isTTS, std::string token) {
    std::string callURL = "/channels/" + std::to_string(channelID) + "/messages";

    json toSend;
    toSend["content"] = message;
    if(nonce != ""){
        toSend["nonce"] = nonce;
    }
    toSend["tts"] = isTTS;

    return DiscordAPI::call(callURL, token, toSend);
}
json DiscordAPI::channels::messages::edit(snowflake channelID, snowflake messageID, std::string message, std::string token) {
    std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID);

    json toSend;
    toSend["content"] = message;

    return DiscordAPI::call(callURL, token, toSend, "PATCH");
}
json DiscordAPI::channels::messages::remove(snowflake channelID, snowflake messageID, std::string token) {
    std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID);
    return DiscordAPI::call(callURL, token, "DELETE");
}
json DiscordAPI::channels::messages::acknowledge(snowflake channelID, snowflake messageID, std::string token){
    std::string callURL = "/channels/" + std::to_string(channelID) + "/messages/" + std::to_string(messageID) + "/ack";
    return DiscordAPI::call(callURL, token);
}

json DiscordAPI::channels::messages::invites::get(snowflake channelID, std::string token){
    return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/invites", token, "GET");
}
json DiscordAPI::channels::messages::invites::create(snowflake channelID, int max_age, int max_uses, bool temporary, bool xkcdpass, std::string token) {
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
json DiscordAPI::channels::messages::typing(snowflake channelID, std::string token){
    return DiscordAPI::call("/channels/" + std::to_string(channelID) + "/typing", token, "POST");
}

json DiscordAPI::guilds::create(std::string name, std::string region, std::string icon, std::string token){
    std::string callURL = "/guilds";

    json toSend;
    toSend["name"] = name;
    toSend["region"] = region;
    toSend["icon"] = icon;

    return DiscordAPI::call(callURL, token, toSend, "POST");
}
json DiscordAPI::guilds::get(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID), token, "GET");
}
json DiscordAPI::guilds::modify(snowflake guildID, json newParams, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID), token, newParams, "PATCH");
}
json DiscordAPI::guilds::channels::createText(snowflake guildID, std::string name, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/channels";

    json toSend;
    toSend["name"] = name;
    toSend["type"] = "text";

    return DiscordAPI::call(callURL, token, toSend, "POST");
}

json DiscordAPI::guilds::channels::createVoice(snowflake guildID, std::string name, int bitrate, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/channels";

    json toSend;
    toSend["name"] = name;
    toSend["type"] = "voice";
    toSend["bitrate"] = bitrate;

    return DiscordAPI::call(callURL, token, toSend, "POST");
}
json DiscordAPI::guilds::channels::get(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/channels", token, "GET");
}
json DiscordAPI::guilds::channels::modify(snowflake guildID, json newParams, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/channels", token, newParams, "PATCH");
}

json DiscordAPI::guilds::members::getInfo(snowflake guildID, snowflake userID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/members/" + std::to_string(userID) , token, "GET");
}
json DiscordAPI::guilds::members::getList(snowflake guildID, int limit, int offset, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/members";

    json toSend;
    toSend["limit"] = limit;
    toSend["offset"] = offset;

    return DiscordAPI::call(callURL, token, toSend, "GET");
}
json DiscordAPI::guilds::members::modify(snowflake guildID, snowflake userID, json newParams, std::string token) {
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/members/" + std::to_string(userID), token, newParams, "PATCH");
}
json DiscordAPI::guilds::members::remove(snowflake guildID, snowflake userID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/members/" + std::to_string(userID) , token, "DELETE");
}
json DiscordAPI::guilds::bans::get(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/bans", token, "GET");
}
json DiscordAPI::guilds::bans::create(snowflake guildID, snowflake userID, int deleteMessageDays, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/bans/" + std::to_string(userID);

    json toSend;
    toSend["delete-message-days"] = deleteMessageDays;

    return DiscordAPI::call(callURL, token, toSend, "PUT");
}
json DiscordAPI::guilds::bans::remove(snowflake guildID, snowflake userID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/bans/" + std::to_string(userID) , token, "DELETE");
}

json DiscordAPI::guilds::roles::get(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles", token, "GET");
}
json DiscordAPI::guilds::roles::create(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles", token, "POST");
}
json DiscordAPI::guilds::roles::modifyBatch(snowflake guildID, std::vector<json> newParams, std::string token) {
    json toSend;
    for(json role : newParams){
        toSend.push_back(role);
    }
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles", token, toSend, "PATCH");
}
json DiscordAPI::guilds::roles::modify(snowflake guildID, snowflake roleID, json newParams, std::string token) {
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/roles/" + std::to_string(roleID), token, newParams, "PATCH");
}
json DiscordAPI::guilds::roles::remove(snowflake guildID, snowflake roleID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/role/" + std::to_string(roleID) , token, "DELETE");
}

json DiscordAPI::guilds::prune::getCount(snowflake guildID, int days, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/prune";

    json toSend;
    toSend["days"] = days;

    return DiscordAPI::call(callURL, token, toSend, "GET");
}
json DiscordAPI::guilds::prune::begin(snowflake guildID, int days, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/prune";

    json toSend;
    toSend["days"] = days;

    return DiscordAPI::call(callURL, token, toSend, "POST");
}

json DiscordAPI::guilds::getVoiceRegions(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/regions", token, "GET");
}
json DiscordAPI::guilds::getInvites(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/invites", token, "GET");
}

json DiscordAPI::guilds::integrations::get(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/integrations", token, "GET");
}
json DiscordAPI::guilds::integrations::create(snowflake guildID, std::string type, snowflake userID, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/integrations";

    json toSend;
    toSend["type"] = type;
    toSend["id"] = userID;

    return DiscordAPI::call(callURL, token, toSend, "POST");
}
json DiscordAPI::guilds::integrations::modify(snowflake guildID, snowflake integrationID, int expire_behavior, int expire_grace_period, bool enable_emoticons, std::string token){
    std::string callURL = "/guilds/" + std::to_string(guildID) + "/integrations/" + std::to_string(integrationID);

    json toSend;
    toSend["expire_behavior"] = expire_behavior;
    toSend["expire_grace_period"] = expire_grace_period;
    toSend["enable_emoticons"] = enable_emoticons;

    return DiscordAPI::call(callURL, token, toSend, "POST");
}
json DiscordAPI::guilds::integrations::remove(snowflake guildID, snowflake integrationID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/integrations/" + std::to_string(integrationID), token, "DELETE");
}
json DiscordAPI::guilds::integrations::sync(snowflake guildID, snowflake integrationID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/integrations/" + std::to_string(integrationID) + "/sync", token, "POST");
}

json DiscordAPI::guilds::embed::get(snowflake guildID, std::string token){
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/embed", token, "GET");
}
json DiscordAPI::guilds::embed::modify(snowflake guildID, json newParams, std::string token) {
    return DiscordAPI::call("/guilds/" + std::to_string(guildID) + "/embed", token, newParams, "PATCH");
}

/* USING THIS MAY RESULT IN A BAN
std::string DiscordAPI::auth::login(std::string email, std::string password) {
    json loginInfo;
    loginInfo["email"]    = email;
    loginInfo["password"] = password;

    //std::cout << loginInfo.dump() << std::endl;

    json returned = DiscordAPI::call("/auth/login", "", loginInfo);

    //std::cout << returned.dump() << std::endl;
    std::string token = returned.at("token").get<std::string>();
    data::lastToken() = token;
    return token;
}
void DiscordAPI::auth::logout(std::string token) {
    DiscordAPI::call("/auth/logout", token);
}
*/

json DiscordAPI::users::queryUsers(int limit, std::string username, std::string token){
    std::string callURL = "/users";

    json toSend;
    if(!username.empty()) {
        toSend["q"] = username;
    }
    toSend["limit"] = limit;

    return DiscordAPI::call(callURL, token, toSend, "POST");
}
json DiscordAPI::users::getUser(snowflake userID, std::string token){
    return DiscordAPI::call("/users/" + std::to_string(userID), token, "GET");
}
json DiscordAPI::users::self::get(std::string token){
    return DiscordAPI::call("/users/@me", token, "GET");
}
json DiscordAPI::users::self::modify(std::string username, std::string token){
    //TODO Handle avatar data
    return DiscordAPI::call("/users/@me", token, {{"username", username}}, "POST");
}
json DiscordAPI::users::self::getGuilds(std::string token){
    return DiscordAPI::call("/users/@me/guilds", token, "GET");
}
json DiscordAPI::users::self::leaveGuild(snowflake guildID, std::string token){
    return DiscordAPI::call("/users/@me" + std::to_string(guildID), token, "DELETE");
}
json DiscordAPI::users::self::getDMs(std::string token){
    return DiscordAPI::call("/users/@me/channels", token, "GET");
}
json DiscordAPI::users::self::createDM(snowflake recipientID, std::string token){
    return DiscordAPI::call("/users/@me/channels", token, {{"recipient_id", recipientID}}, "POST");
}
json DiscordAPI::users::self::getConnections(std::string token){
    return DiscordAPI::call("/users/@me/connections", token, "GET");
}

json DiscordAPI::invites::get(snowflake inviteID, std::string token){
    return DiscordAPI::call("/invites/" + std::to_string(inviteID), token, "GET");
}
json DiscordAPI::invites::remove(snowflake inviteID, std::string token){
    return DiscordAPI::call("/invites/" + std::to_string(inviteID), token, "DELETE");
}
json DiscordAPI::invites::accept(snowflake inviteID, std::string token){
    return DiscordAPI::call("/invites/" + std::to_string(inviteID), token, "POST");
}

json DiscordAPI::voice::listVoiceRegions(std::string token) {
    return DiscordAPI::call("/voice/regions", token, "GET");
}

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;
Client::Client(asio::io_service& asio_ios, const std::string& token, std::map<std::string, std::function<void(json)>> eventResponses)//std::map <std::string, std::string> soft_responses = {})
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

void Client::on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
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

void Client::keepalive(uint32_t ms){
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

void Client::on_open(websocketpp::connection_hdl hdl){
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
std::string Client::fetchGateway(std::string token){
    return DiscordAPI::call("/gateway", token).at("url");
}