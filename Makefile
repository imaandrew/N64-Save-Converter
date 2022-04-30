CC := cc
CFLAGS := -O3 -std=c11 -Wno-unused-result

convertsave: convertsave.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) convertsave

.PHONY: clean