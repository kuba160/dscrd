/*
 *     dscrd/rest: REST API wrapper
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

#include <stdlib.h>
#include <string.h>
#include <rest_client.h>

#include "dscrd.h"
#include "dscrd_i.h"

void * Rest_init (const char *host, int port) {
	RestClient *c = malloc (sizeof(RestClient));
	if (!c) {
		return NULL;
	}

	RestClient_init(c, host, port);
	return c;
}

void Rest_free (void * rest) {
	RestClient_destroy((RestClient *) rest);
	free (rest);
}

int is_RestResponse_invalid (RestResponse *res) {
	if (res->http_code >= 400 && res->http_code < 500) {
		return 1;
	}
	// maybe accept only 2XX codes?
	return 0;
}

RestResp * Rest_call (DiscordClient *client, const char * path, unsigned char method, const char * body) {
	//method: one of: HTTP_POST, HTTP_GET, HTTP_PUT, HTTP_DELETE, HTTP_HEAD, HTTP_OPTIONS
	// body type is application/json

	RestRequest req;
	RestResponse *res = malloc (sizeof(RestResponse));
	dscrd_client_process (client, DSCRD_REST_RESP, CREATE);
	RestFilter* chain = NULL;
	
	RestRequest_init(&req, path, method);
	if (client->auth_string)
		RestRequest_add_header(&req, client->auth_string);

	if (body) {
		RestRequest_set_array_body (&req, body, strlen (body), "application/json");
	}

	RestResponse_init(res);
	chain = RestFilter_add(chain, &RestFilter_execute_curl_request);
	chain = RestFilter_add(chain, &RestFilter_set_content_headers);
	RestClient_execute_request(client->c, chain, &req, res);

	RestFilter_free(chain);
	RestRequest_destroy (&req);

	if (res->curl_error) {
		dscrd_error_set (res->curl_error_message);
		RestResponse_destroy (res);
		free (res);
		return NULL;
	}
	if (is_RestResponse_invalid(res)) {
		if (res->body) {
			dscrd_rest_error_parse (res->http_code, res->body);
		}
		else {
			dscrd_error_set ("Received \"%d %s\".", res->http_code, res->http_status);
		}
		RestResponse_destroy (res);
		dscrd_client_process (client, DSCRD_REST_RESP, DESTROY);
		free (res);
		return NULL;
	}

	RestResp *res2 = malloc (sizeof(RestResp));
	strcpy (res2->http_status, res->http_status);
	res2->http_code = res->http_code;
	// headers?
	res2->content_type = res->content_type;
	res2->content_length = res->content_length;
	res2->body = res->body;

	res2->discord_client = client;
	res2->api_data = res;
	return res2;
}

void Rest_call_free (RestResp *res) {
	RestResponse_destroy ((RestResponse *) res->api_data);
	free (res->api_data);
	dscrd_client_process (res->discord_client, DSCRD_REST_RESP, DESTROY);
	free (res);
}
