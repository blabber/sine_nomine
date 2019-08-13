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
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "fov.h"
#include "level.h"
#include "structs.h"
#include "ui.h"

#define START_RANGE  4

enum {
	HEIGHT = 100,
	WIDTH = 100,
	ROOMS = 5,
	ROOMMINSIZE = 10,
	ROOMMAXSIZE = 20,
};

static struct option long_options[] =
	{
		{"help",    no_argument,       0, 1},
		{"rooms",   required_argument, 0, 2},
		{"height",  required_argument, 0, 3},
		{"width",   required_argument, 0, 4},
		{"range",   required_argument, 0, 5},
		{0, 0, 0, 0}
	};

static void _print_help(char** argv);
static bool _validate_player_position(struct player *_candidate);

static struct level *level;

int
main(int argc, char **argv)
{
	int rooms = ROOMS, height = HEIGHT, width = WIDTH, range = START_RANGE;

	/* options parsing */
	int option, long_index;
	while ((option = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
		switch (option) {
			case 1: _print_help(argv);exit(EXIT_SUCCESS);
			case 2: rooms  = strtol(optarg, NULL, 10); break;
			case 3: height = strtol(optarg, NULL, 10); break;
			case 4: width  = strtol(optarg, NULL, 10); break;
			case 5: range  = strtol(optarg, NULL, 10); break;
			default: _print_help(argv); exit(EXIT_FAILURE);
		}
	}
	if (range > 50) {
		printf("error: range cannot be bigger than 50\n");
		exit(EXIT_FAILURE);
	}

	struct ui_context *ui = ui_create();
	{
		struct dimension d = {height, width};
		struct dimension min = {ROOMMINSIZE, ROOMMINSIZE};
		struct dimension max = {ROOMMAXSIZE, ROOMMAXSIZE};

		level = level_create(d, rooms, min, max);
	}

	struct player player = {.range = range};

	/*
	 * Place player somewhere on the floor. The starting position should be
	 * determined by the dungeon generation algorithm. This in not yet
	 * implemented.
	 */
	while (true) {
		uint8_t y = rand() % (level->dimension.height);
		uint8_t x = rand() % (level->dimension.width);

		if (level->tiles[y][x] & TA_FLOOR) {
			player.position.y = y;
			player.position.x = x;

			break;
		}
	}

	bool running = true;
	while (running) {
		fov_calculate(player, level);
		ui_display(ui, player, level);

		struct player np = player;

		/* Get user input and act on it. */
		switch (ui_get_action(ui)) {
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

		if (_validate_player_position(&np))
			player = np;
	}

	ui_destroy(ui);

	return (EXIT_SUCCESS);
}

static bool
_validate_player_position(struct player *c)
{
	if (c->position.x < 0)
		return false;

	if (c->position.x > level->dimension.width - 1)
		return false;

	if (c->position.y < 0)
		return false;

	if (c->position.y > level->dimension.height - 1)
		return false;

	if (level->tiles[c->position.y][c->position.x] & TA_WALL)
		return false;

	if (level->tiles[c->position.y][c->position.x] & TA_TORCH) {
		level->tiles[c->position.y][c->position.x] &= ~TA_TORCH;
		c->range++;
	}

	return true;
}

static void _print_help(char **argv)
{
	printf("Usage: %s [options]\n", argv[0]);
	printf("       --help                 print this help\n");
	printf("       --rooms  <number>      number of rooms to generate\n");
	printf("       --height <number>      height of the full map\n");
	printf("       --width  <number>      width of the full map\n");
	printf("       --range  <number>      FOV range for the player to start with\n");
}
