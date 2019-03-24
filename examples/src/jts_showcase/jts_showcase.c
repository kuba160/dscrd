/*
 * jts_showcase: jts (json to struct) library usage example
 *
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
#include <stdio.h>
#include <string.h>

#include <jts/jts_b.h>
#include <jts/jts.h>

// simple_type
typedef struct {
	char *name;
	int integer;
} simple_type;

// initiate jts to use with simple_type and which values are present in json
JTS_D(simple_type,
name,
integer
);

// json data we will convert from
const char json_01[] = "{\"name\": \"Hello World!\", \"integer\": 1337}";


int main () {

	// convert json_01 into simple_type
	simple_type *st_p = JTS(simple_type, json_01);

	// JTS() can fail if not enough memory sufficient, jansson library error etc.
	if (st_p) {

		printf ("simple_type:\n");
		printf ("name: %s\n", st_p->name);
		printf ("integer: %d\n", st_p->integer);


		// change integer value and go back to json
		st_p->integer = 24816;
		char *st_s = STJ(simple_type, st_p);
		printf ("JSON: %s\n", st_s);
		STJ_FREE(st_s);

		// Free allocated memory by JTS()
		JTS_FREE(simple_type, st_p);
	}

	return 0;
}


