/*
 * Copyright (c) 2016, prpl Foundation
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Benjamin DuPont, nebland software, LLC (ben@nebland.com).
 */

/*
 * This code and protocol are based work done by Vadim Zaliva and
 * posted here: http://notbrainsurgery.livejournal.com/38622.html
 */

#include <stdio.h>

#include "owiarm.h"

void printusage(char *program)
{
	printf("Missing path to FTDI serial relay port\n");
	printf("  Usage: %s /dev/tty*\n", program);
}

int main(int argc, char **argv)
{
	int retval;

	if (argc != 2)
	{
		printusage(argv[0]);

		return -1;
	}

	if (owi_init_usb(LIBUSB_LOG_LEVEL_ERROR) != 0)
	{
		perror("owi_init_usb");
		return -1;
	}

	if ((retval = owi_find_arm()) != 0)
	{
		if (retval == 1)
		{
			fprintf(stderr, "ARM not found\n");
			return -1;
		}
		else
		{
			perror("owi_find_arm");
			return -1;
		}
	}

	printf("AMR located\n");

	owi_cleanup();

	return 0;
}
