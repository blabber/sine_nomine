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

#include <sine_nomine/dijkstra.h>
#include <sine_nomine/err.h>
#include <sine_nomine/level.h>
#include <sine_nomine/structs.h>

struct _tile_queue {
	struct _tile_queue *next;
	dijkstra *value;
	struct coordinate position;
};

static struct dijkstra_map *_allocate_map(struct level *_level);

static struct _tile_queue *_enqueue(struct _tile_queue *_tail,
    struct coordinate _position, struct dijkstra_map *_map);

static struct _tile_queue *_dequeue(struct _tile_queue *_head);

struct dijkstra_map *
dijkstra_create(struct level *level)
{
	struct dijkstra_map *m = _allocate_map(level);
	m->level = level;

	for (unsigned int y = 0; y < m->level->dimension.height; y++) {
		for (unsigned int x = 0; x < m->level->dimension.width; x++) {
			m->values[y][x] = DIJKSTRA_MAX;
		}
	}

	return (m);
}

void
dijkstra_destroy(struct dijkstra_map *map)
{
	for (unsigned int y = 0; y < map->level->dimension.height; y++) {
		free(map->values[y]);
	}

	free(map->values);
	free(map);
}

void
dijkstra_add_target(
    struct dijkstra_map *map, struct coordinate position, unsigned int value)
{
	if (map->values[position.y][position.x] <= value)
		return;

	struct _tile_queue *head, *tail;
	head = tail = _enqueue(NULL, position, map);
	*tail->value = value;

	for (; head != NULL; head = _dequeue(head)) {
		struct coordinate_offset off[4] = { { -1, 0 }, { 0, 1 },
			{ 1, 0 }, { 0, -1 } };

		for (int i = 0; i < 4; i++) {
			if (!coordinate_check_bounds_offset(
				map->level->dimension, head->position, off[i]))
				continue;

			struct coordinate c =
			    coordinate_add_offset(head->position, off[i]);

			if (map->values[c.y][c.x] <= *head->value + 1)
				continue;

			if (map->level->tiles[c.y][c.x].flags & TA_WALL)
				continue;

			tail = _enqueue(tail, c, map);
			*tail->value = *head->value + 1;
		}
	}
}

static struct dijkstra_map *
_allocate_map(struct level *level)
{
	struct dijkstra_map *m = calloc(1, sizeof(struct dijkstra_map));
	if (m == NULL)
		err("calloc");

	assert(m != NULL);

	m->values = calloc(level->dimension.height, sizeof(*m->values));
	for (unsigned int y = 0; y < level->dimension.height; y++) {
		m->values[y] =
		    calloc(level->dimension.width, sizeof(**m->values));
	}

	return (m);
}

struct _tile_queue *
_dequeue(struct _tile_queue *head)
{
	struct _tile_queue *tmp = head->next;
	free(head);
	return (tmp);
}

struct _tile_queue *
_enqueue(struct _tile_queue *tail, struct coordinate position,
    struct dijkstra_map *map)
{
	struct _tile_queue *new = calloc(1, sizeof(struct _tile_queue));
	if (new == NULL)
		err("calloc");

	assert(new != NULL);

	new->next = NULL;
	new->value = &map->values[position.y][position.x];
	new->position = position;

	if (tail != NULL)
		tail->next = new;

	return (new);
}
