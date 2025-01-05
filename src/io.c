#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

char *read_file(const char *const path)
{
	char err_msg[] = "Failed to read file";
	char *err = malloc(strlen(path) + strlen(err_msg));
	sprintf(err, "%s %s", err_msg, path);

	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		return NULL;
	}

	if (fseek(file, 0, SEEK_END) != 0)
	{
		goto READ_FILE_FAIL;
	}

	long len = ftell(file);
	if (len == -1)
	{
		goto READ_FILE_FAIL;
	}

	if (fseek(file, 0, SEEK_SET) != 0)
	{
		goto READ_FILE_FAIL;
	}

	char *file_buf = malloc(len);
	if (file_buf == NULL)
	{
		goto READ_FILE_FAIL;
	}

	if (fread(file_buf, 1, len, file) != len)
	{
		free(file_buf);
		goto READ_FILE_FAIL;
	}

	fclose(file);
	return file_buf;

READ_FILE_FAIL:
	fclose(file);
	perror(err);
	return NULL;
}
