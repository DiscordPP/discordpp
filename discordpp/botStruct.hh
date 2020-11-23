//
// Created by Aidan on 6/21/2018.
//

#pragma once

#include <iostream>
#include <utility>

#include <boost/asio.hpp>

#include <nlohmann/json.hpp>

#include "call.hh"
#include "alias.hh"

namespace discordpp {
class BotStruct {
  public:
    std::multimap<std::string, handleEvent> handlers;

    virtual ~BotStruct(){};

    virtual void call(sptr<Call> call) = 0;

    virtual void send(const int opcode, sptr<const json> payload,
                      sptr<const handleSent> callback) = 0;

    void run() {
        for (auto module : needInit) {
            if (module.second) {
                std::cerr << "Forgot to initialize: " << module.first << '\n';
                exit(1);
            }
        }
        runctd();
    }

  protected:
    virtual void runctd() {
        std::cerr << "Starting run loop" << '\n';
        work = std::make_unique<boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type>>(
            boost::asio::make_work_guard(*aioc));
        aioc->run();
        std::cerr << "Ending run loop" << '\n';
    }

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void reconnect(const std::string& reason, const bool resume = true) = 0;

    virtual void receivePayload(json payload) = 0;

    std::map<std::string, bool> needInit;
    unsigned int apiVersion = 6;
    std::unique_ptr<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>
        work;
    std::shared_ptr<boost::asio::io_context> aioc;
    std::string token;
    bool connecting_ = false;
    bool connected_ = false;
};
} // namespace discordpp
