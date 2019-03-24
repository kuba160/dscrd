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
 * Type of the token
 *
 */
typedef enum token_type_e {
	Bearer, Bot, User, Anonymous
} token_type;

#define TOKEN_TO_STR(X) (X == Bearer) ? "Bearer" : (X == Bot) ? "Bot" : (X == User) ? "User" : ""

/**
 * @brief Discord session.
 *
 * You should not access any values except @ref DiscordClient.me / @ref DiscordClient.user and @ref DiscordClient.api
 *
 * @see DiscordClient_s for detailed element documentation.
 * @see DiscordClient_open for getting such session.
 */
typedef struct DiscordClient_s {
	/// reserved
	char _name[21];
	//
	uint8_t _type;
	/// Pointer used internally by rest library, it's casted default to void except in library.
	void *c;
	/// Discord API version used in session.
	/// You can overwrite this by calling @ref dscrd_overwrite_api before creating session.
	unsigned char api;
	/// Session type, one of @ref token_type_e.
	token_type type;
	/// pure token
	char * token;
	/// Authentication header used by rest library.
	char * auth_string;

	/// @ref DiscordUser_s structure of current user; equal to @ref DiscordClient.user except it's a data structure.
	struct DiscordUser_s me;
	/// @ref DiscordUser_s pointer of current user; equal to @ref DiscordClient.me except it's a pointer.
	struct DiscordUser_s *user;

	/// current user status, one of @ref dscrd_user_status
	unsigned char user_status;

	/// tells if created @ref DiscordClient is temporary and has to be freed when connected object is freed.
	bool temporary;
	// ref-count, numebers equal entries dscrd_type_e
	short refc[DSCRD_TYPE_LAST];
	/// pointer to DiscordGateway session (if any)
	void *gateway;
	/// flags
	int gateway_flags;
} DiscordClient;

/**
 * Start Discord session. Every session is bound to specified user/bot.
 * Authentication credentials are passed to this function.
 * @ref DiscordClient is needed for most of the dscrd library functions.
 * After you're done with your session you need to free @ref DiscordClient using @ref DiscordClient_close.
 *
 * @param type type of the token you're passing, one of @ref token_type_e
 * @param token pointer to token string
 * @returns @ref DiscordClient* on success
 * @returns @b NULL on failure
 * @see DiscordClient_close
 */
DiscordClient * DiscordClient_open (token_type type, const char * token);

/**
 * Same as @ref DiscordClient_open but takes @ref DscrdAuth as an argument. It's a macro.
 * @param X pointer to @ref DscrdAuth
 * @returns @ref DiscordClient* on success
 * @returns @b NULL on failure
 * @see DiscordClient_open dscrdd_auth
 */
#define DiscordClient_open_auth(X) DiscordClient_open((X)->type, (X)->token)

/**
 * Close Discord session. You should free any resources allocated with given @ref DiscordClient before you close it.
 * dscrd traces allocations and will notify you if you did not free all resources.
 *
 * @param d pointer to @ref DiscordClient session to close
 * @see DiscordClient_open
 */
void DiscordClient_close (DiscordClient *d);

enum gateway_flags {
	LAZYLOAD_GUILDS = 1,
	// last flags
	GATEWAY_FLAGS_LAST = 2
};

/**
 * Enable Gateway API for specific client.
 * It will try to connect and authenticate to the Discord Gateway API.
 *
 * @param client @ref DiscordClient session
 * @param flags Gateway flags, ORed values of @ref gateway_flags
 * @returns 0 on success
 * @returns NOT 0 on failure
 *
 * @see dscrd_has_gateway()
 */
int DiscordClient_gateway (DiscordClient *client, int flags);

/**
 * Set status for @ref DiscordClient
 *
 * @returns 0 on success
 * @returns NOT 0 on failure
 *
 * @see dscrd_has_gateway()
 * @note This function requires @ref DiscordClient_gateway to be enabled.
 */
int DiscordClient_set_status (DiscordClient *client, enum dscrd_user_status status);

/**
 * Set game for @ref DiscordClient
 *
 * @returns 0 on success
 * @returns NOT 0 on failure
 *
 * @see dscrd_has_gateway()
 * @note This function requires @ref DiscordClient_gateway to be enabled.
 */
int DiscordClient_set_game (DiscordClient *client, DiscordActivity *activity);

// PUBLIC_API_END