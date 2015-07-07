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

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <libusb.h>
#include "libcf2.h" 
//#include "crtp.h" 

static void printPacket(CRTPPacket* p);
static void printHeader(CRTPPacket* p);

//USB parameters
#define CRADIO_VID  0x1915
#define CRADIO_PID  0x7777

//Dongle configuration requests
//See http://wiki.bitcraze.se/projects:crazyradio:protocol for documentation
#define SET_RADIO_CHANNEL  0x01
#define SET_RADIO_ADDRESS  0x02
#define SET_DATA_RATE      0x03
#define SET_RADIO_POWER    0x04
#define SET_RADIO_ARD      0x05
#define SET_RADIO_ARC      0x06
#define ACK_ENABLE         0x10
#define SET_CONT_CARRIER   0x20
#define SCANN_CHANNELS     0x21
#define LAUNCH_BOOTLOADER  0xFF



typedef enum {
  CRTP_PORT_CONSOLE     = 0x00,
  CRTP_PORT_PARAM       = 0x02,
  CRTP_PORT_COMMANDER   = 0x03,
  CRTP_PORT_MEM         = 0x04,
  CRTP_PORT_LOG         = 0x05,
  CRTP_PORT_PLATFORM    = 0x0D,
  CRTP_PORT_LINK        = 0x0F,
} CRTPPort;


enum Rate
{
   CR_250K =0,
   CR_1M=1,
   CR_2M=2
};

enum Power 
{
  /*! \brief Power at -18dbm */
  CR_M18DBM = 0,
  /*! \brief Power at -12dbm */
  CR_M12DBM = 1,
  /*! \brief Power at -6dbm */
  CR_M6DBM = 2,
  /*! \brief Power at 0dbm */
  CR_0DBM = 3
};



static libusb_device_handle *dev_handle=NULL; 
static libusb_context *ctx = NULL; 

static double    cr_ver=0.0;
static unsigned char  cAddress[5];
static uint16_t  cr_ARC=-0;
static uint16_t  cr_Channel=0;
static uint16_t  cr_DataRate=0;
static uint16_t  cr_ARD=-0;
static uint16_t  cr_ARDBytes=0;
static uint16_t  cr_Power=0;
static uint16_t  cr_ContCarrier=0;


static int
print_USB_error(int nReturn)
{
  if(nReturn <= 0) 
  {
    switch(nReturn) 
    {
      case LIBUSB_ERROR_TIMEOUT:
        printf("USB timeout\n");
        return -1;
        break;
      case LIBUSB_ERROR_OVERFLOW:
        printf("USB overflow\n");
        return -1;
        break;
      case LIBUSB_ERROR_NO_DEVICE:
        printf("USB overflow\n");
        return -1;
        break;
      case 0:
        break;
      default:
        printf("USB error %d\n",nReturn);
        return -1;
        break;
    }
  }
  return 0;
}


static void 
setChannel(uint16_t nChannel) 
{
  cr_Channel=nChannel;
  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_RADIO_CHANNEL, nChannel, 0, NULL, 0, 1000);
  print_USB_error(nReturn);
}



static void 
setDataRate(uint16_t nDataRate) 
{
  cr_DataRate=nDataRate;
  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_DATA_RATE, nDataRate, 0, NULL, 0, 1000);
  print_USB_error(nReturn);
}


static void
setARC(uint16_t nARC) 
{
  cr_ARC=nARC;
  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_RADIO_ARC, nARC, 0, NULL, 0, 1000);
  print_USB_error(nReturn);
}


static void 
setARD(uint16_t nARD) 
{ 
  cr_ARD = nARD;

  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_RADIO_ARD, nARD, 0, NULL, 0, 1000);
  print_USB_error(nReturn);
}

static void 
setPower(enum Power enumPower) 
{
  cr_Power = enumPower;
  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_RADIO_POWER, cr_Power, 0, NULL, 0, 1000);
  print_USB_error(nReturn);
}

static void 
setAddress(unsigned char* p) 
{

  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_RADIO_ADDRESS, 0, 0, p, 5, 1000);
  print_USB_error(nReturn);
}

