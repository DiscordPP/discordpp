//
// Created by Aidan on 6/21/2018.
//

#pragma once

#include <iostream>
#include <map>
#include <utility>

#ifdef ASIO_STANDALONE
#include <asio.hpp>
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
#endif

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include "alias.hh"
#include "enum.hh"
#include "log.hh"
#include "snowflake.hh"
#include "util.hh"

namespace discordpp {
#ifdef ASIO_STANDALONE
using error_code = std::error_code;
#else
using error_code = boost::system::error_code;
#endif

class BotStruct {
  public:
    bool debugUnhandled = true;
    bool showHeartbeats = true;
    std::multimap<std::string, handleEvent> handlers;

    virtual ~BotStruct(){};

    class RenderedCall;

#define Bot BotStruct

#define BASECALL
#define Class Call
#define function call
#include "macros/defineCallOpen.hh"
    NEW_BASIC_RENDERABLE_FIELD(std::string, method, )
    NEW_BASIC_RENDERABLE_FIELD(std::string, target, )
    NEW_BASIC_RENDERABLE_FIELD(std::string, type, )
    NEW_BASIC_RENDERABLE_FIELD(std::string, body, )
    NEW_BASIC_RENDERABLE_FIELD(handleWrite, onWrite, )
    NEW_BASIC_RENDERABLE_FIELD(handleRead, onRead, )
#include "macros/defineCallClose.hh"

#define Parent Call
#define Class JsonCall
#define function callJson
#include "macros/defineCallOpen.hh"
    NEW_RENDERABLE_FIELD(json, payload, USEDBY(body))
    FORWARD_FIELD(std::string, method, )
    FORWARD_FIELD(std::string, target, )
    STATIC_FIELD(std::string, type, "application/json")
    HIDE_FIELD(body)
    FORWARD_FIELD(handleWrite, onWrite, )
    FORWARD_FIELD(handleRead, onRead, )

  protected:
    virtual sptr<const json> render_payload() {
        return std::make_shared<const json>(*_payload);
    }
    sptr<const std::string> render_body() override {
        return std::make_shared<const std::string>(get_payload()->dump());
    }
#include "macros/defineCallClose.hh"

#define Parent JsonCall
#define Class FileCall
#define function callFile
#include "macros/defineCallOpen.hh"
    NEW_BASIC_RENDERABLE_FIELD(std::string, filename, USEDBY(body))
    NEW_BASIC_RENDERABLE_FIELD(std::string, filetype, USEDBY(body))
    NEW_FIELD(std::string, file, USEDBY(body, boundary))
    FORWARD_FIELD(json, payload, USEDBY(boundary))
    FORWARD_FIELD(std::string, method, )
    FORWARD_FIELD(std::string, target, )
    FORWARD_FIELD(handleWrite, onWrite, )
    FORWARD_FIELD(handleRead, onRead, )

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

#undef Bot

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
        work = std::make_unique<asio::executor_work_guard<
            asio::io_context::executor_type>>(
            asio::make_work_guard(*aioc));
        aioc->run();
        log::log(log::error,
                 [](std::ostream *log) { *log << "Ending run loop" << '\n'; });
    }

    virtual void connect() = 0;
    virtual void disconnect() {
        connected_ = false;
        ready_ = false;
    };
    virtual void reconnect(const std::string &reason,
                           const bool resume = true) = 0;

    virtual void doCall(sptr<RenderedCall> call) = 0;
    virtual void receivePayload(json payload) = 0;

    std::map<std::string, bool> needInit;
    unsigned int apiVersion = -1;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>>
        work;
    sptr<asio::io_context> aioc;
    std::string token;
    bool connecting_ = false;
    bool connected_ = false;
    bool reconnecting_ = false;
    bool ready_ = false;

    std::string encoding_ = "json";
    virtual json parsePayload(const std::string& payload){
        return json::parse(payload);
    };
    virtual std::string buildPayload(const json& payload){
        return payload.dump();
    };

    virtual void hasRateLimitPlugin() = 0;
};
} // namespace discordpp
