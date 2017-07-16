DiscordPP <sup>(it's cool)</sup>
===
A Discord API written in C++.

Links:
* [Discord Server](https://discord.gg/0usP6xmT4sQ4kIDh)
* [Discord API Implementation Trello Board](https://trello.com/b/XZVPL956/discord)

Requirements <sup>(I got these through APT)</sup>
---
* [Boost](http://www.boost.org/)\
Required by what are currently the only modules:
* [OpenSSL](https://www.openssl.org/)
* [Crypto++](https://www.cryptopp.com/)
* [Websocket++](http://www.zaphoyd.com/websocketpp)

Included <sup>(Git submodules)</sup>
--
* [nlohmann::json](https://github.com/nlohmann/json)\
Included by what are currently the only modules:
* [cURLpp](https://github.com/jpbarrette/curlpp)

Building <sup>(Complicated)</sup>
---
* Download:
    1. Download or `git clone` Discord++, a Discord++ REST module, and a Discord++ Websocket module.
    2. Place them all in a subdirectory of your project.
* In CMake:
    1. Add `ADD_SUBDIRECTORY(<relative path>)` for Discord++ and each module.
        
            add_subdirectory(lib/discordpp)
            add_subdirectory(lib/rest-curlpp)
            add_subdirectory(lib/websocket-websocketpp)
        
    2. Add `discordpp` and each module to your `INCLUDE_DIRECTORIES` command.
        
            INCLUDE_DIRECTORIES( ${discordpp_SOURCE_DIR} ${discordpp-rest-curlpp_SOURCE_DIR} ${discordpp-websocket-websocketpp_SOURCE_DIR})
    
* In your code:
    1. Include `discordpp/bot.hh` and the header file from each submodule.
    
        Example:
        
            #include <discordpp/bot.hh>
            #include <discordpp/rest-curlpp.hh>
            #include <discordpp/websocket-websocketpp.hh>
            
    2. Create a Bot object, and pass it your token, a REST module, and a Websocket module.
            
            discordpp::Bot bot(token, std::make_shared<discordpp::RestCurlPPModule>(),std::make_shared<discordpp::WebsocketWebsocketPPModule>());
    
    3. Add responses to events with the Bot's
    
        ```cpp
        bot.addHandler("MESSAGE_CREATE", [](discordpp::Bot* bot, aios_ptr asio_ios, json msg){
            //Do Stuff
        });
        ```
    
    3. Create an `boost::asio::io_service`
    4. Initialize the bot object with its `init(std::shared_pointer<boost::asio::io_service> asio_ios)`.
    5. Run the `boost::asio::io_service`