static void 
setContCarrier(uint16_t bContCarrier) 
{
  cr_ContCarrier = bContCarrier;

  int nReturn = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR, SET_CONT_CARRIER, bContCarrier, 0, NULL, 0, 1000);
  print_USB_error(nReturn);
}


/*
int interface_ref = 0;    
int alt_interface,interface_number;    

static int 
print_configuration(struct libusb_device_handle *hDevice,struct libusb_config_descriptor *config)    
{    
    char *data;    
    int index;    

    data = (char *)malloc(512);    
    memset(data,0,512);    

    index = config->iConfiguration;    

    libusb_get_string_descriptor_ascii(hDevice,index,data,512);    

    printf("\nInterface Descriptors: ");    
    printf("\n\tNumber of Interfaces : %d",config->bNumInterfaces);    
    printf("\n\tLength : %d",config->bLength);    
    printf("\n\tDesc_Type : %d",config->bDescriptorType);    
    printf("\n\tConfig_index : %d",config->iConfiguration);    
    printf("\n\tTotal length : %lu",config->wTotalLength);    
    printf("\n\tConfiguration Value  : %d",config->bConfigurationValue);    
    printf("\n\tConfiguration Attributes : %d",config->bmAttributes);    
    printf("\n\tMaxPower(mA) : %d\n",config->MaxPower);    

    free(data);    
    data = NULL;    
    return 0;    
}    


static struct libusb_endpoint_descriptor* 
active_config(struct libusb_device *dev,struct libusb_device_handle *handle)    
{    
    struct libusb_device_handle *hDevice_req;    
    struct libusb_config_descriptor *config;    
    struct libusb_endpoint_descriptor *endpoint;    
    int altsetting_index,interface_index=0,ret_active;    
    int i,ret_print;    

    hDevice_req = handle;    

    ret_active = libusb_get_active_config_descriptor(dev,&config);    
    ret_print = print_configuration(hDevice_req,config);    

    for(interface_index=0;interface_index<config->bNumInterfaces;interface_index++)    
    {    
        const struct libusb_interface *iface = &config->interface[interface_index];    
        for(altsetting_index=0;altsetting_index<iface->num_altsetting;altsetting_index++)    
        {    
            const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_index];    

            int endpoint_index;    
            for(endpoint_index=0;endpoint_index<altsetting->bNumEndpoints;endpoint_index++)    
            {    
                const struct libusb_endpoint_desriptor *ep = &altsetting->endpoint[endpoint_index];    
                endpoint = ep;      
                alt_interface = altsetting->bAlternateSetting;    
                interface_number = altsetting->bInterfaceNumber;    
            }    

            printf("\nEndPoint Descriptors: ");    
            printf("\n\tSize of EndPoint Descriptor : %d",endpoint->bLength);    
            printf("\n\tType of Descriptor : %d",endpoint->bDescriptorType);    
            printf("\n\tEndpoint Address : 0x0%x",endpoint->bEndpointAddress);    
            printf("\n\tMaximum Packet Size: %x",endpoint->wMaxPacketSize);    
            printf("\n\tAttributes applied to Endpoint: %d",endpoint->bmAttributes);    
            printf("\n\tInterval for Polling for data Tranfer : %d\n",endpoint->bInterval);    
        }    
    }    
    libusb_free_config_descriptor(NULL);    
    return endpoint;    
}    
*/

