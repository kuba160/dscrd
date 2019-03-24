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

#ifndef JTS_E_FOR_EACH_1
#error "include jts_b.h before jts.h"
#endif

/**
 * Create new @ref jsonToStruct data definition
 *
 * @param X Name of the datatype
 * @param ... list of datatype elements to process
 */
#define JTS_D(X, ...) jsonToStruct JTS_NAME(X) = { \
 	(const jsonToStruct_element *[]) {JTS_CONCATENATE(JTS_E_FOR_EACH_, JTS_PPNARG(__VA_ARGS__)) (JTS_ELEMENT, X, __VA_ARGS__)  NULL }, \
 	sizeof(X),\
 	JTS_TOSTRING(X)\
};

/**
 * Create new @ref jsonToStruct custom datatype definition
 *
 * @param X Name of the datatype
 * @param ... list of custom datatype elements in for of two: DATATYPE_NAME, DATATYPE
 * @note specified datatypes have to be declared before with JTS()
 */
#define JTS_D_EX(X, ...) jsonToStruct_custom JTS_CUST_NAME(X) = { \
	(const jsonToStruct_element_custom *[]) {JTS_CONCATENATE(JTS_EX_FOR_EACH_, JTS_PPNARG(__VA_ARGS__)) (JTS_ELEMENT_C, X, __VA_ARGS__)  NULL } \
}

/**
 * Reference exiting @ref jsonToStruct data definition
 *
 * @param X Name of the datatype
 */
#define JTS_E(X) extern jsonToStruct JTS_NAME(X);

/**
 * Reference exiting @ref jsonToStruct extention definition
 *
 * @param X Name of the datatype
 */
#define JTS_E_EX(X) extern jsonToStruct_custom JTS_CUST_NAME(X);

/**
 * Convert json data string into mentioned and predefined structure.
 *
 * @param X Name of the datatype
 * @param Y pointer to json data string
 *
 * @returns pointer to @b X with filled data
 * @see JTS_FREE() - freeing function
 * @see JTS_EX() - extended jts
 */
#define JTS(X,Y) (X *) jts_structify (&JTS_NAME(X), Y, (jsonToStruct_custom *) 0)

/**
 * Convert json data string into mentioned and predefined structure.
 * In comparison to @ref JTS() this function requires that given datatype has its extention defined (see @ref JTS_D_EX())
 *
 * @param X Name of the datatype
 * @param Y pointer to json data string
 *
 * @returns pointer to @b X with filled data
 * @see JTS_FREE_EX() - extended freeing function
 */
#define JTS_EX(X,Y) (X *) jts_structify (&JTS_NAME(X), Y, &JTS_CUST_NAME(X))

/**
 * Free data allocated by @ref JTS().
 *
 * @param X Name of the datatype
 * @param Y pointer to datatype
 *
 * @see JTS_EX_FREE - extended freeing function
 */
#define JTS_FREE(X,Y) jts_structify_reverse (&JTS_NAME(X), Y, (jsonToStruct_custom *) 0)

/**
 * Free data allocated by @ref JTS_EX().
 *
 * @param X Name of the datatype
 * @param Y pointer to datatype
 *
 * @see JTS_FREE - normal freeing function
 */
#define JTS_EX_FREE(X,Y) jts_structify_reverse (&JTS_NAME(X), Y, &JTS_CUST_NAME(X))


/**
 * Converts pointer of previously defined datatype into json datastring.
 *
 * @param X Name of the datatype
 * @param Y Pointer to the datatype
 *
 * @returns json-formated string on success
 * @returns @b NULL on failure
 * @see STJ_FREE - same as free()
 */
#define STJ(X, Y) stj_jsonify (&JTS_NAME(X), Y)

/**
 * Frees pointer allocated by @ref STJ().
 *
 * This function is equal to free().
 *
 *see STJ()
 */
#define STJ_FREE(X) free(X)

/**
 * Converts va_args into json datastring.
 * Va_args have to be formulated in pairs of string and datapointer (void *).
 *
 * @param X Name of the datatype
 * @param ... va_arg list
 *
 * @returns json-formated string on success
 * @returns @b NULL on failure
 * @see STJ_FREE - same as free()
 * @todo update
 */
#define STJ_L(X,...) stj_jsonify_list (&JTS_NAME(X), JTS_PPNARG(__VA_ARGS__)/2, __VA_ARGS__)

// @endcond
