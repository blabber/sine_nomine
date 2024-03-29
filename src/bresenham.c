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

#include <sine_nomine/bresenham.h>
#include <sine_nomine/err.h>
#include <sine_nomine/structs.h>

/*
 * An implementation of Bresenham's Line Algorithm. Takes two coordinates and
 * returns the coordinates that approximate the line connecting the two points.
 *
 * This is a C conversion of the Python implementation that can be found here:
 * http://www.roguebasin.com/index.php?title=Bresenham%27s_Line_Algorithm#Python
 */
struct bresenham_line *
bresenham_create_line(struct coordinate start, struct coordinate stop)
{
	/* Setup initial conditions */
	int dx = stop.x - start.x;
	int dy = stop.y - start.y;

	/* Determine how steep the line is */
	bool is_steep = abs(dy) > abs(dx);

	/* Rotate line */
	if (is_steep) {
		struct coordinate p1 = start;
		struct coordinate p2 = stop;

		start = (struct coordinate) { p1.x, p1.y };
		stop = (struct coordinate) { p2.x, p2.y };
	}

	/* Swap start and end points if necessary and store swap state */
	bool swapped = false;
	if (start.x > stop.x) {
		struct coordinate p1 = start;
		struct coordinate p2 = stop;

		start = (struct coordinate) { p2.y, p2.x };
		stop = (struct coordinate) { p1.y, p1.x };

		swapped = true;
	}

	/* Recalculate differentials */
	dx = stop.x - start.x;
	dy = stop.y - start.y;

	/* Calculate error */
	int error = dx / 2;
	int ystep = start.y < stop.y ? 1 : (-1);

	/* Provide some storage */
	struct bresenham_line *line = calloc(1, sizeof(struct bresenham_line));
	if (line == NULL)
		err("calloc");

	assert(line != NULL);

	line->capacity = 10;
	line->points = calloc(line->capacity, sizeof(struct coordinate));
	if (line->points == NULL)
		err("calloc");

	assert(line->points != NULL);

	/* Iterate over bounding box generating points between start and end */
	unsigned int y = start.y;
	for (unsigned int x = start.x; x <= stop.x; x++) {
		if (++line->elements > line->capacity) {
			line->capacity *= 2;

			line->points = realloc(line->points,
			    line->capacity * sizeof(struct coordinate));
			if (line->points == NULL)
				err("realloc");

			assert(line->points != NULL);
		}

		struct coordinate p;
		if (is_steep)
			p = (struct coordinate) { x, y };
		else
			p = (struct coordinate) { y, x };

		line->points[line->elements - 1] = p;

		error -= abs(dy);
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}

	/* Reverse the list if the coordinates were swapped */
	if (swapped) {
		struct coordinate *op = line->points;

		line->points =
		    calloc(line->elements, sizeof(struct coordinate));
		if (line->points == NULL)
			err("calloc");

		assert(line->points != NULL);

		for (unsigned int i = 0; i < line->elements; i++) {
			line->points[i] = op[line->elements - 1 - i];
		}

		free(op);
	}

	return (line);
}

void
bresenham_free_line(struct bresenham_line *line)
{
	assert(line != NULL);

	free(line->points);
	free(line);
}
