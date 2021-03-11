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

#ifdef BASECALL
class Class : public std::enable_shared_from_this<Class> {
    friend RenderedCall;
#else
class Class : public Parent {
#endif
    friend Bot;

  protected:
#ifdef BASECALL
    explicit Class(BotStruct *bot) : bot_(bot) {}

    // https://stackoverflow.com/a/32172486
    template <typename Derived> std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

  public:
    auto render() { return std::make_shared<RenderedCall>(this); }
    void run() { bot_->doCall(render()); }

  private:
    BotStruct *bot_;
#else
    explicit Class(BotStruct *bot) : Parent(bot) {}
#endif

#define SET_NULL(usedby) _rendered_##usedby = nullptr;
#define USEDBY(...) DPP_FOR_EACH(SET_NULL, __VA_ARGS__)

#define NEW_FIELD(type, name, usedby)                                          \
  public:                                                                      \
    auto name(sptr<type> name##In) {                                           \
        usedby _##name = std::move(name##In);                                  \
        return shared_from_base<Class>();                                      \
    }                                                                          \
    auto name(const type &name##In) {                                          \
        return name(std::make_shared<type>(name##In));                         \
    }                                                                          \
                                                                               \
  protected:                                                                   \
    sptr<type> _##name = nullptr;

#define NEW_RENDERABLE_FIELD(type, name, usedby)                               \
    NEW_FIELD(type, name, usedby)                                              \
  protected:                                                                   \
    sptr<const type> _rendered_##name = nullptr;                               \
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
#define HIDE_FIELD(type, name)                                                 \
  protected:                                                                   \
    using Parent::name;

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

#undef SET_NULL
#undef USEDBY
#undef NEW_FIELD
#undef NEW_RENDERABLE_FIELD
#undef NEW_CUSTOM_RENDERABLE_FIELD
#undef NEW_BASIC_RENDERABLE_FIELD
#undef HIDE_FIELD
#undef FORWARD_FIELD
