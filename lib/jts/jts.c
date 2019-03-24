/*
 *     jts: jsonToStruct source file
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

#include <stdio.h>
#include <string.h>

#include <jansson.h>
#include "jts_b.h"
#include "jts.h"

#ifdef DEBUG
#define trace(...) { fprintf (stdout, __VA_ARGS__); fflush(stdout); }
#else
#define trace(...) {;}
#endif

#define JTS_OBJECT 1
#define JTS_ARRAY 2

void * jts_structify_jp (const jsonToStruct *jts, json_t *root, jsonToStruct_custom *customdef) {
	int json_datatype = json_typeof (root);

	void *temp;
	int i;
	json_t * data;
	const char *s;
	if (json_datatype == JSON_OBJECT) {
		temp = malloc (jts->data_size);
		trace ("ALLOC: OBJECT[%s] (%d bytes) %p\n", jts->name, jts->data_size, temp);
		if (!temp) {
			return NULL;
		}
		memset (temp, 0, jts->data_size);
		for (i = 0; jts->a[i]; i++) {
		    data = json_object_get(root, jts->a[i]->name);
		    void *pointer = ((void *) temp) + jts->a[i]->offset;
		    switch (jts->a[i]->data_type) {
		    	case JTS_CHAR: {
		    		s = json_string_value(data);
		    		if (s)
		    			strcpy (pointer, s);
		    		break;
		    	}
		    	case JTS_CHARP: {
		    		s = json_string_value(data);
		    		if (s) {
		    			// check if it's a pointer or array, will work unless system stores pointers in 1 byte or char is not one byte lol
		    			if (jts->a[i]->align == sizeof (char *)) {
		    				*((char **) pointer) = strdup (s);
		    				trace ("ALLOC: -- STRING (%zu bytes) %p\n", strlen (*((char **) pointer))+1, *((char **) pointer));
		    			}
		    			else {
		    				strncpy (pointer, s, jts->a[i]->size);
		    			}
		    		}
		    		break;
		    	}
		    	case JTS_INT: {
		    		*((int *) pointer) = json_integer_value(data);
		    		break;
		    	}
		    	case JTS_UCHAR: {
		    		*((unsigned char *) pointer) = json_integer_value(data);
		    		break;
		    	}
		    	case JTS_CHARPP:
		    		*((char ***) pointer) = malloc (sizeof(char *) * (json_array_size(data) + 1));
		    		trace ("ALLOC: -- STRING ARRAY (%zu pointers) %p\n", json_array_size(data) + 1, *((char ***) pointer));
		    		if (*((char ***) pointer)) {
		    			int j;
		    			json_t *sub;
		    			for (j = 0; j < json_array_size(data); j++) {
		    				sub = json_array_get (data, j);
		    				(*((char ***) pointer))[j] = strdup (json_string_value(sub));
		    				trace ("ALLOC: ---- SUBSTRING (%zu bytes) %p\n", strlen ((*((char ***) pointer))[j])+1, *((char **) pointer));
		    			}
		    			(*((char ***) pointer))[j] = NULL;
		    		}
		    		break;
		    	case JTS_CUST:
		    		if(data) {
			    		int c;
			    		if (!customdef) {
			    			printf ("JTS[%s] Custom datatype for \"%s\", but no custom types defined!\n", jts->name, jts->a[i]->name);
			    			break;
			    		}
			    		for (c = 0; customdef->a[c]; c++) {
			    			if (strcmp (customdef->a[c]->name, jts->a[i]->name) == 0) {
			    				if (customdef->a[c]->pp == JTS_T_D) {
			    					printf ("TODO! \"%s\" WON'T WORK!\n", jts->a[i]->name);
			    					//trace ("RECURSION\n");
				    				//*((void *) pointer) = jts_structify_jp (customdef->a[c]->jts_cust, data, customdef);
				    			}
			    				// if pointer
				    			else if (customdef->a[c]->pp == JTS_T_P) {
				    				trace ("RECURSION\n");
				    				*((void **) pointer) = jts_structify_jp (customdef->a[c]->jts_cust, data, customdef);
				    			}
				    			// if array of pointers
				    			else if (customdef->a[c]->pp == JTS_T_PP) {
				    				// do the same as we support arrays
				    				trace ("RECURSION\n");
				    				*((void **) pointer) = jts_structify_jp (customdef->a[c]->jts_cust, data, customdef);
				    			}
			    				
			    			}
			    		}
		    	/*case JTS_DUP:
		    		*((DiscordUser **) pointer) = (DiscordUser *) jts_structify_jp (&jDiscordUser, data);
		    		break;
		    		*/
		    	}
		    	case JTS_BOOL:
		    		if (data) {
		    			int type = json_typeof(data);
		    			if (type == JSON_TRUE) {
		    				*((bool *) pointer) = true;
		    			}
		    			else if (type == JSON_FALSE) {
		    				*((bool *) pointer) = false;
		    			}
		    		}
		    		break;
		    	case JTS_INTP:
		    		fprintf (stderr, "unsupported type int *!\n");
		    		//*((unsigned char *) pointer) = json_integer_value(data);
		    		break;
		    	case JTS_FLOAT:
					*((float *) pointer) = json_real_value(data);
		    		break;
		    	case JTS_FLOATP:
		    		fprintf (stderr, "unsupported type float *!\n");
		    		break;
		    	case JTS_DOUBLE:
		    		*((double *) pointer) = json_real_value(data);
		    		break;
		    	case JTS_DOUBLEP:
		    		fprintf (stderr, "unsupported type double *!\n");
		    		break;
		    }
		}
	}
	else if (json_datatype == JSON_ARRAY) {
		int arraycount = json_array_size (root);
		void **temp2= malloc (sizeof(void *) * (arraycount+1));
		trace ("ALLOC: -- CUSTOM ARRAY (%d pointers) %p\n", arraycount + 1, temp2);

		temp = temp2;
		for (i = 0; i < arraycount; i++) {
			json_t *element_json = json_array_get(root, i);
			trace ("ARRAY_ELEMENT\n");
			temp2[i] = jts_structify_jp (jts, element_json, customdef);
		}
		temp2[i] = NULL;
	}
	return temp;
}

