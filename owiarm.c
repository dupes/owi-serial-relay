/*
 * owi.c
 *
 *  Created on: Jun 30, 2016
 *      Author: dupes
 */


#include "owiarm.h"

int owi_init_usb()
{
	return libusb_init(NULL);
}

int owi_find_arm()
{

	return 0;
}
