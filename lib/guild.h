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
 * @brief Discord guild.
 *
 * @see DiscordGuild_s for detailed element documentation.
 */
typedef struct DiscordGuild_s {
	/// guild id
	char id[21];
	//
	uint8_t _type;
	/// guild name
	char * name;
	/// icon hash
	char * icon;
	/// splash hash (?)
	char * splash;
	/// whether or not the user is the owner of the guild
	bool owner;
	/// id of owner
	char owner_id[21];
	/// total permissions for the user in the guild (does not include channel overrides)
	int permissions;
	/// voice region id for the guild
	char * region;
	/// id of afk channel
	char afk_channel_id[21];
	/// afk timeout in seconds
	int afk_timeout;
	/// is this guild embeddable (e.g. widget)
	bool embed_enabled;
	/// id of embedded channel
	char embed_channel_id[21];
	/// verification level required for the guild
	int verification_level;
	/// default message notifications level
	int default_message_notifications;
	/// explicit content filter level
	int explicit_content_filter;
	/// roles in the guild
	/*TODO*/ //
	/// emojis
	/*TODO*/ //
	/// features
	char **features;
	/// required MFA level for the guild
	int mfa_level;
	/// application id of the guild creator if it is bot-created
	char application_id[21];
	/// whether or not the server widget is enabled
	bool widget_enabled;
	/// the channel id for the server widget
	char widget_channel_id[21];
	/// the id of the channel to which system messages are sent
	char system_channel_id[21];
	// These fields are only sent within the GUILD_CREATE event:
	/*
	joined_at? *	ISO8601 timestamp	when this guild was joined at
	large? *	bool	whether this is considered a large guild
	unavailable? *	bool	is this guild unavailable
	member_count? *	integer	total number of members in this guild
	voice_states? *	array of partial voice state objects	(without the guild_id key)
	members? *	array of guild member objects	users in the guild
	channels? *	array of channel objects	channels in the guild
	presences? *
	*/
	struct DiscordClient_s *discord_client;
} DiscordGuild;

/**
 * Gets @ref DiscordGuild after specified guild id.
 * You can get guild id manually by enabling discord developer mode (Settings/Apperance/Developer Mode), and right-clicking desired guild.
 * Returned pointer has to be freed with @ref DiscordGuild_destroy.
 * @param client @ref DiscordClient session
 * @param guild_id guild ID
 * @returns pointer to @ref DiscordGuild on success
 * @returns @b NULL on failure
 */
DiscordGuild * DiscordGuild_get (DiscordClient *client, char *guild_id);

/**
 * Gets @ref DiscordGuild after specified guild name.
 * Returned pointer has to be freed with @ref DiscordGuild_destroy.
 * @param client @ref DiscordClient session
 * @param guild_name guild name
 * @returns pointer to @ref DiscordGuild on success
 * @returns @b NULL on failure
 */
DiscordGuild * DiscordGuild_by_name (DiscordClient *client, char *guild_name);

/**
 * Gets @ref DiscordGuild ** for current user.
 * As stated in discord API documentation it will return maximally 100 guilds.
 * Returned pointer has to be freed with @ref DiscordGuilds_destroy.
 * @param client @ref DiscordClient session
 * @returns @ref DiscordGuild ** on success
 * @returns @b NULL on failure
 */
DiscordGuild ** DiscordGuilds_get (DiscordClient *client);

/**
 * Frees @ref DiscordGuild pointer array.
 * @param guild @ref DiscordGuild array
 */
void DiscordGuilds_destroy (DiscordGuild **guilds);

/**
 * Frees @ref DiscordGuild pointer.
 * @param guild @ref DiscordGuild pointer
 */
void DiscordGuild_destroy (DiscordGuild *guild);

// PUBLIC_API_END
