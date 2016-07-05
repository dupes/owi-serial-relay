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
#include <time.h>

static int fd = -1;

int owi_serial_open(char *path)
{
	struct termios tio;

	fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);

	if (fd == -1)
		return -1;

	tcgetattr(fd, &tio);

	tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;

	tcflush(fd, TCIFLUSH);

	tcsetattr(fd, TCSANOW, &tio);

	return 0;
}

int owi_serial_close()
{
	return close(fd);
}


static int data_available(int timeout_ms)
{
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	tv.tv_sec = 0;
	tv.tv_usec = 1000 * timeout_ms;

	int retval = select(fd + 1, &rfds, NULL, NULL, &tv);

	if (retval == -1)
		return -1;

	if (FD_ISSET(fd, &rfds))
		return 1;

	return 0;
}

int owi_serial_recv(unsigned char *buffer, unsigned int buffer_size, unsigned int timeout_ms)
{
	int bytes_read = 0;
	int bytes_in_buffer = 0;

	while (1)
	{
		if (data_available(timeout_ms) <= 0)
			return 0;

		bytes_read = read(fd, buffer + bytes_in_buffer, buffer_size - bytes_in_buffer);

		if (bytes_read < 0)
			return bytes_read;

		bytes_in_buffer += bytes_read;

		if (bytes_in_buffer == buffer_size)
			return bytes_in_buffer;
	}

	return 0;
}

void owi_serial_flush()
{
	char buffer[128];

	while (data_available(2000))
	{
		read(fd, buffer, sizeof(buffer));
	}
}
