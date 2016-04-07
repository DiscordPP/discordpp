#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/filesystem.hpp>

#include "discordpp.hh"

using json = nlohmann::json;
namespace asio = boost::asio;
using boost::system::error_code;


std::string login();
std::map<std::string, std::string> loadSoftCommands(std::string softFilePath);

int main() {
    //Login and load sodt commands from their respective files.
    std::string token = login("login.dat");
    std::map<std::string, std::string> soft_commands = loadSoftCommands("softcommands.dat");

    //Print user info.
    std::cout << "Getting user info...\n";
    json myInfo = discordpp::users::fetchInfo("@me");
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
            discordpp::auth::logout();
            std::cerr << "Logged out.\n";
            asio_ios.stop();
        }
    });

    //Pass the loop to the a websocket client, which adds its functions to the loop.
    discordpp::Client discordClient(asio_ios, token, soft_commands);

    //run the loop.
    asio_ios.run();

    return 0;
}

std::string login(std::string authFilePath){
    std::cout << "Starting bot...\n\n";

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
    std::string token = discordpp::auth::login(username, password);
    //std::cout << "Retrieved token: " << token << "\n\n";
    std::cout << "Retrieved token.\n\n";

    return token;
}

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