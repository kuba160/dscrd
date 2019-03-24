/*
 *     dscrd/dscrdd: daemon connection
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
 * @ref DscrdAuth structure
 * @see DscrdAuth_s
 */
typedef struct DscrdAuth_s {
	char token[64];
	token_type type;
} DscrdAuth;


/**
 * Try to connect to dscrdd (dscrd daemon) to receive credentials for given app.
 *
 * @param auth pointer to @ref DscrdAuth to write data into
 * @param app_name name of the application
 * @returns 0 on success
 * @returns NOT 0 on failure
 * @todo It's possible that in the future app_name can take password and/or instance number
 * @see dscrdd
 */
int dscrdd_auth (DscrdAuth *auth, const char *app_name);

// PUBLIC_API_END