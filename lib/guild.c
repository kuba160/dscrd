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

DiscordGuild * DiscordGuild_get (DiscordClient *client, char *guild_id) {
	int len = strlen("/guilds/%s") - 2 + strlen (guild_id) + 1;
	char buf[len]; sprintf (buf, "/guilds/%s", guild_id);

	RestResp *res = Rest_call (client, buf, HTTP_GET, NULL);

	if (!res) {
		return NULL;
	}
	else if (!res->body) {
		Rest_call_free (res);
		return NULL;
	}
	DiscordGuild *temp = json_discordobject (client, DSCRD_GUILD, res->body);

	Rest_call_free (res);

	return temp;	
}

DiscordGuild * DiscordGuilds_pop (DiscordGuild **list, int i) {
	int count;
	for (count = 0; list[count]; count++);

	if (i < 0 || i > count) {
		// internal function (as now)
		// dscrd_error_set ()
		return NULL;
	}
	DiscordGuild *ret = list[i];
	memmove (list + (i), (list) + (i+1), sizeof(DiscordGuild *) * (count - i));
	return ret;
}

DiscordGuild * DiscordGuild_by_name (DiscordClient *client, char *guild_name) {
	DiscordGuild **list = DiscordGuilds_get (client);

	int count;
	for (count = 0; list[count]; count++);

	if (list && list[0]) {
		int i;
		for (i = 0; i < count; i++) {
			if (strcmp (list[i]->name, guild_name) == 0) {
				DiscordGuild *ret = DiscordGuilds_pop (list, i);
				DiscordGuilds_destroy (list);
				return ret;
			}
		}
	}
	DiscordGuilds_destroy (list);
	dscrd_error_set ("Guild named \"%s\" not found.", guild_name);
	return NULL;
}


DiscordGuild ** DiscordGuilds_get (DiscordClient *client) {
	RestResp *res = Rest_call (client, "/users/@me/guilds", HTTP_GET, NULL);
	if (res && res->body) {
		DiscordGuild **temp = json_DiscordGuilds (client, res->body);
		Rest_call_free (res);
		return temp;
	}
	return NULL;
}

void DiscordGuilds_destroy (DiscordGuild **guilds) {
	int i;
	for (i = 0; guilds[i]; i++) {
		DiscordGuild_destroy (guilds[i]);
	}
	free (guilds);
}


void DiscordGuild_destroy (DiscordGuild *guild) {
	dscrd_client_process (guild->discord_client, DSCRD_GUILD, DESTROY);
	JTS_FREE(DiscordGuild, guild);
}