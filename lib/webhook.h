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

/// @cond INTERNAL
/// @endcond
// PUBLIC_API_BEGIN

/**
 * @brief Webhook structure.
 *
 * @see Webhook_s for detailed element documentation.
 */
typedef struct Webhook_s {
	// id is in snowflake format, 64-bit -> uint64 -> strlen(uint64_max) == 20 bytes + terminating char
	/// id of the webhook
	char id[21];
	//
	uint8_t _type;
	/// guild id of webhook
	char guild_id[21];
	/// channel id for this webhook
	char channel_id[21];
	/// user object	the user this webhook was created by (not returned when getting a webhook with its token)
	DiscordUser user;
	/// same as user but pointer
	DiscordUser *user_p;
	/// the default name of the webhook (optional)
	char * name;
	/// the default avatar of the webhook (optional)
	char * avatar;
	/// the secure token of the webhook
	char token[128];
	/// prefer to use token instead of auth
	bool use_token;
	/// DiscordClient of the Webhook
	DiscordClient *discord_client;
} Webhook;

/**
 * @brief WebhookList structure.
 *
 * @see WebhookList_s for detailed element documentation.
 */
typedef struct WebhookList_s {
	int count;
	Webhook **w;
} WebhookList;

/**
 * Gets @ref Webhook pointer after webhook id.
 * If you have Webhook url you can extract ID by using following schema:
 * @code https://discordapp.com/api/webhooks/{ID}/{TOKEN} @endcode
 * Returned pointer has to be freed with @ref Webhook_destroy.
 * @param client @ref DiscordClient session
 * @param id webhook ID
 * @returns pointer to @ref Webhook on success
 * @returns @b NULL on failure
 * @see Webhook_get_no_auth
 */
Webhook * Webhook_get (DiscordClient *client, char * id);

/**
 * Gets @ref Webhook pointer after webhook id and token.
 * No DiscordClient needed.
 * If you have Webhook url you can extract ID and TOKEN by using following schema:
 * @code https://discordapp.com/api/webhooks/{ID}/{TOKEN} @endcode
 * Returned pointer has to be freed with @ref Webhook_destroy.
 * @param id webhook ID
 * @param token webhook TOKEN
 * @returns pointer to @ref Webhook on success
 * @returns @b NULL on failure
 * @see Webhook_get_no_auth_url
 */
Webhook * Webhook_get_no_auth (char * id, char * token);

/**
 * Gets @ref Webhook pointer after webhook url.
 * No DiscordClient needed.
 * Returned pointer has to be freed with @ref Webhook_destroy.
 * @param url webhook url
 * @returns pointer to @ref Webhook on success
 * @returns @b NULL on failure
 */
Webhook * Webhook_get_no_auth_url (const char * url);

/**
 * Creates new webhook in given channel.
 * Returned pointer has to be freed with @ref Webhook_destroy.
 * @param channel @ref DiscordChannel pointer
 * @param name webhook name
 * @param avatar avatar data
 * @returns pointer to @ref Webhook on success
 * @returns @b NULL on failure
 */
Webhook * Webhook_create (DiscordChannel * channel, char * name, DscrdFile * avatar);

/**
 * Frees @ref Webhook pointer.
 * @param webhook @ref Webhook pointer
 */
void Webhook_destroy (Webhook * webhook);

/**
 * Execute a @ref Webhook (send a message).
 * @param webhook @ref Webhook pointer
 * @param message text content to be sent
 * @param username override default webhook name, can be @b NULL
 * @param avatar_url avatar URL, can be @b NULL
 * @returns 0 on success
 * @returns NOT 0 on failure
 * @see Webhook_execute_simple
 */
int Webhook_execute (Webhook * webhook, const char * message, const char * username, const char * avatar_url);

/**
 * Execute a @ref Webhook and send a message.
 * In comparison to @ref Webhook_execute this function does not allow to change username or avatar
 * @param webhook @ref Webhook pointer
 * @param message text content to be sent
 * @returns 0 on success
 * @returns NOT 0 on failure
 */
int Webhook_execute_simple (Webhook * webhook, const char * message);

/**
 * Deletes @ref Webhook from Discord servers.
 * 
 * @param webhook @ref Webhook pointer
 * @returns 0 on success
 * @returns NOT 0 on failure
 * @warning This function does not free @ref Webhook pointer. To free @ref Webhook pointer use @ref Webhook_destroy.
 * @n
 * @warning This function is not reversible.
 */
int Webhook_delete (Webhook * webhook);

/**
 * Modifies @ref Webhook values.
 * 
 * @param webhook @ref Webhook pointer
 * @param name new name
 * @param avatar @ref DscrdFile pointer to avatar
 * @returns 0 on success
 * @returns NOT 0 on failure
 * @see Webhook_move to move webhook to different channel
 */
int Webhook_modify (Webhook *webhook, const char *name, DscrdFile *avatar);

/**
 * Moves @ref Webhook into different channel.
 * 
 * @param webhook @ref Webhook pointer
 * @param channel @ref DiscordChannel pointer to new channel
 * @returns 0 on success
 * @returns NOT 0 on failure
 */
int Webhook_move (Webhook *webhook, DiscordChannel *channel);

/**
 * Returns list over Webhooks in specified channel.
 * Returned pointer has to be freed with @ref WebhookList_destroy.
 * @param client @ref DiscordClient session
 * @param channel @ref DiscordChannel pointer
 * @returns pointer to @ref WebhookList on success
 * @returns @b NULL on failure
 */
WebhookList * Webhooks_in_channel (DiscordClient *client, DiscordChannel * channel);

/**
 * Returns list over Webhooks in specified guild.
 * Returned pointer has to be freed with @ref WebhookList_destroy.
 * @param client @ref DiscordClient session
 * @param guild @ref DiscordGuild pointer
 * @returns pointer to @ref WebhookList on success
 * @returns @b NULL on failure
 */
WebhookList * Webhooks_in_guild (DiscordClient *client, DiscordGuild *guild);

/**
 * Frees @ref WebhookList pointer.
 * @param list @ref WebhookList pointer
 */
void WebhookList_destroy (WebhookList *list);

// PUBLIC_API_END