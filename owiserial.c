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

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static int fd = -1;

int owi_serial_open(char *path)
{
	struct termios tio;

	fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);

	if (fd == -1)
		return -1;

	tcgetattr(fd, &tio);

	tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;

	tcflush(fd, TCIFLUSH);

	tcsetattr(fd, TCSANOW, &tio);

	return 0;
}

int owi_serial_close()
{
	return 0;
}

int owi_serial_open_send(unsigned char data1, unsigned char data2, unsigned char data3)
{
	return 0;
}
