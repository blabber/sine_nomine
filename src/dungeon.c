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

#include <sine_nomine/coordinate.h>
#include <sine_nomine/dungeon.h>
#include <sine_nomine/level.h>

static void _carve_room(struct level *_level, struct coordinate *_anchor,
    struct coordinate_dimension _room_min,
    struct coordinate_dimension _room_max);

static void _connect_anchors(
    struct level *_level, unsigned int _rooms, struct coordinate _anchors[]);

/*
 * Applies a trivial dungeon generator to the level.
 *
 * 1. Carve out a number of rectangular room (they may overlap)
 * 2. Define a anchor in every room.
 * 3. Iterate the rooms and carve a floor from the anchor of the current room to
 *    the anchor of the next room.
 */
void
dungeon_generate(struct level *level, unsigned int rooms,
    struct coordinate_dimension room_min, struct coordinate_dimension room_max)
{
	assert(rooms > 0);
	assert(room_min.height <= room_max.height);
	assert(room_min.width <= room_max.width);
	assert(room_max.width <= level->dimension.width - 2 /* borders */);
	assert(room_max.height <= level->dimension.height - 2 /*borders*/);

	for (unsigned int y = 0; y < level->dimension.height; y++) {
		for (unsigned int x = 0; x < level->dimension.width; x++)
			level->tiles[y][x].flags = TA_WALL;
	}

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

	assert(height > 0);
	assert(height >= room_min.height);
	assert(height <= room_max.height);
	assert(height <= level->dimension.height - 2 /* borders */);

	assert(width > 0);
	assert(width >= room_min.width);
	assert(width <= room_max.width);
	assert(width <= level->dimension.width - 2 /* borders */);

	unsigned int oy = (rand() % (level->dimension.height - height - 1)) + 1;
	unsigned int ox = (rand() % (level->dimension.width - width - 1)) + 1;

	assert(oy > 0);
	assert(oy < level->dimension.height - 1);

	assert(ox > 0);
	assert(ox < level->dimension.width - 1);

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			struct coordinate c = { oy + y, ox + x };

			level->tiles[c.y][c.x].flags = TA_FLOOR;

			assert(c.x > 0);
			assert(c.x < level->dimension.width - 1);

			assert(c.y > 0);
			assert(c.y < level->dimension.height - 1);
		}
	}

	anchor->y = oy + (rand() % height);
	anchor->x = ox + (rand() % width);

	assert(level->tiles[anchor->y][anchor->x].flags & TA_FLOOR);
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

			assert(x > 0);
			assert(x < level->dimension.width - 1);
		}

		for (unsigned int y = ay; y <= by; y++) {
			level->tiles[y][stop.x].flags = TA_FLOOR;

			assert(y > 0);
			assert(y < level->dimension.height - 1);
		}
	}
}
