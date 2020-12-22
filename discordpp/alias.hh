//
// Created by Aidan on 10/5/2020.
//

#pragma once

#include <memory>

#include <nlohmann/json.hpp>

namespace discordpp{
using json = nlohmann::json;

template <typename T> using sptr = std::shared_ptr<T>;

using handleEvent = std::function<void(const json msg)>;
using handleAnyEvent = std::function<void(const std::string& name, const json msg)>;
using handleRead = std::function<void(const bool error, const json msg)>;
using handleWrite = std::function<void(const bool error)>;
using handleSent = std::function<void()>;
}
