#include "utils.h"

char *get_substr(char *s, size_t start, size_t end)
{
    if (!s)
        return NULL;
    if (start > end)
        return NULL;
    if (end - start > strlen(s))
        return NULL;
    if (end > strlen(s))
        return NULL;

    size_t sub_size = end - start;
    char *sub = calloc(sub_size + 1, sizeof(char));
    memcpy(sub, s + start, sub_size);

    return sub;
}

bool str_equ(char *s1, char *s2)
{
    return strcmp(s1, s2) == 0;
}

bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

bool str_is_number(char *str)
{
    if (!str)
        return false;

    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (!is_digit(str[i]))
            return false;
    }

    return true;
}

bool is_alpha(char c)
{
    if (c >= 'A' && c <= 'Z')
        return true;
    if (c >= 'a' && c <= 'z')
        return true;
    if (c == '-')
        return true;
    return false;
}

bool is_alphanum(char c)
{
    return is_alpha(c) || is_digit(c);
}

char *my_str_cat(char *s1, char *s2, size_t len_s2)
{
    if (s1 == NULL)
    {
        s1 = strndup(s2, len_s2);
    }
    else
    {
        s1 = realloc(s1, (strlen(s1) + len_s2 + 1) * sizeof(char));
        strncat(s1, s2, len_s2);
    }
    return s1;
}

bool str_is_quoted(char *s)
{
    if (!s || strlen(s) < 2)
        return false;

    size_t len = strlen(s);
    if (s[0] == '\'' && s[len - 1] == '\'')
        return true;
    if (s[0] == '"' && s[len - 1] == '"')
        return true;

    return false;
}
