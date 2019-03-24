/*
 * MIT License
 * 
 * Copyright (c) 2017 RealAlpha
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "websocket.h"
#include <pthread.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>



int discord_protocol(struct lws* wsi, enum lws_callback_reasons reason,
		void* user, void* in, size_t len) {

	client_websocket_t* client = (client_websocket_t*)user;

	/* TODO: figure out what's causing this to degfault */
	/*
	if (client->_disconnect) {
		lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, "", 0);
		client->_connected = 0;
		client->_remain_connected = 0;
		
		return 1;
	}
	*/

	switch (reason) {
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		{
			if (client->_callbacks->on_connection_error) {
				if (in && len)
					client->_callbacks->on_connection_error(client, (char*)in, len);
				else
					client->_callbacks->on_connection_error(client, "Unknown", 7);
			}

			client->_connected = 0;
			break;
		}
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
		{
			printf("Connection Established!\n");
			break;
		}
		case LWS_CALLBACK_CLOSED:
		{
			break;
		}
		case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
		{
			uint16_t close_reason = ntohs(*(uint16_t*)in);

			char buffer[50];
			size_t close_len = sprintf(buffer, "Server sent close %hu", close_reason);

			client->_callbacks->on_connection_error(client, buffer, close_len);

			break;
		}
		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			size_t old_length = client->_current_packet_length;
			size_t new_length = old_length + len;
			void* new_data = realloc(client->_current_packet, new_length);
			if (new_data) {
				client->_current_packet = (char*)new_data;
				client->_current_packet_length = new_length;
			} else {
				return 1;
			}

			memcpy(client->_current_packet + old_length, in, len);
			
			if (lws_is_final_fragment(wsi)) {
				// Add a \0 terminator
				client->_current_packet = realloc(client->_current_packet, client->_current_packet_length + 1);
				client->_current_packet[client->_current_packet_length] = '\0';

				char* data = client->_current_packet;
				size_t data_len = client->_current_packet_length;

				int success = 0;

				if (client->_callbacks->on_receive)
					success = client->_callbacks->on_receive(client, data, data_len);

				free(client->_current_packet);
				client->_current_packet = NULL;
				client->_current_packet_length = 0;

				return success;
			}

			break;
		}
		case LWS_CALLBACK_GET_THREAD_ID:
		{
			return pthread_self();
		}
		case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
		{
			return 1;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

static struct lws_protocols protocols[] = {
	{ "discord-protocol", discord_protocol, 0, 1024, 0, NULL },
	{ NULL, NULL, 0, 0, 0, NULL } /* end */
};

static struct lws_extension exts[] = {
	{ 
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate; client_no_context_takeover; client_max_window_bits"
	},
	{
		"deflate-frame",
		lws_extension_callback_pm_deflate,
		"deflate_frame"
	},
	{ NULL, NULL, NULL } /* end */
};

client_websocket_t* websocket_create(client_websocket_callbacks_t* callbacks) {
	// added by me
	lws_set_log_level (LLL_ERR, NULL);

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;
	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

	client_websocket_t *client = malloc(sizeof(client_websocket_t));
	info.user = client;
	
	struct lws_context* context = lws_create_context(&info);
	client->_context = context;
	client->_remain_connected = 0;
	client->_callbacks = callbacks;

	client->_write_mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(client->_write_mutex, NULL);

	return client;
}

void* websocket_set_userdata(client_websocket_t* client, void* userdata) {
	void* old = client->_userdata;
	client->_userdata = userdata;
	return old;
}

void* websocket_get_userdata(client_websocket_t* client) {
	return client->_userdata;
}

void websocket_free(client_websocket_t* client) {
	if (client->_remain_connected) {
		websocket_disconnect(client);
	}

	/* free the context */
	lws_context_destroy(client->_context);
	//ERR_remove_thread_state(NULL); - Unrequired nowadays?

	/* clean up mutexes */
	pthread_mutex_destroy(client->_write_mutex);

	/* free alloc'd data */
	//free(client->_address);
	//free(client->_path);
	free(client->_write_mutex);
	
	free(client->_callbacks);

	free(client);
}

void websocket_connect(client_websocket_t* client, const char* address) {
	struct lws_client_connect_info info;
	memset(&info, 0, sizeof(info));
	info.context = client->_context;
	info.address = "gateway.discord.gg";
	//info.address = "localhost";
	info.port = 443;
	//info.port = 80;
	info.ssl_connection = 1;
	//info.ssl_connection = 0;
	info.path = "/?v=6&encoding=json";
	info.host = info.address;
	info.origin = info.address;
	info.protocol = "wss"; // ?
	//info.protocol = "ws";
	info.ietf_version_or_minus_one = -1;
	info.client_exts = exts;
	
	info.userdata = client;
	/*
	info.context = client->_context;
	info.ssl_connection = 1;
	info.userdata = client;

	size_t s = strlen(address);
	client->_address = malloc(s + 1);
	memcpy(client->_address, address, s);

	const char* prot = "";
	const char* path = "";

	if (lws_parse_uri(client->_address, &prot, &info.address, &info.port, &path))
		return;

	s = strlen(path);
	client->_path = malloc(s + 2);
	client->_path[0] = '/';
	memcpy(client->_path + 1, path, s);

	info.path = client->_path;
	info.host = info.address;
	info.origin = info.address;
	info.ietf_version_or_minus_one = -1;
	info.client_exts = exts;

	printf("protocol: %s\naddress: %s\nport: %i\npath: %s\nhost: %s\norigin: %s\n", prot, info.address, info.port, info.path, info.host, info.origin);
*/
	client->_remain_connected = 1;
	client->_current_packet_length = 0;
	client->_current_packet = malloc(0);

	client->_wsi = lws_client_connect_via_info(&info);
	client->_connected = 1;
}

void websocket_disconnect(client_websocket_t* client) {
	lws_cancel_service(client->_context);
	//client->_disconnect = 1;
}

void websocket_think(client_websocket_t* client) {
//	pthread_mutex_lock(client->_write_mutex);
	lws_service(client->_context, 500);
//	pthread_mutex_unlock(client->_write_mutex);
}


int websocket_send(client_websocket_t* client, char* data, size_t len, int mode) {
	if (client->_write_mutex) {
		pthread_mutex_lock(client->_write_mutex);
	}
	/* it is safe to write at this point */

	printf("websocket_send: %s\n", data);
	unsigned char cdata[LWS_PRE + len + 1];
	memcpy(&cdata[LWS_PRE], data, len);
	cdata[LWS_PRE + len] = '\0';

	unsigned char* ptr = &cdata[LWS_PRE];

	int status;
	switch (mode) {
		case 1:
			status = lws_write(client->_wsi, ptr, len, LWS_WRITE_BINARY);
			break;
		case 2:
			status = lws_write(client->_wsi, ptr, len, LWS_WRITE_PING);
			break;
		case 0:
		default:
			status = lws_write(client->_wsi, ptr, len, LWS_WRITE_TEXT);
			break;
	}

	/* ensure we unlock the mutex to prevent deadlocks */
	if (client->_write_mutex) {
		pthread_mutex_unlock(client->_write_mutex);
	}

	return status;
}
