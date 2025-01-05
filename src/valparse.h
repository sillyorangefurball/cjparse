#ifndef __CJPARSE_VAL_H
#define __CJPARSE_VAL_H

#include "jsondat.h"

char *parse_str_val(const char **str);
long long parse_int_val(const char **str);
double parse_float_val(const char **str);
unsigned char parse_val_type(const char *str);
json_data *parse_key_val_pair(const char **str);

#endif