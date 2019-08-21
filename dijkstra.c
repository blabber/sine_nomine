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
#include <limits.h>

#include "dijkstra.h"
#include "err.h"
#include "level.h"
#include "structs.h"

struct _tile_queue {
	struct _tile_queue *next;
	struct level_tile *tile;
	struct coordinate position;
};

struct _tile_queue *_enqueue(struct _tile_queue *_tail,
    struct coordinate _position, struct level *_level);

struct _tile_queue *_dequeue(struct _tile_queue *_head);

void
dijkstra_reset(struct level *level)
{
	for (unsigned int y = 0; y < level->dimension.height; y++) {
		for (unsigned int x = 0; x < level->dimension.width; x++) {
			level->tiles[y][x].dijkstra = UINT_MAX;
		}
	}
}

void
dijkstra_add_target(
    struct coordinate origin, struct level *level, unsigned int value)
{
	if (level->tiles[origin.y][origin.x].dijkstra <= value)
		return;

	struct _tile_queue *head, *tail;
	head = tail = _enqueue(NULL, origin, level);
	tail->tile->dijkstra = value;

	for (; head != NULL; head = _dequeue(head)) {
		struct coordinate_offset off[4] = { { -1, 0 }, { 0, 1 },
			{ 1, 0 }, { 0, -1 } };

		for (int i = 0; i < 4; i++) {
			if (!coordinate_check_bounds_offset(
				level->dimension, head->position, off[i]))
				continue;

			struct coordinate c =
			    coordinate_add_offset(head->position, off[i]);

			if (level->tiles[c.y][c.x].dijkstra <=
			    head->tile->dijkstra + 1)
				continue;

			if (level->tiles[c.y][c.x].flags & TA_WALL)
				continue;

			tail = _enqueue(tail, c, level);
			tail->tile->dijkstra = head->tile->dijkstra + 1;
		}
	}
}

struct _tile_queue *
_dequeue(struct _tile_queue *head)
{
	struct _tile_queue *tmp = head->next;
	free(head);
	return (tmp);
}

struct _tile_queue *
_enqueue(
    struct _tile_queue *tail, struct coordinate position, struct level *level)
{
	struct _tile_queue *new = calloc(1, sizeof(struct _tile_queue));
	if (new == NULL)
		err("calloc");

	assert(new != NULL);

	new->next = NULL;
	new->tile = &level->tiles[position.y][position.x];
	new->position = position;

	if (tail != NULL)
		tail->next = new;

	return (new);
}
