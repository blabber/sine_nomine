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

#include <sine_nomine/err.h>
#include <sine_nomine/level.h>
#include <sine_nomine/structs.h>

struct level *
level_create(struct coordinate_dimension d)
{
	assert(d.height > 0);
	assert(d.width > 0);

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
	}

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