int
openRadio(void)
{
	libusb_device **dev; 
	int r; 
	ssize_t cnt; 
	char ver[8];

	r = libusb_init(&ctx); 
	if(r < 0) 
	{
		printf("error initializing USB\n");
		return -1;
	}
	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_WARNING); 

	cnt = libusb_get_device_list(ctx, &dev); 
	if(cnt < 0) 
	{
		printf("error getting device list\n");
		return -1;
	}

	for(unsigned int i = 0; i < cnt; i++) 
	{	
	    libusb_device *devCurrent = dev[i];
	    struct libusb_device_descriptor dd;// = {0};

	    libusb_get_device_descriptor(devCurrent, &dd);

      //active_config(devCurrent,dev_handle);    

    	if(dd.idVendor == 0x1915 && dd.idProduct == 0x7777) 
    	{
    		unsigned char u=dd.bcdDevice >> 8;
    		unsigned char l=dd.bcdDevice & 0x0ff;
    		sprintf(ver, "%.1X.%.2X",u,l);
    		printf("[%s]\n",ver);
    		cr_ver=atof(ver);

       printf("found CrazyRadio version %f\n",cr_ver);


       r = libusb_open(devCurrent, &dev_handle); 
       if(r!=0)
       {
        printf("error opening CrazyRadio\n");
        return -1;
      }		

      if(cr_ver >= 0.4) 
      {

        cAddress[0] = (char)0xe7;
        cAddress[1] = (char)0xe7;
        cAddress[2] = (char)0xe7;
        cAddress[3] = (char)0xe7;
        cAddress[4] = (char)0xe7;

        setChannel(2);
        setAddress(cAddress);
        setContCarrier(0);
        setPower(CR_0DBM);
        setARC(3);
        setARD(0xA0);
        setDataRate(CR_2M);

      }

  
    }

  }


	libusb_free_device_list(dev, 1); //free the list, unref the devices in it

	if(libusb_kernel_driver_active(dev_handle, 0) == 1) 
	{ //find out if kernel driver is attached
		libusb_detach_kernel_driver(dev_handle, 0);
	}
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) 
	{
		printf("error claiming device\n");
		return -1;
	}
	return 0;
}


int
closeRadio(void)
{
    libusb_release_interface(dev_handle, 0); //release the claimed interface
    libusb_close(dev_handle); //close the device we opened
    libusb_exit(ctx); //needs to be called to end the
    return 0;
}


CRTPPacket*
readData(void) 
{
  int nActuallyRead;
  uint8_t buffer[128];

  CRTPPacket* p=calloc(1,sizeof(CRTPPacket)); 

  int nReturn = libusb_bulk_transfer(dev_handle, (0x81 | LIBUSB_ENDPOINT_IN), (unsigned char*)buffer, 128, &nActuallyRead, 50);

  if(nReturn != LIBUSB_SUCCESS) 
  {
    switch(nReturn) 
    {
      case LIBUSB_ERROR_TIMEOUT:
        printf("USB timeout\n");
        return NULL;
        break;
      case LIBUSB_ERROR_OVERFLOW:
        printf("USB overflow\n");
        return NULL;
        break;
      case LIBUSB_ERROR_NO_DEVICE:
        printf("USB overflow\n");
        return NULL;
        break;
      default:
        printf("USB error %d\n",nReturn);
        return NULL;
        break;
    }
    return NULL;
  }  

  printf("* nActuallyRead: %d\n",nActuallyRead);
  printf("* status byte:   %x\n",buffer[0]);
  printf("\n");

  
  return p;
}




CRTPPacket*
readACK() 
{
  CRTPPacket *p = NULL;
  int nBytesRead = 0;

  printf("*********** reading ACK packet\n");
  p=readData();
  printPacket(p);
  printf("*********** end of ACK packet\n");
  return p;
}



CRTPPacket*
writeData(CRTPPacket* p, int nLength) 
{
  int nActuallyWritten;
  CRTPPacket *r = NULL;

  printf("writing packet to USB: \n");
  printPacket(p);

  int nReturn = libusb_bulk_transfer(dev_handle, (0x01 | LIBUSB_ENDPOINT_OUT), (unsigned char*)p, nLength, &nActuallyWritten, 1000);
 if (status != LIBUSB_SUCCESS) {
        std::cerr << "Send " << libusb_error_name(status) << std::endl;
    }
  if(nReturn == LIBUSB_SUCCESS && nActuallyWritten == nLength) 
  {
    printf("wrote %d bytes \n",nActuallyWritten);
    r=readACK();
  } else
  {
    printf("error sending to USB - tried to send %d bytes but actually sent %d bytes\n",nLength,nActuallyWritten);
    return NULL;
  }

  return r;
}

