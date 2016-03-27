#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#include "discordpp.hh"
//#include ""
//#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

bool vectorContains (std::vector<std::string> toCheck, std::string value);

using json = nlohmann::json;
namespace asio = boost::asio;
using boost::system::error_code;


class DiscordClient {
    using client = websocketpp::client<websocketpp::config::asio_tls_client>;
    using message_ptr = websocketpp::config::asio_client::message_type::ptr;
public:
    DiscordClient(asio::io_service& asio_ios, const std::string& token)
            : asio_ios_(asio_ios)
            , token_(token)
            , keepalive_timer_(asio_ios)
    {
        client_.set_access_channels(websocketpp::log::alevel::all);
        client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

        client_.set_tls_init_handler([this](websocketpp::connection_hdl){
            return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
        });

        client_.init_asio(&asio_ios);

        client_.set_message_handler(std::bind(&DiscordClient::on_message, this,
                                              std::placeholders::_1, std::placeholders::_2));
        client_.set_open_handler(std::bind(&DiscordClient::on_open, this,
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
    ~DiscordClient() =default;
    DiscordClient(const DiscordClient&) =delete;
    DiscordClient& operator=(const DiscordClient&) =delete;

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
                if(val["id"] == ready_["d"]["user"]["id"]){
                    std::string message = jmessage["d"]["content"].get<std::string>();
                    std::string target = ready_["d"]["user"]["username"].get<std::string>();
                    std::string targetID = jmessage["d"]["author"]["id"].get<std::string>();

                    message =message.substr(ready_["d"]["user"]["id"].get<std::string>().length() + 4, message.length()); // "<@" + targetID + "> " +

                    std::cout << "Sending message " << jmessage["d"]["content"] << " to user " << jmessage["d"]["author"]["username"] << "\n";

                    discordpp::channels::messages::sendMessage(jmessage["d"]["channel_id"],  message);//, {targetID}
                }
            }
        } else if(jmessage["t"] == "MESSAGE_UPDATE"){

        } else if(jmessage["t"] == "TYPING_START"){

        } else if(jmessage["t"] == "PRESENCE_UPDATE"){

        } else if(jmessage["t"] == "TYPING_START"){

        } else if(jmessage["t"] == "VOICE_STATE_UPDATE"){

        }// else {
            std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << jmessage.dump(3)
            << "\n";
        //}
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
        keepalive_timer_.async_wait(std::bind(&DiscordClient::keepalive, this, ms));
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
                              {"large_threshold", 100}
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
};

int main() {
    std::cout << "Starting bot...\n\n";

    std::cout << "Logging in...\n";

    std::ifstream authfile;
    authfile.open("login.dat");

    std::string username;
    std::string password;

    if (authfile.is_open()) {
        authfile >> username;
        authfile >> password;
    } else {
        std::cerr << "There is no login.dat file. Copy the example to make one.\n";
        exit(1);
    }
    authfile.close();
    std::string token = discordpp::auth::login(username, password);
    //std::cout << "Retrieved token: " << token << "\n\n";
    std::cout << "Retrieved token.\n\n";

    std::cout << "Getting user info...\n";
    json myInfo = discordpp::users::fetchInfo("@me");
    std::cout << "I am " << myInfo.at("username").get<std::string>() << ", my ID number is " << myInfo.at("id").get<std::string>() << ".\n\n";

    std::vector<json> myTextChannels;
    std::cout << "Getting guild and channel lists...\n";
    json myGuilds = discordpp::users::fetchGuilds();
    for (json guild : myGuilds) {
        std::cout << "Found the guild \"" << guild.at("name").get<std::string>() << "\". Its ID number is " << guild.at("id").get<std::string>() << ".\n";
        json guildChannels = discordpp::guilds::fetchChannels(guild.at("id").get<std::string>());
        for (json channel : guildChannels) {
            std::cout << "     Found channel #" << channel.at("name").get<std::string>() << ". ";
            std::cout << "Its ID number is " << channel.at("id").get<std::string>() << ". ";
            std::cout << "It is a " << channel.at("type").get<std::string>() << " channel.\n";
            if(channel.at("type").get<std::string>() == "text"){
                myTextChannels.push_back(channel);
            }
        }
    }
    std::cout << "\n\n";

    asio::io_service asio_ios;
    

    // Construct a signal set registered for process termination.
    boost::asio::signal_set signals(asio_ios, SIGINT, SIGTERM);

    // Start an asynchronous wait for one of the signals to occur.
    signals.async_wait([&asio_ios](const error_code& error, int signal_number){
        if (!error) {
            std::cerr << "User terminated program.\n";
            std::cerr << "Logging out...\n";
            discordpp::auth::logout();
            std::cerr << "Logged out.\n";
            asio_ios.stop();
        }
    });

    DiscordClient discordClient(asio_ios, token);

    asio_ios.run();

    //Old code using POST messages instead of WebSockets
    /*
    std::map<std::string, std::string> channelLastRead;
    std::cout << "Establishing most recent messages...\n";
    for(json channel : myTextChannels){
        std::string lastMessageID;
        try {
            lastMessageID = channel.at("last_message_id").get<std::string>();
            std::cout << "The last message in #" << channel.at("name").get<std::string>() << " was message " << lastMessageID << ".\n";
        } catch (std::domain_error) {
            lastMessageID = "0";
            std::cout << "#" << channel.at("name").get<std::string>() << " has no messages.\n";
        }
        channelLastRead[channel.at("id").get<std::string>()] = lastMessageID;
    }
    std::cout << "\n";
     */
    /*
    time_t startTime = std::time(nullptr);
    std::cout << "Starting loop, the time is " << startTime << ".\n";
    for (int i = 0; startTime + 60 > std::time(nullptr); i++) {
        for(auto item: channelLastRead){
            //std::cout << "Checking #" << item.first.at("name").get<std::string>() << " for new messages.\n";
            try {
                json message;
                message = discordpp::channels::messages::nextMessage(item.first, item.second)[0];
                channelLastRead[item.first] = message.at("id").get<std::string>();
                json channel = discordpp::channels::fetchInfo(item.first);
                json guild = discordpp::guilds::fetchInfo(channel.at("guild_id").get<std::string>());
                std::cout << guild.at("name").get<std::string>() << "#" << discordpp::channels::fetchInfo(item.first).at("name").get<std::string>() << " " << message.at("author").at("username").get<std::string>() << ": " << message.at("content").get<std::string>() << "\n";
                std::vector<std::string> messageMentions = message.at("mentions").get<std::vector<std::string>>();

                bool mentionsMe = true;// vectorContains(messageMentions, myInfo.at("id").get<std::string>());
                bool isTesting = channel.at("name").get<std::string>() == "testing";
                bool rightChannel = guild.at("name").get<std::string>() == "Brotherhood of Seel" || guild.at("name").get<std::string>() == "Discord API" || guild.at("name").get<std::string>() == "Discord Bots";
                if (mentionsMe && isTesting  && rightChannel) {
                    discordpp::channels::messages::sendMessage(channel.at("id").get<std::string>(), message.at("content"), {message.at("author").at("id").get<std::string>()});
                }
            } catch (std::domain_error & e){
                std::cout << e.what() << "\n";
            }
        }
    }
    std::cout << "Loop done.\n";
    */

    return 0;
}