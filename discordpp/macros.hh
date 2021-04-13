//
// Created by Aidan on 2/24/2021.
//

#pragma once

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

//#define SET_RENDERED_NULL(X) _rendered_##X = nullptr;