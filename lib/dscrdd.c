/*
 *     dscrdd: dscrd daemon connect
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

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dscrd.h"
#include "dscrd_i.h"

int dscrdd_auth (DscrdAuth *auth, const char *app_name) {
    const char *xdg_runtime = getenv ("XDG_RUNTIME_DIR");
    if (!xdg_runtime) {
      dscrd_error_set ("Failed to get XDG_RUNTIME_DIR.\n");
      return -1;
    }

    char *socket_path;
    char socket_path_a[strlen(xdg_runtime) + strlen("/dscrdd/socket")];
    strcpy (socket_path_a, xdg_runtime);
    strcat (socket_path_a, "/dscrdd/socket");
    socket_path = socket_path_a;

    int fd;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        dscrd_error_set("Socket error.");
        return -1;
    }

    struct sockaddr_un addr;
    memset (&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        dscrd_error_set("Socket connect error.");
        return -2;
    }

    // connected, authenticate
    int ret = write(fd, app_name, strlen(app_name)+1);
    if (ret != strlen(app_name)+1) {
        dscrd_error_set ("Socket write error.");
        close (fd);
    }

    char buffer[128];
    int retc = read(fd,buffer,sizeof(buffer));
    strcpy (auth->token, buffer);

    char *p = strrchr (buffer, 0);
    if (p) {
        p++;
        if (p < (p+retc)) {
			if (strcmp(p, "Bot") == 0) {
				auth->type = Bot;
			}
			else if (strcmp(p, "Bearer") == 0) {
				auth->type = Bearer;
			}
			else if (strcmp(p, "User") == 0) {
				auth->type = User;
			}
        }
    }

    close (fd);

    if (auth->token[0] == 0) {
        dscrd_error_set ("Error in dscrdd_auth (TODO).");
        return -1;
    }

	return 0;
}
