/*
 * webhook_exec: Chat-like webhook execution example
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

#define HARD_CODED_URL
#ifdef HARD_CODED_URL
// defines WEBHOOK_URL
#include "../auth_data.h"
#endif

int main () {
    // Get webhook url
    char *webhook_url;
    #ifdef HARD_CODED_URL
    webhook_url = WEBHOOK_URL;
    #else
    char whook_s[256];
    printf ("Webhook URL:\n");
    fgets (whook_s, 256, stdin);
    strtok(whook_s, "\n");
    webhook_url = whook_s;
    #endif

    // Initialize dscrd library
    dscrd_initialize ();
    // Get Webhook object
    Webhook *webhook = Webhook_get_no_auth_url (webhook_url);
    if (!webhook) {
        printf ("Receiving webhook failed. %s\n", dscrd_error());
        return 1;
    }

    // Get username
    char username[128];
    printf ("Username: ");
    fgets (username, 128, stdin);
    strtok(username, "\n");

    // Get avatar
    char avatar_url[128];
    printf ("Avatar URL: ");
    fgets (avatar_url, 128, stdin);
    strtok(avatar_url, "\n");

    // Message loop
    while (1) {
        char msg[128];
        printf ("> ");
        fgets (msg, 128, stdin);
        // Stop if empty line
        if (msg[0] == '\n') {
            break;
        }
        strtok(msg, "\n");
    
        // Execute webhook with given message
        int wh_ret = Webhook_execute (webhook, msg, username, avatar_url);
        if (wh_ret) {
            printf ("Executing webhook failed. %s\n", dscrd_error());
            break;
        }
    };

    // Destroy Webhook object
    Webhook_destroy (webhook);
    // Terminate library
    dscrd_terminate ();
    return 0;
}
