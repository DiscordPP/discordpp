DiscordPP <sup>(it's cool)</sup>
===
A Discord API written in C++.

[Discord Server](https://discord.gg/0usP6xmT4sRkB0Vl)

Functionality <sup>(Not much)</sup>
---
Can respond to soft commands `@mention command` that are listed in `softcommands.dat`.

`Softcommands.dat` Format:
```
command 1
help blurb 1 (unused)
response 1
command 2
help blurb 2 (unused)
response 2
multi-line command
help blurb for multi-line commands
{
multi-line
command
}
```


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
