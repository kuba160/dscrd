/*
 * multithread: dscrd message callback example (multiple bots)
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
#include <pthread.h>
#include <unistd.h>

#include "dscrd.h"

#define THREAD_COUNT 2

int callback_message(DiscordMessage *mesg) {

	DiscordChannel *channel = DiscordChannel_get_fast (mesg->discord_client, mesg->channel_id);
	if (channel) {
		char message[64];
		snprintf (message, 64, "Hi <@%s>!\n", mesg->author->id);
		DiscordChannel_send_message_simple (channel, message);
		DiscordChannel_destroy (channel);
	}

	return 0;
}

int threads_terminate = 0;

void * thread(void *userdata) {
	int thread_num = *((int *) userdata);
	printf ("[%d]: started\n", thread_num);

	char name[32];
	snprintf (name, 32, "multithread@%d", thread_num);
	DscrdAuth auth;
	int auth_ret = dscrdd_auth (&auth, name);
	if (auth_ret) {
		printf ("[%d]: Failed to receive credentials. %s\n", thread_num, dscrd_error());
		printf ("[%d]: Is dscrd_daemon running and configured?\n", thread_num);
		return NULL;
	}
	// open new session and authenticate as TOKEN_TYPE with TOKEN
	DiscordClient *client = DiscordClient_open_auth (&auth);
	if (!client) {
		printf ("[%d]: Opening Discord session failed. %s\n", thread_num, dscrd_error());
		return NULL;
	}
	printf ("[%d]: Logged in as %s\n", thread_num, client->me.username);

	// Open gateway
	int ret = DiscordClient_gateway (client, LAZYLOAD_GUILDS);
	if (ret) {
		printf ("[%d]: Opening gateway failed. %s\n", thread_num, dscrd_error());
	}
	else {
		// call callback_message() on every mention
		DiscordMessage_callback_mentions (client, callback_message);
		// wait for terminate commando
		while(!threads_terminate) {
			usleep (100000);
		}
	}
	DiscordClient_close (client);
	return NULL;
}

int main () {
	// Initialize dscrd library
	dscrd_initialize ();

	
	pthread_t threads[THREAD_COUNT];
	int thread_nums[THREAD_COUNT];

	int i;
	for (i = 0; i < THREAD_COUNT; i++) {
		// start threads
		thread_nums[i] = i;
		pthread_create(threads + i, NULL, thread, thread_nums+i);
	}

	// wait for user input (newline/enter)
	char buf[64];
	fgets(buf, 64, stdin);

	// tell threads to terminate
	threads_terminate = 1;

	// wait for each thread
	for (i = 0; i < THREAD_COUNT; i++) {
		pthread_join(threads[i], NULL);
	}

	// Close the library
	dscrd_terminate ();
	return 0;
}

