#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "Bot.h"

using json = nlohmann::json;
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

    //Print user info. (depreaceated?)
    //std::cout << "Getting user info...\n";
    //json myInfo = discordpp::DiscordAPI::users::self::get(token);
    //std::cout << "I am " << myInfo.at("username").get<std::string>() << ", my ID number is " << myInfo.at("id").get<std::string>() << ".\n\n";

    //Print out all soft commands
    std::cout << "soft commands\n";
    for(auto elem : soft_commands)
    {
        std::cout << elem.first << "\n" << elem.second << "\n\n";
    }

    discordpp::Bot* bot = new discordpp::Bot(token);

    bot->addResponse("MESSAGE_CREATE", [soft_commands](discordpp::Bot *bot, json jmessage) {
        for (auto &val : jmessage["d"]["mentions"]) {
            if (val["id"] == bot->me["id"] && jmessage["d"]["content"].get<std::string>().length() > 23) {
                std::string message = jmessage["d"]["content"].get<std::string>();
                message = message.substr(bot->me["id"].get<std::string>().length() + 4, message.length());
                auto it = soft_commands.find(message);
                std::string sid = jmessage["d"]["channel_id"];
                auto id = boost::lexical_cast<discordpp::snowflake>(sid);
                if (it == soft_commands.end()) {
                    discordpp::DiscordAPI::channels::messages::create(id, "`" + message + "` is not a command.");
                } else {
                    discordpp::DiscordAPI::channels::messages::create(id, it->second);
                }
            }
        }
    });
    bot->addResponse("PRESENCE_UPDATE", [](discordpp::Bot *bot, json jmessage) {
        //ignore
    });
    bot->addResponse("TYPING_START", [](discordpp::Bot *bot, json jmessage) {
        //ignore
    });

    bot->start();

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