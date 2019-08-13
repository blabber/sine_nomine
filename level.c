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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include "err.h"
#include "level.h"
#include "structs.h"

static struct level *_allocate(struct dimension _d);

static void _generate_dungeon(struct level  *_l, uint8_t _rooms,
    struct dimension _room_min, struct dimension _room_max);

static void _carve_room(struct level *_level, struct coordinate *_anchor,
    struct dimension _room_min, struct dimension _room_max);

static void _connect_anchors(struct level *_level, uint8_t _rooms,
    struct coordinate _anchors[]);

struct level *
level_create(struct dimension d, uint8_t rooms, struct dimension room_min,
    struct dimension room_max)
{
	assert(d.height > 0);
	assert(d.width > 0);
	assert(rooms > 0);
	assert(room_min.height <= room_max.height);
	assert(room_min.width <= room_max.width);
	assert(room_max.width <= d.width - 2 /* borders */);
	assert(room_max.height <= d.height - 2 /*borders*/);

	struct level *l = _allocate(d);
	_generate_dungeon(l, rooms, room_min, room_max);

	return (l);
}

void level_destroy(struct level *level)
{
	assert(level != NULL);

	for (uint8_t y = 0; y < level->dimension.height; y++) {
		free(level->tiles[y]);
	}
	
	free(level->tiles);
	free(level);
}

static struct level *
_allocate(struct dimension d)
{
	struct level *l = calloc(1, sizeof(struct level));
	if (l == NULL)
		err("level _allocate: calloc");

	assert(l != NULL);

	l->dimension = d;

	if ((l->tiles = calloc(l->dimension.height, sizeof(*l->tiles))) == NULL)
	 	err("level _allocate: calloc");

	assert(l->tiles != NULL);

	for (uint8_t y = 0; y < l->dimension.height; y++) {
		l->tiles[y] = calloc(l->dimension.width, sizeof(**l->tiles));
		if (l->tiles[y] == NULL)
	 		err("level _allocate: calloc");

		assert(l->tiles[y] != NULL);

		memset(l->tiles[y], TA_WALL, l->dimension.width);
	}

	return l;
}

/*
 * Applies a trivial dungeon generator to the level.
 *
 * 1. Carve out a number of rectangular room (they may overlap)
 * 2. Define a anchor in every room.
 * 3. Iterate the rooms and carve a floor from the anchor of the current room to
 *    the anchor of the next room.
 */
static void
_generate_dungeon(struct level *level, uint8_t rooms,
    struct dimension room_min, struct dimension room_max)
{
	sranddev();

	struct coordinate anchors[rooms];
	for (uint8_t i = 0; i < rooms; i++)
		_carve_room(level, &anchors[i], room_min, room_max);

	for (uint8_t i = 0; i < rooms - 1; i++)
		_connect_anchors(level, rooms, anchors);
}

static void _carve_room(struct level *level, struct coordinate *anchor,
    struct dimension room_min, struct dimension room_max)
{
	uint8_t height = (rand() % (room_max.height - room_min.height + 1)) +
	    room_min.height;

	uint8_t width = (rand() % (room_max.width - room_min.width + 1)) +
	    room_min.width;

	uint8_t oy =
	    rand() % (level->dimension.height - height + 1 /* full height */ -
		2 /* borders */);

	uint8_t ox =
	    rand() % (level->dimension.width - width + 1 /* full width */ -
		2 /* borders */);

	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++)
			level->tiles[oy + y + 1][ox + x + 1] = TA_FLOOR;
	}

	anchor->y = oy + (rand() % height);
	anchor->x = ox + (rand() % width);
}

static void _connect_anchors(struct level *level, uint8_t rooms,
    struct coordinate anchors[])
{
	for (uint8_t i = 0; i < rooms - 1; i++) {
		struct coordinate start = anchors[i];
		struct coordinate stop = anchors[i + 1];

		uint8_t ay = start.y;
		uint8_t by = stop.y;

		if (ay > by) {
			ay = stop.y;
			by = start.y;
		}

		uint8_t ax = start.x;
		uint8_t bx = stop.x;

		if (ax > bx) {
			ax = stop.x;
			bx = start.x;
		}

		for (uint8_t x = ax; x <= bx; x++) {
			level->tiles[start.y][x] = TA_FLOOR;
		}

		for (uint8_t y = ay; y <= by; y++) {
			level->tiles[y][stop.x] = TA_FLOOR;
		}
	}
}
