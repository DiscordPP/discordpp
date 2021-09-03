//
// Created by Aidan on 3/10/2021.
//

#include <memory>
#include "../macros.hh"

#ifdef DEFINE_CALL_OPEN
#error There is already an open Call definition
#endif
#define DEFINE_CALL_OPEN

#ifndef Bot
#error The call needs to be in a bot
#endif
#ifndef BASECALL
#ifndef Parent
#error The call needs a parent
#endif
#endif
#ifndef Class
#error The call needs a class name
#endif
#ifndef function
#error The call needs a function name
#endif

public:
#ifdef BASECALL
class Class : public std::enable_shared_from_this<Class> {
    friend RenderedCall;
#else
class Class : public Parent {
#endif
    friend Bot;

#ifdef BASECALL
  protected:
    explicit Class(BotStruct *bot) : bot_(bot) {}

    // https://stackoverflow.com/a/32172486
    template <typename Derived> std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

  public:
    auto render() { return std::make_shared<RenderedCall>(this); }
    auto run() {
        bot_->doCall(render());
        return shared_from_this();
    }

  private:
    BotStruct *bot_;
#else
  protected:
    explicit Class(BotStruct *bot) : Parent(bot) {}

  public:
    auto run() { return std::static_pointer_cast<Class>(Parent::run()); }
#endif

//#define SET_NULL(usedby) _rendered_##usedby = nullptr;
#define SET_CLEAR(usedby) clear_##usedby();
#define AND_CLEAR(usedby, name)                                                \
    usedby;                                                                    \
    SET_CLEAR(name);
#define USEDBY(...) DPP_FOR_EACH(SET_CLEAR, __VA_ARGS__)

#define NEW_FIELD(type, name, usedby)                                          \
  public:                                                                      \
    auto name(sptr<type> name##In) {                                           \
        usedby;                                                                \
        _##name = std::move(name##In);                                         \
        return shared_from_base<Class>();                                      \
    }                                                                          \
    auto name(const type &name##In) {                                          \
        return name(std::make_shared<type>(name##In));                         \
    }                                                                          \
                                                                               \
  protected:                                                                   \
    sptr<type> _##name = nullptr;

#define NEW_RENDERABLE_FIELD(type, name, usedby)                               \
    NEW_FIELD(type, name, AND_CLEAR(usedby, name))                             \
  protected:                                                                   \
    sptr<const type> _rendered_##name = nullptr;                               \
                                                                               \
    void clear_##name() {                                                      \
        _rendered_##name = nullptr;                                            \
        usedby;                                                                \
    }                                                                          \
                                                                               \
  private:                                                                     \
    virtual sptr<const type> get_##name() {                                    \
        return _rendered_##name ? _rendered_##name                             \
                                : _rendered_##name = render_##name();          \
    }

#define NEW_CUSTOM_RENDERABLE_FIELD(type, name, usedby)                        \
    NEW_RENDERABLE_FIELD(type, name, usedby)

#define NEW_BASIC_RENDERABLE_FIELD(type, name, usedby)                         \
    NEW_RENDERABLE_FIELD(type, name, usedby)                                   \
  protected:                                                                   \
    virtual sptr<const type> render_##name() {                                 \
        return _##name ? std::make_shared<const type>(*_##name) : nullptr;     \
    }

#define PFR2(KEY, VAR)                                                         \
    if (!_##VAR) {                                                             \
        throw std::logic_error(DPP_XSTR(Class) " needs " #VAR);                \
    }                                                                          \
    out[KEY] = *_##VAR;
#define PFO2(KEY, VAR)                                                         \
    if (_##VAR) {                                                              \
        out[KEY] = *_##VAR;                                                    \
    }
#define PFR1(VAR) PFR2(#VAR, VAR)
#define PFO1(VAR) PFO2(#VAR, VAR)
#define PFR(...) DPP_VFUNC(PFR, __VA_ARGS__)
#define PFO(...) DPP_VFUNC(PFO, __VA_ARGS__)

#define AUTO_PAYLOAD(PFIELDS)                                                  \
    HIDE_FIELD(payload)                                                        \
  protected:                                                                   \
    sptr<const json> render_payload() override {                               \
        json out;                                                              \
        PFIELDS                                                                \
        return std::make_shared<const json>(std::move(out));                   \
    }

#define REQUIRE_VAR(VAR)                                                       \
    if (!_##VAR) {                                                             \
        throw std::logic_error(DPP_XSTR(Class) " needs " #VAR);                \
    }
#define TARGET_STRING(VAR) , to_string(*_##VAR)

#define QSO2(KEY, VAR)                                                         \
    if (_##VAR) {                                                              \
        out += fmt::format("{}" KEY "={}", first ? "?" : "&", *_##VAR);        \
        first = false;                                                         \
    }
#define QSR2(KEY, VAR)                                                         \
    REQUIRE_VAR(VAR)                                                           \
    QSO2(KEY, VAR)
#define QSO1(VAR) QSO2(#VAR, VAR)
#define QSR1(VAR) QSR2(#VAR, VAR)
#define QSO(...) DPP_VFUNC(QSO, __VA_ARGS__)
#define QSR(...) DPP_VFUNC(QSR, __VA_ARGS__)

#define AUTO_TARGET(TPath, TArgs, QSArgs)                                      \
    HIDE_FIELD(target)                                                         \
  protected:                                                                   \
    sptr<const std::string> render_target() override {                         \
        DPP_FOR_EACH(REQUIRE_VAR, TArgs)                                       \
        std::string out =                                                      \
            fmt::format(TPath DPP_FOR_EACH(TARGET_STRING, TArgs));             \
        bool first = true;                                                     \
        QSArgs;                                                                \
        return std::make_shared<const std::string>(out);                       \
    }

#ifdef Parent
#define HIDE_FIELD(name)                                                       \
  protected:                                                                   \
    using Parent::name;

#define STATIC_FIELD(type, name, value)                                        \
    HIDE_FIELD(name)                                                           \
    sptr<const type> render_##name() override {                                \
        static auto name = std::make_shared<const type>(value);                \
        return name;                                                           \
    }

#define FORWARD_FIELD(type, name, usedby)                                      \
  public:                                                                      \
    auto name(sptr<type> name##In) {                                           \
        usedby;                                                                \
        return std::static_pointer_cast<Class>(Parent::name(name##In));        \
    }                                                                          \
    auto name(const type &name##In) {                                          \
        return name(std::make_shared<type>(name##In));                         \
    }
#endif
