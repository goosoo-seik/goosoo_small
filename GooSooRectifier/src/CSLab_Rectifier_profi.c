

// Include Standard LIB  files
#include "project.h"
#include "CSLab_Rectifier_profi.h"


UINT8 abResponse[ MODBUS_BUF_SIZE ];
UINT8 abReceiveBuf[ MODBUS_BUF_SIZE ];
UINT8 abSendBuffer[ MODBUS_BUF_SIZE ];
//UINT8 abRequest[ 6 ] = { 0x01, 0x03, 0x50, 0x01, 0x00, 0x01 } ;
  
UINT8 ProfiNodeAdd;
UINT8 ProfiMode;
UINT8 ModBusErr;
UINT8 ModBusResult;
UINT8 ModBusRxLength;
short ProfiSetStep;
short RemoteStep;
char ProfiTestOut;
char DemoStep;
char ModBusCall;
char ModBusStep;
short sModBusWait;

UINT16 usModBusFunc;
UINT8 ProfiRegNo;
UINT8 RegisterQty;
UINT8 FBinOffset;
UINT8 FBoutOffset;

UINT16 iCrc;
UINT8 bCharPos = 0;   
char ProfiDebug;
char ProfiRetry;
char ProfiSearch;
char ProfiFind;
char ResponseSize;
char UnExecuteCMD;
short sLineSpeed;

short sRemoteDelay;
char InitOrder;
char InitRegNo;
char RemoteScanNo;
char RemoteScanSpeed;
char  usInitData;
short sMaxRemoteDelay;

/*********************************************/
/* AnyBus PDP operating Functions            */
/*********************************************/
#define REMOTE_BOOT       5
#define REMOTE_BOOT_END   19
#define REMOTE_INIT       20
#define REMOTE_INIT_END   40
#define REMOTE_START      50
#define REMOTE_SCAN       70
#define REMOTE_INIT_FAIL  100
#define REMOTE_SCAN_FAIL  110
#define REMOTE_NODE_FAIL  120
#define REMOTE_BOOT_FAIL  130

// Delay times
#define REMOTE_INIT_DELAY  50
#define REMOTE_RETRY_DELAY 50
#define REMOTE_SCAN_DELAY  1
#define REMOTE_START_DELAY  SEC_1 * 5 // sec

//
// AnyBusDP transive 상태 표시
//
void remote_status_display(void)
{
//  if (ProfiFind == FALSE) printf("Remote FAIL   ");
//  else 
  {
    if (RemoteStatus == REMOTE_INIT) printf("pDP FIND:%5d", iCom0Speed);
    else if (RemoteStatus == REMOTE_SCAN)
    {
      if (!DemoStep) printf("r%d:%02d/s      ", ProfiNodeAdd, RemoteScanSpeed );
      else printf("Remote Demo:%2d", DemoStep );//pri
      //if (!DemoStep) printf("R%d:%02d:%2d:%2d", ProfiNodeAdd, RemoteStep, RemoteScanSpeed, ModBusStep );//printf("Remot:%2d", RemoteStep );
      //else printf("Remote Demo:%2d", DemoStep );//pri
    }
    else if (RemoteStatus == REMOTE_BOOT) 
      printf("pDP BOOT-%2d   ", RemoteStep );
    else if (RemoteStatus == REMOTE_START) 
      printf("r%d:Wait-%2d   ", ProfiNodeAdd, RemoteStep );
    else if (RemoteStatus == REMOTE_INIT_FAIL) 
      printf("rInitErr:%3d  ", RemoteStep );//printf("Remot InitFail");
    else if (RemoteStatus == REMOTE_SCAN_FAIL) 
      printf("r%d:ScanErr-%d", ProfiNodeAdd, ModBusErr );//printf("Remot ScanFail");
    else if (RemoteStatus == REMOTE_NODE_FAIL) 
      printf("pDP Add Error ");
    else if (RemoteStatus == REMOTE_BOOT_FAIL) 
      printf("pDP Boot Retry");
    else printf("r%d:Error-%2d  ", ProfiNodeAdd, RemoteStep );   
  }
}

//
// Code saving functions
//
// profi single read query send
//
char ProfiWait;
void remote_parameter_single_read(char regno)
{
    if (ModBusStep == 0)
    {
      profi_parameter_single_read_query(regno, REMOTE_CALL);
      ProfiWait = 0;
      RemoteStep++;
    }
}
//
// profi single write query send
//
void remote_profi_single_write(char regno, UINT16 usdata)
{
    if (ModBusStep == 0)
    {
      profi_single_write_query(regno, usdata, REMOTE_CALL);
      RemoteStep++;
      ProfiWait = 0;
    }
}

//
// profi single write retry with debug monit
//
void remote_profi_write_retry_debug(void)
{
  UINT16 usdata;
  UINT16 usreg;
  usreg = (pModResponse[2]<<8) + pModResponse[3];
  usdata = (pModResponse[4]<<8) + pModResponse[5];
  if (usInitData != usdata) ModBusErr = MB_WRITE_FAIL;
  
    if (ModBusErr != 0) 
    {
      if (ProfiDebug) print_ModBusErr(ModBusErr);
      if ((ModBusErr == MB_TIMEOUT_ERR)
         |(ModBusErr == MB_CRC_ERR)
         |(ModBusErr == MB_RX_OVER))
      {
        if (++ProfiRetry < 3) RemoteStep -= 2; 
        else RemoteStep = REMOTE_INIT_FAIL;       
      }
      else RemoteStep = REMOTE_INIT_FAIL;
    }
    else 
    {
      RemoteStep++;
      if (ProfiDebug)
        printf("\nRegister[%04X]:%04XH", usreg, usdata);
    }   
}
//
// profi single write retry 
//
void remote_profi_write_retry(void)
{
    if (ModBusErr == 0) RemoteStep++;
    else if ((ModBusErr == MB_TIMEOUT_ERR)
         |(ModBusErr == MB_CRC_ERR)
         |(ModBusErr == MB_RX_OVER))
      {
        if (++ProfiRetry < 3) RemoteStep -= 2; 
        else RemoteStep = REMOTE_INIT_FAIL;       
      }
      else RemoteStep = REMOTE_INIT_FAIL;
}

void RemoteStep_monit(void)
{
  short pos;
  pos = sCurPos;
  goto_cursor(7,2);
  printf("[%2d]", RemoteStep);
  sCurPos = pos;
}

/****************************************/
/* AnyBus DP operate Main Function      */
/****************************************/
/* Reset-Boot                           */
/* AnyBus DP Parameter Initialize       */                           
/* Wate AnyBusDP Normal operation       */
/* Auto Sacnning                        */                
/*(Field Bus data Transmit & Receive)   */
/* Remote Data Update                   */
/* Fail safe(Retry, Re-Init, Re-Boot)   */
/****************************************/
char RemoteStatus;
void AnyBusDP_transive(void)
{
  //char no, data;
  //UINT16  usdata; 
  //if (ProfiDebug) RemoteStep_monit(); 
  
  switch (RemoteStep)
 {   
  case 0:
    //display_mode(0);
    //ProfiDebug = 1;
    //RemoteStep = 1;
    RemoteStatus = 0;
    return;
  case 1:
    //ProfiDebug = 1;
    RemoteReady = 0;
    ProfiSearch = 0;
    RemoteStep = REMOTE_BOOT; 
    return;
 
  case REMOTE_BOOT:
    RemoteStatus = REMOTE_BOOT;
    ProfiFind = FALSE;
    ProfiSearch = 0;    
    sRemoteDelay = 0;
    RemoteStep++;
    return;

  // AnyBusDP Reset
  case REMOTE_BOOT+1:
    profi_reset(ON);
    ProfiRetry = 0;
    RemoteStep++;
    return;
 
  case REMOTE_BOOT+2: 
    if (++sRemoteDelay > 10) RemoteStep++; 
    return;

  case REMOTE_BOOT+3:
    profi_reset(OFF);
    sRemoteDelay = 0;
    RemoteStep++;
    return;
 
  // AnyBusDP Reset Delay
  case REMOTE_BOOT+4: 
    if (++sRemoteDelay > SEC_1) RemoteStep++; 
    return;

  // AnyBusDP AutoBaud Seguence Start
  case REMOTE_BOOT+5:
    remote_parameter_single_read(RDWR_PD_MODE);
    return; 

  // Wait until packet exchange end
  case REMOTE_BOOT+6: 
    if (++ProfiWait > MB_RECEIVE_WAIT)
    {
      ModBusErr = MB_RECEIVE_FAIL;
      RemoteStep = REMOTE_BOOT+7;
    } 
    return;    

  case REMOTE_BOOT+7:
    if ((ModBusErr == 0)&(ModBusRxLength == 5))
    {
      if (ProfiDebug) 
      {
        printf("\nProfi Found");
        printf("\nBaudRate: %d", iCom0Speed);
      }
      ProfiFind = TRUE;     
      RemoteStep = REMOTE_BOOT_END;
    }
    else 
    {
      if (ProfiDebug) print_ModBusErr(ModBusErr);
      if (++ProfiRetry < MB_QUERY_RETRY) RemoteStep = REMOTE_BOOT+5;//RemoteStep++;
       else RemoteStep = REMOTE_BOOT+9;
    }
    sRemoteDelay = 0;
    return;    
 
  // AnyBusDP Reset Delay
  case REMOTE_BOOT+8: 
    if (++sRemoteDelay > 10) RemoteStep = REMOTE_BOOT+5; 
    return;

 
 case REMOTE_BOOT+9:
    if (++ProfiSearch < PROFI_SEARCH_RETRY) 
    {
      if (ProfiDebug) printf("\nRetry-%d", ProfiSearch); 
      RemoteStep = REMOTE_BOOT+1;
    }
    else
    {
      if (ProfiDebug) printf("\nDevice Search Fail!!");
      RemoteStep = REMOTE_BOOT_FAIL;
    }
    return;

  case REMOTE_BOOT_END:
    RemoteStep = REMOTE_INIT;
    return;  
 
//******************************************************
// A:수초간(REMOTE_START_DELAY) 대기후
// AnyBusDP의 Reg[1] = 1(Normal Operation)이면 
// SCAN으로 진행한다.
//  Reg[1] = 0(Start-up Mode)이면  
// 수초간(REMOTE_START_DELAY) 대기 ->
// Reg[1]을 점검하여 '0' 이면 '1'로 세트 ->
// 수초간(REMOTE_START_DELAY) 대기 ->
//******************************************************
  case REMOTE_START:
    ProfiSearch = 0;
    ProfiRetry = 0;   
    RemoteStep++;
    return;

  case REMOTE_START+1: 
    sRemoteDelay = 0;
    RemoteStep++;
    return;

  case REMOTE_START+2:
    if (++sRemoteDelay > REMOTE_START_DELAY) RemoteStep++; 
    return;
    
  case REMOTE_START+3:
    if (ModBusStep == 0)
    {
      profi_parameter_single_read_query(RDWR_PD_MODE, REMOTE_CALL);
      RemoteStep++;
    }
    return; 
  case REMOTE_START+4: 
    if (++ProfiWait > MB_RECEIVE_WAIT)
    {
      ModBusErr = MB_RECEIVE_FAIL;
      RemoteStep = REMOTE_START+5;
    } 
    return; 
  
  case REMOTE_START+5:
    if (ModBusErr == 0) 
    {
      // AnyBus PD Mode
      ProfiMode = pModResponse[4];
      if (ProfiMode == 1)  RemoteStep = REMOTE_SCAN;
      else RemoteStep = REMOTE_START+1; // 무한 반복
      //{
      //  if (++ProfiSearch < 10) RemoteStep = REMOTE_START+10;
      //  else RemoteStep = REMOTE_BOOT;
      //}        
    }
    else RemoteStep++;
    return;
    
  case REMOTE_START+6:
    if (++sRemoteDelay > SEC_1) RemoteStep++;
    return; 
  
  case REMOTE_START+7:
    if (++ProfiRetry < 5) RemoteStep = REMOTE_START+3;
    else RemoteStep = REMOTE_INIT;
    return; 
    
  case REMOTE_START+10:
    ProfiRetry = 0;
    sRemoteDelay = 0;
    RemoteStep++;
    return;

  case REMOTE_START+11:
    if (ModBusStep == 0)
    {
      profi_single_write_query(RDWR_PD_MODE, 1, REMOTE_CALL);
      RemoteStep++;
    }
    return;    

  case REMOTE_START+12:
    return;    

  case REMOTE_START+13:
    if (ModBusErr == 0) RemoteStep = REMOTE_START+1;
    else RemoteStep++;
    return; 
    
  case REMOTE_START+14:
    if (++sRemoteDelay > SEC_1) RemoteStep++;
    return; 
  
  case REMOTE_START+15:
    if (++ProfiRetry < 5) RemoteStep = REMOTE_START+11;
    else RemoteStep = REMOTE_INIT;
    return; 
    
//******************************************************
// AnyBusDP에 초기값을 설정
//******************************************************   
  case REMOTE_INIT:
    RemoteStatus = REMOTE_INIT;
    InitOrder = 0;
    RemoteStep++;
    return;  
 
  case REMOTE_INIT+1:
    if (ProfiDebug) sMaxRemoteDelay = SEC_1; 
    else sMaxRemoteDelay = REMOTE_INIT_DELAY;
    ProfiRetry = 0;
    RemoteStep++;
    return;  
 
  case REMOTE_INIT+2: 
   InitRegNo = PROFI_INIT_SEQUENCE[InitOrder];
   usInitData = PROFI_INIT_DATA[InitOrder];
   remote_profi_single_write(InitRegNo, usInitData);
   if (ProfiDebug) printf("\nInitOrder-%d:%d",InitOrder, InitRegNo); 
   sRemoteDelay = 0;
   return; 

  case REMOTE_INIT+3: 
    if (++ProfiWait > MB_RECEIVE_WAIT)
    {
      ModBusErr = MB_RECEIVE_FAIL;
      RemoteStep = REMOTE_INIT+4;
    }     
    return; 

  case REMOTE_INIT+4:
    if (++sRemoteDelay > sMaxRemoteDelay) 
      remote_profi_write_retry_debug();
    return;
    
  case REMOTE_INIT+5: 
    if (PROFI_INIT_SEQUENCE[++InitOrder] == 0xFF) RemoteStep++;
    else RemoteStep = REMOTE_INIT+1;
    return; 
    
  case REMOTE_INIT+6:
    ProfiRetry = 0;
    sRemoteDelay = 0;
    RemoteStep++; 
    return; 
 
  case REMOTE_INIT+7: 
    if (++sRemoteDelay > SEC_1) RemoteStep++;
    return; 
   
//
// ProfiNode Address Read
//
  case REMOTE_INIT+8:
    if (ModBusStep == 0)
    {
      profi_parameter_single_read_query(READ_FB_ADDRESS_SSC, REMOTE_CALL);
      RemoteStep++;
    }
    return; 
  case REMOTE_INIT+9: 
    if (++ProfiWait > MB_RECEIVE_WAIT)
    {
      ModBusErr = MB_RECEIVE_FAIL;
      RemoteStep = REMOTE_INIT+10;
    }         
    return; 
  
  case REMOTE_INIT+10:
    sRemoteDelay = 0;
    if (ModBusErr == 0) 
    {
      // ProfiNode Address
      ProfiNodeAdd = pModResponse[4];
      ProfiRetry = 0;
      if ((ProfiNodeAdd == 0)|(ProfiNodeAdd > 125)) RemoteStep = REMOTE_NODE_FAIL;
      else RemoteStep = REMOTE_INIT_END;
    }
    else if (++ProfiRetry < 5) RemoteStep = REMOTE_INIT+7;
    else RemoteStep = REMOTE_INIT_FAIL;
    return;
   
  case REMOTE_INIT_END: 
    ProfiSearch = 0;
    RemoteStep = REMOTE_START;//REMOTE_SCAN;//
    return; 
//
// Repeat Read/Write to/from ProfiBus Devive 
//
  case REMOTE_SCAN:
    RemoteStatus = REMOTE_SCAN;
    ProfiRetry = 0;
    // System Status data make
    if (ProfiTestOut == 1) 
    {
      RemoteStep = REMOTE_SCAN+9;
    }
    else
    {
      remote_out_data_generate();       
      RemoteStep++;
    }
    return;    
 
//
// Status Write to ProfiBus Devive 
//
  case REMOTE_SCAN+1:
    if (ModBusStep == 0)
    {
      profi_SCIin_write_query(DEFAULT_FBOUT_OFFSET, PROFI_OUTPUT_SIZE, REMOTE_CALL);
      ProfiWait = 0;
      RemoteStep++;
    }
    return;    

  case REMOTE_SCAN+2: 
    if (++ProfiWait > MB_RECEIVE_WAIT)
    {
      ModBusErr = MB_RECEIVE_FAIL;
      RemoteStep = REMOTE_SCAN+3;
    } 
    return; 

  case REMOTE_SCAN+3:
   if (ModBusErr == 0) 
   {
     ProfiRetry = 0;
     sRemoteDelay = 0;
     RemoteStep = REMOTE_SCAN+9;
   }
   else if (++ProfiRetry > 5) RemoteStep = REMOTE_SCAN+9;//REMOTE_SCAN_FAIL;
   else RemoteStep++;
   sRemoteDelay = 0;
   return;
   
 // Retry Delay
  case REMOTE_SCAN+4: 
    if (++sRemoteDelay > REMOTE_RETRY_DELAY) RemoteStep = REMOTE_SCAN+1;
    return; 

//
// Remote Data Read From ProfiBus Device 
//
  case REMOTE_SCAN+9: 
    if (++sRemoteDelay > REMOTE_SCAN_DELAY) RemoteStep++;
    return; 

  case REMOTE_SCAN+10:
    if (ModBusStep == 0)
    {
      profi_SCIout_read_query(DEFAULT_FBIN_OFFSET, PROFI_INPUT_SIZE, REMOTE_CALL);
      ProfiWait = 0;
      RemoteStep++;
    }
    return;    

  case REMOTE_SCAN+11: 

     if (++ProfiWait > MB_RECEIVE_WAIT)
    {
      ModBusErr = MB_RECEIVE_FAIL;
      RemoteStep = REMOTE_SCAN+12;
    } 
    return;    

  case REMOTE_SCAN+12:
   if (ModBusErr == 0) 
   {
     RemoteReady = 1;
     RemoteError = 0;
     if (!DemoStep) remote_in_data_parsering();
     else remote_in_data_parsering_demo();     
     remote_controlword_parsering();
     operate_by_remote_data();        
     remote_operate_decide();    
     RemoteStep = REMOTE_SCAN+14;
   }
   else if (++ProfiRetry > 5) RemoteStep = REMOTE_SCAN_FAIL;
   else RemoteStep++;
   sRemoteDelay = 0;
   return;    

 // Retry Delay
  case REMOTE_SCAN+13: 
    if (++sRemoteDelay > REMOTE_RETRY_DELAY) RemoteStep = REMOTE_SCAN+2;
    return; 
    
  case REMOTE_SCAN+14:
    if (++RemoteScanNo > 99) RemoteScanNo = 0;
     RemoteStep++;
    return;   
 
  case REMOTE_SCAN+15:
    if (++sRemoteDelay > REMOTE_SCAN_DELAY) RemoteStep = REMOTE_SCAN;
    return;   
    
//
// Init Fail Recover
//
  case REMOTE_INIT_FAIL:
    RemoteError = 1;
    RemoteReady = 0;
    RemoteStatus = REMOTE_INIT_FAIL;
    if (ProfiDebug) printf("\nDevice Search Fail!!");
    ProfiFind = FALSE;
    if (++ProfiSearch > 5) RemoteStep++;
    else RemoteStep = REMOTE_INIT;
    sRemoteDelay = 0;
    return; 

  case REMOTE_INIT_FAIL+1:
    if (++sRemoteDelay > SEC_1*5) RemoteStep = REMOTE_BOOT;
    return;

//
// Scan Fail Recover
//
  case REMOTE_SCAN_FAIL:
    RemoteError = 1;
    RemoteReady = 0;
    sRemoteDelay = 0;
    RemoteStatus = REMOTE_SCAN_FAIL;
    RemoteStep++;
    return;
    
  case REMOTE_SCAN_FAIL+1:
    if (++sRemoteDelay > SEC_1) RemoteStep++;
    return;
 
  case REMOTE_SCAN_FAIL+2:
    if (++ProfiSearch > 5) RemoteStep++;
    else RemoteStep = REMOTE_SCAN;
    return;
    
  case REMOTE_SCAN_FAIL+3:
    if (++sRemoteDelay > SEC_1*5) RemoteStep = REMOTE_BOOT;
    return;
    
//
// AnyBus DP Node Address bad setting error
// REcover : Address DIP switch reset, Power OFF -> ON
//
  case REMOTE_NODE_FAIL:
    RemoteStatus = REMOTE_NODE_FAIL;
    RemoteError = 1;
    RemoteReady = 0;
    RemoteStep++;
    return;
    
  case REMOTE_NODE_FAIL+1:
    return;

//
// AnyBus DP Boor Fail
// Baudrate Change & Retry
//
  case REMOTE_BOOT_FAIL:
    RemoteStatus = REMOTE_BOOT_FAIL;
    RemoteError = 1;
    RemoteReady = 0;
    RemoteStep++;
    return;
    
  case REMOTE_BOOT_FAIL+1:
    profiport_speed_change();
    RemoteStep = REMOTE_BOOT;
    return;
    
  default: 
    //RemoteStep = 0; 
    return;
 }
}

