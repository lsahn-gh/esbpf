PREFIX  = /usr/local
BINDIR  = $(PREFIX)/sbin
DESTDIR =
CFLAGS  = -Wall -Werror
LDFLAGS =

TARGET  = filter_icmp

filter_icmp:
	$(CC) $(CFLAGS) $(LDFLAGS) -o filter_icmp filter_icmp.c

all: $(TARGET)

install: all
	$(INSTALL) -d 755 $(TARGET) $(DESTDIR)/$(BINDIR)

clean:
	rm $(TARGET)

.PHONY : install clean

