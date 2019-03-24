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

/// User status
enum dscrd_user_status {
	/// Online
	STATUS_ONLINE,
	/// Do not disturb
	STATUS_DND,
	/// AFK
	STATUS_IDLE,
	/// Shown as offline
	STATUS_INVISIBLE,
	/// Offline
	STATUS_OFFLINE
};

/**
 * @brief Discord user.
 *
 * @see DiscordUser_s for detailed element documentation.
 */
typedef struct DiscordUser_s {
	/// the user's id identify
	char id[21];
	/// 
	uint8_t _type;
	/// username, not unique across platform identify
	char * username;
	/// user's 4-digit discord-tag identify
	char discriminator[5];
	/// avatar hash
	char *avatar;
	/// whether the user belongs to an OAuth2 application identify
	bool bot;
	/// whether the user has two factor enabled on their account identify
	bool mfa_enabled;
	/// whether the email on this account has been verified email
	bool verified;
	/// the user's email
	char *email;

	struct DiscordClient_s *discord_client;
} DiscordUser;

/**
 * Get current user information.
 * Returned pointer has to be freed with @ref DiscordUser_destroy.
 * @param client @ref DiscordClient session
 * @returns pointer to @ref DiscordUser_s on success
 * @returns @b NULL on failure
 * @deprecated dscrd gets current user information when opening new client.
 * @deprecated You can access it through @ref DiscordClient.me or @ref DiscordClient.user pointer.
 */
DiscordUser * DiscordUser_me (struct DiscordClient_s *client);

/**
 * Get user information after user id.
 * Returned pointer has to be freed with @ref DiscordUser_destroy.
 * @param client @ref DiscordClient session
 * @param user_id user ID string
 * @returns pointer to @ref DiscordUser_s on success
 * @returns @b NULL on failure
 */
DiscordUser * DiscordUser_get (struct DiscordClient_s *client, char * user_id);
/**
 * Frees DiscordUser pointer.
 * @param user @ref DiscordUser_s pointer
 */
void DiscordUser_destroy (DiscordUser *user);

// PUBLIC_API_END
