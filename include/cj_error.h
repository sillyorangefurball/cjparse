#ifndef __CJPARSE_ERROR_H
#define __CJPARSE_ERROR_H

#define CJERR_STRING_NOT_CLOSED 1
#define CJERR_ALLOC_FAIL 2
#define CJERR_READ_FILE_FAIL 3
//TODO: Add more descriptive error codes where this is used
#define CJERR_INV_JSON 4

#define CJERR_INDEX_OUT_OF_BOUNDS 5
#define CJERR_INV_INDEX_FORMAT 6

extern unsigned char g_cjparse_err;

#endif