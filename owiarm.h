
#ifndef OWIARM_H
#define OWIARM_H

#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x1267
#define PRODUCT_ID 0

int owi_init_usb();

int owi_find_arm();

#endif

