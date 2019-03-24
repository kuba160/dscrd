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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <jansson.h>

#include "../lib/dscrd.h"

#include "dscrd_daemon.h"

char *socket_path = 0;
char *socket_dir;

int verbose = 0;
int terminated = 0;

#define TOKEN_TO_STR(X) (X == Bearer) ? "Bearer" : (X == Bot) ? "Bot" : (X == User) ? "User" : ""

#define trace(...) {verbose ? printf (__VA_ARGS); : ;}

unsigned char def_disabled = 0;
char def_token[64];
unsigned char def_type;
char * def_tokens[32] = {NULL};
unsigned char def_tokens_pos = 0;

int dscrdd_connection (int fd, char *ehlo) {
	if (verbose) {
		printf ("dscrdd_connection[%d]: %s\n", fd, ehlo);
	}
	// todo selection and process
	int ret_def_token = 1;
	char *at_pos = strrchr (ehlo, '@');
	int token_num = 0;
	if (at_pos) {
		sscanf(at_pos+1, "%d", &token_num);
	}

	char *tk;
	char *tk_t;
	if (ret_def_token) {
		if (token_num == 0) {
			tk = def_token;
			tk_t = TOKEN_TO_STR(def_type);
		}
		else if (token_num-1 < def_tokens_pos) {
			tk = def_tokens[token_num - 1];
			tk_t = TOKEN_TO_STR(def_type);
		}
		else {
			tk = "";
			tk_t = "";
		}
	}

    int ret = write(fd, tk, strlen(tk)+1);
    if (ret != strlen(tk)+1) {
        return -1;
    }
    ret = write(fd, tk_t, strlen(tk_t)+1);
    if (ret != strlen(tk_t)+1) {
		return -1;
    }
    return 0;
}

int dscrdd_command (int fd, char *cmd) {
	if (verbose) {
		printf ("cmd[%d]: %s \n", fd, cmd);
	}

	if (strcmp(cmd, "quit") == 0) {
		terminated = 1;
		close (fd);
	}
	else if (strncmp(cmd, "available", strlen("available")) == 0) {
		int count = 0;
		if (cmd[strlen("available")] == 0) {
			if (!def_disabled) {
				if (def_token[0] != 0) {
					count++;
				}
				int i;
				for (i = 0; def_tokens[i]; i++);
				count += i;
			}
		}

		char buf[64];
		snprintf (buf, 64, "Available tokens: %d.\n", count);
		socket_send (fd, buf);
	}
	return 0;
}

int config_load (char * file) {
	json_error_t error;
	json_t *root = json_load_file (file, 0, &error);

	if (!root) {
		printf ("error loading config\n");
		// todo error msg
	}

	json_t *jp = json_object_get(root, "default_enabled");
	if ( !jp || (jp && (json_boolean_value(jp) == 0))) {
		def_token[0] = 0;
		def_disabled = 1;
	}
	else {
		jp = json_object_get(root, "default_token");
		const char *p = json_string_value(jp);
		if (p)
			strcpy (def_token, p);
		jp = json_object_get(root, "default_token_type");
		p = json_string_value(jp);
		if (p) {
			if (strcmp(p, "Bot") == 0) {
				def_type = Bot;
			}
			else if (strcmp(p, "Bearer") == 0) {
				def_type = Bearer;
			}
			else if (strcmp(p, "User") == 0) {
				def_type = User;
			}
		}

		jp = json_object_get(root, "default_additional_tokens");
		if (jp) {
			// todo check array type
			int max = json_array_size (jp);
			int i;
			json_t *ij;
			for (i = 0; i < max; i++) {
				ij = json_array_get(jp, i);
				if (ij) {
					const char *s = json_string_value(ij);
					if (s) {
						def_tokens[def_tokens_pos++] = strdup (s);
					}
				}
			}

		}
	}

	json_decref (root);

	return 0;
}

