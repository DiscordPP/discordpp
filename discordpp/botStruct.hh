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

namespace discordpp {
class BotStruct {
  public:
    std::multimap<std::string, handleEvent> handlers;

    virtual ~BotStruct(){};

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

    class RenderedCall;

    class Call {
        friend BotStruct;
        friend RenderedCall;

      protected:
        explicit Call(BotStruct *bot) : bot_(bot) {}

      public:
        auto render() { return std::make_shared<RenderedCall>(this); }
        void run() { bot_->doCall(render()); }

#define FIELD(type, name)                                                      \
  protected:                                                                   \
    sptr<const type> _rendered_##name = nullptr;                               \
    virtual sptr<const type> render_##name() = 0;                              \
                                                                               \
  private:                                                                     \
    virtual sptr<const type> get_##name() {                                    \
        return _rendered_##name ? _rendered_##name                             \
                                : _rendered_##name = render_##name();          \
    }
#define SEPARATOR

        FIELDS

      private:
        BotStruct *bot_;

#undef SEPARATOR
#undef FIELD
    };

    ////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////

#define SET_CALL_FIELD(TYPE, NAME, USEDBY)                                     \
  public:                                                                      \
    auto NAME(sptr<TYPE> NAME##In) {                                           \
        _rendered_##USEDBY = nullptr;                                          \
        _##NAME = std::move(NAME##In);                                         \
        return shared_from_this();                                             \
    }                                                                          \
    auto NAME(const TYPE &NAME##In) {                                          \
        return NAME(std::make_shared<TYPE>(NAME##In));                         \
    }                                                                          \
                                                                               \
  protected:                                                                   \
    sptr<TYPE> _##NAME = nullptr;

#define RENDER_CALL_FIELD(TYPE, NAME)                                          \
  protected:                                                                   \
    sptr<const TYPE> render_##NAME() override {                                \
        return std::make_shared<const TYPE>(*_##NAME);                         \
    }

#define CALL_FIELD(TYPE, NAME, USEDBY)                                         \
    SET_CALL_FIELD(TYPE, NAME, USEDBY)                                         \
    RENDER_CALL_FIELD(TYPE, NAME)

    class RawCall : public Call, public std::enable_shared_from_this<RawCall> {
        friend BotStruct;

      protected:
        explicit RawCall(BotStruct *bot) : Call(bot) {}

#define FIELD(TYPE, NAME) CALL_FIELD(TYPE, NAME, NAME)
#define SEPARATOR
        FIELDS
#undef SEPARATOR
#undef FIELD
    };

    auto call() { return sptr<RawCall>(new RawCall(this)); }

#undef FIELDS

    class JsonCall : public Call,
                     public std::enable_shared_from_this<JsonCall> {
        friend BotStruct;

      protected:
        explicit JsonCall(BotStruct *bot) : Call(bot) {}

        CALL_FIELD(std::string, method, method)
        CALL_FIELD(std::string, target, target)
        CALL_FIELD(handleWrite, onWrite, onWrite)
        CALL_FIELD(handleRead, onRead, onRead)
      protected:
        sptr<const std::string> render_type() override {
            return std::make_shared<const std::string>("application/json");
        }
        SET_CALL_FIELD(json, payload, body)
      protected:
        sptr<const std::string> render_body() override {
            return std::make_shared<const std::string>(_payload->dump());
        }
    };

    auto callJson() { return sptr<JsonCall>(new JsonCall(this)); }

    /*    friend class Call;
        class Call {
    #define SELF Call
            friend class BotStruct;

          protected:
            explicit Call(BotStruct *bot) : bot_(bot) {}

          public:
            void run() { bot_->doCall(self_); }

    #define FORWARD_RUN() \
      public: \ void run() { PARENT::run(); }

    #define CALL_FIELD(TYPE, NAME) \
      public: \
        sptr<SELF> NAME(const TYPE &NAME##In) { \
            return NAME(std::make_shared<const TYPE>(NAME##In)); \
        } \
        sptr<SELF> NAME(sptr<const TYPE> NAME##In) { \
            _##NAME = std::move(NAME##In); \
            return std::static_pointer_cast<SELF>(self_); \
        } \
        virtual sptr<const TYPE> get_##NAME() { return _##NAME; }; \
                                                                                   \
      protected: \ sptr<const TYPE> _##NAME = nullptr;

    #define FORWARD_SET_CALL_FIELD(TYPE, NAME) \
      public: \
        virtual sptr<SELF> NAME(const TYPE &NAME##In) { \
            return std::static_pointer_cast<SELF>(PARENT::NAME(NAME##In)); \
        } \
        virtual sptr<SELF> NAME(sptr<const TYPE> NAME##In) { \
            return std::static_pointer_cast<SELF>( \
                PARENT::NAME(std::move(NAME##In))); \
        } \ sptr<const TYPE> _##NAME = nullptr;

    #define FORWARD_GET_CALL_FIELD(TYPE, NAME) \
      public: \ sptr<const TYPE> get_##NAME() override { return
    PARENT::get_##NAME(); };

    #define FORWARD_CALL_FIELD(TYPE, NAME) \
        FORWARD_SET_CALL_FIELD(TYPE, NAME) \ FORWARD_GET_CALL_FIELD(TYPE,
    NAME)

            CALL_FIELD(std::string, type)
            CALL_FIELD(std::string, target)
            CALL_FIELD(handleWrite, onWrite)
            CALL_FIELD(handleRead, onRead)
            CALL_FIELD(std::string, body)

          protected:
            BotStruct *bot_;
            std::shared_ptr<Call> self_ = nullptr;
        };
    #define BUILD_CALL(name) \
        virtual sptr<SELF> name() { \
            Call *call = new SELF(this); \
            call->self_ = sptr<Call>(call); \
            return std::reinterpret_pointer_cast<SELF>(call->self_); \
        };

        BUILD_CALL(call)
    #undef SELF

        friend class JsonCall;
        class JsonCall : protected Call {
    #define SELF JsonCall
    #define PARENT Call
            friend class BotStruct;

          protected:
            explicit JsonCall(BotStruct *bot) : Call(bot) {}

            FORWARD_RUN()

            FORWARD_CALL_FIELD(std::string, type)
            FORWARD_CALL_FIELD(std::string, target)
            FORWARD_CALL_FIELD(handleWrite, onWrite)
            FORWARD_CALL_FIELD(handleRead, onRead)
            CALL_FIELD(json, payload)

          protected:
            sptr<const std::string> get_body() override {
                return _payload ?
    std::make_shared<std::string>(_payload->dump()) : nullptr;
            };
        };
        BUILD_CALL(callJson)
    #undef PARENT
    #undef SELF*/

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
    virtual void disconnect() = 0;
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
};
} // namespace discordpp
