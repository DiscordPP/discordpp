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

#undef BASECALL
#undef Bot
#undef Parent
#undef Class
#undef function
#undef Fields
#undef DEFINE_CALL_OPEN