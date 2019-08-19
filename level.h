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

#pragma once

#include "coordinate.h"
#include "structs.h"

typedef enum {
	TA_FLOOR = 1U << 0,
	TA_WALL = 1U << 1,
	TA_VISIBLE = 1U << 2,
	TA_KNOWN = 1U << 3,
	TA_TORCH = 1U << 4,
} TILE_ATTRIBUTE;

struct level_tile {
	unsigned int dijkstra;
	unsigned int flags;
};

struct level {
	struct coordinate_dimension dimension;
	struct level_tile **tiles;
};

struct level *level_create(struct coordinate_dimension _level,
    unsigned int rooms, struct coordinate_dimension _room_min,
    struct coordinate_dimension _room_max);

void level_destroy(struct level *_level);

void level_modify_random_floor_tiles(
    struct level *_level, unsigned int _count, unsigned int _mask);
