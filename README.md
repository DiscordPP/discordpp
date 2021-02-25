# Discord++ <sup>(it's cool)</sup>
A Modularized C++ Library for the Discord API

***This library is not a hacked Discord mobile app that will give you free Nitro. Free Nitro does not exist, and any YouTube video saying otherwise is attempting either to get you to install malware or to collect ad revenue off of you.***

[![GitHub stars](https://img.shields.io/github/stars/DiscordPP/discordpp?style=for-the-badge)](https://github.com/DiscordPP/discordpp/stargazers)[![Discord](https://img.shields.io/discord/164234463247597568?color=%237289DA&label=Discord&style=for-the-badge)](https://discord.gg/VHAyrvspCx)

## Links
* [Discord Server](https://discord.gg/VHAyrvspCx)
* [Echo: Example / Template Bot](https://github.com/DiscordPP/echo-bot)

## Dependencies

### Required
* [Boost](http://www.boost.org/)
* A Discord++ REST module
   * [REST: Beast](https://github.com/DiscordPP/rest-beast/)
      * Requires [OpenSSL](https://www.openssl.org/)
* A Discord++ WebSocket module
   * [WebSocket: Beast](https://github.com/DiscordPP/websocket-beast)
      * Currently out of date
   * [WebSocket: Simple-WebSocket-Server](https://github.com/DiscordPP/websocket-simpleweb)

### Included <sup>(Git submodules)</sup>
* [nlohmann::json](https://github.com/nlohmann/json)

### Recommended Plugins
* [Plugin: RateLimit](https://github.com/DiscordPP/plugin-ratelimit) handes rate limiting
   * Without this plugin, Discord++ exits when encountering a rate limit for your safety
* [Plugin: Overload](https://github.com/DiscordPP/plugin-overload) provides overloads for the websocket and REST `call` and `send` functions to create `std::shared_ptr`s for you and provides some sane defaults when you don't need all their arguments.
* [Plugin: Responder](https://github.com/DiscordPP/plugin-responder) provides a simple interface for detecting commands in the form of a character and a string, e.g. `!help`
* You can find more plugins on the [#discordpp-plugin](https://github.com/topics/discordpp-plugin) tag

## Usage

### Templates <sup>This is way easier</sup>
* [Echo Bot](https://github.com/DiscordPP/echo-bot)
   * Contains detailed setup instructions
   * Branches often track development of new features
* Feel free to submit your own templates as a PR

# The insructions that follow are out of date, please use Echo as a template

### Manual
* Download:
1. Download Discord++, a Discord++ REST module, a Discord++ Websocket module, and any plugins you want.
   * `git submodule add <repository> [<path>]` is a nice way to have `git` still track the remote repositiories
      * After adding the submodules you want, use `git submodule update --init --recursive` to initialize them.
2. Place them all in a subdirectory of your project. (I use `lib/`)
* In CMake:
1. Add `ADD_SUBDIRECTORY(<relative path>)` for Discord++ and each module. e.g.

```cmake
add_subdirectory(lib/discordpp)
add_subdirectory(lib/rest-curlpp)
add_subdirectory(lib/websocket-websocketpp)
```
        
2. Add `discordpp` and each module to your `INCLUDE_DIRECTORIES` command.

```cmake
INCLUDE_DIRECTORIES( ${discordpp_SOURCE_DIR} ${discordpp-rest-curlpp_SOURCE_DIR} ${discordpp-websocket-websocketpp_SOURCE_DIR})
```
    
* In your code:
1. Include `discordpp/bot.hh` and the header file from each submodule. e.g.

```cpp
#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
```
            
2. Create a Bot object. e.g.

```cpp
auto bot = std::make_shared<DppBot>();
```
   * For this example, I declared a `DppBot` alias just after the `#include` statements
   
```cpp
using DppBot =
dpp::WebsocketBeast<
		dpp::RestBeast<
				dpp::Bot
		>
>;
```

3. Add responses to events with `handlers.insert` e.g.
    
```cpp
bot->handlers.insert(
	{
		"MESSAGE_CREATE",
		[&bot](json msg){
			//Do Stuff
		}
	}
);
```
    
3. Create a `std::shared_ptr<boost::asio::io_context>`
   * e.g. `auto aioc = std::make_shared<asio::io_context>();`
4. Initialize the bot object and any plugins that require it e.g. `bot->initBot(6, token, aioc)`. The token should be in the form `Bot <token>` where you replace `<token>` with your token.
5. Run the bot: `bot->run();` (This will also run the io_context)
