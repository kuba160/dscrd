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

#ifndef _websocket_h_
#define _websocket_h_

#include <libwebsockets.h>
#include <pthread.h>

typedef struct websocket_callbacks client_websocket_callbacks_t;
typedef struct websocket_client client_websocket_t;

typedef int (*websocket_receive_callback)(client_websocket_t* client, char* data, size_t length);
typedef int (*websocket_connection_error_callback)(client_websocket_t* client, char* reason, size_t length);

struct websocket_callbacks {
	websocket_receive_callback on_receive;
	websocket_connection_error_callback on_connection_error;
};

struct websocket_client {
	client_websocket_callbacks_t* _callbacks;
	struct lws_context* _context;
	struct lws* _wsi;

	char* _address;
	char* _path;

	char* _current_packet;
	size_t _current_packet_length;

	uint8_t _remain_connected;
	uint8_t _connected;
	/*uint8_t _disconnect; */

	void* _userdata;

	pthread_mutex_t* _write_mutex;
};

client_websocket_t* websocket_create(client_websocket_callbacks_t* callbacks);

void* websocket_set_userdata(client_websocket_t* client, void* userdata);
void* websocket_get_userdata(client_websocket_t* client);

void websocket_free(client_websocket_t* client);
void websocket_connect(client_websocket_t* client, const char* address);
void websocket_disconnect(client_websocket_t* client);
void websocket_think(client_websocket_t* websocket);

int websocket_send(client_websocket_t* client, char* data, size_t len, int mode);

#endif /* _websocket_h_ */
