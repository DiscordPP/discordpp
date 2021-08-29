//
// Created by Aidan on 2/24/2021.
//

#pragma once

// https://stackoverflow.com/a/26408195
// get number of arguments with __DPP_NARG__
#define __DPP_NARG__(...)  __DPP_NARG_I_(__VA_ARGS__,__DPP_RSEQ_N())
#define __DPP_NARG_I_(...) __DPP_ARG_N(__VA_ARGS__)
#define __DPP_ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __DPP_RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _DPP_VFUNC_(name, n) name##n
#define _DPP_VFUNC(name, n) _DPP_VFUNC_(name, n)
#define DPP_VFUNC(func, ...) _DPP_VFUNC(func, __DPP_NARG__(__VA_ARGS__)) (__VA_ARGS__)

//https://stackoverflow.com/a/11994395
// Make a FOREACH macro
#define DPP_FE_0(WHAT)
#define DPP_FE_1(WHAT, X) WHAT(X)
#define DPP_FE_2(WHAT, X, ...) WHAT(X)DPP_FE_1(WHAT, __VA_ARGS__)
#define DPP_FE_3(WHAT, X, ...) WHAT(X)DPP_FE_2(WHAT, __VA_ARGS__)
#define DPP_FE_4(WHAT, X, ...) WHAT(X)DPP_FE_3(WHAT, __VA_ARGS__)
#define DPP_FE_5(WHAT, X, ...) WHAT(X)DPP_FE_4(WHAT, __VA_ARGS__)
//... repeat as needed

#define DPP_GET_MACRO(_0,_1,_2,_3,_4,_5,NAME,...) NAME
#define DPP_FOR_EACH(action,...) \
  DPP_GET_MACRO(_0,__VA_ARGS__,DPP_FE_5,DPP_FE_4,DPP_FE_3,DPP_FE_2,DPP_FE_1,DPP_FE_0)(action,__VA_ARGS__)

#define DPP_XSTR(s) DPP_STR(s)
#define DPP_STR(s) #s

#define ARR(...) __VA_ARGS__

//#define SET_RENDERED_NULL(X) _rendered_##X = nullptr;
