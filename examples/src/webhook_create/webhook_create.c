/*
 * webhook_create: Example showing webhook creation
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

#include <dscrd.h>


// defines TOKEN, TOKEN_TYPE, GUILD_NAME, CHANNEL_NAME, AVATAR_PATH
#include "../auth_data.h"

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

    // Get specified guild
    int success = 0;
    DiscordGuild *guild = DiscordGuild_by_name (client, GUILD_NAME);
    if (guild) {
        // Get specified channel in guild
        DiscordChannel *channel = DiscordChannel_by_name (guild, CHANNEL_NAME);
        if (channel) {
            // Create webhook
            DscrdFile *avatar = DscrdFile_get (AVATAR_PATH);
            Webhook *webhook = Webhook_create (channel, "New Webhook (dscrd)", avatar);
            DscrdFile_destroy (avatar);
            if (webhook) {
                // Execute webhook
                if (webhook->name)
                    printf ("Executing webhook \"%s\".\n", webhook->name);
                if (Webhook_execute_simple (webhook, "Hello World!")) {
                    printf ("Webhook execute failed. %s\n", dscrd_error());
                }
                else {
                    success = 1;
                }
                // Delete newly created webhook from discord servers
                Webhook_delete (webhook);

                Webhook_destroy (webhook);
            }
            DiscordChannel_destroy (channel);
        }
        DiscordGuild_destroy (guild);
    }
    if (!success) {
        printf ("%s\n", dscrd_error_long());
    }

    DiscordClient_close (client);
    // Terminate dscrd library
    dscrd_terminate ();
    return 0;
}

