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

// @cond INTERNAL

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

// @endcond