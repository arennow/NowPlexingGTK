CFLAGS=-IPlexMessage/include -D_GNU_SOURCE -std=gnu11 `pkg-config --cflags --libs gtk+-3.0` -lm -lcurl -g
SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

all: $(OBJS) libPlexMessage.o
	$(CC) *.o PlexMessage/libPlexMessage.o $(CFLAGS) -o np

libPlexMessage.o:
	$(MAKE) -C PlexMessage/

clean:
	$(MAKE) -C PlexMessage/ clean
	rm -fv *.o np
