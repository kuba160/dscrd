/*
 *     dscrd/gateway: Gateway API wrapper
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

/// @cond INTERNAL

// todo
void * DiscordGateway_open (DiscordClient *client);

// todo
void DiscordGateway_close (DiscordClient *client);

void DiscordGateway_send (DiscordClient *client, const char *message);

void DiscordGateway_callback_remove (void *gateway, int i);


/// @endcond
// PUBLIC_API_BEGIN

struct DiscordGuild_s;


/**
 * Loads lazyloaded @ref DiscordGuild.
 * Returned pointers will be freed automatically on gateway close.
 * @param client @ref DiscordClient session
 * @returns pointer to @ref DiscordGuild array on success
 * @returns @b NULL on failure
 */
struct DiscordGuild_s ** DiscordClient_lazy_guilds (DiscordClient *client);


typedef struct GatewayResp_s {
	/// response type
	char *event;
	/// data pointer
	void *data;
	/// type of d pointer, one of @ref dscrd_type_e
	int data_type;
	/// tell callback we're finished, free all memory etc.
	unsigned char terminate;
	/// pointer to provided userdata
	void *userdata;
} GatewayResp;

enum gateway_callback_resp {
	CALLBACK_OK,
	CALLBACK_ABORT
};


int DiscordGateway_callback (DiscordClient *client, char *event, int (*func)(GatewayResp *), void *userdata);

// PUBLIC_API_END