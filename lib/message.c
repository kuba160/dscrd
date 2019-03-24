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

#include <dscrd.h>
#include <dscrd_i.h>

struct middleman_capsule {
	int (*func)(DiscordMessage *);
	int type;
	DiscordClient *discord_client;
};

#define FILTER_NONE 0
#define FILTER_MENTIONS_ONLY 2


int DiscordMessage_middleman_callback (GatewayResp *resp) {
	if (resp->terminate) {
		free (resp->userdata);
	}

	if (resp->data_type != DSCRD_MESSAGE) {
		return 0;
	}

	struct middleman_capsule *caps = (struct middleman_capsule *) resp->userdata;
	int (*func)(DiscordMessage *) = caps->func;
	DiscordMessage *mesg = (DiscordMessage *) resp->data;

	int ret = 0;
	mesg->discord_client = caps->discord_client;

	if (caps->type == FILTER_MENTIONS_ONLY) {
		if (mesg->mentions) {
			int i;
			for (i = 0; mesg->mentions[i]; i++) {
				if (strcmp(mesg->mentions[i]->id, caps->discord_client->me.id) == 0) {
					ret = func(mesg);
					break;
				}
			}
		}
	}
	else if (caps->type == FILTER_NONE) {
		ret = func(mesg);
	}

	if (ret == CALLBACK_ABORT) {
		return CALLBACK_ABORT;
	}
	return 0;
}

int DiscordMessage_callback_all (DiscordClient *client, int (*callback)(DiscordMessage *)) {
	NO_GATEWAY_RET();

	if (!client->gateway) {
		DiscordGateway_open (client);
		if (!client->gateway) {
			return -3;
		}
	}

	struct middleman_capsule *caps = malloc (sizeof(struct middleman_capsule));
	if (!caps) {
		return -1;
	}

	caps->func = callback;
	caps->type = FILTER_NONE;
	caps->discord_client = client;

	int ret = DiscordGateway_callback (client, "MESSAGE_CREATE", DiscordMessage_middleman_callback, caps);

	if (ret) {
		free (caps);
		return ret;
	}
}

int DiscordMessage_callback_mentions (DiscordClient *client, int (*callback)(DiscordMessage *)) {
	NO_GATEWAY_RET();

	if (!client->gateway) {
		DiscordGateway_open (client);
		if (!client->gateway) {
			return -3;
		}
	}

	struct middleman_capsule *caps = malloc (sizeof(struct middleman_capsule));
	if (!caps) {
		return -1;
	}

	caps->func = callback;
	caps->type = FILTER_MENTIONS_ONLY;
	caps->discord_client = client;

	int ret = DiscordGateway_callback (client, "MESSAGE_CREATE", DiscordMessage_middleman_callback, caps);

	if (ret) {
		free (caps);
		return ret;
	}
}

int DiscordMessage_delete_id (DiscordClient *client, const char *channel_id, const char *message_id) {
	char path[127];
	snprintf (path, 127, "/channels/%s/messages/%s",channel_id, message_id);
	RestResp *res = Rest_call (client, path, HTTP_DELETE, NULL);
	if (res) {

		Rest_call_free (res);
		return 0;
	}
	return 0;
}

int DiscordMessage_delete (DiscordMessage *message) {
	return DiscordMessage_delete_id (message->discord_client, message->channel_id, message->id);
}

DiscordMessage * DiscordMessage_new_simple (DiscordChannel *channel, const char *message) {
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
		return NULL;
	} 

	RestResp *res = Rest_call (channel->discord_client, buf, HTTP_POST, s);
	free (s);

	DiscordMessage *ret = NULL;
	if (res) {
		if (res->body) {
			ret = (DiscordMessage *) json_discordobject(channel->discord_client, DSCRD_MESSAGE, res->body);
		}
		Rest_call_free (res);
	}
	return ret;
}

void DiscordMessage_destroy (DiscordMessage *msg) {
	dscrd_client_process (msg->discord_client, DSCRD_MESSAGE, DESTROY);
	JTS_FREE(DiscordMessage, msg);
}

DiscordMessage * DiscordMessage_get (DiscordChannel *channel, char *message_id) {
	char buf[strlen("/channels/%s/messages/%s") + strlen (channel->id) + strlen (message_id) + 1];
	sprintf (buf, "/channels/%s/messages/%s", channel->id, message_id);

	RestResp *res = Rest_call (channel->discord_client, buf, HTTP_GET, NULL);

	DiscordMessage *ret = NULL;
	if (res) {
		if (res->body) {
			ret = (DiscordMessage *) json_discordobject(channel->discord_client, DSCRD_MESSAGE, res->body);
		}
		Rest_call_free (res);
	}
	return ret;
}