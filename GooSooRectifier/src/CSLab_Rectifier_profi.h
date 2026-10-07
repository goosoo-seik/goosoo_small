// ----------------------------------------------------------------------------
//         ILMAC system co  -  Lee Y k  -
// ----------------------------------------------------------------------------
// File Name           : ILMAC_Rectifier_profi.h
// Object              : ILMAC_Rectifier_profi.c Header file
// Creation            : 2008/06/06
// ----------------------------------------------------------------------------
/*****************************************************************************
**                                                                         **
** COPYRIGHT NOTIFICATION (c) 1998, 99    HMS Industrial Networks AB.      **
**                                                                         **
** This program is the property of HMS Industrial Networks AB.             **
** It may not be reproduced, distributed, or used without permission       **
** of an authorised company official.                                      **
**                                                                         **
** Symbolic Constants
**
*****************************************************************************
*/
#define BOOL    unsigned char
#define SINT8   signed char 
#define SINT16  signed short  
#define SINT32  int  
#define UINT8   unsigned char  
#define UINT16  unsigned short 
#define UINT32  unsigned int

#define MB_NO_ERR         0
#define MB_ILLEGAL_FUNC   1
#define MB_ILLEGAL_ADD    2
#define MB_ILLEGAL_DATA   3
#define MB_SLAVE_FAILER   4
#define MB_LENGTH_ERR     5
#define MB_TIMEOUT_ERR    6
#define MB_CRC_ERR        7
#define MB_TX_OVER        8
#define MB_RX_OVER        9
#define MB_WRITE_FAIL    10
#define MB_RECEIVE_FAIL  11
// Profi Function Call Source
#define PROFI_CON_CALL  0x11
#define REMOTE_CALL     0x22

#define MODBUS_BUF_SIZE 60 

#define SD_RX_BUFFER_SIZE     10
#define S0RELL_9600           0x0D9
#define S0RELH_9600           0x03
#define MB_MESSAGE_END_9600   4000
#define MB_DEFAULT_TIMEOUT    60000
#define MB_QUERY_RETRY        10
#define PROFI_SEARCH_RETRY    4
#define MB_RECEIVE_WAIT        100
#define MODBUS_READ_WAIT_19200 20
#define MODBUS_READ_WAIT_38400 10
#define MODBUS_READ_WAIT_57600 7
#define MODBUS_WRITE_WAIT     50

extern UINT8 MB_bCRCCounter;
extern UINT8 MB_bTimeOutCounter;
extern UINT16 MB_iTimeOutTime;

void putchar_mb(unsigned char c);
unsigned char getchar_mb(void);
char MB_CharReceived(void);
void ProfiD_packet_exchange( UINT8* query, UINT8 size, UINT8* response, short call );
void ProfiD_packet_exchange1( UINT8 *query, UINT8 size, UINT8* response, short call );
void print_ModBusErr(char err);
void profiport_speed_change(void);
void profiport_buffer_clear(void);
void profi_parameter_multi_read_query(UINT8 code, UINT8 source);
void profi_parameter_single_read_query(UINT8 code, UINT8 source);
char profi_single_write_query(UINT8 code, UINT16 data, UINT8 source);
void profi_SCIout_read_query(UINT8 offset, UINT8 size, UINT8 source);
void profi_SCIin_write_query(UINT8 offset, UINT8 size, UINT8 source);
void remote_out_data_generate(void);
void remote_in_data_parsering(void);
void remote_operate_decide(void);
void remote_controlword_parsering(void);
void remote_in_data_parsering_demo(void);
void operate_by_remote_data(void);

void MB_SetTimeout( UINT16 iTime );
UINT8 MB_SendRecModbusMessage( UINT8* pbData,
                               UINT8 bSize,
                               UINT8* pbResponse );



UINT16 GenerateCrc( UINT8* pabMessage, UINT16 iLength );
BOOL ABIC_AutoBaud( void );
BOOL ABIC_NormalMode (void);		// BEG, 28.03.06
BOOL ABIC_ReadOutData( UINT8 bOffset, UINT8 bSize, UINT8* pData );
BOOL ABIC_WriteInData( UINT8 bOffset, UINT8 bSize, UINT8* pData );
UINT8 SD_GetChar( void );
void SD_Init( void );
BOOL SD_CharReceived( void );

void SD_PutChar( UINT8 bByte );
void sd_RxInterrupt( void );
extern UINT16 TM_iTimeOutTime;
extern UINT16 TM_iResponseTime;
void TM_SetTimer( UINT16 iTime );
void TM_StartTimer( void );
void TM_StopTimer( void );
BOOL TM_TimeOut();

// Profi Device ModBus Functions
#define PROFI_DEV_ADD         0x01
#define PROFI_MULTI_READ      0x03
#define PROFI_INPUT_READ      0x04
#define PROFI_SINGLE_WRITE    0x06
#define PROFI_MULTI_WRITE     0x10

