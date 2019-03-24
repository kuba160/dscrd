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
 * @brief Discord channel.
 *
 * @see DiscordChannel_s for detailed element documentation.
 * @todo detailed element documentation
 */
typedef struct DiscordChannel_s {
	/// channel ID
	char id[21];
	/// reserved for type detection
	uint8_t _type;
	/// channel type, one of @ref channel_types
	int type;
	/// guild ID channel belongs to
	char guild_id[21];
	/// "sorting position of the channel"
	int position;
	// todo permission overwrites
	char *name;
	char *topic;

	bool nsfw;
	char last_message_id[21];

	int bitrate;
	int user_limit;
	// recipients?
	char *icon;
	char owner_id[21];
	char application_id[21];
	char parent_id[21];
	char *last_pin_timestamp;

	DiscordClient *discord_client;
} DiscordChannel;

typedef struct DiscordChannelList_s {
	int count;
	DiscordChannel **c;
} DiscordChannelList;

/**
 * @brief Channel types
 *
 * Possible types to be found in @ref DiscordChannel.type
 */
enum channel_types {
	GUILD_TEXT, DM, GUILD_VOICE, GROUP_DM, GUILD_CATEGORY
};


/**
 * Gets @ref DiscordChannel after specified channel id.
 * You can get channel id manually by enabling discord developer mode (Settings/Apperance/Developer Mode), and right-clicking desired channel
 *
 * Returned pointer has to be freed with @ref DiscordChannel_destroy.
 * @param client @ref DiscordClient session
 * @param channel_id channel ID
 * @returns pointer to @ref DiscordChannel on success
 * @returns @b NULL on failure
 * @note Friendly reminder: Look forward to not hard-code any channel id in your code.
 */
DiscordChannel * DiscordChannel_get (DiscordClient *client, char *channel_id);

/**
 * Gets @ref DiscordChannel after specified channel id.
 * In comparison to @ref DiscordChannel, this will not load any external information about the channel from Discord servers.
 *
 * Returned pointer has to be freed with @ref DiscordChannel_destroy.
 * @param client @ref DiscordClient session
 * @param channel_id channel ID
 * @returns pointer to @ref DiscordChannel on success
 * @returns @b NULL on failure
 * @see DiscordChannel_get
 */
DiscordChannel * DiscordChannel_get_fast (DiscordClient *client, char *channel_id);

/**
 * Modifies @ref DiscordChannel specified values.
 * Changes are listed in pairs also: "key",(value), ...
 * @param channel @ref DiscordChannel pointer
 * @param ... @b key {"name", ...}
 * @param ... @b value (type depends on key)
 * @param ...
 * @returns 0 on success
 * @returns NOT 0 on failure
 * @todo write this function :)
 */
int DiscordChannel_modify (DiscordChannel *channel, ...);

/**
 * Gets @ref DiscordChannel after specified channel name in specified guild.
 *
 * Returned pointer has to be freed with @ref DiscordChannel_destroy.
 * @param guild @ref DiscordGuild pointer
 * @param channel_name channel name in guild
 * @returns pointer to @ref DiscordChannel on success
 * @returns @b NULL on failure
 */
DiscordChannel * DiscordChannel_by_name (DiscordGuild *guild, char *channel_name);

/**
 * Sends text message to given channel.
 *
 * @param channel @ref DiscordChannel pointer
 * @param message message string
 * @returns 0 on success
 * @returns NOT 0 on failure
 * @see DiscordMessage_new_simple
 */
int DiscordChannel_send_message_simple (DiscordChannel *channel, const char *message);

/**
 * Gets @ref DiscordChannelList for current user.
 * Returned pointer has to be freed with @ref DiscordChannelList_destroy.
 * @param guild @ref DiscordGuild pointer
 * @returns pointer to @ref DiscordChannelList on success
 * @returns @b NULL on failure
 */
DiscordChannelList * DiscordChannels_in_guild (DiscordGuild *guild);

/**
 * Removes specified element from @ref DiscordChannelList and returns it.
 *
 * @param list @ref DiscordChannelList
 * @param i element number
 * @returns @ref DiscordChannel pointer
 */
DiscordChannel * DiscordChannelList_pop (DiscordChannelList *list, int i);

/**
 * Frees @ref DiscordChannelList pointer.
 * @param channels @ref DiscordChannelList pointer
 */
void DiscordChannelList_destroy (DiscordChannelList *channels);

/**
 * Frees @ref DiscordChannel pointer.
 * @param channel @ref DiscordChannel pointer
 */
void DiscordChannel_destroy (DiscordChannel * channel);

// PUBLIC_API_END