//***************************************
// 원격서버에 보낼 데이터 만들기
// For AnyBus DP ProfiBus
//***************************************
unsigned short iRemOperAmp;
unsigned short iRemRevOperAmp;
//unsigned short iRemMaxOverAmp;
//unsigned short iRemRevOverAmp;
//
// Make Control data for Remote Server
//
char SystemLive; // 시스템 작동 알림 플래그
UINT16 plc_control_word(void)
{
  UINT16 word;
  word = 0;
// 2009/03/26 4 word PLC 통신 추가
#ifdef PROFI_WORD_4
    if (OperUser == REMOTE) word |= 0x0001;
    if (!ReadyStop)         word |= 0x0002;
    if (SystemRun == ON)    word |= 0x0004;
    if (OperPole != PLUS)   word |= 0x0008;
    if (proTalarm)          word |= 0x0010;
    //if (TotalAlarm)         word |= 0x0010;
    if (TotalError)         word |= 0x0020;
    if (TotalVcsTrip)       word |= 0x0040;
    if (RemoteError)        word |= 0x0080;
    
    if (UnExecuteCMD)       word |= 0x0100;
    if  (DcOverErr)         word |= 0x0200;
    if  (DacError)          word |= 0x0400;
    if  (AdcError)          word |= 0x0800;
    if (AcOverErr)          word |= 0x1000;
    if (BootError)          word |= 0x2000;
    if (LineEmegErr)        word |= 0x4000;
    if (SystemLive)         word |= 0x8000;
#else
    if (OperUser == REMOTE) word |= 0x0001;
    if (!ReadyStop)         word |= 0x0002;
    if (SystemRun == ON)    word |= 0x0004;
    if (OperPole != PLUS)   word |= 0x0008;
    if (TotalAlarm)         word |= 0x0010;
    if (TotalError)         word |= 0x0020;
    if (TotalVcsTrip)       word |= 0x0040;
    if (RemoteError)        word |= 0x0080;
    // 포스코와 협의하여 RemoteError로 대체 2008.06.13
    //if (ExtManualOp)        word |= 0x0080;
    if (UnExecuteCMD)       word |= 0x0100;
    // 2008-12-17 PwMeterError 임시 삭제
    //if (PwMeterError)       word |= 0x0200;
    if (AcLowFault)         word |= 0x0400;
    if (AcLowAlarm)         word |= 0x0400;
    // 2009-01-30 구수 요청으로 추가
    if (SystemLive)         word |= 0x8000;
#endif    
    return word;
}

//
// Make Error Code for Remote Server
//
UINT16 plc_error_code(void)
{
   UINT16 word;
   word = 0;
    if (EmegError)    word |= 0x0001;
    if (ExtError1)    word |= 0x0002;
    if (ExtError2)    word |= 0x0004;
    if (ExtManualOp)  word |= 0x0008; 
    if (ExtError3)    word |= 0x0010;
    if (ExtError4)    word |= 0x0020;
    if (ExtError5)    word |= 0x0040;
    if (ExtError6)    word |= 0x0080;
    
    if (ExtAlarm1)    word |= 0x0100;
    if (ExtAlarm2)    word |= 0x0200;
    if (ExtAlarm3)    word |= 0x0400;
    if (ExtAlarm4)    word |= 0x0800;
    if (ExtAlarm5)    word |= 0x1000;
    if (ExtAlarm6)    word |= 0x2000;
    if (ExtAlarm7)    word |= 0x4000;
    if (ExtAlarm8)    word |= 0x8000;
  return word;
}

char word_to_PDMwritePkt(UINT16 word, UINT8 no)
{
  PDMwritePkt.Data[no++] = word >> 8;
  PDMwritePkt.Data[no++] = word & 0xFF;
  return no;
}

