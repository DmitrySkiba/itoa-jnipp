/*
 * Copyright (C) 2011 Dmitry Skiba
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _JNIPP_GENERATE_MACRO_INCLUDED_
#define _JNIPP_GENERATE_MACRO_INCLUDED_

///////////////////////////////////////////////////////////////////// macros

/* Maximum value of N that both xJNIPP_GENERATE_ARGS and
 *  xJNIPP_GENERATE macros support.
 */
#define xJNIPP_GENERATE_MAX_N 20

///////////////////////////////////////////////// xJNIPP_GENERATE_ARGS

/* xJNIPP_GENERATE_ARGS(N,F,comma) generates sequence
 *  F(1),...F(N-1),F(N)
 *
 * xJNIPP_GENERATE_ARGS(N,F,empty) generates sequence
 *  F(1)....F(N-1)F(N)
 *
 * Both versions generate empty list if N is 0.
 * Values for S other than 'comma' and 'empty' are not supported.
 */
#define xJNIPP_GENERATE_ARGS(N,F,S) xJNIPP_DO_GENERATE_ARGS(N,F,S)

///////////////////////////////////////////////// xJNIPP_GENERATE

/* xJNIPP_GENERATE(0,N,F,D) generates sequence
 *  F(0,D)F(1,D)....F(N-1,D)F(N,D)
 *
 * xJNIPP_GENERATE(1,N,F,D) generates sequence
 *  F(1,D)....F(N-1,D)F(N,D)
 *
 * Values for Z other than 0 and 1 are not supported.
 */
#define xJNIPP_GENERATE(Z,N,F,D) xJNIPP_DO_GENERATE(Z,N,F,D)

///////////////////////////////////////////////////////////////////// implementation

///////////////////////////////////////////////// xJNIPP_GENERATE_ARGS

#define xJNIPP_DO_GENERATE_ARGS(N,F,S) xJNIPP_GENERATE_ARGS_##N(0,F,S)

#define xJNIPP_GENERATE_ARGScomma1 ,
#define xJNIPP_GENERATE_ARGScomma0
#define xJNIPP_GENERATE_ARGSempty1
#define xJNIPP_GENERATE_ARGSempty0

#define xJNIPP_GENERATE_ARGS_0(C,F,S)
#define xJNIPP_GENERATE_ARGS_1(C,F,S) xJNIPP_GENERATE_ARGS_0(1,F,S)F(1)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_2(C,F,S) xJNIPP_GENERATE_ARGS_1(1,F,S)F(2)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_3(C,F,S) xJNIPP_GENERATE_ARGS_2(1,F,S)F(3)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_4(C,F,S) xJNIPP_GENERATE_ARGS_3(1,F,S)F(4)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_5(C,F,S) xJNIPP_GENERATE_ARGS_4(1,F,S)F(5)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_6(C,F,S) xJNIPP_GENERATE_ARGS_5(1,F,S)F(6)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_7(C,F,S) xJNIPP_GENERATE_ARGS_6(1,F,S)F(7)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_8(C,F,S) xJNIPP_GENERATE_ARGS_7(1,F,S)F(8)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_9(C,F,S) xJNIPP_GENERATE_ARGS_8(1,F,S)F(9)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_10(C,F,S) xJNIPP_GENERATE_ARGS_9(1,F,S)F(10)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_11(C,F,S) xJNIPP_GENERATE_ARGS_10(1,F,S)F(11)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_12(C,F,S) xJNIPP_GENERATE_ARGS_11(1,F,S)F(12)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_13(C,F,S) xJNIPP_GENERATE_ARGS_12(1,F,S)F(13)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_14(C,F,S) xJNIPP_GENERATE_ARGS_13(1,F,S)F(14)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_15(C,F,S) xJNIPP_GENERATE_ARGS_14(1,F,S)F(15)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_16(C,F,S) xJNIPP_GENERATE_ARGS_15(1,F,S)F(16)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_17(C,F,S) xJNIPP_GENERATE_ARGS_16(1,F,S)F(17)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_18(C,F,S) xJNIPP_GENERATE_ARGS_17(1,F,S)F(18)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_19(C,F,S) xJNIPP_GENERATE_ARGS_18(1,F,S)F(19)xJNIPP_GENERATE_ARGS##S##C
#define xJNIPP_GENERATE_ARGS_20(C,F,S) xJNIPP_GENERATE_ARGS_19(1,F,S)F(20)xJNIPP_GENERATE_ARGS##S##C
/* xJNIPP_GENERATE_ARGS_ must be in sync with xJNIPP_GENERATE_ */

