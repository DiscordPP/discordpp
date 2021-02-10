//
// Created by Aidan on 5/27/2020.
//

#ifndef ECHO_BOT_LOG_HH
#define ECHO_BOT_LOG_HH

#include <functional>
#include <iostream>

namespace discordpp {
namespace log {
using handleLog = std::function<void(std::ostream *)>;

enum level { trace, debug, info, warning, error, fatal };
inline level filter = warning;
inline std::ostream *out = &std::cout;
inline std::ostream* err = &std::cerr;
inline std::ostream* heartbeat = &std::cout;

inline void log(level l, handleLog msg) {
    if (l >= filter) {
        msg(out);
    }
}
inline void logerr(handleLog msg)
{
    msg(err);
}
inline void logheartbeat(handleLog msg)
{
    msg(heartbeat);
}
} // namespace log
} // namespace discordpp

#endif // ECHO_BOT_LOG_HH
