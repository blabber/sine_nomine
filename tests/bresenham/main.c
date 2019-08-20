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

#include "../../bresenham.h"
#include "../../structs.h"

struct bresenham_test {
	struct coordinate c1;
	struct coordinate c2;
	unsigned int expected_len;
	struct coordinate expected_data[5];
};

/* clang-format off */
static struct bresenham_test tests[] = {
	{ { 0, 0 }, { 3, 4 }, 5, { { 0, 0}, { 1, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4} } },
	{ { 2, 1 }, { 6, 3 }, 5, { { 2, 1}, { 3, 1 }, { 4, 2 }, { 5, 2 }, { 6, 3} } },
};
/* clang-format on */

static void _test_line(struct bresenham_test _test);

int
main()
{
	for (unsigned int i = 0; i < sizeof(tests) / sizeof(*tests); i++) {
		_test_line(tests[i]);
	}

	exit(EXIT_SUCCESS);
}

static void
_test_line(struct bresenham_test test)
{
	struct bresenham_line *line1 = bresenham_create_line(test.c1, test.c2);
	assert(line1 != NULL);
	assert(line1->elements == test.expected_len);

	for (unsigned int i = 0; i < line1->elements; i++) {
		assert(line1->points[i].y == test.expected_data[i].y);
		assert(line1->points[i].x == test.expected_data[i].x);
	}

	struct bresenham_line *line2 = bresenham_create_line(test.c2, test.c1);
	assert(line2 != NULL);
	assert(line2->elements == 5);

	for (unsigned int i = 0; i < line2->elements; i++) {
		assert(line2->points[i].y ==
		    line1->points[line1->elements - 1 - i].y);
		assert(line2->points[i].x ==
		    line1->points[line1->elements - 1 - i].x);
	}

	bresenham_free_line(line1);
	bresenham_free_line(line2);
}
