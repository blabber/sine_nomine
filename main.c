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

#include "level.h"
#include "structs.h"
#include "ui.h"

enum { 
	HEIGHT = 100,
	WIDTH = 100,
	ROOMS = 5,
	ROOMMINSIZE = 10,
	ROOMMAXSIZE = 20,
};

static bool _validate_player_position(struct coordinate _candidate);

static struct level *level;

int
main()
{
	struct ui_context *ui = ui_create();

	{
		struct dimension d = {HEIGHT, WIDTH};
		struct dimension min = {ROOMMINSIZE, ROOMMINSIZE};
		struct dimension max = {ROOMMAXSIZE, ROOMMAXSIZE};

		level = level_create(d, ROOMS, min, max);
	}

	struct coordinate player;

	/*
	 * Place player somewhere on the floor. The starting position should be
	 * determined by the dungeon generation algorithm. This in not yet
	 * implemented.
	 */
	while (true) {
		uint8_t y = rand() % (level->dimension.height);
		uint8_t x = rand() % (level->dimension.width);

		if (level->tiles[y][x] & TA_FLOOR) {
			player.y = y;
			player.x = x;

			break;
		}
	}

	bool running = true;
	while (running) {
		ui_display(ui, player, level);

		struct coordinate np = player;

		/* Get user input and act on it. */
		switch (ui_get_action(ui)) {
		case UA_UP:
			np.y--;
			break;

		case UA_DOWN:
			np.y++;
			break;

		case UA_LEFT:
			np.x--;
			break;

		case UA_RIGHT:
			np.x++;
			break;

		case UA_QUIT:
			running = false;
			break;

		case UA_UNKNOWN:
			break;
		}

		if (_validate_player_position(np))
			player = np;
	}

	ui_destroy(ui);

	return (EXIT_SUCCESS);
}

static bool
_validate_player_position(struct coordinate c)
{
	if (c.x < 0)
		return false;

	if (c.x > level->dimension.width - 1)
		return false;

	if (c.y < 0)
		return false;

	if (c.y > level->dimension.height - 1)
		return false;

	if (level->tiles[c.y][c.x] & TA_WALL)
		return false;

	return true;
}