//
// Make Status data for Remote Server
// 2008-11-26
// MONO_POLE일때와 PR type 일때를 구분
//
#ifdef MONO_POLE
void remote_out_data_generate(void)
{
  int i;
  UINT16 word;
  UINT8 no;
  no = 0;
// 2009/03/26 4 word PLC 통신 추가
 #ifdef PROFI_WORD_4
  word = plc_control_word();          // Control Word         
  no = word_to_PDMwritePkt(word, no); // 1
  if (!SystemRun)
  {
     i = iAmpInput;
    if ( i < 0) i *= -1;
    if ( i < (fMaxOperAmp * 0.02)) i = 0;
    word = i;
  }
  else{
        i = iAmpInput;
          if(i<0)i=0; 
           word = i;                    // Fwd Current
  }
    no = word_to_PDMwritePkt(word, no); // 2
 
  
  word = fVoltInput *100;                      // Frd Volt
  no = word_to_PDMwritePkt(word, no); // 3
  word = plc_error_code();                           // error code
  no = word_to_PDMwritePkt(word, no); // 4
 UnExecuteCMD = 0;

#else
  word = plc_control_word();          // Control Word         
  no = word_to_PDMwritePkt(word, no); // 1
  word = plc_error_code();            // Error Code
  no = word_to_PDMwritePkt(word, no); // 2
  word = fOperAmp;                    // Fwd Current
  no = word_to_PDMwritePkt(word, no); // 3
  word = 0;                           // Rev Current
  no = word_to_PDMwritePkt(word, no); // 4
  word = fMaxOperAmp;                 // Fwd Current Limit
  no = word_to_PDMwritePkt(word, no); // 5
  word = 0;                           // Rev Current Limit
  no = word_to_PDMwritePkt(word, no); // 6
  word = sLineSpeed;                  // Line Speed Return
  no = word_to_PDMwritePkt(word, no); // 7
 #endif
  // 2008-12-03
  // 정지상태에서 전류측정값이 정격최대전류값의 0.2%이내이면 '0'으로 표시
  // Current Actual
 // if (!SystemRun)
// {
 //   i = iAmpInput;
 //   if ( i < (fMaxOperAmp * 0.02)) i = 0;
 //   word = i;
 // }
 // else word = iAmpInput;    

  //no = word_to_PDMwritePkt(word, no); // 8
  //word = iVoltInput;                  // Voltage Actual
 // no = word_to_PDMwritePkt(word, no); // 9
 // UnExecuteCMD = 0;  // 실행불가명령은 1회 송신후 클리어
  
  // 2009-01-09 구수 하과장과 협의
  // 리모트에서 시스템 작동을 확인하기 위해 usScanCount 전달 추가
 // word = usScanCount;                 // Sysyem scan counter
  //no = word_to_PDMwritePkt(word, no); // 10

} 
#else
void remote_out_data_generate(void)
{
  int i;
  UINT16 word;
  UINT8 no;
  no = 0;
// 2009/03/26 4 word PLC 통신 추가
 #ifdef PROFI_WORD_4
  word = plc_control_word();          // Control Word         
  no = word_to_PDMwritePkt(word, no); // 1
  if (OperPole != MINUS) 
  {
     if (!SystemRun)
  {
    i = iAmpInput;
    if ( i < 0) i *= -1;
    if ( i < (fMaxOperAmp * 0.02)) i = 0;
    word = i;
  }
  else word = iAmpInput;                       // fwd Current
  no = word_to_PDMwritePkt(word, no); // 2
 
  
  word = fVoltInput *100;                      // fwd Volt
  no = word_to_PDMwritePkt(word, no); // 3
    word = plc_error_code();                           // error code
    no = word_to_PDMwritePkt(word, no); // 4
  UnExecuteCMD = 0;
  }
 
  else
  {
     if (!SystemRun)
  {
    i = iAmpInput;
    if ( i < 0) i *= -1;
    if ( i < (fMaxOperAmp * 0.02)) i = 0;
    word = i;
  }
  else word = iAmpInput;                       // rev Current
    no = word_to_PDMwritePkt(word, no); // 2
 
  
    word = fVoltInput *100;                      // rev Volt
    no = word_to_PDMwritePkt(word, no); // 3
    word = plc_error_code();                          
    no = word_to_PDMwritePkt(word, no); // 4    //error code
  UnExecuteCMD = 0;
  }
    
 #else
  word = plc_control_word();          // Control Word         
  no = word_to_PDMwritePkt(word, no); // 1
  word = plc_error_code();            // Error Code
  no = word_to_PDMwritePkt(word, no); // 2
  word = fOperAmp;                    // Fwd Current
  no = word_to_PDMwritePkt(word, no); // 3
  word = fRevOperAmp;                 // Rev Current
  no = word_to_PDMwritePkt(word, no); // 4
  word = fMaxOverAmp;                 // Fwd Current Limit
  no = word_to_PDMwritePkt(word, no); // 5
  word = fMaxOverAmp;                 // Rev Current Limit
  no = word_to_PDMwritePkt(word, no); // 6
  word = sLineSpeed;                  // Line Speed Return
  no = word_to_PDMwritePkt(word, no); // 7
#endif
  
  // 2008-12-03
  // 정지상태에서 전류측정값이 정격최대전류값의 0.2%이내이면 '0'으로 표시
  // Current Actual
  if (!SystemRun)
  {
    i = iAmpInput;
    if ( i < 0) i *= -1;
    if ( i < (fMaxOperAmp * 0.02)) i = 0;
    word = i;
  }
  else word = iAmpInput;    
  
  no = word_to_PDMwritePkt(word, no); // 8
  word = fVoltInput * 10;             // Voltage Actual 2008-12-03
  //word = iVoltInput;                // Voltage Actual
  no = word_to_PDMwritePkt(word, no); // 9
  UnExecuteCMD = 0;  // 실행불가명령은 1회 송신후 클리어

  // 2009-01-09 구수 하과장과 협의
  // 리모트에서 시스템 작동을 확인하기 위해 usScanCount 전달 추가
  word = usScanCount;                 // Sysyem scan counter
  no = word_to_PDMwritePkt(word, no); // 10
} 
#endif

//***************************************
// 원격서버로부터 받은 데이터 해석하기
// From AnyBus DP ProfiBus
//***************************************
char RemoteStart;
char RemoteStart0;
char RemoteStop;
char RemoteStop0;
char RemoteClear;
char RemoteClear0;
char RemoteDir;
char RemoteDir0;
unsigned short usRemControlWord;

//
// Remote RUN/STOP를 EXTIN_REMOTE_RUN 신호로 대체
// 2008-11-26
// 2009-02-13 삭제
//
/*
char ExtRemoteRun0;
void extin_remote_operate(void)
{
  if ((!ExtRemoteRun0) & (ExtRemoteRun))
  {
    RemoteStart = 1; 
    RemoteStop  = 0;
  }
  else if ((ExtRemoteRun0) & (!ExtRemoteRun))
  {
    RemoteStart = 0; 
    RemoteStop  = 1;
  }
  else
  {
    RemoteStart = 0; 
    RemoteStop  = 0;
  }
  ExtRemoteRun0 = ExtRemoteRun;
}
*/

char RemoteLive, RemoteLive0, RemoteLiveError;

//
// 2026-10-08 추가: PLC 생존 확인 / 리모트 재기동 조건 (노션 5.3, CSLab_Rectifier_Main.h)
//   RemoteLiveToggle : 통신 정상(RemoteReady) + REMOTE 일 때 생존 비트(0x8000)가 바뀐 횟수 (remote_live_check)
//     어떤 시점 이후 2번 이상 바뀌었으면 실제 PLC 와 정상 통신 중으로 봄.
//     Anybus 재초기화 중 0 데이터는 생존 비트가 0 으로 고정 -> 많아야 1번 바뀜
//   RemArmWait : 1 = 정지 후 PLC 시작 비트 0 -> 1 재입력 대기 (이 동안 리모트 기동 안 함)
//
unsigned char RemoteLiveToggle;
#define PLC_LIVE_SINCE(snap)  ((unsigned char)(RemoteLiveToggle - (snap)) >= 2)
char RemArmWait;
#ifdef REMOTE_START_REARM
static char RemArmStep;                 // 0: 시작 비트 0 대기 / 1: 0 확인, 생존 비트 변화 대기
static unsigned char ucArmLive;
__no_init unsigned int RemArmSave;      // 리셋되어도 유지 : 재입력 대기 상태
#define REMARM_KEY  0x5AA55A00
extern unsigned int uiBootRsr;
#endif

// 정지 후 재입력 대기 시작 (PLC 정지 비트, 패널 모드 STOP 키, 고장 정지)
void remote_rearm_set(void)
{
#ifdef REMOTE_START_REARM
  RemArmWait = 1;
  RemArmStep = 0;
  RemArmSave = REMARM_KEY | 1;
#endif
}

// 부팅 때 (main.c reset_capture() 다음) : 전원 투입이면 대기 없음(자동 재기동),
// 그 밖의 리셋(워치독, 리셋 스위치 등)은 리셋 직전 대기 상태 유지 -> 조작자 정지가 리셋으로 풀리지 않음
void remote_rearm_boot(void)
{
#ifdef REMOTE_START_REARM
  unsigned int type;
  type = (uiBootRsr >> 8) & 0x07;
  if ((type != 0) & (type != 5) & ((RemArmSave & 0xFFFFFF00) == REMARM_KEY)) RemArmWait = RemArmSave & 1;
  else RemArmWait = 0;
  RemArmStep = 0;
  RemArmSave = REMARM_KEY | RemArmWait;
#endif
}

// PLC 데이터 수신 때 (remote_operate_decide) : 시작 비트 0 이 생존 비트 2번 바뀌는 동안 유지되면 대기 해제
#ifdef REMOTE_START_REARM
static void remote_rearm_check(void)
{
  if (!RemArmWait) return;
  if (RemoteStart | RemoteStop | RemoteLiveError) RemArmStep = 0;
  else if (RemArmStep == 0)
  {
    RemArmStep = 1;
    ucArmLive = RemoteLiveToggle;
  }
  else if (PLC_LIVE_SINCE(ucArmLive))
  {
    RemArmWait = 0;
    RemArmStep = 0;
    RemArmSave = REMARM_KEY;
  }
}
#endif

//
// 2026-10-08 추가: 운전 중 PLC 설정전류 0 보류 (REMOTE_SP0_HOLD)
//   return 1 : 이번 수신값은 적용하지 않음 (마지막 설정값 유지)
//
#ifdef REMOTE_SP0_HOLD
static char Sp0Hold, Sp0Lost;
static unsigned int uiSp0Scan;
static unsigned char ucSp0Live;
static char remote_sp0_hold(void)
{
  unsigned short newsp;
  float cursp;
#ifdef MONO_POLE
  newsp = iRemOperAmp;
  cursp = fOperAmp;
#else
  if (OperPole != MINUS) { newsp = iRemOperAmp; cursp = fOperAmp; }
  else { newsp = iRemRevOperAmp; cursp = fRevOperAmp; }
#endif
  if (newsp != 0)
  {
    if (Sp0Hold) dbg_printf("[SP0] cancel t=%u set=%u held=%ums\r\n", uiRstScan * 2, (unsigned int)newsp, (uiRstScan - uiSp0Scan) * 2);
    Sp0Hold = 0;
    return 0;
  }
  if (!Sp0Hold)
  {
    if ((SystemRun == OFF) | (cursp == 0)) return 0;     // 정지 중이거나 이미 0 : 그대로 적용
    Sp0Hold = 1;
    Sp0Lost = 0;
    uiSp0Scan = uiRstScan;
    ucSp0Live = RemoteLiveToggle;
    dbg_printf("[SP0] request t=%u last=%d hold=%ds\r\n", uiRstScan * 2, (int)cursp, REMOTE_SP0_HOLD_SEC);
    return 1;
  }
  if (SystemRun == OFF)                                  // 보류 중 정지됨 : 0 적용
  {
    Sp0Hold = 0;
    dbg_printf("[SP0] accept t=%u held=%ums by=stop\r\n", uiRstScan * 2, (uiRstScan - uiSp0Scan) * 2);
    return 0;
  }
  if ((RemoteLiveError) | (!RemoteReady))                // 생존 비트 끊김 : 계속 보류, 복구 후 다시 2번 확인
  {
    if (!Sp0Lost) dbg_printf("[SP0] hold t=%u live=lost keep=%d\r\n", uiRstScan * 2, (int)cursp);
    Sp0Lost = 1;
    ucSp0Live = RemoteLiveToggle;
    return 1;
  }
  if ((uiRstScan - uiSp0Scan) < (unsigned int)REMOTE_SP0_HOLD_SEC * SEC_1) return 1;
  if (!PLC_LIVE_SINCE(ucSp0Live)) return 1;
  Sp0Hold = 0;
  dbg_printf("[SP0] accept t=%u held=%ums by=plc\r\n", uiRstScan * 2, (uiRstScan - uiSp0Scan) * 2);
  return 0;
}
#endif

void remote_controlword_parsering(void)
{
  // 2009-02-13
  if (usRemControlWord & 0x0001) RemoteStart = 1; else RemoteStart = 0;
  if (usRemControlWord & 0x0002) RemoteStop  = 1; else RemoteStop  = 0;
  //extin_remote_operate();
  if (usRemControlWord & 0x0004) RemoteDir   = 1; else RemoteDir  = 0;
  if (usRemControlWord & 0x0008) LineEmegErr = 1; else LineEmegErr = 0;
  if (usRemControlWord & 0x0010) RemoteClear = 1; else RemoteClear = 0;
// 2009/03/26 4 word PLC 통신 추가
#ifdef PROFI_WORD_4
   if (usRemControlWord & 0x8000) RemoteLive = 1; else RemoteLive = 0;
#endif   

}

//
// Remote PLC 수신값을 설정치로 복사
//
void remote_setting_copy(void)
{
      // 2026-10-08 추가: 웜 리스타트 직후 PLC 설정전류 0 무시 (CSLab_Rectifier_reset.c)
      //   pio_init() 에서 Anybus 가 리셋되어 재초기화되는 동안 0 으로 클리어된 데이터가 올 수 있음.
      //   웜 복귀 후 WARM_SP_HOLD_SEC 이내에는 설정전류 0 을 받으면 리셋 직전 설정값을 유지,
      //   0 이 아닌 값을 한 번 받으면(PLC 데이터 정상) 보류를 끝내고 그 값부터 적용
#ifdef WARM_FIX_PLC_SP0
      if (WarmSpHoldScan)
      {
        if (iRemOperAmp == 0) return;
        WarmSpHoldScan = 0;
      }
#endif
#ifdef REMOTE_SP0_HOLD
      if (remote_sp0_hold()) return;    // 2026-10-08 추가: 운전 중 설정전류 0 보류
#endif
      fOperAmp = iRemOperAmp;           // Fwd Current
      fRevOperAmp = iRemRevOperAmp;     // Rev Current
      // 2008. 6.13 구수현장에서 포스코와 협의하여 삭제
      //if (OperPole == 0) fMaxOverAmp = iRemMaxOverAmp; 
      //else fMaxOverAmp = iRemRevOperAmp; // Fwd Current Limit
}  

//
// Remote 수신데이터로 운전설정값을 전환
//
char OperUser0;
void operate_by_remote_data(void)
{
  if (OperUser == REMOTE) remote_setting_copy();
}

