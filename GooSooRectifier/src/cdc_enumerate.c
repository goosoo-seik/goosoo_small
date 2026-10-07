//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : cdc_enumerate.c
//* Object              : Handle CDC enumeration
//*
//* 1.0 Apr 20 200 	: ODi Creation
//* 1.1 14/Sep/2004 JPP : Minor change
//* 1.1 15/12/2004  JPP : suppress warning
//* 1.2 30-Jun-2006 JPP : Set in AT91C_UDP_EPTYPE_INT_IN
//* 2007.1.29  Lee Y K  : Analysis & Modify for EpiaGate  
//*----------------------------------------------------------------------------
#include "Project.h"
//#include "cdc_enumerate.h"

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

struct _AT91S_CDC 	pCDC;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define AT91C_EP_IN_SIZE 0x40

const char devDescriptor[] = {
	/* Device descriptor */
	0x12,   // bLength
	0x01,   // bDescriptorType
	0x10,   // bcdUSBL
	0x01,   //
	0x02,   // bDeviceClass:    CDC class code
	0x00,   // bDeviceSubclass: CDC class sub code
	0x00,   // bDeviceProtocol: CDC Device protocol
	0x08,   // bMaxPacketSize0
	0xEB,   // idVendorL
	0x03,   //
	0x24,   // idProductL
	0x61,   //
	0x10,   // bcdDeviceL
	0x01,   //
	0x00,   // iManufacturer    // 0x01
	0x00,   // iProduct
	0x00,   // SerialNumber
	0x01    // bNumConfigs
};

const char cfgDescriptor[] = {
	/* ============== CONFIGURATION 1 =========== */
	/* Configuration 1 descriptor */
	0x09,   // CbLength
	0x02,   // CbDescriptorType
	0x43,   // CwTotalLength 2 EP + Control
	0x00,
	0x02,   // CbNumInterfaces
	0x01,   // CbConfigurationValue
	0x00,   // CiConfiguration
	0xC0,   // CbmAttributes 0xA0
	0x00,   // CMaxPower

	/* Communication Class Interface Descriptor Requirement */
	0x09, // bLength
	0x04, // bDescriptorType
	0x00, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x01, // bNumEndpoints
	0x02, // bInterfaceClass
	0x02, // bInterfaceSubclass
	0x00, // bInterfaceProtocol
	0x00, // iInterface

	/* Header Functional Descriptor */
	0x05, // bFunction Length
	0x24, // bDescriptor type: CS_INTERFACE
	0x00, // bDescriptor subtype: Header Func Desc
	0x10, // bcdCDC:1.1
	0x01,

	/* ACM Functional Descriptor */
	0x04, // bFunctionLength
	0x24, // bDescriptor Type: CS_INTERFACE
	0x02, // bDescriptor Subtype: ACM Func Desc
	0x00, // bmCapabilities

	/* Union Functional Descriptor */
	0x05, // bFunctionLength
	0x24, // bDescriptorType: CS_INTERFACE
	0x06, // bDescriptor Subtype: Union Func Desc
	0x00, // bMasterInterface: Communication Class Interface
	0x01, // bSlaveInterface0: Data Class Interface

	/* Call Management Functional Descriptor */
	0x05, // bFunctionLength
	0x24, // bDescriptor Type: CS_INTERFACE
	0x01, // bDescriptor Subtype: Call Management Func Desc
	0x00, // bmCapabilities: D1 + D0
	0x01, // bDataInterface: Data Class Interface 1

	/* Endpoint 1 descriptor */
	0x07,   // bLength
	0x05,   // bDescriptorType
	0x83,   // bEndpointAddress, Endpoint 03 - IN
	0x03,   // bmAttributes      INT
	0x08,   // wMaxPacketSize
	0x00,
	0xFF,   // bInterval

	/* Data Class Interface Descriptor Requirement */
	0x09, // bLength
	0x04, // bDescriptorType
	0x01, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x02, // bNumEndpoints
	0x0A, // bInterfaceClass
	0x00, // bInterfaceSubclass
	0x00, // bInterfaceProtocol
	0x00, // iInterface

	/* First alternate setting */
	/* Endpoint 1 descriptor */
	0x07,   // bLength
	0x05,   // bDescriptorType
	0x01,   // bEndpointAddress, Endpoint 01 - OUT
	0x02,   // bmAttributes      BULK
	AT91C_EP_OUT_SIZE,   // wMaxPacketSize
	0x00,
	0x00,   // bInterval

	/* Endpoint 2 descriptor */
	0x07,   // bLength
	0x05,   // bDescriptorType
	0x82,   // bEndpointAddress, Endpoint 02 - IN
	0x02,   // bmAttributes      BULK
	AT91C_EP_IN_SIZE,   // wMaxPacketSize
	0x00,
	0x00    // bInterval
};

