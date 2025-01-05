#ifndef __CJPARSE_ELEMS_H
#define __CJPARSE_ELEMS_H

#define JSON_STRING 1
#define JSON_INT 2
#define JSON_FLOAT 3
#define JSON_BOOL_TRUE 4
#define JSON_BOOL_FALSE 5
#define JSON_LIST 6
#define JSON_OBJECT 7
#define JSON_NULL 8

typedef struct json_data
{
	struct json_data *next;
	struct json_data *prev;
	struct json_data *child;

	unsigned char type;

	union
	{
		char *strval;
		long long intval;
		double floatval;
	};

	char *key;
} json_data;

#endif