//
// 운전설정값을 Local 데이터로 복구
//
char OperUser0;
void operate_by_local_data(void)
{
  if ((OperUser0 == REMOTE)&(OperUser == LOCAL)) backup_data_read();
  OperUser0 = OperUser;
}
/*
void remote_operate_decide(void)
{
  char run, stop, change, clear;
  run = 0;
  stop = 0;
  change = 0;
  clear = 0;
  
  if ((RemoteStart)&(!RemoteStart0)) run = 1;
  if ((RemoteStop)&(!RemoteStop0)) stop = 1;
  if (RemoteDir != RemoteDir0) change = 1;
  if ((RemoteClear)&(!RemoteClear0)) clear = 1;
 
  RemoteStart0 = RemoteStart;
  RemoteStop0 = RemoteStop;
  RemoteDir0 = RemoteDir;
  RemoteClear0 = RemoteClear;
  
  if ((RemoteReady)&(OperUser == REMOTE))
  //if (OperUser == REMOTE)
  {    
    if (run) 
      if (!ReadyStop) PushKey = 'R';
      else UnExecuteCMD = 1;
      
    if (stop) PushKey = 's'; // 2009/02/14 복원
    //if (stop) system_stop(); // 2009/02/14 
    if (clear) all_error_reset();
    if (change)
    {
      // 운전중에는 운전극성을 전환 할 수 없다
      if (SystemRun) UnExecuteCMD = 1;
      else RemotPole = RemoteDir;
    }
  }
  else    
  {
    if ((run)|(stop)|(change)) UnExecuteCMD = 1;
  }
}
*/
void remote_operate_decide(void)
{
  char run, stop, change, clear;
  run = 0;
  stop = 0;
  change = 0;
  clear = 0;
  
 // if ((RemoteStart)&(!RemoteStart0)) run = 1;  폴링에지
 // if ((RemoteStop)&(!RemoteStop0)) stop = 1;
  if (RemoteStart) run = 1;
  if (RemoteStop) stop = 1;
  if (RemoteDir != RemoteDir0) change = 1;
  if ((RemoteClear)&(!RemoteClear0)) clear = 1;
 
  RemoteStart0 = RemoteStart;
  RemoteStop0 = RemoteStop;
  RemoteDir0 = RemoteDir;
  RemoteClear0 = RemoteClear;
  
  if ((RemoteReady)&(OperUser == REMOTE))//if (OperUser == REMOTE)
  {    
#ifdef REMOTE_START_REARM
    // 2026-10-08 추가: 정지 후에는 시작 비트 0 -> 1 재입력이 있어야 기동 (레벨 기동 유지, 재입력 대기만 추가)
    remote_rearm_check();
    if ((run)&(!stop))
      if ((SystemRun == OFF)&(!ReadyStop)&(!RemArmWait)) PushKey = 'R';
#else
    if ((run)&(!stop)) 
      if ((SystemRun == OFF)&(!ReadyStop)) PushKey = 'R';
#endif
      //else UnExecuteCMD = 1;
      
    //if (stop) PushKey = 's'; 2008/12/24
#ifdef REMOTE_START_REARM
    if (stop) { system_stop(); remote_rearm_set(); }
#else
    if (stop) system_stop();
#endif
    if (clear) all_error_reset();
    if (change)
    {
      if (SystemRun) UnExecuteCMD = 1;
      else RemotPole = RemoteDir;
    }
  }
  //else    
  //{
  //  if ((run)|(stop)|(change)) UnExecuteCMD = 1;
  //}
}


UINT16 usSCIinBuf[16]; 
// 2009/03/26 4 word PLC 통신 추가
#ifdef PROFI_WORD_4
void remote_in_data_parsering(void)
{
  char lp, no;
  no = 3;
  for (lp = 0; lp < 4; lp++)
  {
    no = lp * 2 + 3;
    usSCIinBuf[lp] = (pModResponse[no]<< 8) + pModResponse[no+1];
  }

  usRemControlWord = ( pModResponse[3]<<8 ) + pModResponse[4];
  iRemOperAmp    =   ( pModResponse[5]<<8 ) + pModResponse[6];
  iRemRevOperAmp =   ( pModResponse[7]<<8 ) + pModResponse[8];
  sLineSpeed     =   ( pModResponse[13]<<8) + pModResponse[14];
}   

#else
void remote_in_data_parsering(void)
{
  char lp, no;
  no = 3;
  for (lp = 0; lp < 16; lp++)
  {
    no = lp * 2 + 3;
    usSCIinBuf[lp] = (pModResponse[no]<< 8) + pModResponse[no+1];
  }
  usRemControlWord = ( pModResponse[3]<<8 ) + pModResponse[4];
  iRemOperAmp    =   ( pModResponse[5]<<8 ) + pModResponse[6];
  iRemRevOperAmp =   ( pModResponse[7]<<8 ) + pModResponse[8];
  // 2008. 6.13 구수현장에서 포스코와 협의하여 삭제
  //iRemMaxOverAmp =   ( pModResponse[9]<<8 ) + pModResponse[10];
  //iRemRevOverAmp =   ( pModResponse[11]<<8) + pModResponse[12];
  sLineSpeed     =   ( pModResponse[13]<<8) + pModResponse[14];
}   
#endif

// 매 1초마다 실행되는 함수
short sRemoteCheckTime;
// Remote PLC가 정상운전 중인지를 체크
void remote_live_check(void)
{
  if (OperUser != REMOTE) RemoteLiveError = 0;  
  else if (ProfiFind != TRUE) RemoteLiveError = 0;  
  else if (RemoteLive0 != RemoteLive) 
  {
    RemoteLive0 = RemoteLive;
    sRemoteCheckTime = 0;
    RemoteLiveError = 0;
    if (RemoteReady) RemoteLiveToggle++;    // 2026-10-08 추가: 생존 비트 변화 횟수
  }
  else
  {
    if(++sRemoteCheckTime > SEC_1*3) RemoteLiveError = 1;
  }
}

//
// 2026-10-08 추가: PLC 명령 상태 UART 보고 [PLCCMD] (CSLab_Rectifier_Main.h DBG_PLCCMD)
// 메인루프 매 스캔 호출 (main.c). 직전에 보낸 상태와 다를 때만 한 줄 출력
//   - 제어워드의 생존 비트(0x8000, PLC 가 계속 바꿈)는 비교에서 제외, 생존 여부는 live=ok/lost 로
//   - PLCCMD_MIN_GAP_MS 안에 또 바뀌면 간격이 지난 뒤 그때의 최신 상태를 보냄
//   - 부팅 후 1초(CSV 헤더 이후)부터 보냄. plc_cmd_force() 를 부르면 다음 스캔에 다시 보냄
//   - t 는 CSV t_ms 와 같은 기준 (메인루프 시작 후 스캔 수 x 2ms)
//
#ifdef DBG_PLCCMD
static unsigned short usPlcWord0, usPlcSet0, usPlcRev0;
static char PlcCom0, PlcUser0, PlcLive0, PlcArm0;
static char PlcSent;                    // 0: 아직 안 보냄 / 다시 보내기 요청
static unsigned int uiPlcLastScan;

void plc_cmd_force(void)
{
  PlcSent = 0;
}

void plc_cmd_report(void)
{
  unsigned short word;

  if (uiRstScan < SEC_1) return;
  word = usRemControlWord & 0x7FFF;
  if (PlcSent)
  {
    if ((word == usPlcWord0) & (iRemOperAmp == usPlcSet0) & (iRemRevOperAmp == usPlcRev0)
        & (RemoteReady == PlcCom0) & (OperUser == PlcUser0) & (RemoteLiveError == PlcLive0) & (RemArmWait == PlcArm0)) return;
    if ((uiRstScan - uiPlcLastScan) < (unsigned int)(PLCCMD_MIN_GAP_MS / 2)) return;
  }
  if (!dbg_printf("[PLCCMD] t=%u com=%d user=%s start=%d stop=%d dir=%d emeg=%d clear=%d live=%s arm=%s set=%u rev=%u word=0x%04X\r\n",
                  uiRstScan * 2, RemoteReady ? 1 : 0, (OperUser == REMOTE) ? "REMOTE" : "LOCAL",
                  (word & 0x0001) ? 1 : 0, (word & 0x0002) ? 1 : 0, (word & 0x0004) ? 1 : 0,
                  (word & 0x0008) ? 1 : 0, (word & 0x0010) ? 1 : 0, RemoteLiveError ? "lost" : "ok", RemArmWait ? "wait" : "ok",
                  (unsigned int)iRemOperAmp, (unsigned int)iRemRevOperAmp, (unsigned int)usRemControlWord)) return;  // 송신 버퍼 부족 -> 다음 스캔에 다시
  usPlcWord0 = word;
  usPlcSet0 = iRemOperAmp;
  usPlcRev0 = iRemRevOperAmp;
  PlcCom0 = RemoteReady;
  PlcUser0 = OperUser;
  PlcLive0 = RemoteLiveError;
  PlcArm0 = RemArmWait;
  PlcSent = 1;
  uiPlcLastScan = uiRstScan;
}
#endif

char DemoDelay;
void remote_in_data_parsering_demo(void)
{
  switch(DemoStep)
  {
  case 0: return;
  case 1:
    usRemControlWord = 0;
    iRemOperAmp = 5000;
    iRemRevOperAmp = 3000;
    //iRemMaxOverAmp = 10000;
    //iRemRevOverAmp = 5000;
    sLineSpeed = 0;
    DemoDelay = 0;
    DemoStep++;
    return;
    
  case 2: if (++DemoDelay > 100) DemoStep++; return;
  case 3: usRemControlWord = 1; DemoStep++; return; // start
  case 4: 
    usRemControlWord = 0;
    DemoDelay = 0; 
    DemoStep++;
    return;
    
  case 5:
    sLineSpeed++;
    iRemOperAmp++;
    if (++DemoDelay > 100) DemoStep++;
    return;
    
  case 6: usRemControlWord = 2; DemoStep++; return; // stop
  
  case 7:
    usRemControlWord = 4;
    iRemOperAmp = 5000;
    iRemRevOperAmp = 3000;
    //iRemMaxOverAmp = 10000;
    //iRemRevOverAmp = 5000;
    sLineSpeed = 0;
    DemoDelay = 0;
    DemoStep++;
    return;
    
  case 8: if (++DemoDelay > 100) DemoStep++; return;
  case 9: usRemControlWord = 5; DemoStep++; return; // start
  case 10: 
    usRemControlWord = 0;
    DemoDelay = 0; 
    DemoStep++;
    return;
    
  case 11:
    sLineSpeed++;
    iRemRevOperAmp++;
    if (++DemoDelay > 100) DemoStep++;
    return;
    
  case 12: usRemControlWord = 0x0C; DemoStep++; return; // stop 
  case 13: 
    usRemControlWord = 0;
    DemoDelay = 0; 
    DemoStep++;
    return;
  case 14: if (++DemoDelay > 50) DemoStep++; return;
  case 15: DemoStep = 0; return;
  default: DemoStep = 0; return;
  }
}  

#define	PROFI_CON_MAIN	        10
#define	PROFI_CON_EXE	        100
#define	PROFI_SPEED_CHANGE	200
#define	PROFI_DEVICE_SEARCH     300
#define	PROFI_REMOTE_RESTART	400
#define	PROFI_CON_MONIT	        500
#define	PROFI_OUT_MONIT         510
#define	PROFI_OUTPUT_VIEW 	600
#define	PROFI_CON_SEND1	        700
#define	PROFI_REG_READ	        800
#define	PROFI_INPUT_VIEW	900
#define	PROFI_OUT_WRITE         1000
#define	PROFI_REMOTE_DEMO       1100
#define	PROFI_REG_WRITE	        2000
#define	PROFI_CON_END           2100
#define	PROFI_SAVE_END		2200
/*********************************************/
/* ProfiBus Interface Device Test Functions  */
/* Device: AnyBus PDP                        */
/* Maker:  HMS Networks co. ltd              */
/*********************************************/
char SampleNo;
unsigned short usSample;
void menu_display_profi(char no)
{  
  if (no == 0)       printf("<ProfiDevice Config>");
  else if (no == 1)  
  {
    if (ProfiFind == TRUE) printf("\nDP[%d] Find:%6d", ProfiNodeAdd, iCom0Speed);
    else printf("\nProfiDP Search:FAIL");
  }
  else if (no == 2)  printf("\nBaudrate Change");
  else if (no == 3)  printf("\nProfiDP Re-start");
  else if (no == 4)  printf("\nData View From Remot");
  else if (no == 5)  printf("\nData View To Remote");
  else if (no == 6)  printf("\nData Write To Remote");
  else if (no == 7)  printf("\npDP Register View");
  else if (no == 8)  printf("\npDP Register Set");
  else if (no == 9) printf("\nRemote Demo Start");
  else if (no == 10)  printf("\nSend Test Packet");
  else if (no == 11) printf("\nReturn To Main    ");
  //else if (no == 5)  printf("\nReceive Monit");
}

void ModBusStep_monit(void)
{
  short pos;
  pos = sCurPos;
  goto_cursor(15,7);
  printf("[%2d]", ModBusStep);
  sCurPos = pos;
}

