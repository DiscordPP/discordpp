//
// Created by Aidan on 10/5/2020.
//

#pragma once

#include <memory>
#include <variant>

#include <nlohmann/json.hpp>

namespace discordpp {
using json = nlohmann::json;

using snowflake = uint64_t;

template <typename T> using sptr = std::shared_ptr<T>;

using handleEvent = std::function<void(const json msg)>;
using handleRead = std::function<void(const bool error, const json msg)>;
using handleWrite = std::function<void(const bool error)>;
using handleSent = std::function<void()>;
} // namespace discordpp

namespace discordpp {
using json = nlohmann::json;

using Nonce = std::variant<int, std::string>;
} // namespace discordpp

namespace nlohmann {
template <> struct adl_serializer<discordpp::Nonce> {
    static void to_json(json &j, const discordpp::Nonce &n) {
        std::visit(
            [&](auto &&value) { j = std::forward<decltype(value)>(value); }, n);
    }
    static void from_json(const json &j, discordpp::Nonce &n) {
        if (j.is_number_integer()) {
            n = j.get<int>();
        } else if (j.is_string()) {
            n = j.get<std::string>();
        } else {
            assert(false && "\"nonce\" wasn't a string or int");
        }
    }
};
} // namespace nlohmann
