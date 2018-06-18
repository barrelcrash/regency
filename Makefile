regency: regen.o dictionary.o
	$(CC) -o regency -Wall -Wextra -pedantic -std=c99 regen.o dictionary.o
