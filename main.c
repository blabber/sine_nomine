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

#include "err.h"
#include "fov.h"
#include "game.h"
#include "level.h"
#include "structs.h"
#include "ui.h"

enum {
	HEIGHT = 100,
	WIDTH = 100,
	RANGE = 4,
	ROOMS = 5,
	ROOMMINSIZE = 10,
	ROOMMAXSIZE = 20,
	TORCHESMIN = 5,
	TORCHESMAX = 20,
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

int
main(int argc, char **argv)
{
	struct game_configuration config = {
		.height = HEIGHT,
		.width = WIDTH,
		.range = RANGE,
		.rooms = ROOMS,
		.roomsize = (struct range){ROOMMINSIZE, ROOMMAXSIZE},
		.torches = (struct range){TORCHESMIN, TORCHESMAX},
	};

	/* options parsing */
	int option, long_index;
	while ((option = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
		switch (option) {
			case 1: _print_help(argv); exit(EXIT_SUCCESS);
			case 2: config.rooms  = strtol(optarg, NULL, 10); break;
			case 3: config.height = strtol(optarg, NULL, 10); break;
			case 4: config.width  = strtol(optarg, NULL, 10); break;
			case 5: config.range  = strtol(optarg, NULL, 10); break;
			default: _print_help(argv); exit(EXIT_FAILURE);
		}
	}
	if (config.range > 15)
		die("error: range must not be bigger than 15\n");
	if (config.width < 25)
		die("error: width must not be smaller than 25\n");
	if (config.height < 25)
		die("error: width must not be smaller than 25\n");

	struct game *game = game_create(config);
	game_loop(game);
	game_destroy(game);

	return (EXIT_SUCCESS);
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
