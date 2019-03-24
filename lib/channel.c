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

DiscordChannel * DiscordChannel_get (DiscordClient *client, char *channel_id) {
	int len = strlen("/channels/%s") - 2 + strlen (channel_id) + 1;
	char buf[len];
	sprintf (buf, "/channels/%s", channel_id);

	RestResp *res = Rest_call (client, buf, HTTP_GET, NULL);

	DiscordChannel *temp = NULL;
	if (res) {
		if (res->body) {
			temp = json_discordobject (client, DSCRD_CHANNEL, res->body);
		}
		Rest_call_free (res);
	}
	if (!temp) {
		dscrd_error_set_detailed ("Failed to receive channel in %s(). ", __func__);
	}
	return temp;	
}

DiscordChannel * DiscordChannel_get_fast (DiscordClient *client, char *channel_id) {
	DiscordChannel *temp = malloc (sizeof (DiscordChannel));
	memset (temp, 0, sizeof (DiscordChannel));
	strncpy (temp->id, channel_id, 21);
	temp->discord_client = client;
	dscrd_client_process (client, DSCRD_CHANNEL, CREATE);
	return temp;	
}

int DiscordChannel_modify (DiscordChannel *channel, ...) {
	return 0;
	// TODO DOODOODOD
}

DiscordChannel * DiscordChannel_by_name (DiscordGuild *guild, char *channel_name) {
	DiscordChannelList *list = DiscordChannels_in_guild (guild);
	if (list && list->count) {
		int i;
		for (i = 0; i < list->count; i++) {
			if (strcmp (list->c[i]->name, channel_name) == 0) {
				DiscordChannel *ret = DiscordChannelList_pop (list, i);
				DiscordChannelList_destroy (list);
				return ret;
			}
		}
		// all channels checked
		dscrd_error_set ("Channel \"%s\" not found in given guild.", channel_name);
	}
	dscrd_error_set_detailed ("Failed to get channel in %s(). ", __func__);
	return NULL;
}

int DiscordChannel_send_message_simple (DiscordChannel *channel, const char *message) {
	char buf[strlen("/channels/%s/messages") + strlen (channel->id)];
	sprintf (buf, "/channels/%s/messages", channel->id);

	json_t *root = json_object ();
	char *s = NULL;
	if (root) {
		json_object_set_new (root, "content", json_string(message));

		s = json_dumps (root, 0);
		json_decref (root);
	}
	if (!s) {
		dscrd_error_set ("Failed to compose message to json.\n");
		return -1;
	} 

	RestResp *res = Rest_call (channel->discord_client, buf, HTTP_POST, s);
	free (s);

	int ret = -1;
	if (res) {
		if (res->body) {
			// message object
			ret = 0;
		}
		Rest_call_free (res);
	}
	return ret;
}

DiscordChannel * DiscordChannelList_pop (DiscordChannelList *list, int i) {
	if (i < 0 || i > list->count) {
		dscrd_error_set ("Channel number %d is not valid in given list.", i);
		return NULL;
	}
	DiscordChannel *ret = list->c[i];
	memmove (list->c + (i), (list->c) + (i+1), sizeof(DiscordChannel *) * (list->count - i));
	list->count--;
	return ret;
}


DiscordChannelList * DiscordChannels_in_guild (DiscordGuild *guild) {
	int len = strlen("/guilds/%s/channels") - 2 + strlen (guild->id) + 1;
	char buf[len];
	sprintf (buf, "/guilds/%s/channels", guild->id);

	RestResp *res = Rest_call (guild->discord_client, buf, HTTP_GET, NULL);
	DiscordChannelList *temp = NULL;
	if (res) {
		if (res->body) {
			temp = json_DiscordChannelList (guild->discord_client, res->body);
		}
		Rest_call_free (res);
	}
	if (!temp) {
		dscrd_error_set_detailed ("Failed to get channels in %s(). ", __func__);
	}
	return temp;	
}

void DiscordChannelList_destroy (DiscordChannelList *channels) {
	int i;
	for (i = 0; i < channels->count; i++) {
		DiscordChannel_destroy (channels->c[i]);
	}
	free (channels->c);
	free (channels);
}

void DiscordChannel_destroy (DiscordChannel * channel) {
	dscrd_client_process (channel->discord_client, DSCRD_CHANNEL, DESTROY);
	JTS_FREE (DiscordChannel, channel);
}