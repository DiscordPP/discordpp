DiscordPP <sup>(it's cool)</sup>
===
A Discord API written in C++.

Links:
* [Discord Server](https://discord.gg/0usP6xmT4sQ4kIDh)
* [Discord API Implementation Trello Board](https://trello.com/b/XZVPL956/discord)

Requirements <sup>(I got these through APT)</sup>
---
* [Boost](http://www.boost.org/)
* [OpenSSL](https://www.openssl.org/)
* [Crypto++](https://www.cryptopp.com/)
* [Websocket++](http://www.zaphoyd.com/websocketpp)

Included <sup>(Git submodules)</sup>
---
* [cURLpp](https://github.com/jpbarrette/curlpp)
* [nlohmann::json](https://github.com/nlohmann/json)

Building <sup>(Complicated)</sup>
---
* In CMake:
    1. Add `ADD_SUBDIRECTORY(lib/discordpp)` before your `TARGET_LINK_LIBRARIES` command.
    2. Add `discordpp` to your `TARGET_LINK_LIBRARIES` command.
* In your code:
    1. Add `#include \<discordpp/bot.hh\>`.
    2. Create a Bot object with `Bot(std::string token);`.
    3. Add responses to events with the Bot's `void addResponse(std::string event, std::function<void(Bot *, json)> response);`.
    4. Start the bot object with the Bot's `void run();`.
    * Note: Code run after the Bot's `void run();` will not run until after the Bot stops, which I have not yet written.
