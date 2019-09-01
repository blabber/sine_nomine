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

#include <sine_nomine/coordinate.h>
#include <sine_nomine/dijkstra.h>
#include <sine_nomine/level.h>

enum { HEIGHT = 6,
	WIDTH = 5,
};

static void _test_empty_level_one_target(void);

static void _test_empty_level_two_targets(void);

static void _test_nonempty_level_one_target(void);

static void _test_nonempty_level_two_targets(void);

int
main()
{
	_test_empty_level_one_target();
	_test_empty_level_two_targets();
	_test_nonempty_level_one_target();
	_test_nonempty_level_two_targets();

	exit(EXIT_SUCCESS);
}

static void
_test_empty_level_one_target()
{
	struct coordinate_dimension d = { HEIGHT, WIDTH };
	struct level *l = level_create(d);
	assert(l != NULL);

	struct dijkstra_map *dm = dijkstra_create(l);

	struct coordinate c = { 0, 0 };
	dijkstra_add_target(dm, c, 0);

	/* clang-format off */
	dijkstra expected[HEIGHT][WIDTH] = {
		{ 0, 1, 2, 3, 4 },
		{ 1, 2, 3, 4, 5 },
		{ 2, 3, 4, 5, 6 },
		{ 3, 4, 5, 6, 7 },
		{ 4, 5, 6, 7, 8 },
		{ 5, 6, 7, 8, 9 },
	};
	/* clang-format on */

	for (unsigned int y = 0; y < l->dimension.height; y++) {
		for (unsigned int x = 0; x < l->dimension.width; x++)
			assert(dm->values[y][x] == expected[y][x]);
	}

	dijkstra_destroy(dm);
	level_destroy(l);
}

static void
_test_empty_level_two_targets()
{
	struct coordinate_dimension d = { HEIGHT, WIDTH };
	struct level *l = level_create(d);
	assert(l != NULL);

	struct dijkstra_map *dm = dijkstra_create(l);

	struct coordinate c = { 0, 0 };
	dijkstra_add_target(dm, c, 0);

	c = (struct coordinate) { 3, 3 };
	dijkstra_add_target(dm, c, 3);

	/* clang-format off */
	dijkstra expected[HEIGHT][WIDTH] = {
		{ 0, 1, 2, 3, 4 },
		{ 1, 2, 3, 4, 5 },
		{ 2, 3, 4, 4, 5 },
		{ 3, 4, 4, 3, 4 },
		{ 4, 5, 5, 4, 5 },
		{ 5, 6, 6, 5, 6 },
	};
	/* clang-format on */

	for (unsigned int y = 0; y < l->dimension.height; y++) {
		for (unsigned int x = 0; x < l->dimension.width; x++)
			assert(dm->values[y][x] == expected[y][x]);
	}

	dijkstra_destroy(dm);
	level_destroy(l);
}

static void
_test_nonempty_level_one_target()
{
	struct coordinate_dimension d = { HEIGHT, WIDTH };
	struct level *l = level_create(d);
	assert(l != NULL);

	struct dijkstra_map *dm = dijkstra_create(l);

	for (unsigned int y = 0; y < 4; y++)
		l->tiles[y][2].flags |= TA_WALL;

	struct coordinate c = { 0, 0 };
	dijkstra_add_target(dm, c, 0);

	/* clang-format off */
	dijkstra expected[HEIGHT][WIDTH] = {
		{ 0, 1, DIJKSTRA_MAX, 11, 12 },
		{ 1, 2, DIJKSTRA_MAX, 10, 11 },
		{ 2, 3, DIJKSTRA_MAX,  9, 10 },
		{ 3, 4, DIJKSTRA_MAX,  8,  9 },
		{ 4, 5,            6,  7,  8 },
		{ 5, 6,            7,  8,  9 },
	};
	/* clang-format on */

	for (unsigned int y = 0; y < l->dimension.height; y++) {
		for (unsigned int x = 0; x < l->dimension.width; x++)
			assert(dm->values[y][x] == expected[y][x]);
	}

	dijkstra_destroy(dm);
	level_destroy(l);
}

static void
_test_nonempty_level_two_targets()
{
	struct coordinate_dimension d = { HEIGHT, WIDTH };
	struct level *l = level_create(d);
	assert(l != NULL);

	struct dijkstra_map *dm = dijkstra_create(l);

	for (unsigned int y = 0; y < 4; y++)
		l->tiles[y][2].flags |= TA_WALL;

	struct coordinate c = { 0, 0 };
	dijkstra_add_target(dm, c, 0);

	c = (struct coordinate) { 3, 3 };
	dijkstra_add_target(dm, c, 3);

	/* clang-format off */
	dijkstra expected[HEIGHT][WIDTH] = {
		{ 0, 1, DIJKSTRA_MAX, 6, 7 },
		{ 1, 2, DIJKSTRA_MAX, 5, 6 },
		{ 2, 3, DIJKSTRA_MAX, 4, 5 },
		{ 3, 4, DIJKSTRA_MAX, 3, 4 },
		{ 4, 5,            5, 4, 5 },
		{ 5, 6,            6, 5, 6 },
	};
	/* clang-format on */

	for (unsigned int y = 0; y < l->dimension.height; y++) {
		for (unsigned int x = 0; x < l->dimension.width; x++)
			assert(dm->values[y][x] == expected[y][x]);
	}

	dijkstra_destroy(dm);
	level_destroy(l);
}
