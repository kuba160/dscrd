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

#include <string.h>

#include "dscrd.h"
#include "dscrd_i.h"

void * json_discordobject (DiscordClient *client, dscrd_type type_o, char *json) {
	void *temp = NULL;
	DiscordClient **temp_dclient = NULL;

	// mask only real types
	int type = type_o & (DSCRD_TYPE_LIST - 1);

	// discordclient pointer
	#define DCPTR(X,Y) &(PTR(X,Y)->discord_client)

	switch (type) {
		case DSCRD_GUILD:
			temp = JTS(DiscordGuild, json);
			temp_dclient = DCPTR(DiscordGuild, temp);
			break;
		case DSCRD_CHANNEL:
			temp = JTS(DiscordChannel, json);
			temp_dclient = DCPTR(DiscordChannel, temp);
			break;
		case DSCRD_USER:
			temp = JTS(DiscordUser, json);
			temp_dclient = DCPTR(DiscordUser, temp);
			break;
		case DSCRD_WEBHOOK:
			temp = JTS_EX(Webhook, json);
			temp_dclient = DCPTR(Webhook, temp);
			break;
		case DSCRD_MESSAGE:
			temp = JTS_EX(DiscordMessage, json);
			temp_dclient = DCPTR(DiscordMessage, temp);
			break;
		default:
			return NULL;
	}

	#undef DCPTR

	if (temp) {
		// 0 - not a list, <0 list with n elements
		int list_len = 0;
		if (type_o & DSCRD_TYPE_LIST) {
			// count elements
			if (type == DSCRD_GUILD) {
				for ( ; ((DiscordGuild **)temp)[list_len]; list_len++) {
					// set types because we can
					((DiscordGuild **)temp)[list_len]->discord_client = client;
					(((DiscordGuild **)temp)[list_len]->_type) = type;
				}
			}
			else if (type == DSCRD_CHANNEL) {
				for ( ; ((DiscordChannel **)temp)[list_len]; list_len++) {
					// set types because we can
					((DiscordChannel **)temp)[list_len]->discord_client = client;
					(((DiscordChannel **)temp)[list_len]->_type) = type;
				}
			}
		}

		if (list_len) {
			int i;
			for (i = 0; i < list_len; i++) {
				dscrd_client_process (client, type, CREATE);
			}
		}
		else {
			dscrd_client_process (client, type, CREATE);
			*temp_dclient =  client;
			*((uint8_t *)(temp+offsetof(DiscordClient,_type))) = type;
		}
	}
	else {
		return NULL;
	}
	// post process
	if (type == DSCRD_WEBHOOK) {
		if (PTR(Webhook,temp)->user_p) {
			memcpy (&(PTR(Webhook,temp)->user), PTR(Webhook,temp)->user_p, sizeof(DiscordUser));
		}
	}

	return temp;
}

DiscordChannelList * json_DiscordChannelList (DiscordClient * client, char *json) {
	json_error_t error;
	json_t *root = json_loads(json, 0, &error);
	if (!root) {
		dscrd_error_set ("DiscordChannelList parsing failed on line %d: %s",error.line, error.text);
		return NULL;
	}

	DiscordChannelList *temp = JTS_EX(DiscordChannelList, json);

	if (!temp) {
		return NULL;
	}
	memset (temp, 0, sizeof(DiscordChannelList));

	if (!json_is_array(root)) {
	    dscrd_error_set ("DiscordChannelList error: root is not an array\n");
	    json_decref(root);
	    free (temp);
	    return NULL;
	}
	temp->count = json_array_size (root);
	temp->c = malloc (sizeof(DiscordChannel *) * (temp->count + 1));
	int i;
	for (i = 0; i < temp->count; i++) {
		json_t *sub = json_array_get (root, i);
		if (!sub) {
			break;
		}
		// maybe not the most efficient way...
		char *s = json_dumps (sub, 0);
		temp->c[i] = json_discordobject (client, DSCRD_CHANNEL, s);
		free (s);
	}
	temp->c[i] = NULL;

	json_decref(root);
	return temp;
}

DiscordGuild ** json_DiscordGuilds (DiscordClient * client, char *json) {
	// jts will auto-detect that returned object is an array and will make one
	// however you cannot free it through jts later
	DiscordGuild **guilds = json_discordobject (client, DSCRD_GUILD | DSCRD_TYPE_LIST, json);
	/*
	if (guilds && guilds[0]) {
		int i;
		for(i = 0; guilds[i]; i++) {
			guilds[i]->discord_client = client;
		}
	}*/
	return guilds;
}
/*
	json_error_t error;
	json_t *root = json_loads(json, 0, &error);
	if (!root) {
		printf ("Failed to parse DiscordGuild\n");
		printf("error: on line %d: %s\n", error.line, error.text);
		return NULL;
	}
	DiscordGuildList *temp = malloc (sizeof(DiscordGuildList));
	if (!temp) {
		return NULL;
	}
	memset (temp, 0, sizeof(DiscordGuildList));

	if (!json_is_array(root)) {
	    fprintf(stderr, "error: root is not an array\n");
	    json_decref(root);
	    free (temp);
	    return NULL;
	}
	temp->count = json_array_size (root);
	temp->g = malloc (sizeof(DiscordGuild *) * (temp->count + 1));
	int i;
	for (i = 0; i < temp->count; i++) {
		json_t *sub = json_array_get (root, i);
		if (!sub) {
			break;
		}
		// maybe not the most efficient way...
		char *s = json_dumps (sub, 0);
		temp->g[i] = json_discordobject (client, DSCRD_GUILD, s);
		free (s);
	}
	temp->g[i] = NULL;

	json_decref(root);
	return temp;
}
*/

WebhookList * json_WebhookList (DiscordClient * client, char *json) {
	json_error_t error;
	json_t *root = json_loads(json, 0, &error);
	if (!root) {
		printf ("Failed to parse Webhook\n");
		printf("error: on line %d: %s\n", error.line, error.text);
		return NULL;
	}
	WebhookList *temp = malloc (sizeof(WebhookList));
	if (!temp) {
		return NULL;
	}
	memset (temp, 0, sizeof(WebhookList));

	if (!json_is_array(root)) {
	    fprintf(stderr, "error: root is not an array\n");
	    json_decref(root);
	    free (temp);
	    return NULL;
	}
	temp->count = json_array_size (root);
	temp->w = malloc (sizeof(Webhook *) * (temp->count + 1));
	int i;
	for (i = 0; i < temp->count; i++) {
		json_t *sub = json_array_get (root, i);
		if (!sub) {
			break;
		}
		// maybe not the most efficient way...
		char *s = json_dumps (sub, 0);
		temp->w[i] = json_discordobject (client, DSCRD_WEBHOOK, s);
		free (s);
	}
	temp->w[i] = NULL;

	json_decref(root);
	return temp;
}
