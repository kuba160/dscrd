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

/// @cond INTERNAL

/**
 * Ensures file data is loaded into memory.
 *
 * @param file @ref DscrdFile
 */
void DscrdFile_data (DscrdFile *file);

/**
 * Ensures base64 file data is loaded into memory.
 *
 * @param file @ref DscrdFile
 */
void DscrdFile_data_base64 (DscrdFile *file);

/**
 * Ensures encoded file data is loaded into memory.
 *
 * @param file @ref DscrdFile
 */
void DscrdFile_data_enc (DscrdFile *file);

/**
 * Checks if @ref DscrdFile is an existent file.
 *
 * @param file @ref DscrdFile
 * @returns 1 if @ref DscrdFile refers to a file
 * @returns 0 if @ref DscrdFile does not refer to a file and indicates no file (@ref DSCRD_FILE_NULL)
 * @see DSCRD_FILE_NULL
 */
int is_DscrdFile_real (DscrdFile *file);

/// @endcond
// PUBLIC_API_BEGIN

/// Use this if you want to reset avatar (needed to distinguish from not changing avatar)
#define DSCRD_FILE_NULL ((DscrdFile *) "dummy")

/**
 * @brief Discord file wrapper.
 *
 * @see DscrdFile_s for detailed element documentation.
 * @todo detailed element documentation
 */
typedef struct DscrdFile_s {
	char head[13];
	uint32_t data_len;
	uint32_t data_base64_len;
	uint8_t _type;
	char *mime;
	char *filename;
	char *data;
	char *data_base64;
	char *encoded;
} DscrdFile;

/**
 * Returns @ref DscrdFile pointer for specified filename.
 * Returned pointer has to be freed with @ref DscrdFile_destroy.
 * @param filename path to file
 * @returns pointer to @ref DscrdFile on success
 * @returns @b NULL on failure
 * @see DscrdFile_get_mime
 */
DscrdFile * DscrdFile_get (const char *filename);

/**
 * Returns @ref DscrdFile pointer for specified filename.
 * In comparsion to @ref DscrdFile_get here you can specify MIME type by yourself
 * Returned pointer has to be freed with @ref DscrdFile_destroy.
 * @param filename path to file
 * @param mime_type MIME-type string 
 * @returns pointer to @ref DscrdFile on success
 * @returns @b NULL on failure
 */
DscrdFile * DscrdFile_get_mime (const char *filename, const char *mime_type);

/**
 * Frees @ref DscrdFile pointer.
 * @param file @ref DscrdFile pointer
 */
void DscrdFile_destroy (DscrdFile *file);

// PUBLIC_API_END
