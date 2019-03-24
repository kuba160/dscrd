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

Webhook * Webhook_get (DiscordClient *client, char * id) {
	int len = strlen(id) + strlen ("webhooks/") + 1;
	char buf[len];
	snprintf (buf, len, "%s%s", "webhooks/", id);
	RestResp *res = Rest_call (client, buf, HTTP_GET, NULL);
	Webhook *temp = NULL;
	if (res && res->body) {
		temp = json_discordobject (client, DSCRD_WEBHOOK, res->body);
	}
	if (res) {
		Rest_call_free (res);
	}
	return temp;
}

Webhook * Webhook_get_no_auth (char * id, char * token) {
	int len = strlen(id) + strlen(token) + strlen ("webhooks/%s/%s") - 4 + 1;
	char buf[len];
	snprintf (buf, len, "webhooks/%s/%s", id, token);

	char buf_url[33];
	sprintf (buf_url, "https://discordapp.com/api/v%d/", DISCORD_API);

	DiscordClient *c = DiscordClient_open (Anonymous, NULL);
	c->temporary = 1;

	RestResp *res = Rest_call (c, buf, HTTP_GET, NULL);
	if (res && res->body) {
		Webhook *temp = json_discordobject (c, DSCRD_WEBHOOK, res->body);
		if (temp) {
			temp->discord_client = c;
			temp->use_token = true;
		}
		Rest_call_free (res);
		return temp;
	}
	if (res)
		Rest_call_free (res);
	return NULL;
}

Webhook * Webhook_get_no_auth_url (const char * url) {
	DiscordClient *c = DiscordClient_open (Anonymous, NULL);
	if (c) {
		c->temporary = true;
		char *webhook_to = strstr (url, "/webhooks/");
		Webhook *temp = NULL;
		if (webhook_to) {
			RestResp *res = Rest_call (c, webhook_to, HTTP_GET, NULL);
			if (res) {
				if (res->body) {
					temp = json_discordobject (c, DSCRD_WEBHOOK, res->body);
					if (temp) {
						temp->discord_client = c;
						temp->use_token = true;
					}
				}
				Rest_call_free (res);
				return temp;
			}
			DiscordClient_close (c);
			return NULL;
		}
		DiscordClient_close (c);
		return NULL;
	}
	return NULL;
}


Webhook * Webhook_create (DiscordChannel * channel, char * name, DscrdFile * avatar) {
	char *json_data = NULL;
	{
		json_t *root = json_object();
		json_object_set_new (root, "name", json_string(name));
		if (avatar) {
			DscrdFile_data_base64 (avatar);
			char avatar_s[avatar->data_base64_len + strlen(avatar->mime) + strlen("data:%s;base64,%s") +1];
			sprintf (avatar_s, "data:%s;base64,%s", avatar->mime, avatar->data_base64);
			json_object_set_new (root, "avatar", json_string(avatar_s));
		}
		json_data = json_dumps(root, 0);
		json_decref(root);
	}
	int len = strlen ("/channels/%s/webhooks") - 2 + strlen (channel->id) + 1;
	char buf[len];
	snprintf (buf, len, "/channels/%s/webhooks", channel->id);
	
	Webhook *temp = NULL;
	RestResp *res = Rest_call (channel->discord_client, buf, HTTP_POST, json_data);
	free (json_data);
	if (res) {
		if (res->body) {
			temp = json_discordobject (channel->discord_client, DSCRD_WEBHOOK, res->body);
		}
		Rest_call_free (res);
	}
	else {
		dscrd_error_set_detailed ("Failed to create webhook in %s(). ", __func__);
	}
    return temp;
}

void Webhook_destroy (Webhook * webhook) {
	dscrd_client_process (webhook->discord_client, DSCRD_WEBHOOK, DESTROY);
	JTS_EX_FREE (Webhook, webhook);
}

int Webhook_execute (Webhook * webhook, const char * message, const char * user, const char * avatar_url) {
	// missing tts, file, embeds
	int len = strlen("webhooks/%s/%s") + strlen(webhook->id) + strlen(webhook->token) + 1;
	char buf[len];
	snprintf (buf, len, "webhooks/%s/%s", webhook->id, webhook->token);

	// body
	char *s = NULL;
	{
		json_t *root = json_object();
		if (message)
			json_object_set_new (root, "content", json_string(message));
		if (user)
			json_object_set_new (root, "username", json_string(user));
		if (avatar_url)
			json_object_set_new (root, "avatar_url", json_string(avatar_url));
		s = json_dumps(root, 0);
		json_decref(root);
	}
  
	RestResp *res = Rest_call (webhook->discord_client, buf, HTTP_POST, s);
	free (s);

	if (res) {
		Rest_call_free (res);
	}
	else {
		dscrd_error_set_detailed ("Failed to execute webhook in %s(). ", __func__);
		return 1;
	}
	return 0;
	
}

int Webhook_execute_simple (Webhook * webhook, const char * message) {
	int ret = Webhook_execute (webhook, message, NULL, NULL);
	if (ret) {
		dscrd_error_set_detailed ("Failed to execute webhook in %s(). ", __func__);
	}
	return ret;
}

