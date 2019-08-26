PROG=	sn

OBJS=	main.o \
	bresenham.o \
	coordinate.o \
	dijkstra.o \
	dungeon.o \
	err.o \
	fov.o \
	game.o \
	level.o \
	ui.o

TESTS=	bresenham \
	dijkstra

CC ?=	clang
CFLAGS += -pipe -Wall -I./include -std=c11
LDFLAGS += -lncurses -lm

.if defined(DEBUG) || make(debug)
CFLAGS += -O0 -g
.endif

.PHONY:	all clean debug
.PATH: src

.SUFFIXES: .o
.c.o:
	${CC} ${CFLAGS} -o ${.TARGET} -c ${.IMPSRC}

all: ${PROG}
.for TEST in ${TESTS}
all: tests/${TEST}/tests
.endfor

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} ${CFLAGS} ${OBJS} -o ${.TARGET}

debug: all

clean:
	rm -f ${OBJS}
	rm -f ${PROG}
.for TEST in ${TESTS}
	rm -f tests/${TEST}/tests.o
	rm -f tests/${TEST}/tests
.endfor

.for TEST in ${TESTS}
test:: tests/${TEST}/tests
	@echo "==> running tests for '${TEST}'"
	@tests/${TEST}/tests

# Generate the test binaries. We go the easy way here and depend on all object
# files (with main.o being the obvious exception).
tests/${TEST}/tests: tests/${TEST}/tests.o ${OBJS:Nmain.o}
	${CC} ${LDFLAGS} ${CFLAGS} ${.ALLSRC} -o ${.TARGET}
.endfor
