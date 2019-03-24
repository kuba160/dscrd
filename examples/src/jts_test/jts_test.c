/*
 * jts_test: jts (json to struct) library test suite
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

#define xstr(s) str(s)
#define str(s) #s

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KBLD  "\x1B[1m" // bold
#define KDIM  "\x1B[2m" // dim


typedef struct {
	char *name;
	char **array;
	int integer;
} simple_type;


JTS_D(simple_type,
name,
array,
integer
);


#define CHARP_VAL "Hello World!"
#define INT_VAL_D 1234
#define INT_VAL xstr(INT_VAL_D)
#define CHARPP_1 "first"
#define CHARPP_2 "second"
#define CHARPP_3 "third"

const char json_01[] = "{\"name\": \""
CHARP_VAL
"\", \"array\": [\""
CHARPP_1
"\", \""
CHARPP_2
"\", \""
CHARPP_3
"\"], \"integer\": "
INT_VAL
"}";

//

typedef struct {
	char name[16];
	char name_overflow[16];
	float float_num;
	double double_num;
	bool bool_value;
} simple_type2;

JTS_D(simple_type2,
	name,
	name_overflow,
	float_num,
	double_num,
	bool_value);


#define FLOAT_VAL_N 1.2356
#define FLOAT_VAL xstr(FLOAT_VAL_N)
#define FLOAT_VAL_MIN (FLOAT_VAL_N - 0.0000001)
#define FLOAT_VAL_MAX (FLOAT_VAL_N + 0.0000001)

#define DOUBLE_VAL_N 1.234
#define DOUBLE_VAL xstr(DOUBLE_VAL_N)
#define DOUBLE_VAL_MIN (DOUBLE_VAL_N - 0.000000000000001)
#define DOUBLE_VAL_MAX (DOUBLE_VAL_N + 0.000000000000001)

#define CHARP_VAL_OVERFLOW "This is a very long string. It has to be over 16 bytes to be too long to store in simple_type2."

#define BOOL_VAL_N true
#define BOOL_VAL "true"

const char json_02[] = "{\"name\": \""
CHARP_VAL
"\", \"name_overflow\": \""
CHARP_VAL_OVERFLOW
"\", \"float_num\": "
FLOAT_VAL
", \"double_num\": "
DOUBLE_VAL
", \"bool_value\": "
BOOL_VAL
"}";

int success_v;
int failure_v;

void success(const char *s) {
	printf (KGRN"%s"KNRM"\n", s);
	success_v++;

}

void failure(const char *s) {
	printf (KRED"%s"KNRM"\n", s);
	failure_v++;
}

int main () {

	success_v = 0;
	failure_v = 0;	

	// convert json_01 into simple_type
	simple_type *st_p = JTS(simple_type, json_01);

	// JTS() can fail if not enough memory sufficient, jansson library error etc.
	if (st_p) {
		success("json_01");

		// char *
		if (strcmp (CHARP_VAL, st_p->name) == 0) {
			success("char *");
		}
		else {
			failure("char *");
		}

		// char **
		if (strcmp (CHARPP_1, st_p->array[0]) == 0 && (strcmp (CHARPP_2, st_p->array[1]) == 0) && (strcmp (CHARPP_3, st_p->array[2]) == 0) && (st_p->array[3] ==0)) {
			success("char **");
		}
		else {
			failure("char **");
		}

		// int
		if (st_p->integer == INT_VAL_D) {
			success ("int");
		}
		else {
			failure ("int");
		}

	 
	 	char *json_01_out = STJ(simple_type, st_p);
	    if (json_01_out) {
	    	printf ("%s\n", json_01);
	    	printf ("%s\n", json_01_out);
	    	// we can not rely that json->struct->json convertion will return binary same strings
	    	if (strcmp (json_01, json_01_out) == 0) {
    			// success++;
	    		printf (KGRN"json->struct->json convertion are binary equal\n"KNRM);
	    	}
	    	free (json_01_out);
	    }
	    else {
	    	failure ("failed convert back json");
	    }


		// Free allocated memory by JTS()
		JTS_FREE(simple_type, st_p);
	}
	else {
		failure ("json_01");
		printf ("converting from json failed!\n");
		// todo error handling
	}

	fputc('\n', stdout);

	simple_type2 *st2_p = JTS(simple_type2, json_02);
	if (st2_p) {
		success ("json_02");

		// char[]
		if (strcmp (CHARP_VAL, st2_p->name) == 0) {
			success ("char[]");
		}
		else {
			failure ("char[]");
		}

		// char[] (overflow)
		// name will be truncated
		if (strncmp (CHARP_VAL_OVERFLOW, st2_p->name_overflow, 15) == 0) {
			success ("char[] (overflow)");
		}
		else {
			failure ("char[] (overflow)");
		}


		// float
		if( st2_p->float_num > FLOAT_VAL_MIN && st2_p->float_num < FLOAT_VAL_MAX) {
			success ("float");
		}
		else {
			failure ("float");
		}

		// double
		if( st2_p->double_num > DOUBLE_VAL_MIN && st2_p->double_num < DOUBLE_VAL_MAX) {
			success ("double");
		}
		else {
			failure ("double");
		}

		// bool
		if (st2_p->bool_value == BOOL_VAL_N) {
			success ("bool");
		}
		else {
			failure ("bool");
		}


		JTS_FREE(simple_type2, st2_p);
	}
	else {
		failure ("json_02");
		printf ("converting from json failed!\n");
	}

	printf ("Results:\n");
	printf ("Succeed: %d\n", success_v);
	printf ("Failed: %d\n", failure_v);

	return 0;
}

