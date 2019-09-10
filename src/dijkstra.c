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
#include <stdlib.h>

#include <assert.h>

#include <sine_nomine/dijkstra.h>
#include <sine_nomine/err.h>
#include <sine_nomine/level.h>
#include <sine_nomine/structs.h>

struct dijkstra_map {
	/*
	 * The dijkstra map holds a pointer to the level. This might be a
	 * problem, as the dijkstra map is a snapshot of a specific point in
	 * time, but the level moves on. I am reluctant to create a snapshot of
	 * the level for the dijkstra map, but it might be necessary once
	 * things get more complex. Better to keep this in mind.
	 */
	struct level *level;
	dijkstra *values;
};

struct _queue {
	unsigned int capacity;
	unsigned int head;
	unsigned int tail;

	struct coordinate *buffer;
};

static struct dijkstra_map *_allocate_map(struct level *_level);

static struct _queue *_queue_create(unsigned int _capacity);

static void _queue_destroy(struct _queue *_queue);

static void _enqueue(struct _queue *_queue, struct coordinate _position);

static struct coordinate _dequeue(struct _queue *_queue);

static bool _queue_empty(struct _queue *_queue);

static unsigned int _queue_increment(unsigned int _index, struct _queue *_queue);

unsigned int _index(struct dijkstra_map *_map, struct coordinate _position);

struct dijkstra_map *
dijkstra_create(struct level *level)
{
	assert(level != NULL);

	struct dijkstra_map *m = _allocate_map(level);
	m->level = level;

	for (unsigned int y = 0; y < m->level->dimension.height; y++) {
		for (unsigned int x = 0; x < m->level->dimension.width; x++) {
			struct coordinate c = { y, x };
			m->values[_index(m, c)] = DIJKSTRA_MAX;
		}
	}

	return (m);
}

void
dijkstra_destroy(struct dijkstra_map *map)
{
	assert(map != NULL);

	free(map->values);
	free(map);
}

void
dijkstra_add_target(
    struct dijkstra_map *map, struct coordinate position, dijkstra value)
{
	assert(map != NULL);
	assert(coordinate_check_bounds(map->level->dimension, position));

	if (map->values[_index(map, position)] <= value)
		return;

	struct _queue *q = _queue_create(10);
	_enqueue(q, position);
	map->values[_index(map, position)] = value;

	while (!_queue_empty(q)) {
		struct coordinate c = _dequeue(q);

		struct coordinate_offset off[4] = { { -1, 0 }, { 0, 1 },
			{ 1, 0 }, { 0, -1 } };

		for (int i = 0; i < 4; i++) {
			if (!coordinate_check_bounds_offset(
				map->level->dimension, c, off[i]))
				continue;

			struct coordinate ct = coordinate_add_offset(c, off[i]);

			if (map->values[_index(map, ct)] <=
			    map->values[_index(map, c)] + 1)
				continue;

			if (map->level->tiles[ct.y][ct.x].flags & TA_WALL)
				continue;

			_enqueue(q, ct);
			map->values[_index(map, ct)] =
			    map->values[_index(map, c)] + 1;
		}
	}

	_queue_destroy(q);
}

dijkstra
dijkstra_get_value(struct dijkstra_map *map, struct coordinate position)
{
	assert(map != NULL);
	assert(coordinate_check_bounds(map->level->dimension, position));

	return (map->values[_index(map, position)]);
}

static struct dijkstra_map *
_allocate_map(struct level *level)
{
	struct dijkstra_map *m = calloc(1, sizeof(struct dijkstra_map));
	if (m == NULL)
		err("calloc");

	assert(m != NULL);

	m->values = calloc(level->dimension.height * level->dimension.width,
	    sizeof(*m->values));

	return (m);
}

static struct _queue *
_queue_create(unsigned int capacity)
{
	assert(capacity > 0);

	struct _queue *q = calloc(1, sizeof(struct _queue));
	if (q == NULL)
		err("calloc");

	assert(q != NULL);

	q->capacity = capacity;
	q->buffer = calloc(capacity, sizeof(*q->buffer));
	if (q->buffer == NULL)
		err("calloc");

	assert(q->buffer != NULL);

	return (q);
}

static void
_queue_destroy(struct _queue *queue)
{
	free(queue->buffer);
	free(queue);
}

static bool
_queue_empty(struct _queue *queue)
{
	return (queue->head == queue->tail);
}

static struct coordinate
_dequeue(struct _queue *queue)
{
	assert(queue != NULL);
	assert(!_queue_empty(queue));

	unsigned int current_head = queue->head;
	queue->head = _queue_increment(queue->head, queue);

	return queue->buffer[current_head];
}

static void
_enqueue(struct _queue *queue, struct coordinate position)
{
	assert(queue != NULL);

	/*
	 * Handle buffer overflow.
	 */
	if (_queue_increment(queue->tail, queue) == queue->head) {
		unsigned int new_capacity = queue->capacity * 2;

		struct coordinate *b =
		    calloc(new_capacity, sizeof(*queue->buffer));
		if (b == NULL)
			err("calloc");

		assert(b != NULL);

		unsigned int i = 0;
		while (!_queue_empty(queue)) {
			b[i++] = _dequeue(queue);
		}

		free(queue->buffer);
		queue->buffer = b;

		queue->capacity = new_capacity;
		queue->head = 0;
		queue->tail = i;
	}

	queue->buffer[queue->tail] = position;
	queue->tail = _queue_increment(queue->tail, queue);

	assert(!_queue_empty(queue));
}

static unsigned int
_queue_increment(unsigned int index, struct _queue *queue)
{
	return ((index + 1) % queue->capacity);
}

unsigned int
_index(struct dijkstra_map *map, struct coordinate position)
{
	return (position.y * map->level->dimension.width + position.x);
}
