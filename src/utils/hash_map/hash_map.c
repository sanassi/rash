#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *map = calloc(1, sizeof(struct hash_map));
    if (map == NULL)
        return NULL;

    map->size = size;

    map->data = calloc(size, sizeof(struct pair_list));

    if (map->data == NULL)
        return NULL;

    return map;
}

void hash_map_dump(struct hash_map *hash_map, char *print_format)
{
    for (size_t i = 0; i < hash_map->size; i++)
    {
        if (hash_map->data[i] != NULL)
        {
            struct pair_list *tmp = hash_map->data[i];

            while (tmp->next)
            {
                printf(print_format, tmp->key, *tmp->value);
                tmp = tmp->next;
            }

            printf(print_format, tmp->key, *tmp->value);
        }
    }
}
/*
void hash_map_dump(struct hash_map *hash_map)
{
    for (size_t i = 0; i < hash_map->size; i++)
    {
        if (hash_map->data[i] != NULL)
        {
            struct pair_list *tmp = hash_map->data[i];

            while (tmp->next)
            {
                printf("%s: %s, ", tmp->key, *tmp->value);
                tmp = tmp->next;
            }

            printf("%s: %s", tmp->key, *tmp->value);
            printf("\n");
        }
    }
}
*/
bool hm_insert_int(struct hash_map *h, const char *key, int value,
                   bool *updated)
{
    char *to_str = my_itoa(value);
    bool res = hash_map_insert(h, key, to_str, updated);
    free(to_str);
    return res;
}

static void free_pair(struct pair_list *pair)
{
    free(*pair->value);
    free(pair->value);
    free(pair->key);
    free(pair);
}

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated)
{
    *updated = false;

    if (hash_map == NULL || hash_map->size == 0)
    {
        //*updated = false;
        return false;
    }

    size_t hash_val = hash(key);
    struct pair_list *pair = calloc(1, sizeof(struct pair_list));
    pair->value = calloc(1, sizeof(char *));
    // pair->next = NULL;

    if (hash_val >= hash_map->size)
        hash_val = hash_val % hash_map->size;

    if (pair == NULL)
        return false;

    *pair->value = strdup(value);
    pair->key = strdup(key);

    if (hash_map->data[hash_val] == NULL)
    {
        hash_map->data[hash_val] = pair;
        return true;
    }
    else
    {
        struct pair_list *tmp = hash_map->data[hash_val];
        // do that in while loop
        if (strcmp(tmp->key, key) == 0)
        {
            free(*tmp->value);
            *tmp->value = strdup(value);
            *updated = true;
            free_pair(pair);
            return true;
        }

        *updated = false;

        while (tmp != NULL)
        {
            if (strcmp(tmp->key, key) == 0)
            {
                free(*tmp->value);

                // here
                free(*pair->value);
                free(pair->value);
                free(pair->key);
                *tmp->value = strdup(value);
                *updated = true;
                break;
            }
            tmp = tmp->next;
        }

        if (!(*updated))
        {
            pair->next = hash_map->data[hash_val];
            hash_map->data[hash_val] = pair;
            return true;
        }

        free(pair);
    }

    return true;
}

int hm_get_int(const struct hash_map *h, const char *key)
{
    char *res = hash_map_get(h, key);
    if (res == NULL)
        return -1;
    int value = atoi(res);
    free(res);
    return value;
}

char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    size_t hash_val = hash(key);

    if (hash_map == NULL)
        return NULL;

    if (hash_map->size == 0)
        return NULL;

    if (hash_val >= hash_map->size)
        hash_val = hash_val % hash_map->size;

    struct pair_list *pairs = hash_map->data[hash_val];
    struct pair_list *tmp = pairs;

    while (tmp && strcmp(key, tmp->key) != 0)
        tmp = tmp->next;

    if (tmp == NULL)
        return NULL;

    char *res = strdup(*tmp->value);
    return res;
}

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL)
        return false;

    if (hash_map->size == 0)
        return false;

    size_t hash_val = hash(key);

    if (hash_val >= hash_map->size)
        hash_val = hash_val % hash_map->size;

    struct pair_list *pairs = hash_map->data[hash_val];
    if (pairs == NULL)
        return false;

    struct pair_list *tmp = pairs;

    if (strcmp(key, tmp->key) == 0)
    {
        hash_map->data[hash_val] = tmp->next;
        free_pair(tmp);
        return true;
    }
    else
    {
        while (tmp->next && (strcmp(key, tmp->next->key) != 0))
        {
            tmp = tmp->next;
        }

        if (tmp->next)
        {
            struct pair_list *to_remove = tmp->next;
            tmp->next = to_remove->next;
            free_pair(to_remove);
            return true;
        }

        return false;
    }

    return false;
}

/*changed : freed list -> value*/
void list_destroy(struct pair_list *list)
{
    if (!list->next)
    {
        free(*list->value);
        free(list->value);
        free(list->key);
        free(list);
        list = NULL;
    }
    else
    {
        list_destroy(list->next);
        free(*list->value);
        free(list->key);
        free(list->value);
        free(list);
    }
}

void hash_map_clear(struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        if (hash_map->data[i] != NULL)
            list_destroy(hash_map->data[i]);
        hash_map->data[i] = NULL;
    }
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        if (hash_map->data[i] != NULL)
            list_destroy(hash_map->data[i]);
    }

    free(hash_map->data);
    free(hash_map);
}
