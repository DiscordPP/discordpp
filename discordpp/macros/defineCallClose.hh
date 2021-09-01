//
// Created by Aidan on 3/10/2021.
//

#ifndef DEFINE_CALL_OPEN
#error There is no open Call definition
#endif
}
;

#define CALLFUNC(name)                                                         \
    auto name() { return sptr<Class>(new Class(this)); }

DPP_FOR_EACH(CALLFUNC, function);
#undef CALLFUNC

#undef PFR
#undef PFO
#undef PFR1
#undef PFO1
#undef PFR2
#undef PFO2
#undef QSR
#undef QSO
#undef QSR1
#undef QSO1
#undef QSR2
#undef QSO2
#undef REQUIRE_VAR

#undef TARGET_STRING

#undef AUTO_PAYLOAD
#undef AUTO_TARGET
#undef SET_NULL
#undef USEDBY
#undef NEW_FIELD
#undef NEW_RENDERABLE_FIELD
#undef NEW_CUSTOM_RENDERABLE_FIELD
#undef NEW_BASIC_RENDERABLE_FIELD
#undef HIDE_FIELD
#undef STATIC_FIELD
#undef FORWARD_FIELD

#undef BASECALL
#undef Bot
#undef Parent
#undef Class
#undef function
#undef Fields
#undef DEFINE_CALL_OPEN