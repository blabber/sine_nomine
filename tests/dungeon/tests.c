/*
 * Copyright (C) 2019 by Tobias Rehbein <tobias.rehbein@web.de>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>

#include <assert.h>
#include <limits.h>

#include <sine_nomine/coordinate.h>
#include <sine_nomine/dijkstra.h>
#include <sine_nomine/dungeon.h>
#include <sine_nomine/level.h>

enum { HEIGHT = 1000,
	WIDTH = 200,
	ITERATIONS = 100,
	ROOMS = 10,
	ROOM_MIN = 10,
	ROOM_MAX = 15,
};

static void _check_connectivity(struct level *_level);

static void _flood(struct level *_level);

int
main()
{
	for (int i = 0; i < ITERATIONS; i++) {
		struct coordinate_dimension d = { HEIGHT, WIDTH };
		struct level *l = level_create(d);

		struct coordinate_dimension min = { ROOM_MIN, ROOM_MIN };
		struct coordinate_dimension max = { ROOM_MIN, ROOM_MIN };

		dungeon_generate(l, ROOMS, min, max);
		_check_connectivity(l);
	}

	exit(EXIT_SUCCESS);
}

static void
_check_connectivity(struct level *level)
{
	dijkstra_reset(level);
	_flood(level);

	for (unsigned int y = 0; y < level->dimension.height; y++) {
		for (unsigned int x = 0; x < level->dimension.width; x++) {
			if (level->tiles[y][x].flags & TA_FLOOR) {
				assert(level->tiles[y][x].dijkstra < UINT_MAX);
			}
		}
	}
}

static void
_flood(struct level *level)
{
	for (unsigned int y = 0; y < level->dimension.height; y++) {
		for (unsigned int x = 0; x < level->dimension.width; x++) {
			if (level->tiles[y][x].flags & TA_FLOOR) {
				struct coordinate c = { y, x };
				dijkstra_add_target(c, level, 0);
				return;
			}
		}
	}
}
