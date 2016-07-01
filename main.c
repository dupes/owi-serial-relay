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
#include <ncurses.h>

#include "owiarm.h"
#include "owiserial.h"

void printusage(char *program)
{
	printf("Parameters missing\n\n");

	printf("Relay mode:\n");
	printf("  Usage: %s /dev/tty* [--dry-run]\n", program);
	printf("  Add --dry-run if you don't want commands sent to the robot\n\n");

	printf("Single command mode:\n");
	printf("  Usage: %s command byte1 byte2 byte3\n", program);
	printf("  See: http://notbrainsurgery.livejournal.com/38622.html\n\n");

	printf("Manual mode\n");
	printf("  Usage: %s manual\n", program);
	printf("  Use the keyboard to control the arm\n\n");
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
	int control = 0;

	initscr();
	noecho();

	mvprintw(1, 0, "Manual mode:\n  (q, a): pincher\n  (w, s): wrist\n  (e, d): elbow\n  (r, f): shoulder\n  (t, g): base\n  (y): light\n\n Warning!! DO NOT HOLD THE KEYS DOWN\n");

	while (1)
	{
		char command[3];
		int send_command = 1;

		memset(command, 0x0, 3);

		int c = getch();

		refresh();

		switch (c)
		{
		// q: pincher
		case 113:

			mvprintw(0, 0, "key press pincher close: %d\n", (int)c);
			command[0] = 0x01;
			break;

		// a: pincher
		case 97:
			mvprintw(0, 0, "key press pincher open: %d\n", (int)c);
			command[0] = 0x02;
			break;

		// w: wrist
		case 119:
			mvprintw(0, 0, "key press wrist up: %d\n", (int)c);
			command[0] = 0x04;
			break;

		// s: wrist
		case 115:
			mvprintw(0, 0, "key press wrist down: %d\n", (int)c);
			command[0] = 0x08;
			break;

		// e: elbow
		case 101:
			mvprintw(0, 0, "key press elbow up: %d\n", (int)c);
			command[0] = 0x10;
			break;

		// d: elbow
		case 100:
			mvprintw(0, 0, "key press elbow down: %d\n", (int)c);
			command[0] = 0x20;
			break;

		// r: shoulder
		case 114:
			mvprintw(0, 0, "key press shoulder up: %d\n", (int)c);
			command[0] = 0x40;
			break;

		// f: shoulder
		case 102:
			mvprintw(0, 0, "key press shoulder down: %d\n", (int)c);
			command[0] = 0x80;
			break;

		// t: base
		case 116:
			mvprintw(0, 0, "key press base clockwise: %d\n", (int)c);
			command[1] = 0x01;
			break;

		// g: base
		case 103:
			mvprintw(0, 0, "key press base counterclockwise: %d\n", (int)c);
			command[1] = 0x02;
			break;

		// y: light
		case 121:
			mvprintw(0, 0, "key press blink light: %d\n", (int)c);
			command[2] = 0x01;
			break;

		// h: light
		case 104:
			command[2] = 0x00;
			break;

		default:
			// send_command = 0;
			break;
		}

		if (send_command)
		{
			if (owi_send_command(command) < 0)
			{
				fprintf(stderr, "  Error sending command to arm\n");
				perror("  owi_send_command");

				return -1;
			}

			usleep(250000);

			memset(command, 0x0, 3);

			if (owi_send_command(command) < 0)
			{
				fprintf(stderr, "  Error clearing command\n");
				perror("  owi_send_command");

				return -1;
			}
		}
	}

	endwin();

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
	// manually control arm with keyboard
	//
	if (strcmp(argv[1], "manual") == 0)
	{
		return manual_control(argc, argv);
	}

	//
	// continually relay commands to the received over the serial port
	//

	printf("Initializing serial port\n");

	if (owi_serial_open(argv[1]) != 0)
	{
		fprintf(stderr, "Failed to open serial port: %s\n", argv[1]);
		perror("owi_serial_open");

		return -1;
	}

	int dry_run = 0;

	if (argc == 3 && strcmp(argv[2], "--dry-run") == 0)
	{
		printf("*** Running in dry run mode: commands won't be relayed to the arm ***\n");
		dry_run = 1;
	}

	printf("Serial port initialized.  Entering control loop\n");

	unsigned char buffer[4];

	while (1)
	{
		int num_bytes;

		if ((num_bytes = owi_serial_recv(buffer, sizeof(buffer), 2000)) == sizeof(buffer))
		{
			printf("Command received: %d %d %d, count: %d\n", (int)buffer[0],
					(int)buffer[1], (int)buffer[2], (int)buffer[3]);

			if (!dry_run)
			{
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
		else
		{
			if (num_bytes != 0)
				printf("invalid command received: %d\n", num_bytes);
		}
	}

	owi_cleanup();
	owi_serial_close();

	return 0;
}