void * jts_structify (const jsonToStruct *jts, const char *json_data, jsonToStruct_custom *customdef) {
	json_error_t error;
	json_t *root = json_loads(json_data, 0, &error);
	if (!root) {
		fprintf (stderr, "Failed to parse json data\n");
		fprintf (stderr, "error: on line %d: %s\n", error.line, error.text);
		return NULL;
	}
	void *temp = jts_structify_jp (jts, root, customdef);
	json_decref(root);
	return temp;
}

void jts_structify_reverse (const jsonToStruct *jts, void *structure, jsonToStruct_custom *customdef) {
	int i;
	for (i = 0; jts->a[i]; i++) {
	    void *pointer = ((void *) structure) + jts->a[i]->offset;
	    if (jts->a[i]->data_type == JTS_CHARP && *((char **)pointer)) {
	    	if (jts->a[i]->align == sizeof (char *)){
	    		trace ("FREE: -- STRING (%zu bytes) %p\n", strlen(*((char **)pointer)) + 1, *((char **)pointer));
	    		free (*((char **)pointer));
	    	}
	    }
	    if (jts->a[i]->data_type == JTS_CHARPP && *((char ***)pointer)) {
	    	int j;
	    	for (j = 0; (*((char ***)pointer))[j]; j++) {
	    		trace ("FREE: ---- SUBSTRING (%zu bytes) %p\n", strlen((*((char ***)pointer))[j]) + 1, (*((char ***)pointer))[j]);
	    		free ((*((char ***)pointer))[j]);
	    	}
	    	trace ("FREE: -- STRING ARRAY (%d pointers) %p\n", j, *((char ***) pointer));
	    	free (*((char ***)pointer));
	    }
	    if (jts->a[i]->data_type == JTS_CUST && *((void **)pointer)) {
	    	// type find
	    	int type_num = -1;
	    	int j;
	    	if (!customdef) {
	    		printf ("JTS_FREE[%s] Custom datatype for \"%s\", but no custom types defined!\n", jts->name, jts->a[i]->name);
	    		continue;
	    	}
	    	for (j = 0; customdef->a[j]; j++) {
	    		if (strcmp(customdef->a[j]->name, jts->a[i]->name) == 0) {
	    			type_num = j;
	    			break;
	    		}
	    	}
	    	if (type_num != -1) {
	    		if (customdef->a[type_num]->pp == JTS_T_D) {
	    			// CAUTION!
	    			trace ("RECURSION\n");
	    			jts_structify_reverse (customdef->a[type_num]->jts_cust, *((void **)pointer), customdef);
	    		}
	    		if (customdef->a[type_num]->pp == JTS_T_P) {
	    			trace ("RECURSION\n");
	    			jts_structify_reverse (customdef->a[type_num]->jts_cust, *((void **)pointer), customdef);
	    		}
	    		else if (customdef->a[type_num]->pp == JTS_T_PP) {
	    			if (**((void ***)pointer)) {
	    				trace ("RECURSION\n");
	    				int c_i;
	    				for (c_i = 0; (*((void ***)pointer))[c_i]; c_i++) {
	    					jts_structify_reverse (customdef->a[type_num]->jts_cust, ((void ***)pointer)[0][c_i], customdef);
	    				}
	    				trace ("FREE: -- CUSTOM ARRAY (%d pointers) %p\n", c_i, ((void ***) pointer)[0]);
	    				free (*((void ***)pointer));
	    			}
	    		}
	    	}
	    	else {
	    		printf ("Failed to find correct type for custom object.\n");
	    	}
	    }
	}
	trace ("FREE: OBJECT[%s] (%d bytes) %p\n", jts->name, jts->data_size, structure);
	free (structure);
}


