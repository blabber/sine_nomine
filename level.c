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
#include <stdbool.h>

#include <assert.h>
#include <string.h>

#include "err.h"
#include "level.h"
#include "structs.h"

static struct level *_allocate(struct coordinate_dimension _d);

static void _generate_dungeon(struct level *_l, unsigned int _rooms,
    struct coordinate_dimension _room_min,
    struct coordinate_dimension _room_max);

static void _carve_room(struct level *_level, struct coordinate *_anchor,
    struct coordinate_dimension _room_min,
    struct coordinate_dimension _room_max);

static void _connect_anchors(
    struct level *_level, unsigned int _rooms, struct coordinate _anchors[]);

struct level *
level_create(struct coordinate_dimension d, unsigned int rooms,
    struct coordinate_dimension room_min, struct coordinate_dimension room_max)
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

void
level_destroy(struct level *level)
{
	assert(level != NULL);

	for (unsigned int y = 0; y < level->dimension.height; y++) {
		free(level->tiles[y]);
	}

	free(level->tiles);
	free(level);
}

void
level_modify_random_floor_tiles(
    struct level *level, unsigned int count, unsigned int mask)
{
	for (unsigned int i = 1; i < count; i++) {
		for (;;) {
			unsigned int y = rand() % (level->dimension.height);
			unsigned int x = rand() % (level->dimension.width);

			if (level->tiles[y][x].flags & TA_FLOOR) {
				level->tiles[y][x].flags |= mask;
				break;
			}
		}
	}
}

static struct level *
_allocate(struct coordinate_dimension d)
{
	struct level *l = calloc(1, sizeof(struct level));
	if (l == NULL)
		err("calloc");

	assert(l != NULL);

	l->dimension = d;

	l->tiles = calloc(l->dimension.height, sizeof(*l->tiles));
	if (l->tiles == NULL)
		err("calloc");

	assert(l->tiles != NULL);

	for (unsigned int y = 0; y < l->dimension.height; y++) {
		l->tiles[y] = calloc(l->dimension.width, sizeof(**l->tiles));
		if (l->tiles[y] == NULL)
			err("calloc");

		assert(l->tiles[y] != NULL);

		memset(l->tiles[y], TA_WALL,
		    l->dimension.width * sizeof(**l->tiles));
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
_generate_dungeon(struct level *level, unsigned int rooms,
    struct coordinate_dimension room_min, struct coordinate_dimension room_max)
{
	struct coordinate anchors[rooms];
	for (unsigned int i = 0; i < rooms; i++)
		_carve_room(level, &anchors[i], room_min, room_max);

	for (unsigned int i = 0; i < rooms - 1; i++)
		_connect_anchors(level, rooms, anchors);
}

static void
_carve_room(struct level *level, struct coordinate *anchor,
    struct coordinate_dimension room_min, struct coordinate_dimension room_max)
{
	unsigned int height =
	    (rand() % (room_max.height - room_min.height + 1)) +
	    room_min.height;

	unsigned int width =
	    (rand() % (room_max.width - room_min.width + 1)) + room_min.width;

	unsigned int oy = rand() %
	    (level->dimension.height - height + 1 /* full height */ -
		2 /* borders */);

	unsigned int ox = rand() %
	    (level->dimension.width - width + 1 /* full width */ -
		2 /* borders */);

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++)
			level->tiles[oy + y + 1][ox + x + 1].flags = TA_FLOOR;
	}

	assert(height > 0);
	assert(width > 0);

	anchor->y = oy + (rand() % height);
	anchor->x = ox + (rand() % width);
}

static void
_connect_anchors(
    struct level *level, unsigned int rooms, struct coordinate anchors[])
{
	for (unsigned int i = 0; i < rooms - 1; i++) {
		struct coordinate start = anchors[i];
		struct coordinate stop = anchors[i + 1];

		unsigned int ay = start.y;
		unsigned int by = stop.y;

		if (ay > by) {
			ay = stop.y;
			by = start.y;
		}

		unsigned int ax = start.x;
		unsigned int bx = stop.x;

		if (ax > bx) {
			ax = stop.x;
			bx = start.x;
		}

		for (unsigned int x = ax; x <= bx; x++) {
			level->tiles[start.y][x].flags = TA_FLOOR;
		}

		for (unsigned int y = ay; y <= by; y++) {
			level->tiles[y][stop.x].flags = TA_FLOOR;
		}
	}
}
