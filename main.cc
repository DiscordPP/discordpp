#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/filesystem.hpp>

#include "discordpp.hh"

bool vectorContains (std::vector<std::string> toCheck, std::string value);

using json = nlohmann::json;
namespace asio = boost::asio;
using boost::system::error_code;


int main() {
    std::cout << "Starting bot...\n\n";

    std::cout << "Logging in...\n";

    std::ifstream authfile;
    authfile.open("login.dat");

    std::string username;
    std::string password;

    if (authfile.is_open()) {
        std::getline(authfile, username);
        std::getline(authfile, password);
    } else {
        std::cerr << "CRITICAL: There is no login.dat file. Copy the example to make one.\n";
        exit(1);
    }
    authfile.close();
    std::string token = discordpp::auth::login(username, password);
    //std::cout << "Retrieved token: " << token << "\n\n";
    std::cout << "Retrieved token.\n\n";

    std::cout << "Getting user info...\n";
    json myInfo = discordpp::users::fetchInfo("@me");
    std::cout << "I am " << myInfo.at("username").get<std::string>() << ", my ID number is " << myInfo.at("id").get<std::string>() << ".\n\n";

    std::ifstream softfile;
    softfile.open("softcommands.dat");

    std::map<std::string, std::string> soft_commands;

    if (softfile.is_open()) {
        while(!softfile.eof()){
            std::string command;
            std::getline(softfile, command);
            std::string blurb; // Currently unused.
            std::getline(softfile, blurb);
            std::string output;
            std::getline(softfile, output);
            if(output == "{"){
                output = "";
                std::string nextline;
                std::getline(softfile, nextline);
                while(nextline != "}"){
                    output += nextline;
                    std::getline(softfile, nextline);
                    if(nextline != "}"){
                        output += "\n";
                    }
                }
            }
            soft_commands[command] = output;
        }
    } else {
        std::cerr << "Warning: There is no softcommands.dat file. Copy the example to make one.\n";
    }
    softfile.close();

    std::cout << "soft commands\n";

    for(auto elem : soft_commands)
    {
        std::cout << elem.first << "\n" << elem.second << "\n\n";
    }

    //This is unneeded, as it is retuned by READY.
    /*
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
     */

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

    discordpp::Client discordClient(asio_ios, token, soft_commands);

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