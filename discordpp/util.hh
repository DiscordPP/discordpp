//
// Created by Aidan on 2/26/2021.
//

#pragma once

#include "alias.hh"

namespace discordpp {
namespace util {
// Adapted from https://stackoverflow.com/a/63359171
inline std::string rand_str(int length) {
    static const std::string chars{
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890"}; // "!@#$%^&*()`~-_=+[{]{|;:'",<.>/?"
    static std::random_device rd;
    static std::mt19937 generator(rd());

    std::string output;
    output.reserve(length);

    while (length > 0) {
        auto randNumb = generator();
        while (randNumb > chars.length() && length--) {
            output.push_back(chars[randNumb % chars.length()]);
            randNumb /= chars.length();
        }
    }
    return output;
}

inline std::string generate_boundary(const std::string &payload,
                                     sptr<std::string> file) {
    std::string out = rand_str(8);
    while ((file && file->find(out) != std::string::npos) ||
           payload.find(out) != std::string::npos) {
        out += rand_str(out.length());
    }
    return out;
}
} // namespace util
inline snowflake get_snowflake(std::string src) {
    snowflake out;
    std::istringstream(src) >> out;
    return out;
}
inline snowflake get_snowflake(json src) {
    return get_snowflake(src.get<std::string>());
}

template <typename T> inline std::string to_string(const T t) {
    return std::move(std::to_string(t));
}

template <> inline std::string to_string<std::string>(const std::string s) {
    return s;
}
} // namespace discordpp

namespace nlohmann {
// https://github.com/nlohmann/json/issues/1749#issuecomment-772996219
template <class T> void to_json(nlohmann::json &j, const std::optional<T> &v) {
    if (v.has_value())
        j = *v;
    else
        j = nullptr;
}

template <class T>
void from_json(const nlohmann::json &j, std::optional<T> &v) {
    if (j.is_null())
        v = std::nullopt;
    else
        v = j.get<T>();
}
} // namespace nlohmann