///////////////////////////////////////////////// xJNIPP_GENERATE

#define xJNIPP_DO_GENERATE(Z,N,F,D) xJNIPP_GENERATE_##N(Z,F,D)

#define xJNIPP_GENERATE_00(F,D) F(0,D)
#define xJNIPP_GENERATE_01(F,D)
#define xJNIPP_GENERATE_0(Z,F,D) xJNIPP_GENERATE_0##Z(F,D)
#define xJNIPP_GENERATE_1(Z,F,D) xJNIPP_GENERATE_0(Z,F,D)F(1,D)
#define xJNIPP_GENERATE_2(Z,F,D) xJNIPP_GENERATE_1(Z,F,D)F(2,D)
#define xJNIPP_GENERATE_3(Z,F,D) xJNIPP_GENERATE_2(Z,F,D)F(3,D)
#define xJNIPP_GENERATE_4(Z,F,D) xJNIPP_GENERATE_3(Z,F,D)F(4,D)
#define xJNIPP_GENERATE_5(Z,F,D) xJNIPP_GENERATE_4(Z,F,D)F(5,D)
#define xJNIPP_GENERATE_6(Z,F,D) xJNIPP_GENERATE_5(Z,F,D)F(6,D)
#define xJNIPP_GENERATE_7(Z,F,D) xJNIPP_GENERATE_6(Z,F,D)F(7,D)
#define xJNIPP_GENERATE_8(Z,F,D) xJNIPP_GENERATE_7(Z,F,D)F(8,D)
#define xJNIPP_GENERATE_9(Z,F,D) xJNIPP_GENERATE_8(Z,F,D)F(9,D)
#define xJNIPP_GENERATE_10(Z,F,D) xJNIPP_GENERATE_9(Z,F,D)F(10,D)
#define xJNIPP_GENERATE_11(Z,F,D) xJNIPP_GENERATE_10(Z,F,D)F(11,D)
#define xJNIPP_GENERATE_12(Z,F,D) xJNIPP_GENERATE_11(Z,F,D)F(12,D)
#define xJNIPP_GENERATE_13(Z,F,D) xJNIPP_GENERATE_12(Z,F,D)F(13,D)
#define xJNIPP_GENERATE_14(Z,F,D) xJNIPP_GENERATE_13(Z,F,D)F(14,D)
#define xJNIPP_GENERATE_15(Z,F,D) xJNIPP_GENERATE_14(Z,F,D)F(15,D)
#define xJNIPP_GENERATE_16(Z,F,D) xJNIPP_GENERATE_15(Z,F,D)F(16,D)
#define xJNIPP_GENERATE_17(Z,F,D) xJNIPP_GENERATE_16(Z,F,D)F(17,D)
#define xJNIPP_GENERATE_18(Z,F,D) xJNIPP_GENERATE_17(Z,F,D)F(18,D)
#define xJNIPP_GENERATE_19(Z,F,D) xJNIPP_GENERATE_18(Z,F,D)F(19,D)
#define xJNIPP_GENERATE_20(Z,F,D) xJNIPP_GENERATE_19(Z,F,D)F(20,D)
/* xJNIPP_GENERATE_ must be in sync with xJNIPP_GENERATE_ARGS_ */

/////////////////////////////////////////////////////////////////////

#endif // _JNIPP_GENERATE_MACRO_INCLUDED_
