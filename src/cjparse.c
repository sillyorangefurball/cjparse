#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "io.h"
#include "cjparse.h"
#include "populate.h"
#include "valparse.h"

unsigned char g_cjparse_err;

json_data *read_json(const char *const path)
{
	json_data *root = calloc(sizeof(json_data), 1);
	if (root == NULL)
	{
		return NULL;
	}

	char *file = read_file(path);
	if (file == NULL)
	{
		free(root);
		g_cjparse_err = CJERR_READ_FILE_FAIL;
		return NULL;
	}

	const char *cur = file;

	while (*cur < 33)
	{
		cur++;
	}

	switch (*cur)
	{
	case '{':
		root->type = JSON_OBJECT;
		root->child = populate_object(&cur);
		if (root->child == NULL && g_cjparse_err != 0)
		{
			free(root);
			return NULL;
		}
		break;
	case '[':
		root->type = JSON_LIST;
		root->child = populate_list(&cur);
		if (root->child == NULL && g_cjparse_err != 0)
		{
			free(root);
			return NULL;
		}
		break;
	default:
		free(root);
		g_cjparse_err = CJERR_INV_JSON;
		return NULL;
	}

	free(file);
	return root;
}

// Removes the entire data tree on the same level as the given node and
// below. So all previous and following nodes and their respective
// trees will be freed too
void free_data(json_data *elem)
{
	if (elem == NULL)
	{
		return;
	}

	if (elem->prev != NULL)
	{
		json_data *prev = elem->prev;
		prev->next = NULL;
		free_data(prev);
	}

	if (elem->next != NULL)
	{
		json_data *next = elem->next;
		next->prev = NULL;
		free_data(elem->next);
	}

	if (elem->child != NULL)
	{
		free_data(elem->child);
	}

	if (elem->type == JSON_STRING)
	{
		free(elem->strval);
	}

	if (elem->key != NULL)
	{
		free(elem->key);
	}

	free(elem);
}

// Takes a data node and searches it's entire data tree for the given
// index, which is in the format of <key[...<subkeys>]>. Key is a
// quoted string for objects, and an integer value for arrays. The
// index is to be passed as a string, so when using quoted strings, the
// quotation marks need to be escaped
json_data get_value(const json_data *node, const char *index)
{
	json_data data = {NULL};
	json_data *obj = node->child;

	// To check if all brackets were closed again. If not, an error
	// is returned, because an unintended syntax is assumed
	int paren = 0;
	long long arr_pos;
	const char *key;

parse:
	if (*index < 58 && *index > 47 && obj->key == NULL)
	{
		long long cur = 0;
		arr_pos = parse_int_val(&index);
		while (cur < arr_pos)
		{
			if (obj->next != NULL)
			{
				obj = obj->next;
				cur++;
				continue;
			}

			g_cjparse_err = CJERR_INDEX_OUT_OF_BOUNDS;
			return data;
		}
	}
	else if (*index == '\"' && obj->key != NULL)
	{
		key = parse_str_val(&index);
		if (key == NULL)
		{
			g_cjparse_err = CJERR_INV_INDEX_FORMAT;
			return data;
		}

		while (strcmp(key, obj->key) != 0)
		{
			if (obj->next == NULL)
			{
				g_cjparse_err = CJERR_INDEX_OUT_OF_BOUNDS;
				return data;
			}
			obj = obj->next;
		}
	}
	else
	{
		g_cjparse_err = CJERR_INV_INDEX_FORMAT;
		return data;
	}

	if (*index == '[')
	{
		index++;
		paren++;
		if (obj->child != NULL)
		{
			obj = obj->child;
			goto parse;
		}

		g_cjparse_err = CJERR_INDEX_OUT_OF_BOUNDS;
		return data;
	}
	else if (*index == ']')
	{
		// Checking for valid index syntax. While theoretically not
		// required, it helps detecting errors on the users side
		while (*++index == ']')
		{
			paren--;
		}
		if (paren != 0 || *index != 0)
		{
			g_cjparse_err = CJERR_INV_INDEX_FORMAT;
			return data;
		}
	}
	else if (*index != 0)
	{
		g_cjparse_err = CJERR_INV_INDEX_FORMAT;
		return data;
	}

	data = *obj;

	return data;
}
