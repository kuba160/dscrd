/*
 * gateway_callback: dscrd message callback example
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

#include "dscrd.h"

// defines TOKEN, TOKEN_TYPE
#include "../auth_data.h"

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

int main () {
	// Initialize dscrd library
	dscrd_initialize ();

	// Authenticate using dscrdd, if failed use TOKEN_TYPE and TOKEN
	DscrdAuth auth;
	DiscordClient *client;
	if (dscrdd_auth(&auth, "gateway_callback")) {
		printf ("Connecting with manual auth info.\n");
		client = DiscordClient_open (TOKEN_TYPE, TOKEN);
	}
	else {
		printf ("Connecting with dscrdd auth info.\n");
		client = DiscordClient_open_auth (&auth);
	}
	if (!client) {
		printf ("Opening Discord session failed. %s\n", dscrd_error());
		return 1;
	}
	printf ("dscrd is using Discord API v%d\n",client->api);
	printf ("Logged in as %s\n", client->me.username);

	// Open gateway
	int ret = DiscordClient_gateway (client, LAZYLOAD_GUILDS);
	if (ret) {
		printf ("Opening gateway failed. %s\n", dscrd_error());
	}
	else {
		// call callback_message() on every mention
		DiscordMessage_callback_mentions (client, callback_message);
		// wait for EOF (Ctrl-D)
		while(fgetc(stdin) != EOF);
	}

	DiscordClient_close (client);
	dscrd_terminate ();
	return 0;
}

