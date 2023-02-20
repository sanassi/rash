CC=gcc
CFLAGS= -Wall -Werror -Wextra -fsanitize=address -std=c99 -g -D_XOPEN_SOURCE=700

LDFLAGS=-fsanitize=address

SRC= expansion_test.c expansion.c ../env/env.c\
	 ../utils/hash_map/hash_map.c\
	 ../utils/vector/vector.c ../utils/file_tools.c\
	 ../stream/stream.c ../utils/str_utils.c\
	 ../utils/hash_map/hash.c ../utils/my_itoa/my_itoa.c

OBJS=$(SRC:.c=.o)

BIN=expansion

all:expansion 

expansion: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(BIN)

.PHONY: clean

clean:
	$(RM) $(BIN) $(OBJS)
