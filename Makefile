CFLAGS = -O2 -g -pthread
OBJS = test.o fairlock.o
test: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@
clean:
	rm -f test $(OBJS)