// Profi Device ModBus Memory Area
#define PROFI_SCIIN_DATA_ADD     0
#define PROFI_SCIOUT_DATA_ADD  0x1000

// Profi Device General Parameter
#define RDWR_PD_MODE            1
#define READ_PD_STATUS          2
#define READ_PD_TYPE            3
#define READ_BUS_TYPE           4
#define READ_LED_STATE          7
#define RDWR_CONFIG_BITS        8
#define RDWR_SWITCH_CODEING     9
#define RDWR_OFFLINE_ACTION     10 
#define RDWR_IDLE_ACTION        11
#define RDWR_PD_INT_CONFIG      12
#define READ_PD_INT_CAUSE       13
#define RDWR_SCIRATE_CONFIG     14
#define READ_SCIRATE_ACTUAL     15
#define RDWR_SCISET_CONFIG      16
#define READ_SCISET_ACTUAL      17
#define RDWR_MB_RTU_ADDRESS     22
#define RDWR_MB_CRC_DISABLE     23
#define RDWR_FB_FAULT_VALUE     27
// Profi Device General Parameter
#define RDWR_FB_BYTE_ORDER      40
#define RDWR_FB_OUT_CONFIG      41
#define READ_FB_OUT_ACTUAL      42
#define READ_FB_IN_ACTUAL       43
#define RDWR_FBIN_SSC_OFFSET    44
#define RDWR_FBIN_SSC_SIZE      45
#define RDWR_FBIN_SCI_OFFSET    46
#define RDWR_FBIN_SCI_SIZE      47
#define RDWR_SSC_BYTE_ORDER     50
#define RDWR_SSC_IN_CONFIG      51
#define READ_SSC_IN_AUTO        52
#define READ_SSC_IN_ACTUAL      53
#define RDWR_SSC_OUT_CONFIG     54
#define READ_SSC_OUT_AUTO       55
#define READ_SSC_OUT_ACTUAL     56
#define RDWR_SSC_OUT_FB_OFFSET  57
#define RDWR_SSC_OUT_FB_SIZE    58
#define RDWR_SSC_OUT_SCI_OFFSET 59
#define RDWR_SSC_OUT_SCI_ASIZE  60
#define RDWR_SCI_BYTE_ORDER     63
#define RDWR_SCI_IN_CONFIG      64
#define READ_SCI_IN_ACTUAL      65
#define READ_SCI_OUT_ACTUAL     66
#define RDWR_SCI_OUT_FB_OFFSET  67
#define RDWR_SCI_OUT_FB_SIZE    68
#define RDWR_SCI_OUT_SSC_OFFSET 69
#define RDWR_SCI_OUT_SSC_SIZE   70
#define READ_FB_ADDRESS_SSC     104

// AnyBus Profi Device Initialisation Sequence
const UINT8 PROFI_INIT_SEQUENCE[] =
{
  RDWR_CONFIG_BITS,       //8
  RDWR_SWITCH_CODEING,    //9
  //RDWR_SCIRATE_CONFIG,    //14
  RDWR_FB_BYTE_ORDER,     // 40
  RDWR_FB_OUT_CONFIG,     //41
  RDWR_FBIN_SSC_SIZE,     //45
  RDWR_FBIN_SCI_OFFSET,   //46
  RDWR_FBIN_SCI_SIZE,     //47
  RDWR_SSC_IN_CONFIG,     //51
  RDWR_SSC_OUT_CONFIG,    //54
  RDWR_SCI_BYTE_ORDER,    //63
  RDWR_SCI_IN_CONFIG,     //64
  RDWR_SCI_OUT_FB_OFFSET, //67
  RDWR_SCI_OUT_FB_SIZE,   //68
  RDWR_SCI_OUT_SSC_SIZE,  //70
  RDWR_PD_MODE,           // 1
  0xFF
};

// Profi Device Default Setting
// Profi Device Default Setting
#define DEFAULT_FBIN_OFFSET   0
#define DEFAULT_FBOUT_OFFSET  0
#ifdef PROFI_WORD_4
  #define PROFI_INPUT_SIZE      8
  #define PROFI_OUTPUT_SIZE     8  
#else
  #define PROFI_INPUT_SIZE      32
  #define PROFI_OUTPUT_SIZE     32  
#endif
#define SCI_RATE_AUTO       0
#define SCI_RATE_4800       1
#define SCI_RATE_9600       2
#define SCI_RATE_19200      3
#define SCI_RATE_38400      4
#define SCI_RATE_57600      5

