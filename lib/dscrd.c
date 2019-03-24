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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include <curl/curl.h>
#include "dscrd.h"
#include "dscrd_i.h"

#define TO_STRG(x) #x
const char * dscrd_type_s[] = {
	TO_STRG(DSCRD_TYPE_FIRST),
	TO_STRG(DSCRD_GUILD),
	TO_STRG(DSCRD_CHANNEL),
	TO_STRG(DSCRD_USER),
	TO_STRG(DSCRD_WEBHOOK),
	TO_STRG(DSCRD_CLIENT),
	TO_STRG(DSCRD_REST_RESP),
	TO_STRG(DSCRD_FILE),
	TO_STRG(DSCRD_MESSAGE)
};
#undef TO_STRG

#define ERROR_OFFSET 64

char error_s[256] = "";
char *error_p = error_s + ERROR_OFFSET;


short overwrite_api_v = -1;



void dscrd_client_process (DiscordClient *client, dscrd_type type, int event) {
	if (client) {
		if (event == CREATE) {
			client->refc[type]++;
		}
		else if (event == DESTROY) {
			client->refc[type]--;
			if (type == DSCRD_WEBHOOK && client->temporary) {
				DiscordClient_close (client);
			}
		}
	}
	return;
}

void dscrd_rest_error_parse (int code, const char * body) {
	if (code == 400) {
		// probably invalid json element, let's find which
		json_error_t error;
		json_t *root = json_loads(body, 0, &error);
		if (root && json_is_object(root)) {
			char entry[32];
			char entry_msg[64];
			{
				const char *key;
				json_t *value;
				int i = 0;
				json_object_foreach(root, key, value) {
					if (!i) {
						strncpy (entry, key, 32);
						strncpy (entry_msg, json_string_value(json_array_get(value, 0)), 64);
					}
				}
			}
			dscrd_error_set ("Entry \"%s\": %s", entry, entry_msg);
			json_decref(root);
		}
	}
	else if (code == 401) {
		dscrd_error_set ("Unauthorized.");
	}
	else {
		dscrd_error_set ("HTTP Error %d.",code);
	}
}

dscrd_type dscrd_object_type (void *pointer) {
	return *((uint8_t *)pointer+21);
}

int dscrd_has_gateway (void) {
	#ifdef HAVE_GATEWAY
	return 1;
	#else
	return 0;
	#endif
}

const char * dscrd_error (void) {
	return error_s + ERROR_OFFSET;
}

const char * dscrd_error_long (void) {
	return error_p;
}

void dscrd_error_set (const char* fmt, ...) {
	va_list(args);
	va_start(args, fmt);
	char tmp_error_s[256];
	vsnprintf(tmp_error_s, 256, fmt, args);
	strcpy (error_s + ERROR_OFFSET, tmp_error_s);
	error_p = error_s + ERROR_OFFSET;
}

void dscrd_error_set_detailed (const char* fmt, ...) {
	va_list(args);
	va_start(args, fmt);
	char tmp_error_s[64];
	vsnprintf(tmp_error_s, 64, fmt, args);
	char *newp = error_s + ERROR_OFFSET - strlen(tmp_error_s);
	memcpy (newp, tmp_error_s, strlen(tmp_error_s));
	error_p = newp;
}

void dscrd_overwrite_api (int api) {
	overwrite_api_v = api;
}

int dscrd_initialize () {
	// current rest library suggests this
	curl_global_init(CURL_GLOBAL_DEFAULT);
	return 0;
}

int dscrd_terminate () {
	// current rest library suggests this
	curl_global_cleanup ();
	return 0;
}
