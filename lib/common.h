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

// todo
typedef enum dscrd_event_e {
	CREATE, DESTROY
} dscrd_event;

/// todo
const char * dscrd_user_status_str (int status);

/**
 * Process client for specified event like object creation or deletion.
 * Currently this is used for:
 * * incrementing/decreasing refcount for specified objects
 * * destroying temporary @ref DiscordClient
 *
 * @param client Discord session, can be @b NULL
 * @param type @ref dscrd_type of event
 * @param event @ref dscrd_event, for now either @ref CREATE or @ref DESTROY
 */
void dscrd_client_process (DiscordClient *client, dscrd_type type, int event);

/**
 * Process rest error response and set human readable message.
 * Only called from rest.c when HTTP response is in 4XX row.
 * At the time of writing only Bad Request (400) error is parsed. 
 *
 * @param code HTTP response code
 * @param body raw response data
 */
void dscrd_rest_error_parse (int code, const char * body);

/**
 * Sets formatted data to error string. (printf format)
 *
 * @param fmt string with or without <em>format specifiers</em>
 * @param ... <em>sequence of additional arguments</em>
 */
void dscrd_error_set (const char* fmt, ...);

/**
 * Prefixes current error with given formatted data. (printf format)
 *
 * @param fmt string with or without <em>format specifiers</em>
 * @param ... <em>sequence of additional arguments</em>
 */
void dscrd_error_set_detailed (const char* fmt, ...);

/**
 * Dereference @b void pointer
 *
 * @param X pointer type
 * @param Y pointer name
 */
#define PTR(X,Y) ((X *) Y)

#define NO_GATEWAY_RET() if(!dscrd_has_gateway()){dscrd_error_set ("This library has no gateway support."); return -1;}

#define NO_GATEWAY_RET_P() if(!dscrd_has_gateway()){dscrd_error_set ("This library has no gateway support."); return NULL;}


/// @endcond
// PUBLIC_API_BEGIN

#ifndef uint8_t
#include <stdint.h>
#endif
#ifndef bool
#include <stdbool.h>
#endif

/**
 * Discord API version library was compiled for
 *
 */
#define DISCORD_API 6

// needed for types which come before client.h
struct DiscordClient_s;

// http methods already defined by rest library
#ifndef REST_CLIENT_H_
/**
 * @brief HTTP Methods
 *
 * HTTP Methods which can be used to call REST library.
 */
enum http_method {
	HTTP_POST, HTTP_GET, HTTP_PUT, HTTP_DELETE, HTTP_HEAD, HTTP_OPTIONS, HTTP_PATCH
};
#endif

/**
 * @brief Discord datatypes
 *
 * @see dscrd_type_e for detailed element documentation.
 */
typedef enum dscrd_type_e {
	/// 0 element, will be set if uninitialized, therefore ignored
	DSCRD_TYPE_FIRST,
	/// @ref DiscordGuild
	DSCRD_GUILD,
	/// @ref DiscordChannel
	DSCRD_CHANNEL,
	/// @ref DiscordUser
	DSCRD_USER,
	/// @ref Webhook
	DSCRD_WEBHOOK,
	/// @ref DiscordClient
	DSCRD_CLIENT,
	/// @ref RestResp
	DSCRD_REST_RESP,
	// ignoring list types
	/// @ref DscrdFile
	DSCRD_FILE,
	/// @ref DiscordMessage
	DSCRD_MESSAGE,
	/// indicates last value of types
	DSCRD_TYPE_LAST,
	///
	DSCRD_TYPE_LIST = 256
} dscrd_type;

/**
 * Checks if library has gateway support.
 *
 * @returns 0 if library was compiled without gateway support, all gateway functions will fail
 * @returns 1 if library supports Gateway API
 * @see DiscordClient_gateway() - to be called if Gateway API will be used
 */
int dscrd_has_gateway (void);

/**
 * Read last error message.
 *
 * @see dscrd_error_long
 */
const char * dscrd_error (void);

/**
 * Read last error message.
 * In comparison to @ref dscrd_error() this function will show in what function error happened.
 *
 * @see dscrd_error
 */
const char * dscrd_error_long (void);

