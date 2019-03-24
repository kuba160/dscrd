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

// @cond INTERNAL

/**
 * Parse json string into object based on given @ref dscrd_type
 *
 * @param client Discord session
 * @param type @ref dscrd_type object
 * @param json json string
 * @returns pointer to object
 */
void * json_discordobject (DiscordClient *client, dscrd_type type, char *json);

/**
 * Parse json string into @ref WebhookList object
 *
 * @param client Discord session
 * @param json json string
 * @returns @ref WebhookList pointer
 * @todo migrate into @ref json_discordobject
 */
WebhookList * json_WebhookList (DiscordClient * client, char *json);


DiscordGuild ** json_DiscordGuilds (DiscordClient * client, char *json);

DiscordChannelList * json_DiscordChannelList (DiscordClient * client, char *json);

// @endcond