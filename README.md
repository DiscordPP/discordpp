DiscordPP <sup>(it's cool)</sup>
===
A Discord API written in C++.

Functionality <sup>(Not much)</sup>
---
Logs in. Echoes when you mention it. Echo is broken if the mention of the bot is not the first thing in the row.

Requirements <sup>(You need these)</sup>
---
* [Boost](http://www.boost.org/)
* [OpenSSL](https://www.openssl.org/)
* [Crypto++](https://www.cryptopp.com/)

Included <sup>(Git submodules)</sup>
---
* [cURLpp](https://github.com/jpbarrette/curlpp)
* [nlohmann::json](https://github.com/nlohmann/json)
* [Websocket++](https://github.com/zaphoyd/websocketpp)

Building <sup>(Hell on Earth)</sup>
---
Build CMake targets `discorpp` and `curlpp`. Build Websocket++ separately and modify the `INCLUDE_DIRECTORIES` in `CMakeLists.txt` to reference it.

Make a copy of `login.dat.example` and rename it `login.dat`. Replace the login information with your bots. Bot account support is planned, but not currently implemented.