/* USB standard request code */
#define STD_GET_STATUS_ZERO           0x0080
#define STD_GET_STATUS_INTERFACE      0x0081
#define STD_GET_STATUS_ENDPOINT       0x0082

#define STD_CLEAR_FEATURE_ZERO        0x0100
#define STD_CLEAR_FEATURE_INTERFACE   0x0101
#define STD_CLEAR_FEATURE_ENDPOINT    0x0102

#define STD_SET_FEATURE_ZERO          0x0300
#define STD_SET_FEATURE_INTERFACE     0x0301
#define STD_SET_FEATURE_ENDPOINT      0x0302

#define STD_SET_ADDRESS               0x0500
#define STD_GET_DESCRIPTOR            0x0680
#define STD_SET_DESCRIPTOR            0x0700
#define STD_GET_CONFIGURATION         0x0880
#define STD_SET_CONFIGURATION         0x0900
#define STD_GET_INTERFACE             0x0A81
#define STD_SET_INTERFACE             0x0B01
#define STD_SYNCH_FRAME               0x0C82

/* CDC Class Specific Request Code */
#define GET_LINE_CODING               0x21A1
#define SET_LINE_CODING               0x2021
#define SET_CONTROL_LINE_STATE        0x2221


typedef struct {
	unsigned int dwDTERRate;
	char bCharFormat;
	char bParityType;
	char bDataBits;
} AT91S_CDC_LINE_CODING, *AT91PS_CDC_LINE_CODING;

AT91S_CDC_LINE_CODING line = {
	115200, // baudrate
	0,      // 1 Stop Bit
	0,      // None Parity
	8};     // 8 Data bits

uint currentReceiveBank = AT91C_UDP_RX_DATA_BK0;


static uchar AT91F_UDP_IsConfigured(AT91PS_CDC pCdc);
static uint AT91F_UDP_Read(AT91PS_CDC pCdc, char *pData, uint length);
static uint AT91F_UDP_Write(AT91PS_CDC pCdc, const char *pData, uint length);
static void AT91F_CDC_Enumerate(AT91PS_CDC pCdc);