static int
scanChannelsByRate(int rate)
{
  unsigned char b[1];
  unsigned char a[3];
  unsigned char c[64];
  int r=0;

  a[0]=0xF0;
  a[1]=0x01;
  a[2]=0x01;

  setDataRate(rate);

  r = libusb_control_transfer(dev_handle, 0x40, 0x21, 0, 125, (unsigned char*)&a, 3, 1000);
  if(r<0)
  {
    printf("error writing scan command\n");
    return 0;
  }
  printf("wrote %d bytes to command a scan...\n",r);

  sleep(2);
  memset(c,0x00,64);

  r = libusb_control_transfer(dev_handle, 0xC0, 0x21, 0, 0, (unsigned char*)c, 63, 1000);
  if(r==0)
  {
    printf("got no reply from channel scan\n");
    return -1;
  }
  if(r<0)
  {
    printf("error reading scan results\n");
    return -1;
  }
  printf("read %d bytes of scanned channel data...\n",r);

  for(r=0;r<64;r++)
  {
    if(c[r]>0)
    {
      printf("Channel %d: %d\n",r,c[r]);
      setChannel(r);
      return r;
    }
  }
  return -1;
}


int
scanChannels(void)
{
  int r=-1;

  r=scanChannelsByRate(CR_2M);
  if(r>=0) 
  {
    printf("found CF2 at 2M\n");
    return r;
  }
  r=scanChannelsByRate(CR_1M);
  if(r>=0) 
  {
    printf("found CF2 at 1M\n");
    return r;
  }
  r=scanChannelsByRate(CR_250K);
  if(r>=0) 
  {
    printf("found CF2 at 250K\n");
    return r;
  }

}



static void
printPacket(CRTPPacket* p)
{
  if (p==NULL)
  {
    printf("packet is NULL\n");
  } else 
  {
    unsigned char* c=(unsigned char*)(p->data);;

    printf("packet p: (%p)\n",p);
    printHeader(p);
    for(int x=0;x<15;x++)
    {
      printf("[%.02x] ",*(c+x));
    }
    printf("\n");
    for(int x=15;x<30;x++)
    {
      printf("[%.02x] ",*(c+x));
    }
  }
  printf("\n---------------------------------------\n");
}

static void
printHeader(CRTPPacket* p)
{
  printf("header: [%x]\n",(p->header));
  printf("port:   [%x]\n",(p->port));
  printf("channel:[%x]\n",(p->channel));
}


int
sendCommander(float roll, float pitch, float yaw, uint16_t thrust)
{
  int nLength=15;
  int nActuallyWritten;

  CRTPPacket* p=calloc(1,sizeof(CRTPPacket)); 
  printf("sizeof packet: %d\n",(int)sizeof(CRTPPacket));

  p->channel=0;
  p->port=3;

  float* pf;
  pf=(float*)p->data;
  *pf=roll;
  pf+=1;
  *pf=pitch;
  pf+=1;
  *pf=yaw;
  pf+=1;
  uint16_t* t=(uint16_t*)pf;
  *t=thrust;

  printPacket(p);

  CRTPPacket* r = NULL;
  //r =writeData(p, nLength);

  if(r!=NULL) 
  {
    printf("received packet:  \n");
    printPacket(r);
  }
  else printf("Error\n");

  return 0;
}

int
getNumberOfMems(void)
{
  int nLength=2;
  int nActuallyWritten;

  CRTPPacket* p=calloc(1,sizeof(CRTPPacket)); 
  printf("sizeof packet: %d\n",(int)sizeof(CRTPPacket));

  p->channel=0;
  p->port=4;

  char* q=(char*)(p->data);
  *q=0x01;

  printPacket(p);

  CRTPPacket* r = NULL;
  r =writeData(p, nLength);

  if(r!=NULL) 
  {
    printf("received packet:  \n");
    printPacket(r);
  }
  else printf("Error\n");

  return 0;



}
