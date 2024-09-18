prefix=/usr

MODULES = gtk-column-torrent-name

target=$(shell gcc -dumpmachine)
builddir=.build
libdir = $(DESTDIR)$(prefix)/lib/$(target)
CFLAGS = `pkg-config --cflags libfm libfm-gtk3`
LDFLAGS = -rpath $(libdir) -no-undefined -module -avoid-version

all:
	mkdir -p $(builddir)
	libtool --mode=compile gcc $(CFLAGS) -c src/bencode.c -o $(builddir)/bencode.o
	$(foreach MODULE,$(MODULES),libtool --mode=compile gcc $(CFLAGS) -c src/$(MODULE).c -o $(builddir)/$(MODULE).o;)
	libtool --mode=link gcc $(LDFLAGS) $(builddir)/bencode.lo -o $(builddir)/bencode.la
	$(foreach MODULE,$(MODULES),libtool --mode=link gcc $(LDFLAGS) $(builddir)/$(MODULE).lo $(builddir)/bencode.lo -o $(builddir)/$(MODULE).la;)

install:
	mkdir -p $(libdir)/libfm/modules/
	$(foreach MODULE,$(MODULES),libtool --mode=install install -c $(builddir)/$(MODULE).la $(libdir)/libfm/modules/;)
	libtool --finish $(libdir)/libfm/modules/

