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

#include <limits.h>

#include "dijkstra.h"
#include "fov.h"
#include "game.h"
#include "structs.h"
#include "ui.h"

enum { autoexplore_delay = 100,
};

struct game {
	struct level *level;
	struct player player;
	struct ui_context *ui;
	bool autoexplore;
};

static bool _validate_player_position(
    struct coordinate candidate, struct level *level);

static void _apply_effects(struct game *game);

static UI_ACTION _autoexplore(struct game *game);

struct game *
game_create(struct game_configuration config)
{
	sranddev();

	struct game *g = calloc(1, sizeof(struct game));
	g->ui = ui_create();

	struct coordinate_dimension d = { config.height, config.width };
	struct coordinate_dimension min = { config.roomsize.min,
		config.roomsize.min };
	struct coordinate_dimension max = { config.roomsize.max,
		config.roomsize.max };

	g->player = (struct player) { .range = config.range };
	g->level = level_create(d, config.rooms, min, max);
	g->autoexplore = false;

	unsigned int torches =
	    rand() % (config.torches.max - config.torches.min + 1) +
	    config.torches.min;
	level_modify_random_floor_tiles(g->level, torches, TA_TORCH);

	/*
	 * Place player somewhere on the floor. The starting position should be
	 * determined by the dungeon generation algorithm. This in not yet
	 * implemented.
	 */
	for (;;) {
		unsigned int y = rand() % (g->level->dimension.height);
		unsigned int x = rand() % (g->level->dimension.width);

		if (g->level->tiles[y][x].flags & TA_FLOOR) {
			g->player.position.y = y;
			g->player.position.x = x;

			break;
		}
	}

	return (g);
}

void
game_destroy(struct game *game)
{
	ui_destroy(game->ui);
	level_destroy(game->level);
	free(game);
}

void
game_loop(struct game *game)
{
	bool running = true;
	while (running) {
		fov_calculate(game->player, game->level);
		ui_display(game->ui, game->player, game->level);

		struct coordinate np = game->player.position;

		UI_ACTION ua;
		if (game->autoexplore)
			ua = _autoexplore(game);
		else
			ua = ui_get_action(game->ui);

		/* Get user input and act on it. */
		switch (ua) {
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

		case UA_AUTOEXPLORE:
			ui_timeout(game->ui, autoexplore_delay);
			game->autoexplore = true;
			break;

		case UA_QUIT:
			running = false;
			break;

		case UA_TIMEOUT:
		case UA_UNKNOWN:
			break;
		}

		if (_validate_player_position(np, game->level))
			game->player.position = np;

		_apply_effects(game);
	}
}

static bool
_validate_player_position(struct coordinate candidate, struct level *level)
{
	if (!coordinate_check_bounds(level->dimension, candidate))
		return false;

	if (level->tiles[candidate.y][candidate.x].flags & TA_WALL)
		return false;

	return true;
}

static void
_apply_effects(struct game *game)
{
	struct level_tile **tiles = game->level->tiles;
	struct player player = game->player;

	if (tiles[player.position.y][player.position.x].flags & TA_TORCH) {
		tiles[player.position.y][player.position.x].flags &= ~TA_TORCH;
		player.range++;
	}

	game->player = player;
}

static UI_ACTION
_autoexplore(struct game *game)
{
	dijkstra_reset(game->level);

	/*
	 * Set all unvisited floor tiles as low priority targets.
	 */
	unsigned int targets = 0;
	for (unsigned int y = 0; y < game->level->dimension.height; y++) {
		for (unsigned int x = 0; x < game->level->dimension.height;
		     x++) {
			if (game->level->tiles[y][x].flags & TA_KNOWN)
				continue;

			struct coordinate c = { y, x };
			dijkstra_add_target(c, game->level, 20);
			targets++;
		}
	}

	/*
	 * Set known torches as high priority targets.
	 */
	for (unsigned int y = 0; y < game->level->dimension.height; y++) {
		for (unsigned int x = 0; x < game->level->dimension.height;
		     x++) {
			if (!(game->level->tiles[y][x].flags & TA_FLOOR))
				continue;

			if (!(game->level->tiles[y][x].flags & TA_KNOWN))
				continue;

			if (!(game->level->tiles[y][x].flags & TA_TORCH))
				continue;

			struct coordinate c = { y, x };
			dijkstra_add_target(c, game->level, 0);
			targets++;
		}
	}

	struct coordinate p = game->player.position;

	if (game->level->tiles[p.y][p.x].dijkstra == UINT_MAX ||
	    ui_get_action(game->ui) != UA_TIMEOUT) {
		game->autoexplore = false;
		ui_timeout(game->ui, -1);
		return UA_UNKNOWN;
	}

	unsigned int min_dijkstra = UINT_MAX;
	struct coordinate_offset min_offset = { 0, 0 };

	struct coordinate_offset off[] = { { -1, 0 }, { 0, 1 }, { 1, 0 },
		{ 0, -1 } };
	for (int i = 0; i < 4; i++) {
		if (!coordinate_check_bounds_offset(
			game->level->dimension, game->player.position, off[i]))
			continue;

		struct coordinate c =
		    coordinate_add_offset(game->player.position, off[i]);

		if (game->level->tiles[c.y][c.x].dijkstra < min_dijkstra) {
			min_dijkstra = game->level->tiles[c.y][c.x].dijkstra;
			min_offset = off[i];
		}
	}

	if (min_offset.y == -1)
		return UA_UP;

	if (min_offset.y == 1)
		return UA_DOWN;

	if (min_offset.x == -1)
		return UA_LEFT;

	if (min_offset.x == 1)
		return UA_RIGHT;

	return UA_UNKNOWN;
}
