#ifndef UTILS_H
#define UTILS_H

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "vector/vector.h"

char *get_substr(char *s, size_t start, size_t end);
bool str_equ(char *s1, char *s2);
bool is_digit(char c);
bool is_alpha(char c);
bool is_alphanum(char c);
char *my_str_cat(char *s1, char *s2, size_t len_s2);
int get_number_len(int n);
int get_nth_digit(int n, int nth);
char *my_itoa(int value);
bool file_exists(const char *path);
bool file_is_readable(const char *file);
bool is_dir(const char *path);
ssize_t get_file_len(char *path);
size_t hash(const char *str);
bool str_is_number(char *str); 
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
bool str_is_quoted(char *s);
#endif /* ! UTILS_H */
