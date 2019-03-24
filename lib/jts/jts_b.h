/*
 *     jts: jsonToStruct header file
 *     Copyright (C) 2018 Jakub Wasylków
 *
 *     This file is part of dscrd library.
 *     This file is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This file is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this file. If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2018 Jakub Wasylków
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// @cond INTERNAL

#ifndef json_t
#include <jansson.h>
#endif

#define JTS_STRINGIFY(x) #x
#define JTS_TOSTRING(x) JTS_STRINGIFY(x)
#define JTS_NAME(x) jts_ ##x
#define JTS_CUST_NAME(x) jts_ext_ ##x
#ifndef offsetof
#include <stddef.h>
#endif
#ifndef alignof
#include <stdalign.h>
#endif
#ifndef bool
#include <stdbool.h>
#endif
//struct jsonToStruct_element_s;
struct jsonToStruct_s;

/**
 * @brief jsonToStruct element structure
 *
 */
typedef struct jsonToStruct_element_s {
	const char *name;
	const size_t offset;
	const size_t align;
	const int data_type;
	const int size;
	const int pp;
	const struct jsonToStruct_s * jts_cust;
} jsonToStruct_element;

typedef struct jsonToStruct_element_custom_s {
	const char *name;
	const int size;
	const int pointer_type;
	const struct jsonToStruct_s * jts_cust;
  const int pp;
} jsonToStruct_element_custom;

/**
 * @brief jsonToStruct_custom structure
 *
 * Here goes all external definitions.
 */
typedef struct jsonToStruct_custom_s {
	const jsonToStruct_element_custom **a;
} jsonToStruct_custom;


/**
 * @brief jsonToStruct structure (array)
 *
 */
typedef struct jsonToStruct_s {
	/// array to jsonToStruct elements (@ref jsonToStruct_element_s)
	const jsonToStruct_element **a;
	/// size of data
	const int data_size;

  const char *name;
} jsonToStruct;

#define JTS_CHAR 0 // unable to detect with gcc (for now)
#define JTS_CHARP 1 // character pointer
#define JTS_INT 2
#define JTS_UCHAR 3
#define JTS_CHARPP 4
//#define JTS_DUP 5
//#define JTS_DU 6
#define JTS_CUST 7
#define JTS_BOOL 8
#define JTS_INTP 9
#define JTS_FLOAT 10
#define JTS_FLOATP 11
#define JTS_DOUBLE 12
#define JTS_DOUBLEP 13

// name, type
#define JTS_ELEMENT_C(G, X, Y) \
	&(jsonToStruct_element_custom) { \
		.name = (const char *) JTS_TOSTRING(X), \
		.pointer_type = JTS_TYPE_P(Y, ((G *)0)->X), \
		.jts_cust = & JTS_NAME(Y), \
    .pp = JTS_TYPE_P(Y, ((G *)0)->X) \
	},

#define JTS_ELEMENT(X, Y) \
	&(jsonToStruct_element) { \
		.name = JTS_TOSTRING(Y), \
		.offset = offsetof(X, Y), \
		.align = alignof(((X *)0)->Y), \
		.size = sizeof( ((X *)0)->Y), \
		.data_type = JTS_TYPE(((X *)0)->Y) \
	},

#define JTS_ELEMENTT(G, X, Y) \
	&(jsonToStruct_element) { \
		.name = JTS_TOSTRING(Y), \
		.offset = offsetof(X, Y), \
		.size = sizeof( ((X *)0)->Y), \
		.data_type = JTS_TYPE(((X *)0)->Y), \
		.pp = JTS_TYPE_P( ((G *)0)->X) \
	},

#define JTS_TYPE(T) _Generic( (T), \
	char: JTS_CHAR, \
	char *: JTS_CHARP, \
	int: JTS_INT, \
	unsigned char: JTS_UCHAR, \
	char **: JTS_CHARPP, \
	bool: JTS_BOOL, \
	int *: JTS_INTP, \
	float: JTS_FLOAT, \
	float*: JTS_FLOATP,\
	double: JTS_DOUBLE, \
	double*: JTS_DOUBLEP,\
	default: JTS_CUST)

