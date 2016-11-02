DiscordPP <sup>(it's cool)</sup>
===
A Discord API written in C++.

Links:
* [Discord Server](https://discord.gg/0usP6xmT4sRkB0Vl)
* [Trello Board](trello.com/b/3CU2iRSo)

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
* [Websocket++](http://www.zaphoyd.com/websocketpp)

Included <sup>(Git submodules)</sup>
---
* [cURLpp](https://github.com/jpbarrette/curlpp)
* [nlohmann::json](https://github.com/nlohmann/json)

Building <sup>(Almost Hell on Earth)</sup>
---
Build CMake targets `discorpp` and `curlpp`.

Make a copy of `login.dat.example` and rename it `login.dat`. Replace the login information with your bots. Bot account support is planned, but not currently implemented.
