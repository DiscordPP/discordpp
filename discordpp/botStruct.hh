//
// Created by Aidan on 6/21/2018.
//

#pragma once

#include <iostream>
#include <utility>

#include <boost/asio.hpp>

#include <nlohmann/json.hpp>

#include "alias.hh"
#include "log.hh"
#include "util.hh"

namespace discordpp {
class BotStruct {
  public:
    std::multimap<std::string, handleEvent> handlers;

    virtual ~BotStruct(){};

    class RenderedCall;

#define BASECALL
#define Bot BotStruct
#define Class Call
#define function call
#define Fields                                                                 \
    NEW_BASIC_RENDERABLE_FIELD(std::string, method, )                          \
    NEW_BASIC_RENDERABLE_FIELD(std::string, target, )                          \
    NEW_BASIC_RENDERABLE_FIELD(std::string, type, )                            \
    NEW_BASIC_RENDERABLE_FIELD(std::string, body, )                            \
    NEW_BASIC_RENDERABLE_FIELD(handleWrite, onWrite, )                         \
    NEW_BASIC_RENDERABLE_FIELD(handleRead, onRead, )

#include "macros/defineCallOpen.hh"
    // This line intentionally left blank
#include "macros/defineCallClose.hh"

#define Bot BotStruct
#define Parent Call
#define Class JsonCall
#define function callJson
#define Fields                                                                 \
    NEW_RENDERABLE_FIELD(json, payload, USEDBY(body))                          \
    FORWARD_FIELD(std::string, method, )                                       \
    FORWARD_FIELD(std::string, target, )                                       \
    HIDE_FIELD(type)                                                           \
    HIDE_FIELD(body)                                                           \
    FORWARD_FIELD(handleWrite, onWrite, )                                      \
    FORWARD_FIELD(handleRead, onRead, )

#include "macros/defineCallOpen.hh"
  protected:
    sptr<const std::string> render_type() override {
        static auto type =
            std::make_shared<const std::string>("application/json");
        return type;
    }
    virtual sptr<const json> render_payload() {
        return std::make_shared<const json>(*_payload);
    }
    sptr<const std::string> render_body() override {
        return std::make_shared<const std::string>(get_payload()->dump());
    }
#include "macros/defineCallClose.hh"

#define Bot BotStruct
#define Parent JsonCall
#define Class FileCall
#define function callFile
#define Fields                                                                 \
    NEW_BASIC_RENDERABLE_FIELD(std::string, filename, USEDBY(body))            \
    NEW_BASIC_RENDERABLE_FIELD(std::string, filetype, USEDBY(body))            \
    NEW_FIELD(std::string, file, USEDBY(body, boundary))                       \
    FORWARD_FIELD(json, payload, USEDBY(boundary))                             \
    FORWARD_FIELD(std::string, method, )                                  \
    FORWARD_FIELD(std::string, target, )                                       \
    FORWARD_FIELD(handleWrite, onWrite, )                                      \
    FORWARD_FIELD(handleRead, onRead, )

#include "macros/defineCallOpen.hh"
  protected:
    sptr<const std::string> render_type() override {
        return std::make_shared<const std::string>(
            "multipart/form-data; boundary=" + *render_boundary());
    }

  protected:
    sptr<const std::string> _rendered_boundary = nullptr;
    sptr<const std::string> render_boundary() {
        return _rendered_boundary
                   ? _rendered_boundary
                   : _rendered_boundary = std::make_shared<const std::string>(
                         util::generate_boundary(
                             get_payload() ? get_payload()->dump() : "",
                             _file));
    }

    sptr<const std::string> render_body() override {
        auto boundary = render_boundary();
        auto body = std::make_shared<std::string>("--");
        *body += *boundary;
        if (get_payload()) {
            *body += "\r\nContent-Disposition: form-data; name=\"payload_json\""
                     "\r\nContent-Type: application/json\r\n\r\n";
            *body += get_payload()->dump();
            *body += "\r\n--";
            *body += *boundary;
        }
        if (_file) {
            *body += "\r\nContent-Disposition: form-data; name=\"file\"; "
                     "filename=\"";
            *body += *_filename;
            *body += "\"\r\nContent-Type: ";
            *body += *_filetype;
            *body += "\r\n\r\n";
            *body += *_file;
            *body += "\r\n--";
            *body += *boundary;
        }
        *body += "--";
        return std::const_pointer_cast<const std::string>(body);
    }

    void clear_boundary() {
        _rendered_boundary = nullptr;
        clear_type();
        clear_body();
    }
#include "macros/defineCallClose.hh"

#define FIELDS                                                                 \
    FIELD(std::string, method)                                                 \
    SEPARATOR                                                                  \
    FIELD(std::string, target)                                                 \
    SEPARATOR                                                                  \
    FIELD(std::string, type)                                                   \
    SEPARATOR                                                                  \
    FIELD(std::string, body)                                                   \
    SEPARATOR                                                                  \
    FIELD(handleWrite, onWrite)                                                \
    SEPARATOR                                                                  \
    FIELD(handleRead, onRead)

    class RenderedCall {
      public:
#define FIELD(type, name) name(src->get_##name())
#define SEPARATOR ,
        explicit RenderedCall(Call *src) : FIELDS {}
#undef SEPARATOR
#undef FIELD

#define SEPARATOR ,
#define FIELD(type, name) const sptr<const type> &name
        explicit RenderedCall(FIELDS)
#undef FIELD
#define FIELD(type, name) name(name)
            : FIELDS {
        }
#undef FIELD
#undef SEPARATOR

#define FIELD(type, name) const sptr<const type> name;
#define SEPARATOR
        FIELDS
#undef SEPARATOR
#undef FIELD
    };

#undef FIELDS

    virtual void send(const int opcode, sptr<const json> payload,
                      sptr<const handleSent> callback) = 0;

    void run() {
        for (const auto &module : needInit) {
            if (module.second) {
                log::log(log::error, [module](std::ostream *log) {
                    *log << "Forgot to initialize: " << module.first << '\n';
                });
                exit(1);
            }
        }
        runctd();
    }

  protected:
    virtual void runctd() {
        log::log(log::error, [](std::ostream *log) {
            *log << "Starting run loop" << '\n';
        });
        work = std::make_unique<boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type>>(
            boost::asio::make_work_guard(*aioc));
        aioc->run();
        log::log(log::error,
                 [](std::ostream *log) { *log << "Ending run loop" << '\n'; });
    }

    virtual void connect() = 0;
    virtual void disconnect(){
        connected_ = false;
        ready_ = false;
    };
    virtual void reconnect(const std::string &reason,
                           const bool resume = true) = 0;

    virtual void doCall(sptr<RenderedCall> call) = 0;
    virtual void receivePayload(json payload) = 0;

    std::map<std::string, bool> needInit;
    unsigned int apiVersion = 6;
    std::unique_ptr<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>
        work;
    sptr<boost::asio::io_context> aioc;
    std::string token;
    bool connecting_ = false;
    bool connected_ = false;
    bool ready_ = false;

    virtual void hasRateLimitPlugin() = 0;
};
} // namespace discordpp
