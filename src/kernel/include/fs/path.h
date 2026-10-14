#ifndef __PATH_H__
#define __PATH_H__

#include <common.h>

typedef struct path_root PATH;

bool path_is_valid(const char *path);
PATH* path_parse(const char *path, const char *crt_dir_path);
void path_destroy(PATH *p);
char* path_to_string(PATH *p);

#endif