const UINT16 PROFI_INIT_DATA[] =
{
  0x03,                 //  8,CONFIG_BITS(SSCI= 1, SSCO = 1)
  0x01,                 //  9,SWITCH_CODEING(HEX)
  //SCI_RATE_AUTO,      // 14,SCIRATE_CONFIG
  0,                    // 40,FB_BYTE_ORDER
  PROFI_OUTPUT_SIZE,    // 41,FB_OUT_CONFIG
  0,                    // 45,FBIN_SSC_SIZE
  DEFAULT_FBIN_OFFSET,  // 46,FBIN_SCI_OFFSET  
  PROFI_OUTPUT_SIZE,    // 47,FBIN_SCI_SIZE
  0,                    // 51,SSC_IN_CONFIG
  0,                    // 54,SSC_OUT_CONFIG
  0,                    // 63,SCI_BYTE_ORDER
  PROFI_OUTPUT_SIZE,    // 64,SCI_IN_CONFIG
  DEFAULT_FBOUT_OFFSET, // 67,SCI_OUT_FB_OFFSET
  PROFI_INPUT_SIZE,     // 68,SCI_OUT_FB_SIZE
  0,                    // 70,SCI_OUT_SSC_SIZE  
  1                     //  1,SCI_NORMAL_MODE
};
  
const UINT16 MBaddForProfiFunc[] =
{
// General Parameter
       0, 0x5001, 0x5002, 0x5003, // 0-3
  0x5004,      0,      0, 0x5007, // 4-7
  0x5008, 0x5009, 0x500A, 0x500B, // 8-11
  0x500C, 0x500D, 0x500E, 0x500F, // 12-15
  0x5010, 0x5011, 0xFFFF, 0xFFFF, // 16-19
  0xFFFF, 0xFFFF, 0x5016, 0xFFFF, // 20-23
       0,      0,      0, 0x501B, // 24-27
  0x501C, 0x501D, 0x501E, 0x501F, // 28-31
  0x5020, 0x5021, 0x5022, 0x5023, // 32-35
  0x5024, 0x5025, 0x5026, 0x5027, // 36-39
// I/O Parameter
  0x6000, 0x6001, 0x6002, 0x6003, // 40-43
  0x6004, 0x6005, 0x6006, 0x6007, // 44-47
       0,      0, 0x600A, 0x600B, // 48-51
  0x600C, 0x600D, 0x600E, 0x600F, // 52-55
  0x6010, 0x6011, 0x6012, 0x6013, // 56-59
  0x6014,      0,      0, 0x6017, // 60-63
  0x6018, 0x6019, 0x601A, 0x601B, // 64-67
  0x601C, 0x601D, 0x601E,      0, // 68-71
  0, 0, 0, 0, 0, 0, 0, 0,         // 72-79 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 80-89 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 90-99  
  0x7000, 0x7001, 0xFFFF, 0x7003, //100-103
  0x7004, 0x7005, 0x7006, 0x7007, //104-107
  0x7008, 0x7009, 0x700A, 0x700B, //108-111
  0x700C, 0x700D, 0x700E, 0X700F  //112-114  
};

const UINT8 MBsizeForProfiFunc[] =
{
// General Parameter
  0, 2, 2, 2, // 1-3
  2, 0, 0, 2, // 4-7
  2, 1, 1, 1, // 8-11
  2, 2, 1, 1, // 12-15
  1, 1, 1, 1, // 16-19
  1, 1, 1, 1, // 20-23
  0, 0, 0,24, // 24-27
  2, 2, 2, 2, // 28-31
  2, 2, 2, 2, // 32-35
  2, 2, 2, 2, // 36-39
// I/O Parameter
  1, 2, 2, 2, // 40-43
  2, 2, 2, 2, // 44-47
  0, 0, 1, 1, // 48-51
  2, 2, 2, 2, // 52-55
  2, 2, 2, 2, // 56-59
  2, 0, 0, 1, // 60-63
  2, 2, 2, 2, // 64-67
  2, 2, 2, 0, // 68-71
  0, 0, 0, 0, 0, 0, 0, 0,         // 72-79 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 80-89 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 90-99  
  2, 2, 0, 2, //100-103
  2, 2, 2, 2, //104-107
  2, 2, 2, 2, //108-111
  2, 2, 2, 24 //112-115  
};

// 1: Read/Write enable, 0: Read only
const UINT8 MBrwForProfiFunc[] =
{
// General Parameter
  0, 1, 0, 0, // 1-3
  0, 0, 0, 0, // 4-7
  1, 1, 1, 1, // 8-11
  0, 0, 0, 0, // 12-15
  0, 0, 0, 0, // 16-19
  0, 0, 0, 0, // 20-23
  0, 0, 0, 1, // 24-27
  1, 1, 1, 1, // 28-31
  1, 1, 1, 1, // 32-35
  1, 1, 1, 1, // 36-39
// I/O Parameter
  1, 1, 0, 0, // 40-43
  1, 1, 1, 1, // 44-47
  0, 0, 1, 1, // 48-51
  0, 0, 1, 0, // 52-55
  0, 1, 1, 1, // 56-59
  0, 0, 1, 1, // 60-63
  1, 0, 0, 1, // 64-67
  1, 1, 1, 0, // 68-71
  0, 0, 0, 0, 0, 0, 0, 0,         // 72-79 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 80-89 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 90-99  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //100-109  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //110-115  
};

