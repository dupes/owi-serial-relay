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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "owiarm.h"
#include "owiserial.h"

void printusage(char *program)
{
	printf("Parameters missing\n");
	printf("  Usage: %s /dev/tty*\n\n", program);

	printf("Missing path to FTDI serial relay port\n");
	printf("  Usage: %s command byte1 byte2 byte3\n", program);
}

int send_single_command(int argc, char **argv)
{
	if (argc != 5)
	{
		printusage(argv[0]);
		return -1;
	}

	unsigned char command[3];

	command[0] = (unsigned char)strtol(argv[2], NULL, 16);
	command[1] = (unsigned char)strtol(argv[3], NULL, 16);
	command[2] = (unsigned char)strtol(argv[4], NULL, 16);

	if (owi_send_command(command) < 0)
	{
		fprintf(stderr, "  Error sending command to arm\n");
		perror("  owi_send_command");

		return -1;
	}

	sleep(1);

	memset(command, 0x0, 3);

	if (owi_send_command(command) < 0)
	{
		fprintf(stderr, "  Error clearing command\n");
		perror("  owi_send_command");

		return -1;
	}

	return 0;
}

int manual_control(int argc, char **argv)
{

	return 0;
}

int main(int argc, char **argv)
{
	int retval;

	if (argc == 1)
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

	printf("ARM located\n");

	//
	// send a single command to the arm
	//
	if (strcmp(argv[1], "command") == 0)
	{
		return send_single_command(argc, argv);
	}

	//
	// send a single command to the arm
	//
	if (strcmp(argv[1], "manual") == 0)
	{
		return manual_control(argc, argv);
	}

	//
	// continually relay commands to the arm over the serial port
	//

	printf("Initializing serial port\n");

	if (owi_serial_open(argv[1]) != 0)
	{
		fprintf(stderr, "Failed to open serial port: %s\n", argv[1]);
		perror("owi_serial_open");

		return -1;
	}

	printf("Serial port initialized.  Entering control loop\n");

	unsigned char buffer[3];

	while (1)
	{
		if (owi_serial_recv(buffer, 10000) == 3)
		{
			printf("Command received: %d %d %d\n", (int)buffer[0],
					(int)buffer[1], (int)buffer[2]);

			if (owi_send_command(buffer) < 0)
			{
				fprintf(stderr, "  Error sending command to arm\n");
				perror("  owi_send_command");
			}
			else
			{
				printf("  Command successfully sent\n");
			}
		}
	}

	owi_cleanup();
	owi_serial_close();

	return 0;
}