#define JTS_T_D 0
#define JTS_T_P 1
#define JTS_T_PP 2
#define JTS_T_U 3

#define JTS_TYPE_P(P,T) _Generic( (T), \
	P: JTS_T_D, \
	P *: JTS_T_P, \
	P **: JTS_T_PP, \
	default: JTS_T_U)

#define JTS_STRINGIZE(arg)  JTS_STRINGIZE1(arg)
#define JTS_STRINGIZE1(arg) JTS_STRINGIZE2(arg)
#define JTS_STRINGIZE2(arg) #arg

#define JTS_CONCATENATE(arg1, arg2)   JTS_CONCATENATE1(arg1, arg2)
#define JTS_CONCATENATE1(arg1, arg2)  JTS_CONCATENATE2(arg1, arg2)
#define JTS_CONCATENATE2(arg1, arg2)  arg1##arg2

#define JTS_E_FOR_EACH_1(what, x, y)\
  what(x, y)
#define JTS_E_FOR_EACH_2(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_1(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_3(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_2(what, x, __VA_ARGS__)
#define JTS_E_FOR_EACH_4(what, x, y, ...)\
  what(x,y)\
  JTS_E_FOR_EACH_3(what, x, __VA_ARGS__)
#define JTS_E_FOR_EACH_5(what, x, y, ...)\
  what(x, y)\
 JTS_E_FOR_EACH_4(what, x, __VA_ARGS__)
#define JTS_E_FOR_EACH_6(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_5(what, x, __VA_ARGS__)
#define JTS_E_FOR_EACH_7(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_6(what, x, __VA_ARGS__)
#define JTS_E_FOR_EACH_8(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_7(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_9(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_8(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_10(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_9(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_11(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_10(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_12(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_11(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_13(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_12(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_14(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_13(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_15(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_14(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_16(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_15(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_17(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_16(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_18(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_17(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_19(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_18(what, x,  __VA_ARGS__)
#define JTS_E_FOR_EACH_20(what, x, y, ...)\
  what(x, y)\
  JTS_E_FOR_EACH_19(what, x,  __VA_ARGS__)



#define JTS_EX_FOR_EACH_2(what, g, x, y)\
  what(g, x, y)
  //JTS_EX_FOR_EACH_1(what, __VA_ARGS__)
#define JTS_EX_FOR_EACH_4(what, g, x, y, ...)\
  what(g, x,y)\
  JTS_EX_FOR_EACH_2(what, g, __VA_ARGS__)
#define JTS_EX_FOR_EACH_6(what, g, x, y, ...)\
  what(g, x, y)\
  JTS_EX_FOR_EACH_4(what, g, __VA_ARGS__)
#define JTS_EX_FOR_EACH_8(what, g, x, y, ...)\
  what(g, x, y)\
  JTS_EX_FOR_EACH_6(what, g,  __VA_ARGS__)


#define JTS_PPNARG(...) \
         JTS_PPNARG_(__VA_ARGS__,JTS_PPRSEQ_N())
#define JTS_PPNARG_(...) \
         JTS_PPARG_N(__VA_ARGS__)
#define JTS_PPARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define JTS_PPRSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0


void * jts_structify_jp (const jsonToStruct *jts, json_t *root, jsonToStruct_custom *customdef);

void * jts_structify (const jsonToStruct *jts, const char *json_data, jsonToStruct_custom *customdef);

void jts_structify_reverse (const jsonToStruct *jts, void *structure, jsonToStruct_custom *customdef);

char * stj_jsonify (const jsonToStruct *jts, void *ptr);

char * stj_jsonify_list (const jsonToStruct *jts, int num, ...);

// @endcond