UINT16  ProfiDataH, ProfiDataL; 
void profi_set_function(void)
{
  char lp, result, data, line;
  UINT16  usdata;
  
 //ModBusStep_monit(); 
  switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    CursorUse = 1;
    MenuStart = 0;
    MenuEnd = 7;
    MenuSize = 12;
    sExecStep = PROFI_CON_MAIN;
    return;
    
  case PROFI_CON_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    debug_monit(MONOUT);
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_profi(lp);
    cursor_move_home();
    sExecStep++;
    return;
  
  case PROFI_CON_MAIN+1:
    if (MENU_UP) popup_menu_update(PROFI_SET,UP); 
    else if (MENU_DN) popup_menu_update(PROFI_SET,DOWN); 
    else if (ENTER_KEY) sExecStep = PROFI_CON_EXE; 
    else if (step_delay(SEC_1*60)) execmode_change(RUN_STATUS);    
    printf("\r");
    return;

  case PROFI_CON_MAIN+2:
    sExecStep--;
    return;

  case PROFI_CON_EXE:
    DelayStep = 0;
    LineBlink = 0;
    MenuNo = MenuStart + find_cursor_vpos();
    if (MenuNo == 0) execmode_change(SYSTEM_TEST);
    else if (MenuNo == 1) sExecStep = PROFI_DEVICE_SEARCH;
    else if (MenuNo == 2) sExecStep = PROFI_SPEED_CHANGE;
    else if (MenuNo == 3) sExecStep = PROFI_REMOTE_RESTART;
    else if (MenuNo == 4) sExecStep = PROFI_INPUT_VIEW;
    else if (MenuNo == 5) sExecStep = PROFI_OUT_MONIT;
    else if (MenuNo == 6) sExecStep = PROFI_OUT_WRITE;
    else if (MenuNo == 7) sExecStep = PROFI_REG_READ;
    else if (MenuNo == 8) sExecStep = PROFI_REG_WRITE;
    else if (MenuNo == 9) sExecStep = PROFI_REMOTE_DEMO;
    else if (MenuNo == 10) sExecStep = PROFI_CON_SEND1;
    else if (MenuNo == 11) execmode_change(SYSTEM_TEST);
    //else if (MenuNo == 5) sExecStep = PROFI_OUTPUT_VIEW;
    else sExecStep = PROFI_CON_MAIN; 
    return;

//*****************************************
// Profi Device Search
//*****************************************
 case PROFI_DEVICE_SEARCH:
    screen_clear();
    printf(">Profi Device Search" );
    printf("\nCancel: Push Wheel");
    printf("\nTrying" ); 
    ProfiFind = FALSE;
    iTempSet = iCom0Speed;
    RemoteStep = 0;
    ProfiSearch = 0;
    DelayStep = 0;
    sExecStep++;
    return;

  case PROFI_DEVICE_SEARCH+1:
    profi_reset(ON);
    ProfiRetry = 0;
    sExecStep++;
    return;
 
  case PROFI_DEVICE_SEARCH+2: step_delay(10); return;

  case PROFI_DEVICE_SEARCH+3:
    profi_reset(OFF);
    sExecStep++;
    return;
 
  case PROFI_DEVICE_SEARCH+4: step_delay(SEC_1); return;

  case PROFI_DEVICE_SEARCH+5:
    if (ModBusStep == 0)
    {
      profi_parameter_multi_read_query(RDWR_PD_MODE, PROFI_CON_CALL);
      sExecStep++;
    }
    return;    

  case PROFI_DEVICE_SEARCH+6:
    if (ENTER_KEY) 
    {
      printf("\n Search Cencel..");
      sExecStep = PROFI_CON_END;
     }
    return;    

  case PROFI_DEVICE_SEARCH+7:
    if ((ModBusErr == 0)&(ModBusRxLength == 5))
    {
      printf("\nProfi Device Found");
      printf("\nBaudRate: %d", iCom0Speed);
      //printf("\nRxLength: %d", ModBusRxLength);
      ProfiFind = TRUE;     
      sExecStep = PROFI_DEVICE_SEARCH+9;
    }
    else 
    {
      printf(".%d", ModBusErr);
      if (++ProfiRetry > MB_QUERY_RETRY) sExecStep++;
       else sExecStep = PROFI_DEVICE_SEARCH+5;
    }
    return;    
 
  case PROFI_DEVICE_SEARCH+8:
    if (++ProfiSearch < PROFI_SEARCH_RETRY) 
    {
      printf("\nRetry-%d", ProfiSearch); 
      sExecStep = PROFI_DEVICE_SEARCH+1;
    }
    else
    {
      printf("\nDevice Search Fail!!");
      ProfiFind = FALSE;
      sExecStep = PROFI_CON_END;
    }
    return;

 case PROFI_DEVICE_SEARCH+9:
    if (iTempSet != iCom0Speed ) sExecStep = PROFI_SAVE_END;
    else sExecStep = PROFI_CON_END;
    return;    

//*****************************************
// Profi Device baudrate change and detect
//*****************************************
  case PROFI_SPEED_CHANGE:
    screen_clear();
      printf("< Baudrate Change >" );
    printf("\n< Profi DP Search >" );
    printf("\nCancel: Push Wheel");
    printf("\nTrying" ); 
    ProfiFind = FALSE;
    iTempSet = iCom0Speed;
    RemoteStep = 0;
    ProfiSearch = 0;
    DelayStep = 0;
    sExecStep++;
    return;
    
  case PROFI_SPEED_CHANGE+1:
    profi_reset(ON);
    profiport_speed_change();
    ProfiRetry = 0;
    sExecStep++;
    return;
 
  case PROFI_SPEED_CHANGE+2: step_delay(10); return;

  case PROFI_SPEED_CHANGE+3:
    profi_reset(OFF);
    sExecStep++;
    return;
 
  case PROFI_SPEED_CHANGE+4: step_delay(SEC_1); return;

  case PROFI_SPEED_CHANGE+5:
    if (ModBusStep == 0)
    {
      profi_parameter_multi_read_query(RDWR_PD_MODE, PROFI_CON_CALL);
      sExecStep++;
    }
    return;    

  case PROFI_SPEED_CHANGE+6:
    if (ENTER_KEY) 
    {
      printf("\n Search Cencel..");
      sExecStep = PROFI_CON_END;
     }
    return;    

  case PROFI_SPEED_CHANGE+7:
    //if (ModBusErr == 0) 
    if ((ModBusErr == 0)&(ModBusRxLength == 5))
    {
      printf("\nProfi Device Found");
      printf("\nBaudRate: %d", iCom0Speed);
      //printf("\nRxLength: %d", ModBusRxLength);
      ProfiFind = TRUE;     
      sExecStep = PROFI_SPEED_CHANGE+9;
    }
    else 
    {
      printf(".%d", ModBusErr);
      if (++ProfiRetry > MB_QUERY_RETRY) sExecStep++;
        else sExecStep = PROFI_SPEED_CHANGE+5;
    }
    return;    
 
  case PROFI_SPEED_CHANGE+8:
    if (++ProfiSearch < PROFI_SEARCH_RETRY) 
    {
      printf("\nRetry-%d", ProfiSearch); 
      sExecStep = PROFI_SPEED_CHANGE+1;
    }
    else
    {
      printf("\nDevice Search Fail!!");
      ProfiFind = FALSE;
      sExecStep = PROFI_CON_END;
    }
    return;

  case PROFI_SPEED_CHANGE+9:
    if (iTempSet != iCom0Speed ) sExecStep = PROFI_SAVE_END;
    else sExecStep = PROFI_CON_END;
    return;    
    
  case PROFI_OUTPUT_VIEW:
    screen_clear();
    printf("<Remote Rx Monit>");
    rs485_direction(READ);
    sExecStep++;
    return;

  case PROFI_OUTPUT_VIEW+1:
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    return;
    
//*****************************************
// Remote Operation Restart
//*****************************************
 case PROFI_REMOTE_RESTART:
    screen_clear();
    printf("<ProfiDP Re-start>");
    RemoteStep = 1;
    ProfiDebug = 1;
    DelayStep = 0;
    sExecStep++;
    return;

  case PROFI_REMOTE_RESTART+1:
    if (ENTER_KEY) sExecStep++;
    else if (step_delay(SEC_1*30)) sExecStep++;
    return;    

  case PROFI_REMOTE_RESTART+2:
    ProfiDebug = 0;
    sExecStep = PROFI_CON_END;
    return;    

//*****************************************
// Profi Remote Control Demo start
//*****************************************
 
  case PROFI_REMOTE_DEMO:
    DemoStep = 1;
    execmode_change(RUN_STATUS);
    return;   
    
//*****************************************
// Profi Device Sample Packet Exanchange 1
//*****************************************
 
  case PROFI_CON_MONIT:
    screen_clear();
    printf("<PROFI_CON_MONIT>");
    printf("\n>Under constraction..");
    DelayStep = 0;
    sExecStep = PROFI_CON_END;
    return;
    
//*****************************************
// Profi Device Sample Packet Exanchange 1
//*****************************************
  case PROFI_CON_SEND1:
    screen_clear();
    
    printf("<Send Test Packet>\n");
    DelayStep = 0;
    sExecStep++;
    return;    

  case PROFI_CON_SEND1+1:
    if (ModBusStep == 0)
    {
      profi_parameter_multi_read_query(READ_PD_STATUS, PROFI_CON_CALL);
      ProfiDebug = 1;
      sExecStep++;
    }
    return;    

  case PROFI_CON_SEND1+2:
   return;    

 case PROFI_CON_SEND1+3:
   if (ModBusErr != 0) print_ModBusErr(ModBusErr);
   else printf("\nErr:0 RxLength:%d", ModBusRxLength);
   ProfiDebug = 0;
   ProfiDebug = 0;
   sExecStep++;
   return;    

  case PROFI_CON_SEND1+4:
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    else if (step_delay(SEC_1*30)) sExecStep = PROFI_CON_MAIN;
    return;    

//*****************************************
// Profi Device Single Register Read
//***************************************** 
   case PROFI_REG_READ:
    screen_clear();
    printf("<pDP Register View>");
    if (ProfiRegNo == 0) ProfiRegNo = 1;
    iTempSet = ProfiRegNo;
    printf("\nReg[%2d:", iTempSet);
    printf("%04X]:", MBaddForProfiFunc[iTempSet]);
    DelayStep = 0;
    sExecStep++;
    return;    

  case PROFI_REG_READ+1:
    if (wheel_input_speed(1, 115, 1)) 
    {
       ProfiRegNo = iTempSet;
       usModBusFunc = MBaddForProfiFunc[ProfiRegNo];
       sExecStep = PROFI_CON_MAIN;
    }
    return;    
 
  case PROFI_REG_READ+2:
    while((MBaddForProfiFunc[iTempSet] == 0)|
          (MBaddForProfiFunc[iTempSet] == 0xFFFF)) 
      if (iTempSet > ProfiRegNo) iTempSet++;
      else if (iTempSet < ProfiRegNo) iTempSet--;

    if (iTempSet == ProfiRegNo) printf("\r"); else printf("\n");
    ProfiRegNo = iTempSet;
    sExecStep++;
    return;  
 
  case PROFI_REG_READ+3:
    if (ModBusStep == 0)
    {
      profi_parameter_single_read_query(ProfiRegNo, PROFI_CON_CALL);
      //ProfiDebug = 1;
      sExecStep++;
    }
    return;    

  case PROFI_REG_READ+4:
   return;    

  case PROFI_REG_READ+5:  
    ProfiDebug = 0;
    printf("Reg[%2d:", ProfiRegNo);
    printf("%04X]:", MBaddForProfiFunc[iTempSet]);
    if (ModBusErr != 0) print_ModBusErr(ModBusErr);
    else 
    {      
      usdata = (pModResponse[3]<<8) + pModResponse[4];
      printf("%04XH", usdata);
    }
    sExecStep = PROFI_REG_READ+1;
    return;    
   
 case PROFI_REG_READ+8:
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    else if (step_delay(SEC_1*30)) sExecStep = PROFI_CON_MAIN;
    return;    
    
//*****************************************
// Profi Device Multi Register Read
//***************************************** 
   case PROFI_INPUT_VIEW:
    screen_clear();
      printf("<  Data View >");
    printf("\n< From Remote >");
    iTempSet = FBinOffset;
    printf("\nOffset:%2d", iTempSet);
    DelayStep = 0;
    sExecStep = PROFI_INPUT_VIEW+7;
    //sExecStep++;
    return;    

  case PROFI_INPUT_VIEW+1:
    if (wheel_input_speed(0, 99, 1)) 
    {
       FBinOffset = iTempSet;
       sExecStep = PROFI_INPUT_VIEW+3;
    }
    return;    
 
  case PROFI_INPUT_VIEW+2:
    printf("\rOffset:%2d", iTempSet);
    sExecStep--;
    return;  
 
  case PROFI_INPUT_VIEW+3:
    //goto_cursor(0, 2);
    sExecStep++;
    return;  
 
  case PROFI_INPUT_VIEW+4:
    if (ModBusStep == 0)
    {
      profi_SCIout_read_query(FBinOffset, PROFI_INPUT_SIZE, PROFI_CON_CALL);
      //ProfiDebug = 1;
      sExecStep++;
    }
    return;    

  case PROFI_INPUT_VIEW+5: return;    

  case PROFI_INPUT_VIEW+6:
   ProfiDebug = 0;
   if (ModBusErr != 0) print_ModBusErr(ModBusErr);
   else 
   {
     printf("Err:0 Rx:%d", ModBusRxLength);
     printf("\nOffset:%d", FBinOffset);
     data = 0;
     line = 0;
     for( lp = 3; lp < ModBusRxLength ; lp++ ) 
     {
       if (line == 0) printf("\n");
       if (++line == 8) line = 0;
       printf("%02X", abReceiveBuf[lp]);
       if (++data == 2) { data = 0; printf(" "); };
     }
   }
   sExecStep++;
   return;    

  case PROFI_INPUT_VIEW+7:
    //screen_clear();
    goto_cursor(0,2);
    line = 0;
    for (lp = 0; lp < PROFI_INPUT_SIZE/2; lp++) 
    {
      printf("%04X ", usSCIinBuf[lp]);
      if (++line == 4) line = 0;
      if (line == 0) printf("\n");
    }
    sExecStep++;
    return; 
    
  case PROFI_INPUT_VIEW+8:
   sExecStep++;
   DelayStep = 0;
   return;   
   
 case PROFI_INPUT_VIEW+9:
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    else if (ANY_KEY) sExecStep = PROFI_INPUT_VIEW+7;
    else if (step_delay(50)) sExecStep = PROFI_INPUT_VIEW+7;//PROFI_CON_MAIN;
    return; 
    
//*****************************************
// Test Data Write to Remote
//***************************************** 
   case PROFI_OUT_WRITE:
    screen_clear();
      printf("< Test Data Out >");
    printf("\n<   To Remote   >");
    iTempSet = FBoutOffset;
    ProfiTestOut = 1;
    DelayStep = 0;
    sExecStep++;
    return;    

  case PROFI_OUT_WRITE+1:
    iTempSet = 0;
    usSample = 0;
    sExecStep++;
    return;    
 
  case PROFI_OUT_WRITE+2:
    sExecStep++;
    return;  
 
  case PROFI_OUT_WRITE+3:
    // Test data make
    //remote_out_data_generate();
    PDMwritePkt.Data[0] = (iTempSet >> 24)& 0xFF;
    PDMwritePkt.Data[1] = (iTempSet >> 16)& 0xFF;
    PDMwritePkt.Data[2] = (iTempSet >> 8)& 0xFF;
    PDMwritePkt.Data[3] = iTempSet & 0xFF;
    SampleNo = 4;;
    for( lp = 4; lp < PROFI_INPUT_SIZE/2 + 2 ; lp++ )
    {
      usSample += (lp-4) * 10;
      PDMwritePkt.Data[SampleNo++] = usSample >> 8;
      PDMwritePkt.Data[SampleNo++] = usSample & 0xFF;
    }
      //PDMwritePkt.Data[lp] = lp+FBoutOffset;
    sExecStep++;
   return;    
 
  case PROFI_OUT_WRITE+4:
    // Data Out to ProfiBus Query
    if (ModBusStep == 0)
    {
      ProfiDebug = 1;
      profi_SCIin_write_query(FBoutOffset, PROFI_OUTPUT_SIZE, PROFI_CON_CALL);
      sExecStep++;
    }
    return;    

  case PROFI_OUT_WRITE+5: return; 

  case PROFI_OUT_WRITE+6:
    ProfiDebug = 0;
   if (ModBusErr != 0) print_ModBusErr(ModBusErr);
   else printf("\nErr:0 RxLength:%d", ModBusRxLength);
   sExecStep++;
   return;    
 
  case PROFI_OUT_WRITE+7:
   printf("\nOffset:%d\n", FBoutOffset);
   data = 0;
   line = 0;
   for( lp = 0; lp < PROFI_OUTPUT_SIZE ; lp++ ) 
   {
     printf("%02X", PDMwritePkt.Data[lp]);
     if (++data == 2) { data = 0; printf(" "); };
     if (++line == 8) { line = 0; printf("\n"); };
   }       
   sExecStep++;
   return; 
   
  case PROFI_OUT_WRITE+8:
    if (ENTER_KEY) sExecStep = 9;
    else if (ANY_KEY)
    {
      iTempSet = iTempSet * 2;
      if ( iTempSet == 0) iTempSet = 1;
      sExecStep = PROFI_OUT_WRITE+2;
    }
    return;   
   
 case PROFI_OUT_WRITE+9:
    ProfiTestOut = 0;
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    else if (step_delay(SEC_1*30)) sExecStep = PROFI_CON_MAIN;
    return;    

//*****************************************
// Profi Device Out Data Monit
//***************************************** 
   case PROFI_OUT_MONIT:
    screen_clear();
      printf("< Remote Out >");
    printf("\n< Data Monit >");
    DelayStep = 0;
    sExecStep++;
    return;    

  case PROFI_OUT_MONIT+1:
    //remote_out_data_generate();
    sExecStep++;
    return;    
 
  case PROFI_OUT_MONIT+2:
    // Remote Transmit Data display
    goto_cursor(0,2);
    line = 0;
    for (lp = 0; lp < PROFI_INPUT_SIZE/2; lp++) 
    {
      data = lp * 2;
      usSample = (PDMwritePkt.Data[data] << 8) + PDMwritePkt.Data[data+1];      
      printf("%04X ", usSample);
      if (++line == 4) line = 0;
      if (line == 0) printf("\n");
    }
    printf("\nRemote Step:%d", RemoteStep); // 2009-01-30
    sExecStep++;
   return;    
   
 case PROFI_OUT_MONIT+3:
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    else if (ANY_KEY) sExecStep = PROFI_OUT_MONIT;
    else if (step_delay(50)) sExecStep = PROFI_OUT_MONIT+1;
    return;    
        
//*****************************************
// Profi Device Single Register Write
//***************************************** 
  case PROFI_REG_WRITE:
    screen_clear();
    printf("<pDP Register Set>");
    if (ProfiRegNo == 0) ProfiRegNo = 1;
    iTempSet = ProfiRegNo;
    ProfiRetry = 0;
    printf("\nRegister:%2d", iTempSet);
    printf(" MOD:%04X", MBaddForProfiFunc[iTempSet]);
    DelayStep = 0;
    sExecStep++;
    return;    

  case PROFI_REG_WRITE+1:
    if (wheel_input_speed(1, 70, 1)) 
    {
       ProfiRegNo = iTempSet;
       usModBusFunc = MBaddForProfiFunc[ProfiRegNo];
       sExecStep = PROFI_REG_WRITE+3;
    }
    return;    
 
  case PROFI_REG_WRITE+2:
    while((MBrwForProfiFunc[iTempSet] == 0)) 
            if (iTempSet > ProfiRegNo) iTempSet++;
            else if (iTempSet < ProfiRegNo) iTempSet--;
    
    ProfiRegNo = iTempSet;
    printf("\rRegister:%2d", iTempSet);
    printf(" MOD:%04X", MBaddForProfiFunc[iTempSet]);
    sExecStep--;
    return;  
 
  case PROFI_REG_WRITE+3:
    if ((MBrwForProfiFunc[iTempSet] == 0))
    {
      printf("\nReadOnly or Not Used");
      sExecStep = PROFI_CON_END;
    }
    else sExecStep++;
    return;  
 
  case PROFI_REG_WRITE+4:
    iTempSet = 0;
    printf("\nData_H:%02XH", iTempSet);
    sExecStep++;
    return;    

  case PROFI_REG_WRITE+5:
    if (wheel_input_speed(0, 255, 10)) 
    {
       ProfiDataH = iTempSet;
       sExecStep = PROFI_REG_WRITE+7;
    }
    return;    
 
  case PROFI_REG_WRITE+6:
    printf("\rData_H:%02XH", iTempSet);
    sExecStep--;
    return;  
 
  case PROFI_REG_WRITE+7:
    iTempSet = ProfiDataL;
    printf("\nData_L:%02XH", iTempSet);
    sExecStep++;
    return;    

  case PROFI_REG_WRITE+8:
    if (wheel_input_speed(0, 255, 10)) 
    {
       ProfiDataL = iTempSet;
       sExecStep = PROFI_REG_WRITE+11;
    }
    return;    
 
  case PROFI_REG_WRITE+9:
    printf("\rData_L:%02XH", iTempSet);
    sExecStep--;
    return;  

  case PROFI_REG_WRITE+11:
    if (ModBusStep == 0)
    {
      usdata = (ProfiDataH << 8) + ProfiDataL; 
      profi_single_write_query(ProfiRegNo, usdata, PROFI_CON_CALL);
      ProfiDebug = 1;
      sExecStep++;
    }
    return;    

  case PROFI_REG_WRITE+12:
    return;    

  case PROFI_REG_WRITE+13:
    ProfiDebug = 0;
    if (ModBusErr != 0) 
    {
      print_ModBusErr(ModBusErr);
      if ((ModBusErr == MB_TIMEOUT_ERR)
         |(ModBusErr == MB_CRC_ERR)
         |(ModBusErr == MB_RX_OVER))
      {
        if (++ProfiRetry > 3) sExecStep++;
        sExecStep = PROFI_REG_WRITE+11; 
      }
      else sExecStep++;
    }
    else 
    {
      //printf("\nErr:0 RxLength:%d", ModBusRxLength);
      usdata = (pModResponse[2]<<8) + pModResponse[3];
      printf("\nRegister[%04X]:", usdata);
      usdata = (pModResponse[4]<<8) + pModResponse[5];
      printf("%04XH", usdata);
      sExecStep++;
    }   
    return;    

  case PROFI_REG_WRITE+14:
    DelayStep = 0;
    sExecStep++;
    return;   

  case PROFI_REG_WRITE+15:
    if (ENTER_KEY) sExecStep = PROFI_CON_MAIN;
    else if (step_delay(SEC_1*30)) sExecStep = PROFI_CON_MAIN;
    return;    
    
//*************************************************
//   변경된 데이터를 저장하고 
//   3초간 지연 또는 키입력으로 메인메뉴로 복귀
//*************************************************
  case PROFI_SAVE_END:
    result = backup_data_save();
    if (result == true) printf("\nAdjust Data Save OK");
    else 
    {
      printf("\nBackup Memory Error!!");
      printf("\nData Save Fail..");
    }
    DelayStep = 0;
    sExecStep++;
    return;
    
  case PROFI_SAVE_END+1:
    if (ANY_KEY) sExecStep = PROFI_CON_MAIN;
    else if (step_delay(SEC_1*3)) sExecStep = PROFI_CON_MAIN;
    return;    
//
//   1Sec 지연후 메인메뉴로 복귀
//
  case PROFI_CON_END:
    DelayStep = 0;
    sExecStep = PROFI_SAVE_END+1;
    return;
    
  default:
    sExecStep = PROFI_CON_MAIN;
    return;
 };
}

