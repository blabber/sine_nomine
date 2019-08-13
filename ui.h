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

#pragma once

#include "level.h"
#include "structs.h"

typedef enum {
	UA_UNKNOWN,
	UA_QUIT,
	UA_UP,
	UA_DOWN,
	UA_LEFT,
	UA_RIGHT,
} UI_ACTION;

struct ui_context;

struct ui_context *ui_create(void);

void ui_destroy(struct ui_context *_context);

void ui_display(struct ui_context *_context, struct player _player,
    struct level *_level);

UI_ACTION ui_get_action(struct ui_context *_context);