/*
void json_cpy (void *to, void *from, jsonToStruct *jts) {
	int i;
	for (i = 0; jts->names[i]; i++) {
	    void *pointer = ((void *) to) + jts->offset[i];
	    void *pointer_from = ((void *) from) + jts->offset[i];
	    switch (jts->data_type[i]) {
	    	case JC_CP: {
	    		strcpy (pointer, pointer_from);
	    		break;
	    	}
	    	case JC_DP: {
	    		*((char **) pointer) = strdup (pointer_from);
	    		break;
	    	}
	    	case JC_INT: {
	    		*((int *) pointer) = *((int *) pointer_from);
	    		break;
	    	}
	    	case JC_UCHAR: {
	    		*((unsigned char *) pointer) = *((unsigned char *) pointer_from);
	    		break;
	    	}
	    	case JC_CHARPP:
	    	{
	    		int j;
	    		for (j = 0; (*((char ***) pointer_from))[j]; j++);
	    		*((char ***) pointer) = malloc (sizeof(char *) * (j + 1));
	    		if (*((char ***) pointer)) {
	    			int j;
	    			for (j = 0; (*((char ***) pointer_from))[j]; j++) {
	    				(*((char ***) pointer))[j] = (*((char ***) pointer_from))[j];
	    			}
	    			(*((char ***) pointer))[j] = NULL;
	    		}
	    		break;
	    	}
	    	case JC_DUP:
	    	{
	    		DiscordUser *dup = malloc (sizeof(DiscordUser));
	    		json_cpy (dup, pointer_from, &jDiscordUser);
	    		*((void **) pointer) = (void *) dup;
	    	}
	    }
	}
	memcpy (((void *) to) + jts->dc_offset, ((void *) from) + jts->dc_offset, sizeof (DiscordClient *));
}
*/



typedef union {
    void *v_ptr;
    char *c_ptr;
    int i;
    bool b;
    unsigned char u_c;
    char **c_pp;
} unknown_val;


