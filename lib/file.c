/*
 *     dscrd/file_process: file processing functions
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

#define INCLUDE_LIBMAGIC
#ifdef INCLUDE_LIBMAGIC
#include <magic.h>
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dscrd.h"
#include "dscrd_i.h"
#include "base64/base64.h"


DscrdFile * DscrdFile_get (const char *filename) {
	DscrdFile *temp = NULL;
	#ifdef INCLUDE_LIBMAGIC
    magic_t magic_cookie = magic_open(MAGIC_MIME_TYPE);
    if (magic_cookie) {
	    if (magic_load(magic_cookie, NULL) == 0) {
		    const char *magic_full = magic_file(magic_cookie, filename);
		    if (magic_full) {
		    	temp = DscrdFile_get_mime (filename, magic_full);
		    }
	    }
	    magic_close(magic_cookie);
	}
	#else
	return DscrdFile_get_mime (filename, NULL);
	#endif
	return temp;
}

DscrdFile * DscrdFile_get_mime (const char *filename, const char *mime_type) {
	if( access( filename, R_OK ) == -1 ) {
		dscrd_error_set ("Cannot open file \"%s\" for reading.", filename);
		return NULL;
	}

	DscrdFile *file = malloc (sizeof(DscrdFile));
	if (!file) {
		return NULL;
	}
	memset (file, 0, sizeof(DscrdFile));
	file->_type = DSCRD_FILE;

	if (mime_type) {
		file->mime = strdup(mime_type);
	}
	else {
		#ifdef INCLUDE_LIBMAGIC
	    magic_t magic_cookie = magic_open(MAGIC_MIME_TYPE);
	    if (magic_cookie) {
		    if (magic_load(magic_cookie, NULL) == 0) {
			    const char *magic_full = magic_file(magic_cookie, filename);
			    if (magic_full)
			    	file->mime = strdup (magic_full);
		    }
		    magic_close(magic_cookie);
		}
		#endif
	}

	file->filename = strdup (filename);

	return file;
}

void DscrdFile_data (DscrdFile *file) {
	if (!file->data) {
		char * buffer = 0;
		long length = 0;
		FILE * f = fopen (file->filename, "rb");
		if (f) {
		  fseek (f, 0, SEEK_END);
		  length = ftell (f);
		  fseek (f, 0, SEEK_SET);
		  buffer = malloc (length + 1);
		  if (buffer) {
		    fread (buffer, 1, length, f);
		    //buffer[length + 1] = 0;
		  }
		  fclose (f);
		}

		file->data = buffer;
		file->data_len = length;
	}
}

void DscrdFile_data_base64 (DscrdFile *file) {
	if (!file->data_base64) {
		if (!file->data) {
			DscrdFile_data (file);
		}

		// todo maybe encode later on use
		if (file->data) {
			size_t out_len;
			file->data_base64 = (char *) base64_encode((unsigned char *)file->data, file->data_len, &out_len);
			file->data_base64_len = out_len;
		}
	}
}

void DscrdFile_data_enc (DscrdFile *file) {
	if (!file->data_base64)
		DscrdFile_data_base64 (file);
	if (file->data_base64_len && !file->encoded) {
		char *string_s = malloc (file->data_base64_len + strlen(file->mime) + strlen("data:%s;base64,%s") - 4 + 1);
		if (string_s) {
			sprintf (string_s, "data:%s;base64,%s", file->mime, file->data_base64);
		}
		file->encoded = string_s;
	}
}

int is_DscrdFile_real (DscrdFile *file) {
	if (strcmp (file->head, "dummy") == 0 || strcmp(file->head, "empty") == 0) {
		return 0;
	}
	return 1;
}

void DscrdFile_destroy (DscrdFile *file) {
	if (!file)
		return;
	if (file->mime) {
		free (file->mime);
	}
	if (file->filename) {
		free (file->filename);
	}
	if (file->data) {
		free (file->data);
	}
	if (file->data_base64) {
		free (file->data_base64);
	}
	if (file->encoded) {
		free (file->encoded);
	}
	free (file);
}
