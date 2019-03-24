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

#include "dscrd.h"
#include "dscrd_i.h"

extern const char * dscrd_type_s[];

extern short overwrite_api_v;

DiscordClient * DiscordClient_open (token_type type, const char * token) {
	if (type != Anonymous && !token) {
		dscrd_error_set ("Token cannot be empty for this type.");
		return NULL;
	}

	char buf[33];
	{
		int num;
		if (overwrite_api_v == -1) {
			num = DISCORD_API;
		}
		else {
			num = overwrite_api_v;
		}
		sprintf (buf, "https://discordapp.com/api/v%d/", num);
	}
	DiscordClient* d = malloc (sizeof(DiscordClient));
	memset (d, 0, sizeof(DiscordClient));

	d->c = Rest_init (buf, 443);
	d->api = DISCORD_API;
	d->_type = DSCRD_CLIENT;

	// auth
	if (type != Bot) {
		// let's hope it will work fine
		//printf ("token not bot todo\n");
	}
	if (type != Anonymous) {
		char *buf2 = malloc (256);
		if (type == Bot) {
			strcpy (buf, "Authorization: Bot");
		}
		else if (type == Bearer) {
			strcpy (buf, "Authorization: Bearer");
		}
		else if (type == User) {
			strcpy (buf, "Authorization: User");
		}
		snprintf (buf2, 256, "%s %s", buf, token);
		buf2 = realloc (buf2, strlen(buf2) + 1);
		d->auth_string = buf2;
		d->token = strstr (buf2, token);

		// get DiscordUser me
		DiscordUser *user = DiscordUser_me (d);
		if (user) {
			memcpy (&(d->me), user, sizeof(DiscordUser));
			user->discord_client = d;
			d->user = user;
		}
		else {
			// error string set by DiscordUser_me
			return NULL;
		}
	}
	else {
		d->user = calloc (1, sizeof(DiscordUser));
		d->refc[DSCRD_USER]++;
		d->user->discord_client = d;
		d->user->username = strdup ("Anonymous");
		memcpy (&(d->me), d->user, sizeof(DiscordUser));
	}
	d->type = type;

	return d;
};

void DiscordClient_close (DiscordClient *d) {
	Rest_free (d->c);
	if (d->user) {
		DiscordUser_destroy (d->user);
	}
	if (d->auth_string) {
		free (d->auth_string);
	}
	if (d->gateway) {
		DiscordGateway_close (d);
	}
	int i;
	for (i = 0; i < DSCRD_TYPE_LAST; i++) {
		if (d->refc[i]) {
			fprintf (stderr, "DiscordClient has %d refc for %s on close!\n", d->refc[i], dscrd_type_s[i]);
		}
	}
	free (d);
};

int DiscordClient_gateway (DiscordClient *client, int flags) {
	NO_GATEWAY_RET();

	if (client->gateway) {
		return 0;
	}
	else {
		client->gateway_flags = flags;
		if (DiscordGateway_open (client)) {
			return 0;
		}
		return -2;
	}
}


int DiscordClient_set_status (DiscordClient *client, enum dscrd_user_status status) {
	NO_GATEWAY_RET();
	#ifdef HAVE_GATEWAY

	if (!client->gateway) {
		dscrd_error_set ("Client is not connected to Discord Gateway.");
		return -1;
	}

	char *json_data = NULL;
	{
		json_t *root = json_object();
		json_t *d = json_object();
		{
			json_object_set_new (root, "op", json_integer(3));
			{
				json_object_set_new (d, "status", json_string(dscrd_user_status_str (status)));
				json_object_set_new (d, "afk", json_false());

				json_object_set_new (d, "since", json_null());
				json_object_set_new (d, "game", json_null());
			}
			json_object_set_new (root, "d", d);
		}
		json_data = json_dumps (root, 0);
		json_decref (root);
	}
	//	char buffer[] = "{ \"op\": 3, \"d\": {\"since\": 0, \"game\": {\"name\": \"Save the Oxford Comma\", \"type\": 0 }, \"status\": \"online\", \"afk\": false }}";
	//	char buffer[] = "{ \"op\": 3, \"d\": {\"since\": 0, \"game\": {}, \"status\": \"online\", \"afk\": false }}";
	if (json_data) {
		DiscordGateway_send (client, json_data);
		free (json_data);
	}
	else {
		dscrd_error_set ("Failed to parse json_data.");
		return 1;
	}

	client->user_status = status;

	#endif
	return 0;
}

int DiscordClient_set_game (DiscordClient *client, DiscordActivity *activity) {
	NO_GATEWAY_RET();
	#ifdef HAVE_GATEWAY

	if (!client->gateway) {
		dscrd_error_set ("Client is not connected to Discord Gateway.");
		return -1;
	}

	// todo
	char *json_data = NULL;
	{
		json_t *root = json_object();
		json_t *d = json_object();
		{
			json_object_set_new (root, "op", json_integer(3));
			{
				json_object_set_new (d, "status", json_string(dscrd_user_status_str(client->user_status)));
				json_object_set_new (d, "afk", json_false());

				json_object_set_new (d, "since", json_null());
				json_t *game = json_object();
				{
					json_object_set_new (game, "name", json_string(activity->name));
					json_object_set_new (game, "type", json_integer(activity->type));
					if (activity->type == ACTIVITY_STREAMING)
						json_object_set_new (game, "url", json_string(activity->url));
					if (activity->application_id[0])
						json_object_set_new (game, "application_id", json_string(activity->application_id));
					json_object_set_new (game, "details", json_string(activity->details));
					json_object_set_new (game, "state", json_string(activity->state));
				}
				json_object_set_new (d, "game", game);
			}
			json_object_set_new (root, "d", d);
		}
		json_data = json_dumps (root, 0);
		json_decref (root);
	}
	if (json_data) {
		DiscordGateway_send (client, json_data);
		free (json_data);
	}
	else {
		dscrd_error_set ("Failed to parse json_data.");
		return 1;
	}

	#endif
	return 0;
}
