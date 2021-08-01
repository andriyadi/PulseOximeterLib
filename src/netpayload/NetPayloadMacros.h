//
// Created by Andri Yadi on 1/25/17.
//

/*
 * Adapted from:
 * https://github.com/SodaqMoja/SodaqOne-UniversalTracker/blob/master/SodaqOneTracker/MacroOverload.h
 * https://github.com/SodaqMoja/SodaqOne-UniversalTracker/blob/master/SodaqOneTracker/MacroCount.h
 * https://github.com/SodaqMoja/SodaqOne-UniversalTracker/blob/master/SodaqOneTracker/MacroSum.h
 *
*/

#ifndef TRACKERZERO_MACROS_H
#define TRACKERZERO_MACROS_H

/*
 * Usage:   // definition for FOO
 *          #define FOO(...) VFUNC(FOO, __VA_ARGS__)
 *
 * (from http://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments)
 */

// get number of arguments with __NARG__
#define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

#define COUNT(...) VA_NUM_ARGS_IMPL(__VA_ARGS__,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,N,...) N


#define SUM(...) VFUNC(SUM, __VA_ARGS__)
#define SUM1(_1) (_1)
#define SUM2(_1,_2) (_1+_2)
#define SUM3(_1,_2,_3) (_1+_2+_3)
#define SUM4(_1,_2,_3,_4) (_1+_2+_3+_4)
#define SUM5(_1,_2,_3,_4,_5) (_1+_2+_3+_4+_5)
#define SUM6(_1,_2,_3,_4,_5,_6) (_1+_2+_3+_4+_5+_6)
#define SUM7(_1,_2,_3,_4,_5,_6,_7) (_1+_2+_3+_4+_5+_6+_7)
#define SUM8(_1,_2,_3,_4,_5,_6,_7,_8) (_1+_2+_3+_4+_5+_6+_7+_8)
#define SUM9(_1,_2,_3,_4,_5,_6,_7,_8,_9) (_1+_2+_3+_4+_5+_6+_7+_8+_9)
#define SUM10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10)
#define SUM11(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11)
#define SUM12(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12)
#define SUM13(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13)
#define SUM14(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14)
#define SUM15(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15)
#define SUM16(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16)
#define SUM17(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17)
#define SUM18(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18)
#define SUM19(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19)
#define SUM20(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_20)
#define SUM21(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21)
#define SUM22(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21+_22)
#define SUM23(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21+_22+_23)
#define SUM24(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21+_22+_23+_24)
#define SUM25(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21+_22+_23+_24+_25)
#define SUM26(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21+_22+_23+_24+_25+_26)
#define SUM27(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27) (_1+_2+_3+_4+_5+_6+_7+_8+_9+_10+_11+_12+_13+_14+_15+_16+_17+_18+_19+_21+_22+_23+_24+_25+_26+_27)

#endif //TRACKERZERO_MACROS_H
