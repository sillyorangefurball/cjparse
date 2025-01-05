#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "populate.h"
#include "valparse.h"
#include "cj_error.h"

char *parse_str_val(const char **str)
{
	const char *end = strpbrk(++(*str), "\"");
	if (end == NULL)
	{
		g_cjparse_err = CJERR_STRING_NOT_CLOSED;
		return NULL;
	}
	while (*(end - 1) == '\\')
	{
		end = strpbrk(end + 1, "\"");
		if (end == NULL)
		{
			g_cjparse_err = CJERR_STRING_NOT_CLOSED;
			return NULL;
		}
	}

	long len = end - *str;
	char *val = calloc(len, 1);
	if (val == NULL)
	{
		g_cjparse_err = CJERR_ALLOC_FAIL;
		return NULL;
	}
	strncpy(val, *str, len);

	*str += len + 1;

	return val;
}

long long parse_int_val(const char **str)
{
	long long val = 0;
	char is_negative = 0;
	const char *cur = *str;

	if (*cur == '-')
	{
		is_negative = 1;
		cur++;
	}

	while (*cur > 47 && *cur < 58)
	{
		val = (val * 10) + (*cur - 48);
		cur++;
	}

	*str = cur;

	if (is_negative)
	{
		return (val * -1);
	}

	return val;
}

double parse_float_val(const char **str)
{
	double val = 0.0;
	int cnt = 1;
	char is_negative = 0;
	char is_exponent = 0;
	long long exp = 0;
	const char *cur = *str;

	if (*cur == '-')
	{
		is_negative = 1;
		cur++;
	}

	while (*cur > 47 && *cur < 58)
	{
		val = (val * 10.0) + (*cur - 48);
		cur++;
	}
	cur++;
	while (*cur > 47 && *cur < 58)
	{
		val += (*cur - 48) / (10.0 * cnt);
		cnt *= 10;
		cur++;
	}

	*str = cur;

	if (*cur == 'e' || *cur == 'E')
	{
		*str = ++cur;
		is_exponent = 1;
		exp = parse_int_val(str);
	}


	if (is_negative)
	{
		if (is_exponent)
		{
			return (val * pow(10.0, (double)exp)) * -1.0;
		}
		return val * -1.0;
	}

	if (is_exponent)
	{
		return (val * pow(10.0, (double)exp));
	}

	return val;
}

unsigned char parse_val_type(const char *str)
{
	const char *cur = str;
	const char accept[] = "]},";
	char fail = 0;

	if (*cur == '\"')
	{
		const char *end = strpbrk(cur + 1, "\"");
		if (end == NULL)
		{
			g_cjparse_err = CJERR_STRING_NOT_CLOSED;
			return -1;
		}
		while (*(end - 1) == '\\')
		{
			end = strpbrk(end + 1, "\"");
			if (end == NULL)
			{
				g_cjparse_err = CJERR_STRING_NOT_CLOSED;
				return -1;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (*(end + 1) != accept[i] && *(end + 1) > 32)
			{
				fail++;
			}
		}
		if (fail > 3)
		{
			g_cjparse_err = CJERR_INV_JSON;
			return -1;
		}
		return JSON_STRING;
	}
	if ((*cur < 58 && *cur > 47) || *cur == '-')
	{
		char is_float = 0;
		if (*cur == '-')
		{
			cur++;
		}

		while ((*cur < 58 && *cur > 47) || *cur == '.')
		{
			if (*cur == '.' && is_float)
			{
				g_cjparse_err = CJERR_INV_JSON;
				return -1;
			}
			if (*cur == '.')
			{
				is_float = 1;
				if (*(cur + 1) < 47 || *(cur + 1) > 57)
				{
					g_cjparse_err = CJERR_INV_JSON;
					return -1;
				}
			}
			cur++;
		}

		if (*cur == 'e' || *cur == 'E')
		{
			is_float = 1;
			cur++;
			while (*cur < 58 && *cur > 47)
			{
				cur++;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (*cur != accept[i] && *cur > 32)
			{
				fail++;
			}
		}

		if (fail > 3)
		{
			g_cjparse_err = CJERR_INV_JSON;
			return -1;
		}

		if (is_float)
		{
			return JSON_FLOAT;
		}
		return JSON_INT;
	}
	if (strncmp(cur, "null", 4) == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (*(cur + 4) != accept[i] && *(cur + 4) > 32)
			{
				fail++;
			}
		}
		if (fail > 3)
		{
			g_cjparse_err = CJERR_INV_JSON;
			return -1;
		}
		return JSON_NULL;
	}
	if (strncmp(cur, "true", 4) == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (*(cur + 4) != accept[i] && *(cur + 4) > 32)
			{
				fail++;
			}
		}
		if (fail > 3)
		{
			g_cjparse_err = CJERR_INV_JSON;
			return -1;
		}
		return JSON_BOOL_TRUE;
	}
	if (strncmp(cur, "false", 5) == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (*(cur + 5) != accept[i] && *(cur + 5) > 32)
			{
				fail++;
			}
		}
		if (fail > 3)
		{
			g_cjparse_err = CJERR_INV_JSON;
			return -1;
		}
		return JSON_BOOL_FALSE;
	}
	if (*cur == '[')
	{
		return JSON_LIST;
	}
	if (*cur == '{')
	{
		return JSON_OBJECT;
	}
	g_cjparse_err = CJERR_INV_JSON;
	return -1;
}

json_data *parse_key_val_pair(const char **str)
{
	json_data *elem = calloc(1, sizeof(json_data));

	while (**str < 33)
	{
		(*str)++;
	}

	elem->key = parse_str_val(str);
	if (elem->key == NULL)
	{
		free(elem);
		return NULL;
	}

	while (**str < 33)
	{
		(*str)++;
	}

	if (*(*str)++ != ':')
	{
		g_cjparse_err = CJERR_INV_JSON;
		free(elem);
		return NULL;
	}

	while (**str < 33)
	{
		(*str)++;
	}

	switch (parse_val_type(*str))
		{
		case JSON_STRING:
			elem->type = JSON_STRING;
			elem->strval = parse_str_val(str);
			if (elem->strval == NULL)
			{
				free(elem);
				return NULL;
			}
			break;
		case JSON_INT:
			elem->type = JSON_INT;
			elem->intval = parse_int_val(str);
			break;
		case JSON_FLOAT:
			elem->type = JSON_FLOAT;
			elem->floatval = parse_float_val(str);
			break;
		case JSON_BOOL_FALSE:
			elem->type = JSON_BOOL_FALSE;
			(*str) += 5;
			break;
		case JSON_BOOL_TRUE:
			elem->type = JSON_BOOL_TRUE;
			(*str) += 4;
			break;
		case JSON_NULL:
			elem->type = JSON_NULL;
			(*str) += 4;
			break;
		case JSON_LIST:
			elem->type = JSON_LIST;
			elem->child = populate_list(str);
			if (elem->child == NULL && g_cjparse_err != 0)
			{
				free(elem);
				return NULL;
			}
			break;
		case JSON_OBJECT:
			elem->type = JSON_OBJECT;
			elem->child = populate_object(str);
			if (elem->child == NULL && g_cjparse_err != 0)
			{
				free(elem);
				return NULL;
			}
			break;
		default:
			g_cjparse_err = CJERR_INV_JSON;
			free(elem);
			return NULL;
		}

	return elem;
}
