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

#include <stdbool.h>

#include "coordinate.h"

bool
coordinate_check_bounds(
    struct coordinate_dimension dimension, struct coordinate coordinate)
{
	if (coordinate.y > dimension.height - 1)
		return (false);

	if (coordinate.x > dimension.width - 1)
		return (false);

	return (true);
}

bool
coordinate_check_bounds_offset(struct coordinate_dimension dimension,
    struct coordinate coordinate, struct coordinate_offset offset)
{
	if (coordinate.y + offset.y < 0)
		return (false);

	if (coordinate.x + offset.x < 0)
		return (false);

	struct coordinate c = coordinate_add_offset(coordinate, offset);
	return coordinate_check_bounds(dimension, c);
}

struct coordinate
coordinate_add_offset(
    struct coordinate coordinate, struct coordinate_offset offset)
{
	if (coordinate.y + offset.y < 0 || coordinate.x + offset.x < 0)
		return (coordinate);

	struct coordinate c = { coordinate.y + offset.y,
		coordinate.x + offset.x };
	return (c);
}

struct coordinate_offset
coordinate_get_offset(struct coordinate a, struct coordinate b)
{
	struct coordinate_offset off = { a.y - b.y, a.x - b.x };
	return (off);
}
