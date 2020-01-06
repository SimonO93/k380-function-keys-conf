/*
 * based on
 * k380_conf.c by Jerguš Greššák
 *          https://github.com/jergusg/k380-function-keys-conf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "hidapi.h"

#define HID_VENDOR_ID_LOGITECH			0x046d
#define HID_DEVICE_ID_K380              0xb342
#define MAX_STR 255


const unsigned char k380_seq_fkeys_on[]  = {0x10, 0xff, 0x0b, 0x1e, 0x00, 0x00, 0x00};
const unsigned char k380_seq_fkeys_off[] = {0x10, 0xff, 0x0b, 0x1e, 0x01, 0x00, 0x00};

const char opt_on[]  = "on";
const char opt_off[] = "off";

void send(hid_device *fd, const unsigned char * buf, const int len)
{
	int res;

	/* Send sequence to the Device */
	res = hid_write(fd, buf, len);

	if (res < 0)
	{
		printf("Error: %d\n", errno);
		perror("write");
	}
	else if (res == len)
       	{
		printf("Configuration sent.\n");
	}
	else
	{
		errno = ENOMEM;
		printf("write: %d were written instead of %d.\n", res, len);
	}
}

int main(int argc, char **argv)
{
	hid_device *fd;
	int res;
	wchar_t wstr[MAX_STR];
	const char * seq;
	char *dev = NULL;
	int flag_fkeys = 1;
	int c;

	if (argc < 3)
	{
		printf("Logitech Keyboard Configurator (by trial-n-error)\n\n");
		printf("Usage: %s -f {on|off}:\n\n", argv[0]);
		printf("-f <on|off>\n"
		       "   To enable direct access to F-keys.\n");
		printf("\n");
		return 1;
	}

	while ((c = getopt (argc, argv, "d:f:")) != -1)
	{
		switch (c)
		{
			case 'f':
				if (strcmp(opt_on, optarg) == 0)
				{
					flag_fkeys = 1;
				}
				else if (strcmp(opt_off, optarg) == 0)
				{
					flag_fkeys = 0;
				}
				else
				{
					fprintf (stderr, "Option -%c requires argument '%s' or '%s'.\n", optopt, opt_on, opt_off);
					return 1;
				}
				break;
			case '?':
				if (optopt == 'f')
				{
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if (isprint (optopt))
				{
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else
				{
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				}
				return 1;
			default:
				abort ();
		}
	}

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	fd = hid_open(HID_VENDOR_ID_LOGITECH, HID_DEVICE_ID_K380, NULL);
	if (!fd) {
		printf("unable to open device\n");
 		return 1;
	}

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(fd, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(fd, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	if (flag_fkeys)
	{
		printf("Sending ON: \n");
		send(fd, k380_seq_fkeys_on,  sizeof(k380_seq_fkeys_on));
	}
	else
	{
		printf("Sending OFF: \n");
		send(fd, k380_seq_fkeys_off, sizeof(k380_seq_fkeys_off));
	}

	// Close the device
	hid_close(fd);

	// Finalize the hidapi library
	res = hid_exit();
	return 0;
}
