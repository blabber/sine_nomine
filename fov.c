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

#include <math.h>

#include "bresenham.h"
#include "fov.h"
#include "game.h"
#include "level.h"
#include "structs.h"

void
fov_calculate(struct player player, struct level *level)
{
	for (uint8_t y = 0; y < level->dimension.height; y++) {
		for (uint8_t x = 0; x < level->dimension.width; x++)
			level->tiles[y][x] &= ~TA_VISIBLE;
	}


	for (int16_t yoff = -player.range; yoff <= player.range; yoff++) {
		int16_t y = player.position.y + yoff;

		if (y < 0 || y > level->dimension.height - 1)
			continue;

		for (int16_t xoff = -player.range; xoff <= player.range; xoff++) {
			int16_t x = player.position.x + xoff;

			if (x < 0 || x > level->dimension.width - 1)
				continue;

			double len = hypot((double)xoff, (double)yoff);
			if (round(len) > (double)player.range)
				continue;

			struct coordinate p = {y, x};
			struct bresenham_line *l =
			    bresenham_create_line(player.position, p);

			for (uint8_t i = 0; i < l->elements; i++) {
				struct coordinate v = l->points[i];
				level->tiles[v.y][v.x] |=
				    (TA_VISIBLE | TA_KNOWN);

				if (level->tiles[v.y][v.x] & TA_WALL)
					break;
			}

			bresenham_free_line(l);
		}
	}
}