int Webhook_delete (Webhook * webhook) {
	int len = strlen("/webhooks/") + strlen (webhook->id) + 1;
	char buf[len];
	sprintf (buf, "/webhooks/%s", webhook->id);
	RestResp *res = Rest_call (webhook->discord_client, buf, HTTP_DELETE, NULL);
	// check status
	// todo
	Rest_call_free (res);
	return 0;
}

int Webhook_modify (Webhook *webhook, const char *name, DscrdFile *avatar) {
	char *json_data = NULL;
	{
		json_t *root = json_object();
		if (name) {
			json_object_set_new (root, "name", json_string(name));
		}
		if (avatar) {
			DscrdFile_data_enc (avatar);
			if (is_DscrdFile_real(avatar)) {
				json_object_set_new (root, "avatar", json_string(avatar->encoded));
			}
			else {
				json_object_set_new (root, "avatar", json_null());
			}
		}
		json_data = json_dumps(root, 0);
		json_decref(root);
	}
	int len = strlen ("/webhooks/%s") - 2 + strlen (webhook->id) + 1;
	if (webhook->use_token) {
		len += strlen (webhook->token);
	}
	char buf[len];
	if (webhook->use_token) {
		snprintf (buf, len, "/webhooks/%s/%s", webhook->id, webhook->token);
	}
	else {
		snprintf (buf, len, "/webhooks/%s", webhook->id);
	}
	
	RestResp *res = Rest_call (webhook->discord_client, buf, HTTP_PATCH, json_data);
	free (json_data);
	if (res) {
		if (res->body) {
			Webhook *new = json_discordobject (NULL, DSCRD_WEBHOOK, res->body);
			if (new) {
				if (name) {
					if (webhook->name) {
						free (webhook->name);
						webhook->name = NULL;
					}
					if (new->name) {
						webhook->name = strdup (new->name);
					}
				}
				if (avatar) {
					if (webhook->avatar) {
						free (webhook->avatar);
						webhook->avatar = NULL;
					}
					if (new->avatar) {
						webhook->avatar = strdup (new->avatar);
					}
				}
				Webhook_destroy (new);
			}
			else {
				Rest_call_free (res);
				dscrd_error_set_detailed ("Failed to modify webhook in %s(). ", __func__);
				return 1;
			}
		}
		Rest_call_free (res);
		return 0;
	}
	// error set in rest.c
	dscrd_error_set_detailed ("Failed to modify webhook in %s(). ", __func__);
	return 1;
}

int Webhook_move (Webhook *webhook, DiscordChannel *channel) {
	char *json_data = NULL;
	{
		json_t *root = json_object();
		json_object_set_new (root, "channel_id", json_string(channel->id));
		json_data = json_dumps(root, 0);
		json_decref(root);
	}
	int len = strlen ("/webhooks/%s") - 2 + strlen (webhook->id) + 1;
	char buf[len];
	snprintf (buf, len, "/webhooks/%s", webhook->id);
	
	RestResp *res = Rest_call (webhook->discord_client, buf, HTTP_PATCH, json_data);
	free (json_data);

	if (res) {
		if (res->body) {
			Webhook *new = json_discordobject (NULL, DSCRD_WEBHOOK, res->body);
			if (new) {
				if (new->channel_id) {
					strcpy (webhook->channel_id, new->channel_id);
				}
				Webhook_destroy (new);
			}
			else {
				Rest_call_free (res);
				dscrd_error_set_detailed ("Failed to modify webhook in %s(). ", __func__);
				return 1;
			}
		}
		else {
			Rest_call_free (res);
			dscrd_error_set_detailed ("Failed to modify webhook in %s(). ", __func__);
			return 1;
		}
		Rest_call_free (res);
		return 0;
	}
	dscrd_error_set_detailed ("Failed to modify webhook in %s(). ", __func__);
	return 1;
}

WebhookList * Webhooks_in_channel (DiscordClient *client, DiscordChannel * channel) {
	int len = strlen("/channels/%s/webhooks") - 2 + strlen (channel->id) + 1;
	char buf[len];
	sprintf (buf, "/channels/%s/webhooks", channel->id);

	RestResp *res = Rest_call (client, buf, HTTP_GET, NULL);

	WebhookList *temp = NULL;
	if (res) {
		if (res->body) {
			temp = json_WebhookList (client, res->body);
		}
		Rest_call_free (res);
	}
	if (!res) {
		dscrd_error_set_detailed ("Failed to receive webhooks in %s(). ", __func__);
	}
	return temp;
}

WebhookList * Webhooks_in_guild (DiscordClient *client, DiscordGuild *guild) {
	int len = strlen("/guilds/%s/webhooks") - 2 + strlen (guild->id) + 1;
	char buf[len];
	sprintf (buf, "/guilds/%s/webhooks", guild->id);

	RestResp *res = Rest_call (client, buf, HTTP_GET, NULL);

	if (!res) {
		return NULL;
	}
	else if (!res->body) {
		return NULL;
	}
	WebhookList *temp = json_WebhookList (client, res->body);

	Rest_call_free (res);

	return temp;
}

void WebhookList_destroy (WebhookList *list) {
	int i;
	for (i = 0; i < list->count; i++) {
		Webhook_destroy (list->w[i]);
	}
	free (list->w);
	free (list);
}