typedef struct _ProfiDevRead
{
  UINT8 SlaveAdd; 
  UINT8 FuncCode;  
  UINT8 StartAddH;
  UINT8 StartAddL;
  UINT8 ReadQtyH;
  UINT8 ReadQtyL;
} ProfiDevRead;

typedef struct _ProfiDevSingleWrite
{
  UINT8 SlaveAdd; 
  UINT8 FuncCode;  
  UINT8 StartAddH;
  UINT8 StartAddL;
  UINT8 DataH;
  UINT8 DataL;
} ProfiDevSingleWrite;

typedef struct _ProfiDevMultiWrite
{
  UINT8 SlaveAdd; 
  UINT8 FuncCode;
  UINT8 StartAddH;
  UINT8 StartAddL;  
  UINT8 RegQtyH;
  UINT8 RegQtyL;
  UINT8 ByteQty;
  UINT8 Data[PROFI_OUTPUT_SIZE];
} ProfiDevMultiWrite;

ProfiDevRead        PDreadPkt;
ProfiDevSingleWrite PDSwritePkt;
ProfiDevMultiWrite  PDMwritePkt;
//ProfiDevRead *pOutPkt = &TxSerialPkt;
//UINT8*  ptr = &TxSerialPkt;
UINT8  *PtrModQuery;
//UINT8* pModQuery;
UINT8* pModResponse;

/****************************/
/*  MSG packet send to ZCM  */
/****************************/
/*
void send_API_MSG_packet(void)
{
  MSG_HANDLE TxMsg;
  MSG_HANDLE *pMsg = &TxMsg;

  char  *Ptr = (char*)&TxSerialPkt;
  Ptr += 2;
  
  pMsg->ProfileId_L = 1;
  pMsg->ProfileId_H = 0;
  pMsg->ClusterId = 0x13;
  //pMsg->AddrMode = 0x01;
  //memcpy(pMsg->Address, &ZEDaddress16, 8);
  pMsg->AddrMode = 0x00;  
  memcpy(pMsg->Address, &ZEDaddress, 8);
  pMsg->DstEndPoint = 1;
  pMsg->SrcEndPoint = 1;
  pMsg->Reserved = 0;
  pMsg->MessageLength = APP_MAX_DATA_SIZE;
  memcpy(pMsg->Message, &TxmMessage, pMsg->MessageLength);
  
  pOutPkt->PKT.layer = APP_LAYER;               // 0x80
  pOutPkt->PKT.Opcode = APP_MSG;		// 0x01	
  pOutPkt->PKT.Type = REQUEST;			// 0x10
  pOutPkt->PKT.Size = sizeof(MSG_HANDLE) + pMsg->MessageLength - APP_MAX_DATA_SIZE;
  printf("\nSIZE:%02X",pOutPkt->PKT.Size);
  
  memcpy(pOutPkt->PKT.Arguments, &TxMsg, pOutPkt->PKT.Size);
  pOutPkt->LEN = pOutPkt->PKT.Size + 4;
  put_serial_write(Ptr, pOutPkt->LEN); 
}
*/
typedef struct sd_DataType
{

   /*
   ** Output enabled flag.
   */

   BOOL fEnabled;

   /*
   ** Ring buffers & buffer pointers.
   */

   UINT8 abRxBuffer[ SD_RX_BUFFER_SIZE ];

   /*
   ** Recievd char put position
   */

   UINT16 iRxGet;

   /*
   ** Recievd char get position
   */

   UINT16 iRxPut;

   /*
   ** Number of characters in rx buffer
   */

   UINT16 iRxSize;

   /*
   ** Transmit buffer get position
   */

   UINT16 iTxGet;

   /*
   ** Number of characters in tx buffer
   */

   UINT16 iTxSize;

}
sd_DataType;

extern sd_DataType sd_s;

/*
  case PROFI_REG_READ+3:
    if (ModBusStep == 0)
    {
      profi_single_read_query(ProfiRegNo, PROFI_CON_CALL);
      sExecStep++;
    }
    return;    

  case PROFI_REG_WRITE+11:
    if (ModBusStep == 0)
    {
      usdata = (ProfiDataH << 8) + ProfiDataL; 
      profi_single_write_query(ProfiRegNo, usdata, PROFI_CON_CALL);
      sExecStep++;
    }
    return;    
*/
