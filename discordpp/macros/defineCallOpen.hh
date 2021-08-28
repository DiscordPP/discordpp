//
// Created by Aidan on 3/10/2021.
//

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
#ifndef Fields
#error The call needs some fields
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

#define LAST(FIELD) FIELD

    Fields

#ifdef TPath
#ifdef TFields
        protected: sptr<const std::string>
                    render_target() override {
#define FIELD(VAR)                                                             \
    if (!_##VAR)                                                               \
        throw std::logic_error(DPP_STR(Class) " needs " #VAR);
#define USEFIELDS(...) DPP_FOR_EACH(FIELD, __VA_ARGS__)
        USEFIELDS(TFields)
#undef USEFIELDS
#undef FIELD
#define FIELD(VAR) , std::to_string(*_##VAR)
#define USEFIELDS(...) DPP_FOR_EACH(FIELD, __VA_ARGS__)
        return std::make_shared<const std::string>(
            fmt::format(TPath USEFIELDS(TFields)));
#undef USEFIELDS
#undef FIELD
    }

#undef TFields
#endif
#undef TPath
#endif

#ifdef PFields
  protected:
    sptr<const json> render_payload() override {
        json out;
#define PFR2(KEY, VAR)                                                         \
    if (!_##VAR)                                                               \
        throw std::logic_error(DPP_XSTR(Class) " needs " #VAR);                \
    out[KEY] = *_##VAR;
#define PFO2(KEY, VAR)                                                         \
    if (_##VAR)                                                                \
        out[KEY] = *_##VAR;
#define PFR1(VAR) PFR2(#VAR, VAR)
#define PFO1(VAR) PFO2(#VAR, VAR)
#define PFR(...) DPP_VFUNC(PFR, __VA_ARGS__)
#define PFO(...) DPP_VFUNC(PFO, __VA_ARGS__)
        PFields
#undef PFR
#undef PFO
#undef PFR1
#undef PFO1
#undef PFR2
#undef PFO2
            return std::make_shared<const json>(std::move(out));
    }

#undef PFields

#endif

#undef SET_NULL
#undef USEDBY
#undef NEW_FIELD
#undef NEW_RENDERABLE_FIELD
#undef NEW_CUSTOM_RENDERABLE_FIELD
#undef NEW_BASIC_RENDERABLE_FIELD
#undef HIDE_FIELD
#undef STATIC_FIELD
#undef FORWARD_FIELD
