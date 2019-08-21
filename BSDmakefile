PROG=		sn
SRCS=		main.c \
		bresenham.c \
		coordinate.c \
		dijkstra.c \
		dungeon.c \
		err.c \
		fov.c \
		game.c \
		level.c \
		ui.c \

SUBDIR_TARGETS+= test
SUBDIR=		tests

CSTD?=		c11
WARNS?=		6

LDADD=		-lncurses \
		-lm \

MK_MAN=		no

.include <bsd.prog.mk>
