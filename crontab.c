/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2017, Jakob Kaivo <jkk@ung.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define _XOPEN_SOURCE 700
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

#define CRONTAB_PATH "/var/cron/"

static char *getcrontabname(void)
{
	static char path[PATH_MAX] = CRONTAB_PATH;
	struct passwd *pwd = getpwuid(getuid());
	if (pwd == NULL) {
		fprintf(stderr, "crontab: Couldn't determine username\n");
		exit(1);
	}
	strcat(path, pwd->pw_name);
	return path;
}

static int readcrontab(const char *file)
{
	if (file && !strcmp(file, getcrontabname())) {
		/* daemonize */
		return 0;
	}

	FILE *in = file ? fopen(file, "r") : stdin;
	if (in == NULL) {
		fprintf(stderr, "crontab: Couldn't open %s: %s\n", file, strerror(errno));
		return 1;
	}
	FILE *out = fopen(getcrontabname(), "w");
	if (out == NULL) {
		fprintf(stderr, "crontab: Couldn't open entry to save: %s\n", strerror(errno));
		return 1;
	}

	while (!feof(in)) {
		/* read input */
		/* verify formatting */
		/* write output */
	}

	fclose(in);
	fclose(out);

	/* advanced: daeemonize and run the thing */
	
	return 0;
}

static int listcrontab(void)
{
	FILE *f = fopen(getcrontabname(), "r");
	if (f == NULL) {
		fprintf(stderr, "crontab: Couldn't open entry: %s\n", strerror(errno));
		return 1;
	}

	int c;
	while ((c = fgetc(f)) != EOF) {
		putchar(c);
	}

	fclose(f);

	return 0;
}

static int removecrontab(void)
{
	if (remove(getcrontabname()) != 0) {
		fprintf(stderr, "Couldn't remove entry: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

static int editcrontab(void)
{
	char *editor = getenv("EDITOR");
	if (editor == NULL) {
		editor = "vi";
	}

	execlp(editor, editor, getcrontabname(), 0);

	fprintf(stderr, "crontab: Couldn't edit entry: %s\n", strerror(errno));

	return 1;
}

int main(int argc, char *argv[])
{
	enum { READ, EDIT, LIST, REMOVE } mode = READ;
	int c;
	while ((c = getopt(argc, argv, "elr")) != -1) {
		switch (c) {
		case 'e':	/** edit the current user's crontab entry **/
			mode = EDIT;
			break;

		case 'l':	/** list the current user's crontab entry **/
			mode = LIST;
			break;

		case 'r':	/** remove the current user's crontab entry **/
			mode = REMOVE;
			break;

		default:
			return 1;
		}
	}

	if (mode != READ && optind != argc) {
		fprintf(stderr, "crontab: Option takes no additional arguments\n");
		return 1;
	}

	if (mode == LIST) {
		return listcrontab();
	}

	if (mode == REMOVE) {
		return removecrontab();
	}

	if (mode == EDIT) {
		return editcrontab();
	}
	
	if (optind < argc - 1) {
		fprintf(stderr, "crontab: At most one file may be specified\n");
		return 1;
	}

	return readcrontab(argv[optind]);
}
