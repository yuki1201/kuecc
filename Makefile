CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
9cc: $(OBJS)
	$(CC) -o kuecc $(OBJS) $(LDFLAGS)
$(OBJS): ycc.h
test: ycc
	./test.sh
clean:
	rm -f ycc *.o *~ tmp*
.PHONY: test clean