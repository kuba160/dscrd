/*
 *     dscrd: Discord C library
 *     Copyright (C) 2018 Jakub Wasylków
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 
// This file was autogenerated by gen_dscrd_i_h.sh.

// todo
typedef enum dscrd_event_e {
	CREATE, DESTROY
} dscrd_event;

/// todo
const char * dscrd_user_status_str (int status);

/**
 * Process client for specified event like object creation or deletion.
 * Currently this is used for:
 * * incrementing/decreasing refcount for specified objects
 * * destroying temporary @ref DiscordClient
 *
 * @param client Discord session, can be @b NULL
 * @param type @ref dscrd_type of event
 * @param event @ref dscrd_event, for now either @ref CREATE or @ref DESTROY
 */
void dscrd_client_process (DiscordClient *client, dscrd_type type, int event);

/**
 * Process rest error response and set human readable message.
 * Only called from rest.c when HTTP response is in 4XX row.
 * At the time of writing only Bad Request (400) error is parsed. 
 *
 * @param code HTTP response code
 * @param body raw response data
 */
void dscrd_rest_error_parse (int code, const char * body);

/**
 * Sets formatted data to error string. (printf format)
 *
 * @param fmt string with or without <em>format specifiers</em>
 * @param ... <em>sequence of additional arguments</em>
 */
void dscrd_error_set (const char* fmt, ...);

/**
 * Prefixes current error with given formatted data. (printf format)
 *
 * @param fmt string with or without <em>format specifiers</em>
 * @param ... <em>sequence of additional arguments</em>
 */
void dscrd_error_set_detailed (const char* fmt, ...);

/**
 * Dereference @b void pointer
 *
 * @param X pointer type
 * @param Y pointer name
 */
#define PTR(X,Y) ((X *) Y)

#define NO_GATEWAY_RET() if(!dscrd_has_gateway()){dscrd_error_set ("This library has no gateway support."); return -1;}

#define NO_GATEWAY_RET_P() if(!dscrd_has_gateway()){dscrd_error_set ("This library has no gateway support."); return NULL;}



/**
 * Ensures file data is loaded into memory.
 *
 * @param file @ref DscrdFile
 */
void DscrdFile_data (DscrdFile *file);

/**
 * Ensures base64 file data is loaded into memory.
 *
 * @param file @ref DscrdFile
 */
void DscrdFile_data_base64 (DscrdFile *file);

/**
 * Ensures encoded file data is loaded into memory.
 *
 * @param file @ref DscrdFile
 */
void DscrdFile_data_enc (DscrdFile *file);

/**
 * Checks if @ref DscrdFile is an existent file.
 *
 * @param file @ref DscrdFile
 * @returns 1 if @ref DscrdFile refers to a file
 * @returns 0 if @ref DscrdFile does not refer to a file and indicates no file (@ref DSCRD_FILE_NULL)
 * @see DSCRD_FILE_NULL
 */
int is_DscrdFile_real (DscrdFile *file);


/**
 * Parse json string into object based on given @ref dscrd_type
 *
 * @param client Discord session
 * @param type @ref dscrd_type object
 * @param json json string
 * @returns pointer to object
 */
void * json_discordobject (DiscordClient *client, dscrd_type type, char *json);

/**
 * Parse json string into @ref WebhookList object
 *
 * @param client Discord session
 * @param json json string
 * @returns @ref WebhookList pointer
 * @todo migrate into @ref json_discordobject
 */
WebhookList * json_WebhookList (DiscordClient * client, char *json);


DiscordGuild ** json_DiscordGuilds (DiscordClient * client, char *json);

DiscordChannelList * json_DiscordChannelList (DiscordClient * client, char *json);


// todo
void * DiscordGateway_open (DiscordClient *client);

// todo
void DiscordGateway_close (DiscordClient *client);

void DiscordGateway_send (DiscordClient *client, const char *message);

void DiscordGateway_callback_remove (void *gateway, int i);



/**
 * @brief Rest Response structure.
 *
 * Previously used RestResponse directly from rest-client-c library but now it's made API independent.
 */
typedef struct RestResp_s {
	/// HTTP status response (if any), for example "BAD REQUEST"
	char http_status[255];
	/// HTTP code response, for example 404 (Not Found)
	int http_code;
	// include headers in this structure? for now not needed
	/// response body
	char *body;
	/// body's content type (if body), for example "application/json"
	char *content_type;
	/// length of response body
	int content_length;

	/// pointer @ref DiscordClient of this response
	DiscordClient *discord_client;
	/// private pointer
	void *api_data;
} RestResp;

/**
 * Allocates Rest library access pointer.
 * @param host address of the host to connect
 * @param port port to connect
 */
void * Rest_init (const char *host, int port);

/**
 * Frees pointer allocated by Rest_init.
 * @param rest Rest library access pointer
 */
void Rest_free (void * rest);

/**
 * Makes a REST call.
 * Returned pointer has to be freed with @ref Rest_call_free.
 * @param client @ref DiscordClient session
 * @param path API path
 * @param method HTTP method, one of @ref http_method
 * @param body data to send
 * @returns pointer to @ref RestResp on success
 * @returns @b NULL on failure
 */
RestResp * Rest_call (DiscordClient *client, const char * path, unsigned char method, const char * body);

/**
 * Frees pointer allocated by Rest_call.
 * @param res Rest response structure (@ref RestResp)
 */
void Rest_call_free (RestResp *res);


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


JTS_E(DiscordUser);

JTS_E(Webhook);

JTS_E_EX(Webhook);

JTS_E(DiscordChannel);

JTS_E(DiscordGuild);

JTS_E(DiscordChannelList);

JTS_E_EX(DiscordChannelList);

JTS_E(DiscordMessage);

JTS_E_EX(DiscordMessage);

