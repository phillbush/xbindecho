include config.mk

PROGS = xbindecho xkeyecho xbuttonecho
OBJS  = xbindecho.o xkeyecho.o xbuttonecho.o

all: ${PROGS}

xbindecho: xbindecho.o
	${CC} -o $@ xbindecho.o ${LDFLAGS}

xkeyecho: xkeyecho.o
	${CC} -o $@ xkeyecho.o ${LDFLAGS}

xbuttonecho: xbuttonecho.o
	${CC} -o $@ xbuttonecho.o ${LDFLAGS}

.c.o:
	${CC} ${CFLAGS} -c $<

clean:
	-rm ${OBJS} ${PROGS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	install -m 755 xbuttonecho ${DESTDIR}${PREFIX}/bin/xbuttonecho
	install -m 755 xbindecho ${DESTDIR}${PREFIX}/bin/xbindecho
	install -m 755 xkeyecho ${DESTDIR}${PREFIX}/bin/xkeyecho
	install -m 644 xbuttonecho.1 ${DESTDIR}${MANPREFIX}/man1/xbuttonecho.1
	install -m 644 xbindecho.1 ${DESTDIR}${MANPREFIX}/man1/xbindecho.1
	install -m 644 xkeyecho.1 ${DESTDIR}${MANPREFIX}/man1/xkeyecho.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/xbuttonecho
	rm -f ${DESTDIR}${PREFIX}/bin/xbindecho
	rm -f ${DESTDIR}${PREFIX}/bin/xkeyecho
	rm -f ${DESTDIR}${MANPREFIX}/man1/xbuttonecho.1
	rm -f ${DESTDIR}${MANPREFIX}/man1/xbindecho.1
	rm -f ${DESTDIR}${MANPREFIX}/man1/xkeyecho.1

.PHONY: all clean install uninstall
