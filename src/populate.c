#include <string.h>
#include <stdlib.h>
#include "populate.h"
#include "cjparse.h"
#include "valparse.h"

json_data *populate_object(const char **buf)
{
	json_data *now = NULL;
	json_data *prev = NULL;
	int comma;

	(*buf)++;

	while (**buf < 33)
	{
		(*buf)++;
	}

	while (**buf != '}')
	{
		now = parse_key_val_pair(buf);
		if (now == NULL)
		{
			return NULL;
		}
		now->prev = prev;

		if (prev != NULL)
		{
			prev->next = now;
		}

		prev = now;

		comma = 0;
		while (**buf < 33 || **buf == ',')
		{
			if (**buf == ',' && comma)
			{
				g_cjparse_err = CJERR_INV_JSON;
				free_data(now);
				return NULL;
			}
			if (**buf == ',')
			{
				comma = 1;
			}
			(*buf)++;
		}
	}
	(*buf)++;

	if (now != NULL)
	{
		while (now->prev != NULL)
		{
			now = now->prev;
		}
	}

	return now;
}

json_data *populate_list(const char **buf)
{
	json_data *now = NULL;
	json_data *prev = NULL;
	int comma;

	(*buf)++;

	while (**buf < 33)
	{
		(*buf)++;
	}

	while (**buf != ']')
	{
		now = calloc(sizeof(json_data), 1);
		if (now == NULL)
		{
			if (prev != NULL)
			{
				free_data(prev);
			}
			g_cjparse_err = CJERR_ALLOC_FAIL;
			return NULL;
		}
		now->prev = prev;

		switch (parse_val_type(*buf))
		{
		case JSON_STRING:
			now->type = JSON_STRING;
			now->strval = parse_str_val(buf);
			if (now->strval == NULL)
			{
				free_data(now);
				return NULL;
			}
			break;
		case JSON_INT:
			now->type = JSON_INT;
			now->intval = parse_int_val(buf);
			break;
		case JSON_FLOAT:
			now->type = JSON_FLOAT;
			now->floatval = parse_float_val(buf);
			break;
		case JSON_BOOL_FALSE:
			now->type = JSON_BOOL_FALSE;
			(*buf) += 5;
			break;
		case JSON_BOOL_TRUE:
			now->type = JSON_BOOL_TRUE;
			(*buf) += 4;
			break;
		case JSON_NULL:
			now->type = JSON_NULL;
			(*buf) += 4;
			break;
		case JSON_LIST:
			now->type = JSON_LIST;
			now->child = populate_list(buf);
			if (now->child == NULL && g_cjparse_err != 0)
			{
				free_data(now);
				return NULL;
			}
		case JSON_OBJECT:
			now->type = JSON_OBJECT;
			now->child = populate_object(buf);
			if (now->child == NULL && g_cjparse_err != 0)
			{
				free_data(now);
				return NULL;
			}
			break;
		default:
			g_cjparse_err = CJERR_INV_JSON;
			free_data(now);
			return NULL;
		}

		if (prev != NULL)
		{
			prev->next = now;
		}

		prev = now;

		comma = 0;
		while (**buf < 33 || **buf == ',')
		{
			if (**buf == ',' && comma)
			{
				g_cjparse_err = CJERR_INV_JSON;
				free_data(now);
				return NULL;
			}
			if (**buf == ',')
			{
				comma = 1;
			}
			(*buf)++;
		}
	}
	(*buf)++;

	if (now != NULL)
	{
		while (now->prev != NULL)
		{
			now = now->prev;
		}
	}

	return now;
}
