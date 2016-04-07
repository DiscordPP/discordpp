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

    return 0;
}