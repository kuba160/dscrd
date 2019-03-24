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

const char * dscrd_user_status_str (int status) {
	switch (status) {
		case STATUS_ONLINE:
			return "online";
		case STATUS_DND:
			return "dnd";
		case STATUS_IDLE:
			return "idle";
		case STATUS_INVISIBLE:
			return "invisible";
		case STATUS_OFFLINE:
			return "offline";
		default:
			// make this function always return a string for safety
			return "offline";
	}
}

DiscordUser * DiscordUser_me (DiscordClient *client) {
	return DiscordUser_get (client, "@me");
}

DiscordUser * DiscordUser_get (DiscordClient *client, char * user_id) {
	char buf[strlen(user_id) + strlen ("users/%s")];
	sprintf (buf, "users/%s", user_id);
	RestResp *res = Rest_call (client, buf, HTTP_GET, NULL);

	if (res && res->body) {
		DiscordUser *temp = JTS(DiscordUser, res->body);
		if (temp) {
			dscrd_client_process (client, DSCRD_USER, CREATE);
		}
		Rest_call_free (res);
		return temp;
	}
	return NULL;
}

void DiscordUser_destroy (DiscordUser *user) {
	dscrd_client_process (user->discord_client, DSCRD_USER, DESTROY);
	JTS_FREE(DiscordUser, user);
}