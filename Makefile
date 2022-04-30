CC := cc
CFLAGS := -O3 -std=c11 -Wno-unused-result

convertsave: convertsave.c
	$(CC) $(CFLAGS) -o $@ $^

debug: convertsave.c
	$(CC) $(CFLAGS) -g -o convertsave $^

clean:
	$(RM) convertsave
