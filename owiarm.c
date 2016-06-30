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

#include <libusb-1.0/libusb.h>

#include "owiarm.h"

static libusb_device **devs = NULL;
static libusb_device *dev = NULL;
static struct libusb_device_handle *devh = NULL;
static ssize_t device_count = 0;

static libusb_device * find_arm(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0;

	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);

		if (r < 0)
		{
			return NULL;
		}

		if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
		{
			return dev;
		}
	}
	return NULL;
}

int owi_init_usb(int debug_level)
{
	int result;

	if ((result = libusb_init(NULL)) != 0)
		return result;

	libusb_set_debug(NULL, debug_level);

	device_count = libusb_get_device_list(NULL, &devs);

	if (device_count > 0)
		return 0;

	return device_count;
}

int owi_find_arm()
{
    dev = find_arm(devs);

    if(!dev)
    {
	    return ARM_NOT_FOUND;
    }

    return libusb_open(dev, &devh);
}

void owi_cleanup()
{
	if (devh != NULL)
		libusb_close(devh);

	devh = NULL;

	if (devs != NULL)
		libusb_free_device_list(devs, 1);

	devs = NULL;

	libusb_exit(NULL);
}