//************************************************
// modbus_message_exchange()
// MODBUS protocol 로 Query packet을 보내고 
// Response packet를 수신.
// 성공하면 TRUE, 실패하면 FALSE를 리턴한다.
// 호출함수의 Step를 1 증가시켜 다음스텝으로 진행시킨다.
// 에러원인은 ModbusErr에 기록된다.
//************************************************
#define MODBUS_CHECK      10
#define MODBUS_END        20
char ModPacketSize;
char ProfiWaitTime;
//
// modbus_message_exchange()를 동작시키기 위한 변수 설정
//
void ProfiD_packet_exchange( UINT8 *query, UINT8 size, UINT8* response, short call )
{
  PtrModQuery = query;
  ModPacketSize = size;
  pModResponse = response;
  ModBusCall = call; 
  ModBusStep = 1;
}

void modbus_message_exchange( void )
{
  char lp;
  switch(ModBusStep)
  {
  case 0: return;
  case 1:
    profiport_buffer_clear();
    
    if( ModPacketSize > 249 )
    {
      ModBusErr = MB_TX_OVER;
      ModBusResult = FALSE;
      ModBusStep = MODBUS_END;
    }
    else 
    {
   // ModBus Rx buffer clear
      for( lp = 0; lp < MODBUS_BUF_SIZE ; lp++ ) pModResponse[lp] = 0;
      ModBusStep++;
     }     
    return;
    
  case 2:   
      if (ProfiDebug) printf("\nTx:");

   // Copy Data To send buffer    
      for( bCharPos = 0 ; bCharPos < ModPacketSize ; bCharPos++ )
          abSendBuffer[ bCharPos ] = *PtrModQuery++;

   // Generate CRC for the message to send
      iCrc = GenerateCrc( abSendBuffer, bCharPos );

   // Add CRC to the end of the message to send
      abSendBuffer[ bCharPos ] = ( iCrc >> 8 ) & 0x00FF;
      abSendBuffer[ bCharPos + 1] = iCrc & 0x00FF;

   // Send Modbus Request
      for( bCharPos = 0; bCharPos < ( ModPacketSize + 2 ) ; bCharPos++ )
          putchar_mb( abSendBuffer[ bCharPos ] );
   
   ModBusStep++;
   return;
   
  case 3:
    sModBusWait = 0;
    bCharPos = 0;
    if (ProfiDebug) printf("\nRx:");
    ModBusStep++;
    return;
    
  //Read response
  case 4:
    for (lp = 0; lp < 20; lp++)
    {
      if (MB_CharReceived())
      {
        pModResponse[ bCharPos++ ] = getchar_mb();
        if (bCharPos >= ResponseSize) ModBusStep = MODBUS_CHECK;
        else if (bCharPos >= MODBUS_BUF_SIZE) ModBusStep = MODBUS_CHECK;
        else if ((bCharPos >= 5)&((pModResponse[1] & 0x80) != 0))
        ModBusStep = MODBUS_CHECK;
      }
      else lp = 20;
    }
    
    if (++sModBusWait > ProfiWaitTime) ModBusStep = MODBUS_CHECK;
    return;
    
  case MODBUS_CHECK:
    ModBusResult = FALSE;
    if (bCharPos == 0)
    {
      // Receive Wait time over
      ModBusErr = MB_TIMEOUT_ERR;
    }
    else if (bCharPos >=  MODBUS_BUF_SIZE) 
    {
      // Receive Buffer overflow
      ModBusErr = MB_RX_OVER;
    }
    else 
    {
      ModBusRxLength = bCharPos - 2;
      // Generate CRC for the response message
      iCrc = GenerateCrc( pModResponse, bCharPos - 2 );
      // Check CRC
      if( ( ( (UINT8)( iCrc >> 8 ) ) == pModResponse[ bCharPos - 2 ] ) &
          ( ( (UINT8)iCrc ) == pModResponse[ bCharPos - 1 ] ) )   
      {
        if ((pModResponse[1]&0x80) != 0) 
        {
          // ModBus Exception error
          ModBusErr = pModResponse[2];
        }
        else
        {
          // Correct CRC return received length
          ModBusErr = MB_NO_ERR;
          ModBusResult = TRUE;
        }
      }
      else
      {
        // CRC error
        ModBusErr = MB_CRC_ERR;   
      }
   }/* end if response received */ 
   
   ModBusStep = MODBUS_END;
   return;
   
  case MODBUS_END:
    if (ModBusCall == PROFI_CON_CALL) sExecStep++;
    //else if (ModBusCall == REMOTE_CALL) RemoteStep++;
    else RemoteStep++;
    ModBusCall = 0;
    ModBusStep = 0;
    return;
    
  default: //ModBusStep = 0; 
  return;
  }
}



//
// Profi ModBus communacation error display
//
void print_ModBusErr(char err)
{
   if (ModBusErr == MB_TIMEOUT_ERR)       printf("\n>!Rx Wait Time Over");
   else if (ModBusErr == MB_LENGTH_ERR)   printf("\n>!Rx Length Err");
   else if (ModBusErr == MB_CRC_ERR)      printf("\n>!Rx CRC Check Err");
   else if (ModBusErr == MB_TX_OVER)      printf("\n>!Tx Length Over");
   else if (ModBusErr == MB_RX_OVER)      printf("\n>!Rx Buffer Over");
   else if (ModBusErr == MB_ILLEGAL_FUNC) printf("\n>!Illegal Function");
   else if (ModBusErr == MB_ILLEGAL_ADD)  printf("\n>!Illegal Address");
   else if (ModBusErr == MB_ILLEGAL_DATA) printf("\n>!Illegal Data");
   else if (ModBusErr == MB_SLAVE_FAILER) printf("\n>!Slave Failer");
   else if (ModBusErr == MB_WRITE_FAIL)   printf("\n>!Data Write Fail");
   else if (ModBusErr == MB_RECEIVE_FAIL) printf("\n>!Data Receiv Fail");
}
//
// put char to Profi Device
//
void putchar_mb(unsigned char c)
{
  putchar0(c);
  if (ProfiDebug) printf("%02X ", c);
}

