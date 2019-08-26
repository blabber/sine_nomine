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

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <curses.h>
#include <errno.h>
#include <string.h>

#include <sine_nomine/err.h>
#include <sine_nomine/ui.h>

static void __err(bool _print_errno, const char *_fmt, va_list _ap);

void
_err(const char *fmt, ...)
{
	assert(fmt != NULL);

	va_list ap;
	va_start(ap, fmt);

	__err(true, fmt, ap);

	va_end(ap);
}

void
die(const char *fmt, ...)
{
	assert(fmt != NULL);

	va_list ap;
	va_start(ap, fmt);

	ui_emergency_exit();
	__err(false, fmt, ap);

	va_end(ap);
}

static void
__err(bool print_errno, const char *fmt, va_list ap)
{
	vfprintf(stderr, fmt, ap);

	if (print_errno)
		fprintf(stderr, ": %s\n", strerror(errno));

	exit(EXIT_FAILURE);
}
