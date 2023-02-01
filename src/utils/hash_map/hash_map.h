#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "../utils.h"

struct pair_list
{
    char *key;
    char **value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

// size_t hash(const char *str);

struct hash_map *hash_map_init(size_t size);
void hash_map_free(struct hash_map *hash_map);

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated);
// void hash_map_dump(struct hash_map *hash_map);
char *hash_map_get(const struct hash_map *hash_map, const char *key);
bool hash_map_remove(struct hash_map *hash_map, const char *key);

// used the hashmap to insert/retrieve integer values
// (convert to int to string before insertion/get)
bool hm_insert_int(struct hash_map *h, const char *key, int value,
                   bool *updated);
int hm_get_int(const struct hash_map *h, const char *key);
void hash_map_dump(struct hash_map *hash_map, char *print_format);
void hash_map_clear(struct hash_map *hash_map);
#endif /* !HASH_MAP_H */
