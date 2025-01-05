#ifndef __CJPARSE_POPULATE_H
#define __CJPARSE_POPULATE_H

#include "jsondat.h"

json_data *populate_object(const char **buf);
json_data *populate_list(const char **buf);

#endif