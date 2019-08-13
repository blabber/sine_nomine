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
#include <stdint.h>
#include <stdlib.h>

#include "fov.h"
#include "game.h"
#include "structs.h"
#include "ui.h"

struct game {
	struct level *level;
	struct player player;
	struct ui_context *ui;
};

static bool _validate_player_position(struct player candidate,
    struct level *level);

static void _apply_effects(struct game *game);

struct game *game_create(struct game_configuration config) {
	sranddev();

	struct game *g = calloc(1, sizeof(struct game));
	g->ui = ui_create();

	struct dimension d = {config.height, config.width};
	struct dimension min = {config.roomsize.min, config.roomsize.min};
	struct dimension max = {config.roomsize.max, config.roomsize.max};

	g->player = (struct player){.range = config.range};
	g->level = level_create(d, config.rooms, min, max);

	uint8_t torches =
	    rand() % (config.torches.max - config.torches.min + 1) +
	    config.torches.min;
	level_modify_random_floor_tiles(g->level, torches, TA_TORCH);

	/*
	 * Place player somewhere on the floor. The starting position should be
	 * determined by the dungeon generation algorithm. This in not yet
	 * implemented.
	 */
	for (;;) {
		uint8_t y = rand() % (g->level->dimension.height);
		uint8_t x = rand() % (g->level->dimension.width);

		if (g->level->tiles[y][x] & TA_FLOOR) {
			g->player.position.y = y;
			g->player.position.x = x;

			break;
		}
	}

	return (g);
}

void game_destroy(struct game *game)
{
	ui_destroy(game->ui);
	level_destroy(game->level);
	free(game);
}

void game_loop(struct game *game)
{
	bool running = true;
	while (running) {
		fov_calculate(game->player, game->level);
		ui_display(game->ui, game->player, game->level);

		struct player np = game->player;

		/* Get user input and act on it. */
		switch (ui_get_action(game->ui)) {
		case UA_UP:
			np.position.y--;
			break;

		case UA_DOWN:
			np.position.y++;
			break;

		case UA_LEFT:
			np.position.x--;
			break;

		case UA_RIGHT:
			np.position.x++;
			break;

		case UA_QUIT:
			running = false;
			break;

		case UA_UNKNOWN:
			break;
		}

		if (_validate_player_position(np, game->level))
			game->player = np;

		_apply_effects(game);
	}
}

static bool
_validate_player_position(struct player candidate, struct level *level)
{
	if (candidate.position.x < 0)
		return false;

	if (candidate.position.x > level->dimension.width - 1)
		return false;

	if (candidate.position.y < 0)
		return false;

	if (candidate.position.y > level->dimension.height - 1)
		return false;

	if (level->tiles[candidate.position.y][candidate.position.x] & TA_WALL)
		return false;

	return true;
}

static void
_apply_effects(struct game *game)
{
	uint8_t **tiles = game->level->tiles;
	struct player player = game->player;

	if (tiles[player.position.y][player.position.x] & TA_TORCH) {
		tiles[player.position.y][player.position.x] &= ~TA_TORCH;
		player.range++;
	}

	game->player = player;

}
