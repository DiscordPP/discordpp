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
level filter = warning;
std::ostream *out = &std::cout;

void log(level l, handleLog msg) {
    if (l >= filter) {
        msg(out);
    }
}
} // namespace log
} // namespace discordpp

#endif // ECHO_BOT_LOG_HH
