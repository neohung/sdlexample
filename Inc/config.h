#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "list.h"

#define CONFIG_MAX_LINE_LEN	256

typedef struct {
	char *key;
	char *value;
} ConfigKeyValuePair;

typedef struct {
	char *name;
	List *keyValuePairs;
} ConfigEntity;

typedef struct {
	List *entities;
} Config;

Config * config_file_parse(char * filename);
void config_file_write(char *filename, Config *config);
char * config_entity_value(ConfigEntity *entity, char *key);
void config_entity_set_value(ConfigEntity *entity, char *key, char *value);

char * String_Create(const char * stringWithFormat, ...) ;
void String_Destroy(char *string) ;
#endif
