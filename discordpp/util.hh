//
// Created by Aidan on 2/26/2021.
//

#pragma once

#include "alias.hh"
#include <random>
#include <optional>

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
} // namespace discordpp

// Begin https://github.com/nlohmann/json/issues/1261#issuecomment-426209912
namespace detail
{
template <std::size_t N>
struct variant_switch
{
    template <typename Variant>
    void operator()(int index, nlohmann::json const &value, Variant &v) const
    {
        if (index == N)
            v = value.get<std::variant_alternative_t<N, Variant>>();
        else
            variant_switch<N - 1>{}(index, value, v);
    }
};

template <>
struct variant_switch<0>
{
    template <typename Variant>
    void operator()(int index, nlohmann::json const &value, Variant &v) const
    {
        if (index == 0)
            v = value.get<std::variant_alternative_t<0, Variant>>();
        else
        {
            throw std::runtime_error(
                "while converting json to variant: invalid index");
        }
    }
};
}

namespace nlohmann
{
template <typename ...Args>
struct adl_serializer<std::variant<Args...>>
{
    static void to_json(json& j, std::variant<Args...> const& v)
    {
        std::visit([&](auto&& value) {
            j["index"] = v.index();
            j["value"] = std::forward<decltype(value)>(value);
        }, v);
    }

    static void from_json(json const& j, std::variant<Args...>& v)
    {
        auto const index = j.at("index").get<int>();
        ::detail::variant_switch<sizeof...(Args) - 1>{}(index, j.at("value"), v);
    }
};
}
// End https://github.com/nlohmann/json/issues/1261#issuecomment-426209912

namespace nlohmann {
// https://github.com/nlohmann/json/issues/1749#issuecomment-772996219
template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(nlohmann::json &j, const std::optional<T> &v) {
        if(v.has_value()){
            j = v.value();
        }else{
            j = nullptr;
        }
    }

    static void from_json(const nlohmann::json &j, std::optional<T> &v) {
        if (j.is_null()) {
            v = std::nullopt;
        }else {
            v = j.get<T>();
        }
    }
};

/*template <typename T> struct adl_serializer<std::vector<std::shared_ptr<T>>> {
    static void to_json(json &j, const std::vector<std::shared_ptr<T>> &v) {
        j = json::array();
        for (auto t : v) {
            j.push_back(*t);
        }
    }

    static void from_json(const json &j, std::vector<std::shared_ptr<T>> &v) {
        if (!j.is_array()) {
            throw json::type_error::create(317, "j must be an array", j);
        }
        for (auto t : j) {
            v.push_back(std::make_shared<T>(j.get<T>()));
        }
    }
};*/
} // namespace nlohmann