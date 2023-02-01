#include "../utils.h"
int get_number_len(int n)
{
    if (n < 10)
    {
        return 1;
    }
    int len = 0;

    while (n != 0)
    {
        len += 1;
        n = n / 10;
    }

    return len;
}

int get_nth_digit(int n, int nth)
{
    while (nth > 0 && n > 1)
    {
        nth -= 1;
        n = n / 10;
    }

    return n % 10;
}

char *my_itoa(int value)
{
    char *s = calloc(get_number_len(value) + 1, sizeof(char));
    if (value == 0)
    {
        s[0] = '0';
        s[1] = '\0';
        return s;
    }
    int sign = 1;
    if (value < 0)
    {
        sign = -1;
        value *= -1;
    }

    int start = 0;
    if (sign < 0)
    {
        s[start] = '-';
        start += 1;
    }

    int digit_len = get_number_len(value) - 1;

    while (digit_len >= 0)
    {
        s[start] = get_nth_digit(value, digit_len) + '0';
        digit_len -= 1;
        start += 1;
    }

    s[start] = '\0';
    return s;
}
