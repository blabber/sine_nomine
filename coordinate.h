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

#include <stdbool.h>

struct coordinate {
	unsigned int y;
	unsigned int x;
};

struct coordinate_offset {
	int y;
	int x;
};

struct coordinate_dimension {
	unsigned int height;
	unsigned int width;
};

bool coordinate_check_bounds(
    struct coordinate_dimension _dimension, struct coordinate _coordinate);

bool coordinate_check_bounds_offset(struct coordinate_dimension _dimension,
    struct coordinate _coordinate, struct coordinate_offset _offset);

struct coordinate coordinate_add_offset(
    struct coordinate _coordinate, struct coordinate_offset _offset);

struct coordinate_offset coordinate_get_offset(
    struct coordinate _a, struct coordinate _b);
