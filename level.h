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

#include <stdint.h>

#include "structs.h"

#define MAX_TORCHES 20
#define MIN_TORCHES  5

typedef enum {
	TA_FLOOR = 1U << 0,
	TA_WALL = 1U << 1,
	TA_VISIBLE = 1U << 2,
	TA_KNOWN = 1U << 3,
	TA_TORCH = 1U << 4,
} TILE_ATTRIBUTE;

struct level {
	struct dimension dimension;
	uint8_t **tiles;
};

struct level *level_create(struct dimension _level, uint8_t rooms,
    struct dimension _room_min, struct dimension _room_max);

void level_destroy(struct level *_level);
