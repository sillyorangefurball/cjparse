#ifndef __CJPARSE_H
#define __CJPARSE_H

#define CJPARSE_OK 0
#define CJPARSE_ERR -1

#include "jsondat.h"
#include "cj_error.h"

json_data *read_json(const char *const path);
void free_data(json_data *elem);
json_data get_value(const json_data *node, const char *index);

#endif
