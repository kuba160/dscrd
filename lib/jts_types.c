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

#include "dscrd.h"
#include "dscrd_i.h"

JTS_D(DiscordUser,
id,
username,
discriminator,
avatar,
bot,
mfa_enabled,
verified
);

JTS_D(Webhook,
name,
channel_id,
token,
avatar,
guild_id,
id,
user
);

JTS_D_EX(Webhook,
user, DiscordUser
);

JTS_D(DiscordChannel,
id,
type,
guild_id,
position,
name,
nsfw,
last_message_id,
bitrate,
user_limit,
icon,
owner_id,
application_id,
parent_id,
last_pin_timestamp
);

JTS_D(DiscordGuild,
id,
name,
icon,
splash,
owner,
owner_id,
permissions,
region,
afk_channel_id,
afk_timeout,
embed_enabled,
embed_channel_id,
verification_level,
default_message_notifications,
explicit_content_filter,
mfa_level,
application_id,
widget_enabled,
widget_channel_id,
system_channel_id
);

JTS_D(DiscordChannelList,
c
);

JTS_D_EX(DiscordChannelList,
c, DiscordChannel
);

JTS_D(DiscordMessage,
	id,
	channel_id,
	author,
	content,
	timestamp,
	edited_timestamp,
	tts,
	mention_everyone,
	mentions,
	nonce,
	pinned,
	webhook_id,
	type
	);

JTS_D_EX(DiscordMessage,
	author, DiscordUser,
	mentions, DiscordUser);