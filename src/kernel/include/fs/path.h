#ifndef __PATH_H__
#define __PATH_H__

#include <common.h>

typedef struct path_root PATH;
typedef struct path_token PATH_TOKEN;

bool path_is_valid(const char *path);
PATH* path_parse(const char *path, const char *crt_dir_path);
void path_destroy(PATH *p);
char* path_to_string(PATH *p);

int path_get_drive_number(PATH *p);
PATH_TOKEN* path_get_first_token(PATH *p);
PATH_TOKEN* path_get_next_token(PATH_TOKEN *pt);
const char* path_get_token_text(PATH_TOKEN *pt);

#endif