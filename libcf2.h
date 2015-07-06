/**
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

#ifndef __C_CRAZYFLIE_H__
#define __C_CRAZYFLIE_H__

//#include "crtp.h" 

#define CRTP_MAX_DATA_SIZE 30


typedef struct _CRTPPacket
{
  union 
  {
    uint8_t header;
    struct 
    {
      uint8_t channel     : 2;
      uint8_t reserved    : 2;
      uint8_t port        : 4;
    };
  };
  uint8_t data[CRTP_MAX_DATA_SIZE-1];
} __attribute__((packed)) CRTPPacket;

typedef struct _CommanderCrtpValues
{
  float roll;
  float pitch;
  float yaw;
  uint16_t thrust;
} __attribute__((packed)) CommanderCrtpValues;



int openRadio(void);
int closeRadio(void);
CRTPPacket* readData(void);
CRTPPacket* readACK(void);
CRTPPacket* writeData(CRTPPacket* p, int nLength);

int scanChannels(void);
int sendCommander(float roll, float pitch, float yaw, uint16_t thrust);

int getNumberOfMems(void);



#endif
