#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "discordpp.hh"
#include "resource.hh"

using json = nlohmann::json;
namespace asio = boost::asio;
using boost::system::error_code;


std::string login(std::string authFilePath);
std::map<std::string, std::string> loadSoftCommands(std::string softFilePath);
std::string readTokenFile(std::string tokenFilePath);

int main() {
    std::cout << "Starting bot...\n\n";
    //Login and load soft commands from their respective files.
    std::string token;
    if(boost::filesystem::exists("token.dat")){
        token = readTokenFile("token.dat");
    /* USING THIS MAY RESULT IN A BAN
    } else if(boost::filesystem::exists("login.dat")){
        token = login("login.dat");
    */
    } else {
        std::cerr << "CRITICAL: There is no valid way for Discord++ to obtain a token! Copy the example login.dat or token.dat to make one.\n";
        exit(1);
    }
    std::map<std::string, std::string> soft_commands = loadSoftCommands("softcommands.dat");

    //Print user info.
    std::cout << "Getting user info...\n";
    json myInfo = discordpp::DiscordAPI::users::self::get(token);
    std::cout << "I am " << myInfo.at("username").get<std::string>() << ", my ID number is " << myInfo.at("id").get<std::string>() << ".\n\n";

    //Print out all soft commands
    std::cout << "soft commands\n";
    for(auto elem : soft_commands)
    {
        std::cout << elem.first << "\n" << elem.second << "\n\n";
    }

    //Create an asio asychronous loop for websocketpp.
    asio::io_service asio_ios;

    // Construct a signal set registered for process termination.
    boost::asio::signal_set signals(asio_ios, SIGINT, SIGTERM);

    // Start an asynchronous wait for one of the signals to occur.
    signals.async_wait([&asio_ios](const error_code& error, int signal_number){
        if (!error) {
            std::cerr << "User terminated program.\n";
            std::cerr << "Logging out...\n";
            //discordpp::DiscordAPI::auth::logout();
            std::cerr << "Logged out.\n";
            asio_ios.stop();
        }
    });

    std::map<std::string, std::function<void(json)>> eventResponses;
    eventResponses["READY"] = [](json jmessage) {
        std::cout << "Recieved READY payload.\n";
        std::cout << jmessage.dump(4) << "\n\n\n";
        resource::discord::gatewayVersion() = jmessage["d"]["v"];
        resource::discord::me() = jmessage["d"]["user"];
        resource::discord::privateChannels() = jmessage["d"]["private_channels"];
        resource::discord::guilds() = jmessage["d"]["guilds"];
        resource::discord::readState() = jmessage["d"]["read_state"];
        resource::discord::sessionID() = jmessage["d"]["session_id"];
    };
    eventResponses["MESSAGE_CREATE"] = [soft_commands](json jmessage){
        for(auto& val : jmessage["d"]["mentions"]){
            if(val["id"] == resource::discord::me()["id"] && jmessage["d"]["content"].get<std::string>().length() > 23){
                std::string message = jmessage["d"]["content"].get<std::string>();
                message = message.substr(resource::discord::me()["id"].get<std::string>().length() + 4, message.length());
                auto it = soft_commands.find(message);
                std::string sid = jmessage["d"]["channel_id"];
                auto id = boost::lexical_cast<discordpp::snowflake>(sid);
                if(it == soft_commands.end()){
                    discordpp::DiscordAPI::channels::messages::create(id, "`" + message + "` is not a command.");
                } else {
                    discordpp::DiscordAPI::channels::messages::create(id, it->second);
                }
            }
        }
    };
    eventResponses["PRESENCE_UPDATE"] = [](json jmessage){
        //ignore
    };
    eventResponses["TYPING_START"] = [](json jmessage){
        //ignore
    };

    //Pass the loop to the a websocket client, which adds its functions to the loop.
    discordpp::Client discordClient(asio_ios, token, eventResponses);

    //run the loop.
    asio_ios.run();

    return 0;
}

/* USING THIS MAY RESULT IN A BAN
std::string login(std::string authFilePath){
    std::cout << "Logging in...\n";

    std::ifstream authFile;
    authFile.open(authFilePath);

    std::string username;
    std::string password;

    if (authFile.is_open()) {
        std::getline(authFile, username);
        std::getline(authFile, password);
    } else {
        std::cerr << "CRITICAL: There is no such file as " + authFilePath + "! Copy the example login.dat to make one.\n";
        exit(1);
    }
    authFile.close();
    std::string token = discordpp::DiscordAPI::auth::login(username, password);
    //std::cout << "Retrieved token: " << token << "\n\n";
    std::cout << "Retrieved token.\n\n";

    return token;
}
*/

std::map<std::string, std::string> loadSoftCommands(std::string softFilePath){
    std::ifstream softfile;
    softfile.open(softFilePath);

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
        std::cerr << "Warning: There is no such files as " + softFilePath + "! Copy the example softcommands.dat to make one.\n";
    }
    softfile.close();

    return soft_commands;
}

std::string readTokenFile(std::string tokenFilePath){
    std::ifstream tokenFile;
    tokenFile.open(tokenFilePath);

    std::string token;

    if (tokenFile.is_open()) {
        std::getline(tokenFile, token);
    } else {
        std::cerr << "CRITICAL: There is no such file as " + tokenFilePath + "! Copy the example login.dat to make one.\n";
        exit(1);
    }
    tokenFile.close();
    std::cout << "Retrieved token.\n\n";

    return token;
}