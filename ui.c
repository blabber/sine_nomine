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
#include <curses.h>

#include "err.h"
#include "level.h"
#include "ui.h"

enum { CP_VISIBLE = 1,
};

struct ui_context {
	WINDOW *window;
};

/*
 * The ui_context is expected to be passed in by the caller in most functions.
 * The only exception is the killing of the context before error messages are
 * emitted. In these cases the context is usually not available, that's why it
 * kept redundantly in this variable. ui_emergency_exit() makes use of it.
 */
static struct ui_context *_ui_context;

static struct coordinate_dimension _screen_dimension(
    struct ui_context *_ui_context);

struct ui_context *
ui_create(void)
{
	struct ui_context *c = _ui_context =
	    calloc(1, sizeof(struct ui_context));
	if (c == NULL)
		err("calloc");

	assert(c != NULL);

	c->window = initscr();
	cbreak();
	noecho();
	keypad(c->window, TRUE);

	start_color();
	init_pair(CP_VISIBLE, COLOR_GREEN, COLOR_BLACK);

	return (c);
}

void
ui_destroy(struct ui_context *context)
{
	assert(context != NULL);

	_ui_context = NULL;
	endwin();
}

void
ui_display(
    struct ui_context *context, struct player player, struct level *level)
{
	assert(context != NULL);
	assert(level != NULL);

	struct coordinate_dimension screen = _screen_dimension(context);
	struct coordinate center = { screen.height / 2, screen.width / 2 };

	struct coordinate_offset offset =
	    coordinate_get_offset(center, player.position);

	werase(context->window);
	for (unsigned int y = 0; y < level->dimension.height; y++) {
		for (unsigned int x = 0; x < level->dimension.width; x++) {
			struct coordinate tile = { y, x };

			if (!coordinate_check_bounds_offset(
				screen, tile, offset))
				continue;

			struct coordinate screen_coordinate =
			    coordinate_add_offset(tile, offset);

			if (!(level->tiles[y][x].flags & TA_KNOWN))
				continue;

			wattrset(context->window, A_NORMAL);
			if (level->tiles[tile.y][tile.x].flags & TA_VISIBLE) {
				wattron(
				    context->window, COLOR_PAIR(CP_VISIBLE));

				wattron(context->window, A_BOLD);
			}

			char t = '#';
			if (level->tiles[tile.y][tile.x].flags & TA_FLOOR)
				t = '.';
			if (level->tiles[tile.y][tile.x].flags & TA_TORCH)
				t = 'T';

			mvwaddch(context->window, screen_coordinate.y,
			    screen_coordinate.x, t);
		}
	}

	wattrset(context->window, A_NORMAL);
	mvwaddch(context->window, center.y, center.x, '@');

	wmove(context->window, screen.height - 1, screen.width - 1);
	wrefresh(context->window);
}

void
ui_timeout(struct ui_context *context, unsigned int timeout)
{
	wtimeout(context->window, timeout);
}

UI_ACTION
ui_get_action(struct ui_context *context)
{
	assert(context != NULL);

	int c = wgetch(context->window);

	switch (c) {
	case KEY_UP:
	case 'k':
		return (UA_UP);

	case KEY_DOWN:
	case 'j':
		return (UA_DOWN);

	case KEY_LEFT:
	case 'h':
		return (UA_LEFT);

	case KEY_RIGHT:
	case 'l':
		return (UA_RIGHT);

	case 'a':
		return (UA_AUTOEXPLORE);

	case 'q':
		return (UA_QUIT);

	case ERR:
		return (UA_TIMEOUT);
	}

	return (UA_UNKNOWN);
}

void
ui_emergency_exit(void)
{
	ui_destroy(_ui_context);
}

static struct coordinate_dimension
_screen_dimension(struct ui_context *context)
{
	int height, width;
	getmaxyx(context->window, height, width);

	struct coordinate_dimension d = { height, width };

	return (d);
}