//
// get char from Profi Device
//
unsigned char getchar_mb(void)
{
  unsigned char c;
  c = getchar0();
  if (ProfiDebug) printf("%02X ", c);
  return c;
}

//
// Com port buffer clear for Profi Device
//
void profiport_buffer_clear(void)
{
  com0_buffer_clear();
}

//
// Com port baudrate change for Profi Device
// 57600 또는 38400으로 한정 - 2008/06/13
//
void profiport_speed_change(void)
{
//    if (iCom0Speed == 9600) iCom0Speed = 19200;
    if (iCom0Speed == 38400) iCom0Speed = 57600;
    else if (iCom0Speed == 57600) iCom0Speed = 38400;
    //else if (iCom0Speed == 19200) iCom0Speed = 38400;
    else iCom0Speed = DEFAULT_SPEED_COM0;
    
    com0_mode_set( iCom0Speed, 'n', 8, 1);
}

//
// detect receive data in ModBus port
//
char MB_CharReceived(void)
{
  if (RxUSART0) return TRUE;
  else return FALSE;
}

char MB_read_wait_time(void)
{
  char time;
  if (iCom0Speed == 38400)    time =  MODBUS_READ_WAIT_38400;
  else if (iCom0Speed == 57600)  time =  MODBUS_READ_WAIT_57600;
  else if (iCom0Speed == 19200)  time =  MODBUS_READ_WAIT_19200;
  else if (iCom0Speed == 115200) time =  MODBUS_READ_WAIT_57600;
  else if (iCom0Speed == 9600)   time =  MODBUS_READ_WAIT_19200 * 2;
  else if (iCom0Speed == 4800)   time =  MODBUS_READ_WAIT_19200 * 4;
  else time =  MODBUS_READ_WAIT_19200 * 4;
  return time;
}

//
//Profi device MULTI_READ function Query Transmit
//
void profi_parameter_multi_read_query(UINT8 code, UINT8 source)
{
  UINT16 func;
  UINT8 qty;
      func = MBaddForProfiFunc[code]; // Get ModBus Address
      qty = MBsizeForProfiFunc[code]/2;
      if (qty == 0) qty = 1;          // Get Register Size
      ResponseSize = (qty * 2) + 5; 
      ProfiWaitTime = MB_read_wait_time();
      if (ProfiDebug) printf("\nFuncNO:%02d[0x%04X]", code, func);
      PDreadPkt.SlaveAdd = PROFI_DEV_ADD;
      PDreadPkt.FuncCode = PROFI_MULTI_READ;
      PDreadPkt.StartAddH = func >> 8;
      PDreadPkt.StartAddL = func & 0xFF;
      PDreadPkt.ReadQtyH = qty >> 8;
      PDreadPkt.ReadQtyL = qty;
      ProfiD_packet_exchange( (UINT8*)&PDreadPkt, sizeof(ProfiDevRead), abResponse, source);
      
}

//
//Profi device INPUT_READ function Query Transmit
//
void profi_SCIout_read_query(UINT8 offset, UINT8 size, UINT8 source)
{
  UINT16  add;
      add = PROFI_SCIOUT_DATA_ADD + offset;
      size = size / 2;
      if (size == 0) size = 1;
      ResponseSize = (size * 2) + 5; 
      ProfiWaitTime = MB_read_wait_time();
      if (ProfiDebug) printf("\nOffset:%02d", offset);
      PDreadPkt.SlaveAdd = PROFI_DEV_ADD;
      PDreadPkt.FuncCode = PROFI_INPUT_READ;
      PDreadPkt.StartAddH = add >> 8;
      PDreadPkt.StartAddL = add & 0xFF;
      PDreadPkt.ReadQtyH = 0;
      PDreadPkt.ReadQtyL = size;
      ProfiD_packet_exchange( (UINT8*)&PDreadPkt, sizeof(ProfiDevRead), abReceiveBuf, source);      
}
//
//Profi device Multi Write function Query Transmit
//
void profi_SCIin_write_query(UINT8 offset, UINT8 size, UINT8 source)
{
    UINT16  add;
      add = PROFI_SCIIN_DATA_ADD + offset;
      size = size / 2;
      if (size == 0) size = 1;

      ResponseSize = 8;
      ProfiWaitTime = MB_read_wait_time();
      if (ProfiDebug) printf("\nOffset:%02d", offset);
      PDMwritePkt.SlaveAdd = PROFI_DEV_ADD;
      PDMwritePkt.FuncCode = PROFI_MULTI_WRITE;
      PDMwritePkt.StartAddH = add >> 8;
      PDMwritePkt.StartAddL = add & 0xFF;
      PDMwritePkt.RegQtyH = 0;
      PDMwritePkt.RegQtyL = size;
      PDMwritePkt.ByteQty = size * 2;
      ProfiD_packet_exchange( (UINT8*)&PDMwritePkt, sizeof(ProfiDevMultiWrite), abResponse, source);
}

//
//Profi device Single READ function Query Transmit
//
void profi_parameter_single_read_query(UINT8 code, UINT8 source)
{
  UINT16 func;
  UINT8 qty;
      func = MBaddForProfiFunc[code];
      qty = 1;
      ResponseSize = (qty * 2) + 5;  
      ProfiWaitTime = MB_read_wait_time();
      if (ProfiDebug) printf("\nFuncNO:%02d[0x%04X]", code, func);
      PDreadPkt.SlaveAdd = PROFI_DEV_ADD;
      PDreadPkt.FuncCode = PROFI_MULTI_READ;
      PDreadPkt.StartAddH = func >> 8;
      PDreadPkt.StartAddL = func & 0xFF;
      PDreadPkt.ReadQtyH = qty >> 8;
      PDreadPkt.ReadQtyL = qty;
      ProfiD_packet_exchange( (UINT8*)&PDreadPkt, sizeof(ProfiDevRead), abResponse, source);
}

//
//Profi device Single Write function Query Transmit
//
char profi_single_write_query(UINT8 code, UINT16 data, UINT8 source)
{
  UINT16 func;
  UINT8 rw;
    func = MBaddForProfiFunc[code];
    rw = MBrwForProfiFunc[code];
    if (rw == 1) 
    {
      ResponseSize = 8;
      ProfiWaitTime = MODBUS_WRITE_WAIT;
      if (ProfiDebug) printf("\nFuncNO:%02d[0x%04X]", code, func);
      PDSwritePkt.SlaveAdd = PROFI_DEV_ADD;
      PDSwritePkt.FuncCode = PROFI_SINGLE_WRITE;
      PDSwritePkt.StartAddH = func >> 8;
      PDSwritePkt.StartAddL = func & 0xFF;
      PDSwritePkt.DataH = data >> 8;
      PDSwritePkt.DataL = data;
      ProfiD_packet_exchange( (UINT8*)&PDSwritePkt, sizeof(ProfiDevSingleWrite), abResponse, source);
      return TRUE;
    }
    else return FALSE;    
}



/*******************************************************************************
** This program is the property of HMS Industrial Networks AB.                **
** It may not be reproduced, distributed, or used without permission          **
** of an authorised company official.                                         **
**                                                                            **
** Company: HMS Industrial Networks AB
**          Pilefeltsgatan 93-95
**          S-302 50  Halmstad
**          SWEDEN
**          Tel:     +46 (0)35 - 17 29 00
**          Fax:     +46 (0)35 - 17 29 09
**          e-mail:  info@hms.se
**
**
** Change Log
** ----------
**
** Latest Revision:
**
**    Rev 0.10    12 jun 2002   Created by AnN
**    Rev 1.00    12 jul 2002   First Release
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** GenerateCrc()
**------------------------------------------------------------------------------
*/
UINT16 GenerateCrc( UINT8* pabMessage, UINT16 iLength )
{
   const UINT8 abCrcHi[] =
   {
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
      0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
      0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
      0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
      0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
      0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
      0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
      0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
      0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
      0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
      0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
      0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
   };

   const UINT8 abCrcLo[] =
   {
      0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
      0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
      0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
      0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
      0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
      0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
      0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
      0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
      0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
      0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
      0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
      0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
      0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
      0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
      0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
      0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
      0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
      0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
      0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
      0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
      0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
      0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
      0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
      0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
      0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
      0x43, 0x83, 0x41, 0x81, 0x80, 0x40
   };

   UINT8 bCrcHi = 0xFF;
   UINT8 bCrcLo = 0xFF;
   UINT16 iIndex;

    while( iLength-- )
    {
        iIndex = bCrcHi ^ *pabMessage++;
        bCrcHi = bCrcLo ^ abCrcHi[iIndex];
        bCrcLo = abCrcLo[iIndex];
    }

    return( bCrcHi << 8 | bCrcLo );

}/* end GenerateCrc */

/********************************************************************************
**
** Description
** -----------
** This file contains functions for interfacing the AnyBus-IC
**
********************************************************************************
**
** Change Log
** ----------
**
** Latest Revision:
**
**    Rev 0.10    12 jun 2002   Created by AnN
**    Rev 1.00    12 jul 2002   First Release
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABIC_AutoBaud()
**------------------------------------------------------------------------------
*/

UINT8 ABIC_AutoBaud( void )
{

   UINT8 abResponse[ 10 ];
   UINT8 abRequest[ 6 ] = { 0x01, 0x03, 0x50, 0x01, 0x00, 0x01 } ;
   UINT8 bRetryCount;

   /*
   ** Do 15 tries to make the AnyBus-IC Autobaud.
   ** If no succes return FALSE
   */

   for( bRetryCount = 0; bRetryCount < 15 ; bRetryCount++ )
   {

      /*
      ** We send a request to read modbus address 0x5001 ( Parameter #1 )
      ** The response should be 6 byte long.
      */

      if( MB_SendRecModbusMessage( abRequest, 6, abResponse) == 5 )
      {

         /*
         ** We got an response on the AutoBaud Message,
         ** the AutoBaud is complete. Return TRUE,
         */

         return( TRUE );

      }/* end if Response received */

   }/* end for */

   return( FALSE );

}/* end AutoBaud */


/*------------------------------------------------------------------------------
** ABIC_NormalMode()
**------------------------------------------------------------------------------
*/

BOOL ABIC_NormalMode( void )
{

   UINT8 abResponse[ 10 ];
   UINT8 abSetRequest[ 6 ] = { 0x01, 0x06, 0x50, 0x01, 0x00, 0x01 } ;
   UINT8 abCheckRequest[ 6 ] = { 0x01, 0x04, 0x50, 0x01, 0x00, 0x01 } ;


   /*
   ** Try to AnyBus-IC in Normal mode
   */

   if( MB_SendRecModbusMessage( abSetRequest, 6, abResponse ) != 6 )
   {

      return( FALSE );

   }/* end if */


   /*
   ** Check if the AnyBus-IC is in Normal mode.
   ** For different reasons the attemt to set it in normal mode can
   ** have failed. For example faulty IO configuration.
   */

   if( MB_SendRecModbusMessage( abCheckRequest, 6, abResponse ) == 6 )
   {

      /*
	  ** Byte 4 and 5 contains the parameter value and it should be 0x0001
	  */

      if( abResponse[ 4 ] == 0 && abResponse[ 5 ] == 1 )
      {

         return( TRUE );

      }/* end if */

   }/* end if */


   /*
   ** We faild to read the current mode
   */

   return( FALSE );


}/* end ABIC_NormalMode */


/*------------------------------------------------------------------------------
** ABIC_ReadOutData()
**------------------------------------------------------------------------------
*/

BOOL ABIC_ReadOutData( UINT8 bOffset, UINT8 bSize, UINT8* pData )
{

   UINT8 bCount;
   UINT8 bReadSize;
   UINT8 abRequest[ 6 ];
   UINT8 abResponse[ 10 ];

   /*
   ** Set up Read Input registers request
   */

   abRequest[ 0 ] = 0x01;           /* Modbus Address           */
   abRequest[ 1 ] = 0x04;           /* Modbus Function Code     */
   abRequest[ 2 ] = 0x10;           /* Modbus Address High Byte */
   abRequest[ 3 ] = bOffset;        /* Modbus Address Low Byte  */
   abRequest[ 4 ] = 0x00;           /* No. of Points High       */
   abRequest[ 5 ] = bSize;          /* No. of Points Low        */

   /*
   ** Send the Request
   */

   bReadSize = MB_SendRecModbusMessage( abRequest, 6, abResponse ) - 3;

   /*
   ** Check if we received the amount of data we requested.
   ** If we did, copy it to data buffer
   */

   if( bReadSize == ( bSize * 2 ) )
   {

      for( bCount = 0; bCount < ( bSize * 2 ) ; bCount++ )
      {

         pData[ bCount ] = abResponse[ bCount + 3 ];

      }/* end for */


      return TRUE;

   }/* end if right size read */


   return FALSE;

}/* end ABIC_ReadOutData */



/*------------------------------------------------------------------------------
** ABIC_WriteInData()
**------------------------------------------------------------------------------
*/

BOOL ABIC_WriteInData( UINT8 bOffset, UINT8 bSize, UINT8* pData )
{

   UINT8 bCount;
   UINT8 abRequest[ 11 ];
   UINT8 abResponse[ 10 ];

   /*
   ** Set up Preset Multiple Registers request
   */

   abRequest[ 0 ] = 0x01;           /* Modbus Address           */
   abRequest[ 1 ] = 0x10;           /* Modbus Function Code     */
   abRequest[ 2 ] = 0x00;           /* Starting Address High    */
   abRequest[ 3 ] = bOffset;        /* Starting Address Low     */
   abRequest[ 4 ] = 0x00;           /* No. of Registers High    */
   abRequest[ 5 ] = bSize;          /* No. of Registers Low     */
   abRequest[ 6 ] = bSize * 2;      /* Byte Count               */

   /*
   ** Copy Data to Request
   */

   for( bCount = 0; bCount < ( bSize * 2 ) ; bCount++ )
   {

      abRequest[ bCount + 7 ] = pData[ bCount ];

   }/* end for */

   /*
   ** Send the Modbus Request and check if the response size is
   ** the length of a Preset Multiple Register Response ( 6 )
   */

   if( MB_SendRecModbusMessage( abRequest, 7 + ( 2 * bSize ), abResponse ) == 6 )
   {


      return( TRUE );

   }/* end if command succeded */


   return( FALSE );


}/* end ABIC_WriteInData */


