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

#include "dijkstra.h"
#include "err.h"
#include "level.h"
#include "structs.h"

struct _tile_queue {
	struct _tile_queue *next;
	struct level_tile *tile;
	struct coordinate position;
};

void
dijkstra_refresh(struct coordinate origin, struct level *level)
{
	for (unsigned int y = 0; y < level->dimension.height; y++) {
		for (unsigned int x = 0; x < level->dimension.width; x++) {
			level->tiles[y][x].dijkstra = -1;
		}
	}

	struct _tile_queue *head, *tail;
	head = tail = calloc(1, sizeof(struct _tile_queue));
	if (tail == NULL)
		err("dijkstra_refresh: calloc");

	assert(head != NULL);

	head->next = NULL;
	head->tile = &level->tiles[origin.y][origin.x];
	head->tile->dijkstra = 0;
	head->position = (struct coordinate) { origin.y, origin.x };

	while (head != NULL) {
		unsigned int y = head->position.y;
		unsigned int x = head->position.x;

		struct offset off[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 },
			{ 0, -1 } };

		for (int i = 0; i < 4; i++) {
			if (y + off[i].y < 0 ||
			    y + off[i].y > level->dimension.height - 1)
				continue;

			if (x + off[i].x < 0 ||
			    x + off[i].x > level->dimension.width - 1)
				continue;

			struct coordinate c = { y + off[i].y, x + off[i].x };

			if (level->tiles[c.y][c.x].dijkstra >= 0)
				continue;

			if (level->tiles[c.y][c.x].flags & TA_WALL)
				continue;

			struct _tile_queue *new =
			    calloc(1, sizeof(struct _tile_queue));
			if (new == NULL)
				err("dijkstra_refresh: calloc");

			assert(new != NULL);

			new->next = NULL;
			new->tile = &level->tiles[c.y][c.x];
			new->tile->dijkstra = head->tile->dijkstra + 1;
			new->position = (struct coordinate) { c.y, c.x };

			tail->next = new;
			tail = new;
		}

		struct _tile_queue *tmp = head->next;
		free(head);
		head = tmp;
	}
}