//*----------------------------------------------------------------------------
//* \fn    AT91F_CDC_Open
//* \brief
//*----------------------------------------------------------------------------
AT91PS_CDC AT91F_CDC_Open(AT91PS_CDC pCdc, AT91PS_UDP pUdp)
{
	if (USBdebug) printf("\nAT91PS_CDC AT91F_CDC_Open");
        pCdc->pUdp = pUdp;
	pCdc->currentConfiguration = 0;
	pCdc->currentConnection    = 0;
	pCdc->currentRcvBank       = AT91C_UDP_RX_DATA_BK0;
	pCdc->IsConfigured = AT91F_UDP_IsConfigured;
	pCdc->Write        = AT91F_UDP_Write;
	pCdc->Read         = AT91F_UDP_Read;
        if (USBdebug) printf("->pCdc: %05x",pCdc);
	return pCdc;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_IsConfigured
//* \brief Test if the device is configured and handle enumeration
//*----------------------------------------------------------------------------
// UDP_ICR : (UDP Offset: 0x20) USB Interrupt Clear Register
		
static uchar AT91F_UDP_IsConfigured(AT91PS_CDC pCdc)
{
	AT91PS_UDP pUDP = pCdc->pUdp;
	AT91_REG isr = pUDP->UDP_ISR;

         //if (USBdebug) printf("i");
         if (isr & AT91C_UDP_ENDBUSRES) {    // (UDP) USB End Of Bus Reset Interrupt
                if (USBdebug) printf("R");
		pUDP->UDP_ICR = AT91C_UDP_ENDBUSRES;
		// reset all endpoints
		pUDP->UDP_RSTEP  = (unsigned int)-1;
		pUDP->UDP_RSTEP  = 0;
		// Enable the function
		pUDP->UDP_FADDR = AT91C_UDP_FEN;
		// Configure endpoint 0
		pUDP->UDP_CSR[0] = (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_CTRL);
	}
	else if (isr & AT91C_UDP_EPINT0) {
                if (USBdebug) printf("E");
                pUDP->UDP_ICR = AT91C_UDP_EPINT0;
		AT91F_CDC_Enumerate(pCdc);
	}// else if (USBdebug) printf("?");
        //if (USBdebug) printf("%02x",pCdc->currentConfiguration);
	return pCdc->currentConfiguration;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_Read
//* \brief Read available data from Endpoint OUT
//*----------------------------------------------------------------------------
static uint AT91F_UDP_Read(AT91PS_CDC pCdc, char *pData, uint length)
{
	AT91PS_UDP pUdp = pCdc->pUdp;
	uint packetSize, nbBytesRcv = 0, currentReceiveBank = pCdc->currentRcvBank;

	//if (USBdebug) printf("\nUDP_Read.."); // for debug
        while (length) {
                //if (USBdebug) printf(" %5d ",length);
		if ( !AT91F_UDP_IsConfigured(pCdc) )
			break;
                //if (USBdebug) printf("-");
		if ( pUdp->UDP_CSR[AT91C_EP_OUT] & currentReceiveBank )
                {
			if (USBdebug) printf("m");
                        packetSize = MIN(pUdp->UDP_CSR[AT91C_EP_OUT] >> 16, length);
			length -= packetSize;
			if (packetSize < AT91C_EP_OUT_SIZE)
				length = 0;
			while(packetSize--)
                        {
                                if (USBdebug) printf(" %02x",pUdp->UDP_FDR[AT91C_EP_OUT]);
				pData[nbBytesRcv++] = pUdp->UDP_FDR[AT91C_EP_OUT];
                        }
			pUdp->UDP_CSR[AT91C_EP_OUT] &= ~(currentReceiveBank);
			if (currentReceiveBank == AT91C_UDP_RX_DATA_BK0)
				currentReceiveBank = AT91C_UDP_RX_DATA_BK1;
			else
				currentReceiveBank = AT91C_UDP_RX_DATA_BK0;
		} else break;
	}
	pCdc->currentRcvBank = currentReceiveBank;
        //if (USBdebug) printf("->nbBytesRcv: %05x",nbBytesRcv);
	return nbBytesRcv;
}

/********************************************/
/*       Data receive from PC by USB        */
/* This function must locate in main loop   */ 
/********************************************/
AT91PS_CDC pCdc;
AT91PS_UDP pUdp;
#define MSG_SIZE 	0x40
unsigned int PacketSize, nbBytesRcv = 0, CurrentReceiveBank;
unsigned int USBlength;
char *pData;
unsigned char StepUSBrx;
unsigned char IsConfig;
void link_receive_USB(void)
{
  switch (StepUSBrx) 
  {
  case 0:
    return;
    
  case 1:
    if (USBdebug) printf("\nUDP_Read..");
    CurrentReceiveBank = pCdc->currentRcvBank;
    pUdp = pCdc->pUdp;
    StepUSBrx++;
    if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
    USBdelay = 0;
    return;
  
  case 2:
    StepUSBrx++;
    if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
    if (++USBdelay == 1000) 
    {
    USBdelay = 0;
    StepUSBrx++;
    if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
    }
    return;
    
  case 3:
    if (USBdebug) printf(".");
    IsConfig = AT91F_UDP_IsConfigured(&pCDC);
    //if (AT91F_UDP_IsConfigured(pCdc) ) 
    if (IsConfig) 
    {
      StepUSBrx++;
      if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
      USBdelay = 0;
    } else if (USBdebug) printf("!");
   //if (++USBdelay == 200) 
   // {
   //   USBdelay = 0;
   //   if (USBdebug) printf(".");
   //   StepUSBrx++;
   //   if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
   // } 
    return;
    
  case 4:
//UDP_CSR[6]; 	Endpoint Control and Status Register
// -------- UDP_CSR : (UDP Offset: 0x30) USB Endpoint Control and Status Register -------- 
//#define AT91C_UDP_TXCOMP      ((unsigned int) 0x1 <<  0) // (UDP) Generates an IN packet with data previously written in the DPR
//#define AT91C_UDP_RX_DATA_BK0 ((unsigned int) 0x1 <<  1) // (UDP) Receive Data Bank 0
//#define AT91C_UDP_RXSETUP     ((unsigned int) 0x1 <<  2) // (UDP) Sends STALL to the Host (Control endpoints)
//#define AT91C_UDP_ISOERROR    ((unsigned int) 0x1 <<  3) // (UDP) Isochronous error (Isochronous endpoints)
//#define AT91C_UDP_TXPKTRDY    ((unsigned int) 0x1 <<  4) // (UDP) Transmit Packet Ready
//#define AT91C_UDP_FORCESTALL  ((unsigned int) 0x1 <<  5) // (UDP) Force Stall (used by Control, Bulk and Isochronous endpoints).
//#define AT91C_UDP_RX_DATA_BK1 ((unsigned int) 0x1 <<  6) // (UDP) Receive Data Bank 1 (only used by endpoints with ping-pong attributes).
//#define AT91C_UDP_DIR         ((unsigned int) 0x1 <<  7) // (UDP) Transfer Direction
//#define AT91C_UDP_EPTYPE      ((unsigned int) 0x7 <<  8) // (UDP) Endpoint type
//#define AT91C_UDP_EPTYPE_CTRL     ((unsigned int) 0x0 <<  8) // (UDP) Control
//#define AT91C_UDP_EPTYPE_ISO_OUT  ((unsigned int) 0x1 <<  8) // (UDP) Isochronous OUT
//#define AT91C_UDP_EPTYPE_BULK_OUT ((unsigned int) 0x2 <<  8) // (UDP) Bulk OUT
//#define AT91C_UDP_EPTYPE_INT_OUT  ((unsigned int) 0x3 <<  8) // (UDP) Interrupt OUT
//#define AT91C_UDP_EPTYPE_ISO_IN   ((unsigned int) 0x5 <<  8) // (UDP) Isochronous IN
//#define AT91C_UDP_EPTYPE_BULK_IN  ((unsigned int) 0x6 <<  8) // (UDP) Bulk IN
//#define AT91C_UDP_EPTYPE_INT_IN   ((unsigned int) 0x7 <<  8) // (UDP) Interrupt IN
//#define AT91C_UDP_DTGLE       ((unsigned int) 0x1 << 11) // (UDP) Data Toggle
//#define AT91C_UDP_EPEDS       ((unsigned int) 0x1 << 15) // (UDP) Endpoint Enable Disable
//#define AT91C_UDP_RXBYTECNT   ((unsigned int) 0x7FF << 16) // (UDP) Number Of Bytes Available in the FIFO

    if ( pUdp->UDP_CSR[AT91C_EP_OUT] & CurrentReceiveBank )
     {
        if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
	//PacketSize = MIN(pUdp->UDP_CSR[AT91C_EP_OUT] >> 16, MSG_SIZE);
        USBdelay = 30; 
        PacketSize = MIN(USBdelay, MSG_SIZE);
	USBlength -= PacketSize;
	if (PacketSize < AT91C_EP_OUT_SIZE) USBlength = 0;
        if (USBdebug) printf(" PacketSize: %04x",PacketSize);
	while(PacketSize--)
          {
            //if (USBdebug) printf(" %02x",pUdp->UDP_FDR[AT91C_EP_OUT]);
	    //pData[nbBytesRcv++] = pUdp->UDP_FDR[AT91C_EP_OUT];
           }
        //pUdp->UDP_CSR[AT91C_EP_OUT] &= ~(CurrentReceiveBank);
	if (CurrentReceiveBank == AT91C_UDP_RX_DATA_BK0) CurrentReceiveBank = AT91C_UDP_RX_DATA_BK1;
	 else CurrentReceiveBank = AT91C_UDP_RX_DATA_BK0;
        StepUSBrx++;
        if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
      }
    return;
    
  case 5:
    if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
    pCdc->currentRcvBank = CurrentReceiveBank;
    if (USBdebug) printf("->nbBytesRcv: %05x",nbBytesRcv);
    StepUSBrx++;
    return;
    
  case 6:
    if (USBdebug) printf("\nStepUSBrx: %02x", StepUSBrx);
    StepUSBrx++;
    return;
    
  case 7:
    StepUSBrx++;
    return;
    
  case 8:
    StepUSBrx++;
    return;
    
  case 9:
    StepUSBrx = 1;
    return;
 }
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CDC_Write
//* \brief Send through endpoint 2
//*----------------------------------------------------------------------------
static uint AT91F_UDP_Write(AT91PS_CDC pCdc, const char *pData, uint length)
{
	AT91PS_UDP pUdp = pCdc->pUdp;
	uint cpt = 0;

	if (USBdebug) printf("\nUDP_Write..");
        // Send the first packet
	cpt = MIN(length, AT91C_EP_IN_SIZE);
	length -= cpt;
	while (cpt--) pUdp->UDP_FDR[AT91C_EP_IN] = *pData++;
	pUdp->UDP_CSR[AT91C_EP_IN] |= AT91C_UDP_TXPKTRDY;

	while (length) {
		// Fill the second bank
		cpt = MIN(length, AT91C_EP_IN_SIZE);
		length -= cpt;
		while (cpt--) pUdp->UDP_FDR[AT91C_EP_IN] = *pData++;
		// Wait for the the first bank to be sent
		while ( !(pUdp->UDP_CSR[AT91C_EP_IN] & AT91C_UDP_TXCOMP) )
			if ( !AT91F_UDP_IsConfigured(pCdc) ) return length;
		pUdp->UDP_CSR[AT91C_EP_IN] &= ~(AT91C_UDP_TXCOMP);
		while (pUdp->UDP_CSR[AT91C_EP_IN] & AT91C_UDP_TXCOMP);
		pUdp->UDP_CSR[AT91C_EP_IN] |= AT91C_UDP_TXPKTRDY;
	}
	// Wait for the end of transfer
	while ( !(pUdp->UDP_CSR[AT91C_EP_IN] & AT91C_UDP_TXCOMP) )
		if ( !AT91F_UDP_IsConfigured(pCdc) ) return length;
	pUdp->UDP_CSR[AT91C_EP_IN] &= ~(AT91C_UDP_TXCOMP);
	while (pUdp->UDP_CSR[AT91C_EP_IN] & AT91C_UDP_TXCOMP);

	if (USBdebug) printf("->length: %05x",length);
        return length;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_USB_SendData
//* \brief Send Data through the control endpoint
//*----------------------------------------------------------------------------
unsigned int csrTab[100];
unsigned char csrIdx = 0;

static void AT91F_USB_SendData(AT91PS_UDP pUdp, const char *pData, uint length)
{
	uint cpt = 0;
	AT91_REG csr;

	if (USBdebug) printf("\nSendData:");
        do {
		cpt = MIN(length, 8);
		length -= cpt;

		while (cpt--)
                {
                        if (USBdebug) printf(" %02x",*pData);
			pUdp->UDP_FDR[0] = *pData++;
                }

		if (pUdp->UDP_CSR[0] & AT91C_UDP_TXCOMP) {
			pUdp->UDP_CSR[0] &= ~(AT91C_UDP_TXCOMP);
			while (pUdp->UDP_CSR[0] & AT91C_UDP_TXCOMP);
		}

		pUdp->UDP_CSR[0] |= AT91C_UDP_TXPKTRDY;
		do {
			csr = pUdp->UDP_CSR[0];

			// Data IN stage has been stopped by a status OUT
			if (csr & AT91C_UDP_RX_DATA_BK0) {
				pUdp->UDP_CSR[0] &= ~(AT91C_UDP_RX_DATA_BK0);
				return;
			}
		} while ( !(csr & AT91C_UDP_TXCOMP) );

	} while (length);

	if (pUdp->UDP_CSR[0] & AT91C_UDP_TXCOMP) {
		pUdp->UDP_CSR[0] &= ~(AT91C_UDP_TXCOMP);
		while (pUdp->UDP_CSR[0] & AT91C_UDP_TXCOMP);
	}
        if (USBdebug) printf("-> END");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_USB_SendZlp
//* \brief Send zero length packet through the control endpoint
//*----------------------------------------------------------------------------
void AT91F_USB_SendZlp(AT91PS_UDP pUdp)
{
	if (USBdebug) printf("\nUSB_SendZlp");
        pUdp->UDP_CSR[0] |= AT91C_UDP_TXPKTRDY;
	while ( !(pUdp->UDP_CSR[0] & AT91C_UDP_TXCOMP) );
	pUdp->UDP_CSR[0] &= ~(AT91C_UDP_TXCOMP);
	while (pUdp->UDP_CSR[0] & AT91C_UDP_TXCOMP);
        if (USBdebug) printf("-> END");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_USB_SendStall
//* \brief Stall the control endpoint
//*----------------------------------------------------------------------------
void AT91F_USB_SendStall(AT91PS_UDP pUdp)
{
	if (USBdebug) printf("USB_SendStall");
        pUdp->UDP_CSR[0] |= AT91C_UDP_FORCESTALL;
	while ( !(pUdp->UDP_CSR[0] & AT91C_UDP_ISOERROR) );
	pUdp->UDP_CSR[0] &= ~(AT91C_UDP_FORCESTALL | AT91C_UDP_ISOERROR);
	while (pUdp->UDP_CSR[0] & (AT91C_UDP_FORCESTALL | AT91C_UDP_ISOERROR));
        if (USBdebug) printf("-> END");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CDC_Enumerate
//* \brief This function is a callback invoked when a SETUP packet is received
//*----------------------------------------------------------------------------
static void AT91F_CDC_Enumerate(AT91PS_CDC pCdc)
{
	AT91PS_UDP pUDP = pCdc->pUdp;
	uchar bmRequestType, bRequest;
	ushort wValue, wIndex, wLength, wStatus;


	if (USBdebug) printf(".");
        if ( !(pUDP->UDP_CSR[0] & AT91C_UDP_RXSETUP) )
		return;
        
        if (USBdebug) printf("\nEnumerate");
	bmRequestType = pUDP->UDP_FDR[0];
	bRequest      = pUDP->UDP_FDR[0];
	wValue        = (pUDP->UDP_FDR[0] & 0xFF);
	wValue       |= (pUDP->UDP_FDR[0] << 8);
	wIndex        = (pUDP->UDP_FDR[0] & 0xFF);
	wIndex       |= (pUDP->UDP_FDR[0] << 8);
	wLength       = (pUDP->UDP_FDR[0] & 0xFF);
	wLength      |= (pUDP->UDP_FDR[0] << 8);
        //Debug code
        if (USBdebug) printf("_Request: %04x",((bRequest << 8) | bmRequestType));
	if (bmRequestType & 0x80) {
		pUDP->UDP_CSR[0] |= AT91C_UDP_DIR;
		while ( !(pUDP->UDP_CSR[0] & AT91C_UDP_DIR) );
	}
	pUDP->UDP_CSR[0] &= ~AT91C_UDP_RXSETUP;
	while ( (pUDP->UDP_CSR[0]  & AT91C_UDP_RXSETUP)  );

	// Handle supported standard device request Cf Table 9-3 in USB specification Rev 1.1
	switch ((bRequest << 8) | bmRequestType) {
	case STD_GET_DESCRIPTOR:
          //Debug code
          if (USBdebug) printf("->case: STD_GET_DESCRIPTOR");
		if (wValue == 0x100)       // Return Device Descriptor
			AT91F_USB_SendData(pUDP, devDescriptor, MIN(sizeof(devDescriptor), wLength));
		else if (wValue == 0x200)  // Return Configuration Descriptor
			AT91F_USB_SendData(pUDP, cfgDescriptor, MIN(sizeof(cfgDescriptor), wLength));
		else
			AT91F_USB_SendStall(pUDP);
		break;
	case STD_SET_ADDRESS:
          //Debug code
          if (USBdebug) printf("->case: STD_SET_ADDRESS");
		AT91F_USB_SendZlp(pUDP);
		pUDP->UDP_FADDR = (AT91C_UDP_FEN | wValue);
		pUDP->UDP_GLBSTATE  = (wValue) ? AT91C_UDP_FADDEN : 0;
		break;
	case STD_SET_CONFIGURATION:
          //Debug code
          if (USBdebug) printf("->case: STD_SET_CONFIGURATION");
		pCdc->currentConfiguration = wValue;
		AT91F_USB_SendZlp(pUDP);
		pUDP->UDP_GLBSTATE  = (wValue) ? AT91C_UDP_CONFG : AT91C_UDP_FADDEN;
		pUDP->UDP_CSR[1] = (wValue) ? (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_BULK_OUT) : 0;
		pUDP->UDP_CSR[2] = (wValue) ? (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_BULK_IN)  : 0;
		pUDP->UDP_CSR[3] = (wValue) ? (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_INT_IN)   : 0;
		break;
	case STD_GET_CONFIGURATION:
          //Debug code
          if (USBdebug) printf("->case: STD_GET_CONFIGURATION");
		AT91F_USB_SendData(pUDP, (char *) &(pCdc->currentConfiguration), sizeof(pCdc->currentConfiguration));
		break;
	case STD_GET_STATUS_ZERO:
          //Debug code
          if (USBdebug) printf("->case: STD_GET_STATUS_ZERO");
		wStatus = 0;
		AT91F_USB_SendData(pUDP, (char *) &wStatus, sizeof(wStatus));
		break;
	case STD_GET_STATUS_INTERFACE:
          //Debug code
          if (USBdebug) printf("->case: STD_GET_STATUS_INTERFACE");
		wStatus = 0;
		AT91F_USB_SendData(pUDP, (char *) &wStatus, sizeof(wStatus));
		break;
	case STD_GET_STATUS_ENDPOINT:
          //Debug code
          if (USBdebug) printf("->case: STD_GET_STATUS_ENDPOINT");
		wStatus = 0;
		wIndex &= 0x0F;
		if ((pUDP->UDP_GLBSTATE & AT91C_UDP_CONFG) && (wIndex <= 3)) {
			wStatus = (pUDP->UDP_CSR[wIndex] & AT91C_UDP_EPEDS) ? 0 : 1;
			AT91F_USB_SendData(pUDP, (char *) &wStatus, sizeof(wStatus));
		}
		else if ((pUDP->UDP_GLBSTATE & AT91C_UDP_FADDEN) && (wIndex == 0)) {
			wStatus = (pUDP->UDP_CSR[wIndex] & AT91C_UDP_EPEDS) ? 0 : 1;
			AT91F_USB_SendData(pUDP, (char *) &wStatus, sizeof(wStatus));
		}
		else
			AT91F_USB_SendStall(pUDP);
		break;
	case STD_SET_FEATURE_ZERO:
          //Debug code
          if (USBdebug) printf("->case: STD_SET_FEATURE_ZERO");
		AT91F_USB_SendStall(pUDP);
	    break;
	case STD_SET_FEATURE_INTERFACE:
          //Debug code
          if (USBdebug) printf("->case: STD_SET_FEATURE_INTERFACE");
		AT91F_USB_SendZlp(pUDP);
		break;
	case STD_SET_FEATURE_ENDPOINT:
          //Debug code
          if (USBdebug) printf("->case: STD_SET_FEATURE_ENDPOINT");
		wIndex &= 0x0F;
		if ((wValue == 0) && wIndex && (wIndex <= 3)) {
			pUDP->UDP_CSR[wIndex] = 0;
			AT91F_USB_SendZlp(pUDP);
		}
		else
			AT91F_USB_SendStall(pUDP);
		break;
	case STD_CLEAR_FEATURE_ZERO:
          //Debug code
          if (USBdebug) printf("->case: STD_CLEAR_FEATURE_ZERO");
		AT91F_USB_SendStall(pUDP);
	    break;
	case STD_CLEAR_FEATURE_INTERFACE:
          //Debug code
          if (USBdebug) printf("->case: STD_CLEAR_FEATURE_INTERFACE");
		AT91F_USB_SendZlp(pUDP);
		break;
	case STD_CLEAR_FEATURE_ENDPOINT:
          //Debug code
          if (USBdebug) printf("->case: STD_CLEAR_FEATURE_ENDPOINT");
		wIndex &= 0x0F;
		if ((wValue == 0) && wIndex && (wIndex <= 3)) {
			if (wIndex == 1)
				pUDP->UDP_CSR[1] = (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_BULK_OUT);
			else if (wIndex == 2)
				pUDP->UDP_CSR[2] = (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_BULK_IN);
			else if (wIndex == 3)
				pUDP->UDP_CSR[3] = (AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_ISO_IN);
			AT91F_USB_SendZlp(pUDP);
		}
		else
			AT91F_USB_SendStall(pUDP);
		break;

	// handle CDC class requests
	case SET_LINE_CODING:
          //Debug code
          if (USBdebug) printf("->case: SET_LINE_CODING");
		while ( !(pUDP->UDP_CSR[0] & AT91C_UDP_RX_DATA_BK0) );
		pUDP->UDP_CSR[0] &= ~(AT91C_UDP_RX_DATA_BK0);
		AT91F_USB_SendZlp(pUDP);
		break;
	case GET_LINE_CODING:
          //Debug code
          if (USBdebug) printf("->case: GET_LINE_CODING");
		AT91F_USB_SendData(pUDP, (char *) &line, MIN(sizeof(line), wLength));
		break;
	case SET_CONTROL_LINE_STATE:
          //Debug code
          if (USBdebug) printf("->case: SET_CONTROL_LINE_STATE");
		pCdc->currentConnection = wValue;
		AT91F_USB_SendZlp(pUDP);
		break;
	default:
          //Debug code
          if (USBdebug) printf("->case: ???");
		AT91F_USB_SendStall(pUDP);
	    break;
	}
        if (USBdebug) printf("-> END");
}