/*******************************************************************************
**
** End of ABIC.C
**
********************************************************************************
*/
/*******************************************************************************
**
** Description
** -----------
** This file contains routines for sending Modbus messages
** on the serial interface
**
*/

UINT8 MB_bCRCCounter;
UINT8 MB_bTimeOutCounter;
UINT16 MB_iTimeOutTime = MB_DEFAULT_TIMEOUT;

/*------------------------------------------------------------------------------
** MB_Init()
**------------------------------------------------------------------------------
*/

void MB_SetTimeout( UINT16 iTime )
{

   /*
   ** Set Timeout time
   */

   MB_iTimeOutTime = iTime;


}/* end of MB_Init() */


/*------------------------------------------------------------------------------
** MB_SendModbusMessage()
**------------------------------------------------------------------------------
*/

UINT8 MB_SendRecModbusMessage( UINT8* pbData, UINT8 bSize, UINT8* pbResponse )
{

   UINT8 abSendBuffer[ 30 ];
   UINT16 iCrc;
   UINT8 bCharPos = 0;


   /*
   ** Check if the message is to long
   */

   if( bSize > 249 )
   {

      return( 0 );

   }/* end if message to long */

   /*
   ** Copy Data To send buffer
   */

   for( bCharPos = 0 ; bCharPos < bSize ; bCharPos++ )
   {

      abSendBuffer[ bCharPos ] = pbData[ bCharPos ];

   }/* end for */

   /*
   ** Generate CRC for the message to send
   */

   iCrc = GenerateCrc( abSendBuffer, bCharPos );

   /*
   ** Add CRC to the end of the message to send
   */

   abSendBuffer[ bCharPos ] = ( iCrc >> 8 ) & 0x00FF;
   abSendBuffer[ bCharPos + 1] = iCrc & 0x00FF;

   /*
   ** Send Modbus Request
   */

   for( bCharPos = 0; bCharPos < ( bSize + 2 ) ; bCharPos++ )
   {

      SD_PutChar( abSendBuffer[ bCharPos ] );

   }/* end for */


   TM_SetTimer( MB_iTimeOutTime );
   TM_StartTimer();


   /*
   ** Wait for repsonse to arrive or timeout
   */

   while( !TM_TimeOut() )
   ;


   TM_StopTimer();

   /*
   ** Read response
   */

   bCharPos = 0;

   while( SD_CharReceived() )
   {

      pbResponse[ bCharPos ] = SD_GetChar();

      bCharPos++;

   }/* end while rx buffer not emty */


   if( bCharPos == 0 )
   {

      MB_bTimeOutCounter++;

   }
   else if( bCharPos == 1 )
   {

      MB_bCRCCounter++;

   }/* end if */


   if( bCharPos > 1 )
   {

      /*
      ** Generate CRC for the response message
      */

      iCrc = GenerateCrc( pbResponse, bCharPos - 2 );


      /*
      ** Check CRC
      */

      if( ( ( (UINT8)( iCrc >> 8 ) ) == pbResponse[ bCharPos - 2 ] ) &&
          ( ( (UINT8)iCrc ) == pbResponse[ bCharPos - 1 ] ) )
      {

         /*
         ** Correct CRC return received length
         */

         return( bCharPos - 2 );

      }
      else
      {


         /*
         ** CRC error
         */

         MB_bCRCCounter++;

      }/* end if right crc */

   }/* end if response received */


   /*
   ** We failed to get a error free response
   */

   return( 0 );


}/* end MB_SendRecModbusMessage */


/*******************************************************************************
**
** End of MB.C
**
********************************************************************************
*/
/*******************************************************************************
**
** Description
** -----------
** This file contains timer routines used to determin modbus timouts.
** - 3.5 char timout ( end of message )
** - Response timout ( no response )
**
**
********************************************************************************
*/

UINT16 TM_iResponseTime;
UINT16 TM_iTimeOutTime;

/*------------------------------------------------------------------------------
** TM_StartTimer()
**------------------------------------------------------------------------------
*/

void TM_StartTimer()
{

   /*
   ** Timer 0 start
   */

   //TR0=1;      /* Timer Start                 */
   //TF0=0;      /* Clear timer flag            */
   //ET0=0;      /* Disable interrupt           */

   TM_iResponseTime = 0;

}/* end TM_StartTimer */


/*------------------------------------------------------------------------------
** TM_SetTimer()
**------------------------------------------------------------------------------
*/

void TM_SetTimer( UINT16 iTime )
{

   /*
   ** If this is the first SetTimer since starttimer we should calculate
   ** the response time.
   */


   TM_iTimeOutTime = iTime;


   /*
   ** Set new timout time
   */


   //*TH0=0xD8;    /* 10 ms: FFFF-D8EF=2710 (dec.10000) */
   //*TL0=0xEF;*/



}/* end TM_SetTimer */


/*------------------------------------------------------------------------------
** TM_SetTimer()
**------------------------------------------------------------------------------
*/

void TM_StopTimer()
{

}/* end TM_StopTimer */


/*------------------------------------------------------------------------------
** TM_TimeOut()
**------------------------------------------------------------------------------
*/

BOOL TM_TimeOut()
{

   /*
   ** Wait for response
   */

return( TRUE );
}/* end TM_TimeOut */


/*******************************************************************************
**
** End of TM.C
**
********************************************************************************
*/
/*******************************************************************************
**
** Description
** -----------
** This file contains the main function for the AnyBus-IC sample code.
** This example runs on a AnyBus-IC preconfigured to have a 2 byte input and
** 2 bytes output data on the SCI interface.
** This sample code should be considured as an example on how to communicate
** with the AnyBus-IC and it is not a drive routine.
** HMS Industrial Networks AB is not responsible for any damages caused by
** this program.
**
********************************************************************************
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** ABIC_MonitorSwitch
**
**------------------------------------------------------------------------------
**
** Switch "Monitor"
**
**------------------------------------------------------------------------------
*/

char ABIC_MonitorSwitch = 0x91;

/*------------------------------------------------------------------------------
**
** ABIC_StepSwitch
**
**------------------------------------------------------------------------------
**
** Switch "STEP"
**
**------------------------------------------------------------------------------
*/

char ABIC_StepSwitch = 0x90;


/*------------------------------------------------------------------------------
**
** ABIC_IntPin
**
**------------------------------------------------------------------------------
**
** AnyBus Int signal
**
**------------------------------------------------------------------------------
*/

char ABIC_IntPin = 0xB3;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** AD_GetValue()
**------------------------------------------------------------------------------
*/

UINT8 AD_GetValue( UINT8 bChannel )
{

   if( bChannel == 0 )
   {

      //ADCON1 = 0x07;

   }
   else
   {

       //ADCON1 = 0x06;

   }/* end if */

   //ADDATL = 0x0;               /* Wandlung Start */

   //while( BSY )
   ;                           /* warten bis Wandlung fertig */

   //return( ADDATH );
  return( 0 );
}/* end AD_GetValue */


/*------------------------------------------------------------------------------
** main()
**------------------------------------------------------------------------------
*/

void abic_main( void )
{

   BOOL fResult;
   //UINT8 bDisplayState = 0;
   UINT8 abInData[ 2 ];
   UINT8 abOutData[ 2 ];

   //UINT8 bLoopCounter = 0;
   //UINT8 bWriteErrorCounter = 0;
   //UINT8 bReadErrorCounter = 0;

   screen_clear();


   /*
   ** Initiate Serial Driver
   SD_Init();
   */

   /*
   ** Make the AnyBus-IC autodetect our Baud Rate
   */
   fResult = ABIC_AutoBaud();


   if( !fResult )
   {

      /*
      ** The baud rate auto detection failed.
      ** Write error message and stay here for ever
      */

      printf( "\nERROR!");
      printf( "\nAutoBaud fault!");

      while( 1 )
      ;

   }/* end if */


   /*
   ** When the AnyBus-IC is configured and ready we will
   ** have to start the data excange by setting the Device Mode
   ** parameter ( #1 ) to 1 ( Normal Operation ).
   ** In this sample we assume that the configuration already are done
   ** and stored in flash. Then the only thing we have to do is to set
   ** the module in Normal Operational mode.
   */

   if( ABIC_NormalMode() )
   {

      /*
      ** The baud rate auto detection failed.
      ** Write error message and stay here for ever
      */

      //LCD_SelectRow( 0 );
      printf ( "ERROR!");
      //LCD_SelectRow( 1 );
      printf( "Set Mode Fault!");

      while( 1 )
      ;

   }/* end if */


   while( 1 )
   {

      /*
      ** Get indata values from the CPU AD converter
      */
      abInData[ 0 ] = AD_GetValue( 0 );
      abInData[ 1 ] = AD_GetValue( 1 );


      /*
      ** Write the In Data values to the AnyBus-IC
      */
      ABIC_WriteInData( 0, 1, abInData );


      /*
      ** Print the In Data values on the display.
      */
      printf( "\n In Data: ");
      printf( "%02X ", abInData[ 0 ]  );
      printf( " ");
      printf( "%02X ", abInData[ 1 ]  );


      printf( "\nOut Data: ");


      /*
      ** Read the Out Data values from the AnyBus-IC
      */
      fResult = ABIC_ReadOutData( 0, 1, abOutData );

      if( fResult )
      {

         /*
         ** Print the Out Data values on the display
         */
         printf( " %02X ", abOutData[ 0 ]  );
         printf( " ");
         printf( " %02X ", abOutData[ 1 ]  );

      }
      else
      {

         /*
         ** We could not get correct Out Data
         */
         printf( "-- -- ");

      }/* end if */

   }/* end main loop */

}/* end main */


/*******************************************************************************
**
** End of MAIN.C
**
********************************************************************************
*/
/*
//
// modbus_message_exchange()를 동작시키기 위한 변수 설정
//
void ProfiD_packet_exchange_old( UINT8* query, UINT8 size, UINT8* response, short call )
{
  pModQuery = query;
  ModPacketSize = size;
  pModResponse = response;
  ModBusCall = call; 
  ModBusStep = 1;
}

void modbus_message_exchange1( void )
{
  char lp;
  switch(ModBusStep)
  {
  case 0: return;
  case 1:
    if( ModPacketSize > 249 )
    {
      ModBusErr = MB_TX_OVER;
      ModBusResult = FALSE;
      ModBusStep = MODBUS_END;
    }
    else 
    {
   // ModBus Rx buffer clear
    for( lp = 0; lp < MODBUS_RXBUF_SIZE ; lp++ ) pModResponse[lp] = lp;
    ModBusStep++;
    }
    return;
    
  case 2:
   //profiport_buffer_clear();
   if (ProfiDebug) printf("\nTx:");
   // Copy Data To send buffer    
   for( bCharPos = 0 ; bCharPos < ModPacketSize ; bCharPos++ )
      abSendBuffer[ bCharPos ] = pModQuery[ bCharPos ];
   // Generate CRC for the message to send
   iCrc = GenerateCrc( abSendBuffer, bCharPos );
   // Add CRC to the end of the message to send
   abSendBuffer[ bCharPos ] = ( iCrc >> 8 ) & 0x00FF;
   abSendBuffer[ bCharPos + 1] = iCrc & 0x00FF;
   // Send Modbus Request
   for( bCharPos = 0; bCharPos < ( ModPacketSize + 2 ) ; bCharPos++ )
      putchar_mb( abSendBuffer[ bCharPos ] );
   
   ModBusStep++;
   return;
   
  case 3:
    sModBusWait = 0;
    bCharPos = 0;
    if (ProfiDebug) printf("\nRx:");
    ModBusStep++;
    return;
    
  //Read response
  case 4:
    for (lp = 0; lp < 15; lp++)
    {
      if (MB_CharReceived())
      {
        //sModBusWait = 0;
        pModResponse[ bCharPos++ ] = getchar_mb();
        if (bCharPos >= ResponseSize) ModBusStep = 5;
        else if (bCharPos >= MODBUS_RXBUF_SIZE) ModBusStep = 5;
      }
      else lp = 15;
    }
    if (++sModBusWait > ProfiWaitTime) ModBusStep = 5;
    return;
    
  case 5:
    //if (ProfiDebug) 
    printf("\n%02X, %02X, %02X", pModResponse[1], pModResponse[2], pModResponse[1]&0x80);
    ModBusResult = FALSE;
    // Receive time over
    if (bCharPos == 0) ModBusErr = MB_TIMEOUT_ERR;
    // Receive Buffer Overflow
    else if (bCharPos >=  MODBUS_RXBUF_SIZE)  ModBusErr = MB_RX_OVER;
    // ModBus Exception Error
    else if ((pModResponse[1]&0x80) != 0) ModBusErr = pModResponse[2];
    else       
    {
      // Generate CRC for the response message
      iCrc = GenerateCrc( pModResponse, bCharPos - 2 );
      // Check CRC
      if( ( ( (UINT8)( iCrc >> 8 ) ) == pModResponse[ bCharPos - 2 ] ) &
          ( ( (UINT8)iCrc ) == pModResponse[ bCharPos - 1 ] ) )   
      {
      // Correct CRC return received length
        ModBusErr = MB_NO_ERR;
        ModBusResult = TRUE;
        ModBusRxLength = bCharPos - 2;
      }
      else
      {
        // CRC error
        ModBusErr = MB_CRC_ERR;
        ModBusRxLength = bCharPos - 2;
      }
   }
   
   ModBusStep = MODBUS_END;
   return;
   
  case MODBUS_END:
    if (ModBusCall == PROFI_CON_CALL) sExecStep++;
    else if (ModBusCall == REMOTE_CALL) RemoteStep++;
    ModBusCall = 0;
    ModBusStep = 0;
    return;
    
  default: ModBusStep = 0; return;
  }
}
*/
