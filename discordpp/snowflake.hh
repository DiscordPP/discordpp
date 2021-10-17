//
// Created by Aidan on 9/1/2021.
//

#pragma once
#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <nlohmann/json.hpp>

namespace discordpp {
using json = nlohmann::json;

template <typename T> inline std::string to_string(const T &t) {
    return std::move(std::to_string(t));
}

template <> inline std::string to_string<std::string>(const std::string &s) {
    return s;
}

struct Snowflake {
    Snowflake() : _value(0) {}
    Snowflake(uint64_t value) : _value(value) {}
    Snowflake(std::string value) : _value(std::stoull(value)) {}
    Snowflake(const Snowflake &) = default;

    operator uint64_t &() { return _value; }

    operator uint64_t() const { return _value; }

    operator std::string() const { return to_string(_value); }

    Snowflake &operator=(uint64_t val) {
        _value = val;
        return *this;
    }

    Snowflake &operator=(const std::string &str) {
        _value = std::stoull(str);
        return *this;
    }

    Snowflake &operator=(const Snowflake &s) {
        _value = s._value;
        return *this;
    }

    bool operator==(const Snowflake &rhs) const { return _value == rhs._value; }
    bool operator!=(const Snowflake &rhs) const { return !(rhs == *this); }

    friend void to_json(nlohmann::json &j, const Snowflake &s) { j = s._value; }
    friend void from_json(const nlohmann::json &j, Snowflake &s) {
        s._value = std::stoull(j.get<std::string>());
    }

  protected:
    uint64_t _value;
};
} // namespace discordpp