int main(int argc, char *argv[]) {

	char *config_file = NULL;
	int socket_connected = 0;
	int start_daemon = 1;
	int read_response = 0;
	int cmd = 1;
	if (argc > 1) {
		int fd;
		int i;
		for (i = 1; i <= argc-1; i++) {
			char *msg = NULL;
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
				printf ("dscrdd: dscrd daemon\n");
				printf ("Usage: %s [options]\n", argv[0]);
				printf ("\t--quit or -q - terminate the daemon\n");
				printf ("\t--verbose or -v - verbose log output\n");
				printf ("\t--config [FILE] or -c [FILE] - load config file\n");
				printf ("\t--available or -a - return number of available tokens\n");
				return 0;
			}
			else if (strcmp(argv[i],"--quit") == 0 || strcmp(argv[i],"-q") == 0) {
				msg = "quit";
				start_daemon = 0;
			}
			else if (strcmp(argv[i],"--verbose") == 0 || strcmp(argv[i],"-v") == 0) {
				verbose = 1;
			}
			else if (strcmp(argv[i],"--config") == 0 || strcmp(argv[i],"-c") == 0) {
				i++;
				if (i > argc-1 || argv[i][0] == '-') {
					printf ("Bad or no filename for argument '-c'.\n");
					return -2;
				}
				config_file = argv[i];
			}
			else if (strcmp(argv[i],"--available") == 0 || strcmp(argv[i],"-a") == 0) {
				msg = "available";
				read_response = 1;
				start_daemon = 0;
			}
			else {
				if (argv[i][0] == '-') {
					printf ("Unknown command: %s\n", argv[i]);
					return 0;
				}
				else if (argc == 2) {
					cmd = 0;
					read_response = 1;
					start_daemon = 0;
					msg = argv[i];
				}
			}

			if (msg) {
				if (!socket_connected) {
					fd = socket_connect ();
					socket_connected = 1;
				}
				if (cmd) {
					socket_send_cmd (fd, msg);
				}
				else {
					socket_send (fd, msg);
				}
			}
		}
		if (read_response) {
			char buf[128];
			int rd = read(fd, buf,sizeof(buf));
			if (rd) {
				fputs (buf, stdout);
			}

		}
	}

	if (!start_daemon) {
		return 0;
	}

	if (config_file) {
		config_load (config_file);
	}
	else {
		printf ("No config file loaded!\n");
		return -1;
	}

	int fd = socket_start();
	int ret = socket_loop (fd);


	if (def_tokens[0]) {
		int i;
		for (i = 0; def_tokens[i]; i++) {
			free (def_tokens[i]);
		}
	}
	return ret;
}

int socket_start () {
	const char *xdg_runtime = getenv ("XDG_RUNTIME_DIR");
	if (!xdg_runtime) {
		printf ("Failed to get XDG_RUNTIME_DIR.\n");
		return -1;
	}

	char socket_dir_a[strlen(xdg_runtime) + strlen("/dscrdd")];
	strcpy (socket_dir_a, xdg_runtime);
	strcat (socket_dir_a, "/dscrdd");
	socket_dir = socket_dir_a;

	// make directory if doesn't exist
	{
		struct stat st = {0};
		if (stat(socket_dir, &st) == -1) {
	 	   int ret = mkdir(socket_dir, 0700);
	 	   if (ret) {
	 	   		printf ("Failed to create directory \"%s\". %s\n", socket_dir, strerror(errno));
	 	   }
		}
	}

	char socket_path_a[strlen(socket_dir) + strlen("/socket")];
	strcpy (socket_path_a, socket_dir_a);
	strcat (socket_path_a, "/socket");
	socket_path = socket_path_a;



	
	int fd;
	if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		return -1;
	}

	struct sockaddr_un addr;
  	memset(&addr, 0, sizeof(addr));
  	addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
    unlink(socket_path);

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	    perror("bind error");
	    return -1;
 	}

	if (listen(fd, 5) == -1) {
		perror("listen error");
		return -1;
	}
	return fd;
}

int socket_loop (int fd) {
	char buffer[100];
	int cl, rc;
 	while (1) {
	    if ( (cl = accept(fd, NULL, NULL)) == -1) {
	      perror("accept error");
	      continue;
	    }

	    while ( (rc=read(cl,buffer,sizeof(buffer))) > 0) {
	    	// main read part
	     	//printf("read %u bytes: %.*s\n", rc, rc, buffer);
	     	if (buffer[0] == 0 && rc > 1) {
	     		dscrdd_command (cl, buffer+1);
	     	}
	     	else {
	    		dscrdd_connection (cl, buffer);
	    	}
	    }
	    if (rc == -1) {
		    if (terminated) {
		    	return 0;
		    }
		    perror("read");
		    return -1;
	    }
	    else if (rc == 0) {
			close(cl);
	    }
  	}
  	return 0;
}

int socket_connect () {
    const char *xdg_runtime = getenv ("XDG_RUNTIME_DIR");
    if (!xdg_runtime) {
      fprintf (stderr, "Failed to get XDG_RUNTIME_DIR.\n");
      return -1;
    }

    char *socket_path;
    char socket_path_a[strlen(xdg_runtime) + strlen("/dscrdd/socket")];
    strcpy (socket_path_a, xdg_runtime);
    strcat (socket_path_a, "/dscrdd/socket");
    socket_path = socket_path_a;

    int fd;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        fprintf (stderr, "Socket error.");
        return -1;
    }

    struct sockaddr_un addr;
    memset (&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        fprintf (stderr, "Socket connect error.\n");
        return -2;
    }

    return fd;
}

int socket_send(int fd, char *msg) {
    int ret = write(fd, msg, strlen(msg) + 1);
    if (ret != strlen(msg) + 1) {
        return -1;
    }
    return 0;
}


int socket_send_cmd(int fd, char *cmd) {
	int cmd_len = strlen(cmd);
	char buf[cmd_len + 2];
	*buf = 0;
	strcpy (buf + 1, cmd);

    int ret = write(fd, buf, cmd_len + 2);
    if (ret != cmd_len + 2) {
        return -1;
    }
    return 0;

}
