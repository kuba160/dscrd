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
 * @brief Activity types
 *
 * @note Bots are only able to send name, type, and optionally url.
 * @see DiscordActivity_s for detailed element documentation
 */
enum activity_types {
	/// Playing ...
	ACTIVITY_GAME,
	/// Streaming ...
	ACTIVITY_STREAMING,
	/// Listening to ...
	ACTIVITY_LISTENING
};

/**
 * @brief Activity object (game)
 *
 * @note Bots are only able to send name, type, and optionally url.
 * @see DiscordActivity_s for detailed element documentation
 */
typedef struct DiscordActivity_s {
	/// application id for the game
	char application_id[21];
	/// reserved
	uint8_t _type;
	/// guild id of webhook
	/// the activity's name
	char *name;
	/// activity type
	int type;
	/// stream url, required if type is @ref ACTIVITY_STREAMING, only twitch.tv supported
	char *url;
	/// TODO timestamp_x -> timestamps
	/// unix timestamps for start and/or end of the game
	int timestamp_start;

	int timestamp_stop;

	/// what the player is currently doing
	char *details;
	/// the user's current party status
	char *state;

	// information for the current party of the player
	// party object

	// images for the presence and their hover texts
	// assets object

	// secrets for Rich Presence joining and spectating
	// secrets object

	/// whether or not the activity is an instanced game session
	bool instance;
	/// activity flags ORd together, describes what the payload includes
	int flags;
} DiscordActivity;