char * stj_jsonify_list (const jsonToStruct *jts, int num, ...) {
	
	va_list valist;
	va_start(valist, num);

	json_t *root = json_object();

	/* access all the arguments assigned to valist */
	char *str = NULL;
	int i;
	for (i = 0; i < num; i++) {
     	str = va_arg(valist, char *);


      	unknown_val p;
      	p.v_ptr = va_arg (valist, void *);

        // find this type
        const jsonToStruct_element *current_el = NULL;
        int j;
        for (j = 0; jts->a[j]; j++) {
            if (strcmp (str, jts->a[j]->name) == 0) {
                current_el = jts->a[j];
                break;
            }
        }

        if (!current_el) {
            printf ("failed to find element %d\n", i);
            continue;
        }

        const char *name = current_el->name;
        switch (current_el->data_type) {
            case JTS_CHAR: {
            	// char array?
                json_object_set_new (root, name, json_string(p.c_ptr));
                break;
            }
            case JTS_CHARP: {
                json_object_set_new (root, name, json_string(p.c_ptr));
                break;
            }
            case JTS_INT: {
                json_object_set_new (root, name, json_integer(p.i));
                break;
            }
            case JTS_UCHAR: {
                json_object_set_new (root, name, json_integer(p.u_c));
                break;
            }
            case JTS_CHARPP: {
            	// array of char (ended with NULL)
            	json_t *arr = json_array();
            	char ** c_arr = p.c_pp;
            	int ai;
            	for (ai = 0; c_arr[ai]; ai++) {
            		json_array_append_new(arr, json_string(c_arr[ai]));
            	}
            	json_object_set_new (root, name, arr);
                break;
            }
            case JTS_BOOL: {
            	json_t * vaal;
            	if (p.b) {
            		vaal = json_true ();
            	}
            	else {
            		vaal = json_false ();
            	}
                json_object_set_new (root, name, vaal);
                break;
            }
            default: {
            	printf ("did not find type for %s\n", name);
            	break;
            }
        }
   }

   char *s = json_dumps (root, 0);

   json_decref (root);
	
   /* clean memory reserved for valist */
   va_end(valist);

   return s;
}

char * stj_jsonify (const jsonToStruct *jts, void *ptr) {
	

	json_t *root = json_object();

	/* access all the arguments assigned to valist */
	const char *str = NULL;
	int i;
	for (i = 0; jts->a[i]; i++) {

     	str = jts->a[i]->name;
      	unknown_val p;
      	p.v_ptr = *(void **)(ptr + (jts->a[i]->offset));

        // find this type
        const jsonToStruct_element *current_el = NULL;
        int j;
        for (j = 0; jts->a[j]; j++) {
            if (strcmp (str, jts->a[j]->name) == 0) {
                current_el = jts->a[j];
                break;
            }
        }

        if (!current_el) {
            printf ("failed to find element %d\n", i);
            continue;
        }

        const char *name = current_el->name;
        switch (current_el->data_type) {
            case JTS_CHAR: {
            	// char array?
                json_object_set_new (root, name, json_string(p.c_ptr));
                break;
            }
            case JTS_CHARP: {
                json_object_set_new (root, name, json_string(p.c_ptr));
                break;
            }
            case JTS_INT: {
                json_object_set_new (root, name, json_integer(p.i));
                break;
            }
            case JTS_UCHAR: {
                json_object_set_new (root, name, json_integer(p.u_c));
                break;
            }
            case JTS_CHARPP: {
            	// array of char (ended with NULL)
            	json_t *arr = json_array();
            	char ** c_arr = p.c_pp;
            	int ai;
            	for (ai = 0; c_arr[ai]; ai++) {
            		json_array_append_new(arr, json_string(c_arr[ai]));
            	}
            	json_object_set_new (root, name, arr);
                break;
            }
            case JTS_BOOL: {
            	json_t * vaal;
            	if (p.b) {
            		vaal = json_true ();
            	}
            	else {
            		vaal = json_false ();
            	}
                json_object_set_new (root, name, vaal);
                break;
            }
            default: {
            	printf ("did not find type for %s\n", name);
            	break;
            }
        }
   }

   char *s = json_dumps (root, 0);

   json_decref (root);


   return s;
}


/*
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
*/