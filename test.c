/*
 *
 * Copyright (C) 2015 Gregory C. Herlein
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libusb.h>
#include "libcf2.h"
//#include "crtp.h"


int main(void)
{

	openRadio();
	printf("success\n");

	uint8_t header;
	CRTPPacket p;
	CRTPPacket* r;
	uint8_t port=0;
	uint8_t channel=0;

	int x;

	x=scanChannels();


	x=getNumberOfMems();
	

/*
	for(int x=0;x<1;x++)
	{
		x=sendCommander(0.0,0.0,0.0,15000);
		sleep(1);
	}

	sleep(15);
*/
	closeRadio();
	return 0;
}