/**
 * Overwrite library API version dscrd uses.
 * For current Discord API version look https://discordapp.com/developers/docs/reference#api-versioning-api-versions
 * @param api API version number
 * @see DISCORD_API
 */
void dscrd_overwrite_api (int api);

/**
 * Initialize dscrd library. You should call this function before calling any other function.
 * 
 * @returns 0 on success
 * @see dscrd_terminate
 * @note At the time of writing this function always returns 0.
 */
int dscrd_initialize (void);

/**
 * Terminate dscrd library. Call this function if you're done with dscrd library. 
 * You should not call any library function after this function.
 * 
 * @returns 0 on success
 * @see dscrd_initialize
 * @note At the time of writing this function always returns 0.
 */
int dscrd_terminate (void);

//
// Things which hard to assign somewhere else...
//

enum dscrd_verification_level {
	/// unrestricted
	NONE,
	/// must have verified email on account
	LOW,
	/// must be registered on Discord for longer than 5 minutes
	MEDIUM,
	/// must be a member of the server for longer than 10 minutes
	HIGH,
	/// must have a verified phone number
	VERY_HIGH
};

/**
 * @brief Permissions also described at https://discordapp.com/developers/docs/topics/permissions
 */
typedef enum permissions_e {
	/// Allows creation of instant invites
	CREATE_INSTANT_INVITE	=	0x00000001,
	/// Allows kicking members
	KICK_MEMBERS			=	0x00000002,
	/// Allows banning members
	BAN_MEMBERS				=	0x00000004,
	/// Allows all permissions and bypasses channel permission overwrites
	ADMINISTRATOR			=	0x00000008,
	/// Allows management and editing of channels
	MANAGE_CHANNELS			=	0x00000010,
	/// Allows management and editing of the guild
	MANAGE_GUILD			=	0x00000020,
	/// Allows for the addition of reactions to messages
	ADD_REACTIONS			=	0x00000040,
	/// Allows for viewing of audit logs
	VIEW_AUDIT_LOG			=	0x00000080,
	/// Allows guild members to view a channel, which includes reading messages in text channels
	VIEW_CHANNEL			=	0x00000400,
	/// Allows for sending messages in a channel
	SEND_MESSAGES			=	0x00000800,
	/// Allows for sending of /tts messages
	SEND_TTS_MESSAGES		=	0x00001000,
	/// Allows for deletion of other users messages
	MANAGE_MESSAGES			=	0x00002000,
	/// Links sent by users with this permission will be auto-embedded
	EMBED_LINKS				=	0x00004000,
	/// Allows for uploading images and files
	ATTACH_FILES			=	0x00008000,
	/// Allows for reading of message history
	READ_MESSAGE_HISTORY	=	0x00010000,
	/// Allows for using the \@everyone tag to notify all users in a channel, and the \@here tag to notify all online users in a channel
	MENTION_EVERYONE		=	0x00020000,
	/// Allows the usage of custom emojis from other servers
	USE_EXTERNAL_EMOJIS		=	0x00040000,
	/// Allows for joining of a voice channel
	CONNECT					=	0x00100000,
	/// Allows for speaking in a voice channel
	SPEAK					=	0x00200000,
	/// Allows for muting members in a voice channel
	MUTE_MEMBERS			=	0x00400000,
	/// Allows for deafening of members in a voice channel
	DEAFEN_MEMBERS			=	0x00800000,
	/// Allows for moving of members between voice channels
	MOVE_MEMBERS			=	0x01000000,
	/// Allows for using voice-activity-detection in a voice channel
	USE_VAD					=	0x02000000,
	/// Allows for using priority speaker in a voice channel
	PRIORITY_SPEAKER		=	0x00000100,
	/// Allows for modification of own nickname
	CHANGE_NICKNAME			=	0x04000000,
	/// Allows for modification of other users nicknames
	MANAGE_NICKNAMES		=	0x08000000,
	/// Allows management and editing of roles
	MANAGE_ROLES			=	0x10000000,
	/// Allows management and editing of webhooks
	MANAGE_WEBHOOKS			=	0x20000000,
	/// Allows management and editing of emojis
	MANAGE_EMOJI			=	0x40000000
} permissions;

#define ID(X) X ## "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

// PUBLIC_API_END
