

// Include Standard LIB  files
#include "project.h"

#define	SV_STX		ASC_STX
#define	SV_ETX		ASC_ETX

unsigned short usRxPacketNo;
unsigned short usRxDeviceNo;
unsigned short usTxPacketNo, usTxPacketNo0;
unsigned short usSeverMsg;
unsigned short usReplyMsg;
unsigned short RxDataLenth;
unsigned short TxDataLenth;
unsigned char ErrGenerate;
char Com2Mode;

void sample_data_send(unsigned short msg);
char sv_msgpkt_varify(unsigned char size);
short calc_checksum(unsigned char *ptr, char size);
void put_remote(unsigned char c);
unsigned char get_remote(void);
char remote_detect(void);
void putstring_modem(unsigned char* ptr);

int calculate_tx_time(length, speed)
{
  int time;
  time = length * 10 * 500 / speed;
  time++;
  return time;
}

/************************************/
/* RS485 통신포트 시험 프로그램     */ 
/* 통신속도 설정, 시험패킷 송수신등 */ 
/************************************/
#define	REMOTE_TEST_MAIN	10
#define	REMOTE_TEST_EXE	        100
#define	REMOTE_SPEED_CHANGE	200
#define	REMOTE_TEST_PING	300
#define	REMOTE_TEST_SPEED	400
#define	REMOTE_TEST_MONIT	500
#define	REMOTE_TEST_RXMONIT 	600
#define	REMOTE_TEST_SEND1	700
#define	REMOTE_TEST_SEND2	800
#define	REMOTE_TEST_SEND3	900
#define	REMOTE_TEST_SEND4	1000
#define	REMOTE_TEST_END         1100
#define	REMOTE_SAVE_END		1200

/**********************************/
/* Remote Control Test Functions  */
/**********************************/
char PacketMonit;
short sWriteTime;
char PingNo;
void menu_display_remote(char no)
{  
  if (no == 0)       printf("<<RS485 Set Menu>>");
  else if (no == 1)  printf("\nBaud Rate:%6d", iCom1Speed);
  else if (no == 2)  printf("\nPing Packet Test");
  else if (no == 3)  printf("\nSpeed/Error Test");
  else if (no == 4) {printf("\nPacket Monit[%1d]", PacketMonit);}
  else if (no == 5)  printf("\nReceive Monit");
  else if (no == 6)  printf("\nSend Test Packet1");
  else if (no == 7)  printf("\nSend Test Packet2");
  else if (no == 8)  printf("\nSend Test Packet3");
  else if (no == 9)  printf("\nSend Test Packet3");
  else if (no == 10) printf("\nReturn To Main    ");
}

void remote_test_function(void)
{
  char lp, result, data;
  switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    CursorUse = 1;
    MenuStart = 0;
    MenuEnd = 7;
    MenuSize = 11;
    sExecStep = REMOTE_TEST_MAIN;
    return;
    
  case REMOTE_TEST_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    debug_monit(MONOUT);
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_remote(lp);
    cursor_move_home();
    sExecStep++;
    return;
  
  case REMOTE_TEST_MAIN+1:
    if (MENU_UP) popup_menu_update(REMOTE_SET,UP); 
    else if (MENU_DN) popup_menu_update(REMOTE_SET,DOWN); 
    else if (ENTER_KEY) sExecStep = REMOTE_TEST_EXE; 
    else if (step_delay(SEC_1*60)) execmode_change(RUN_STATUS);    
    printf("\r");
    return;

  case REMOTE_TEST_MAIN+2:
    sExecStep--;
    return;

  case REMOTE_TEST_EXE:
    DelayStep = 0;
    LineBlink = 0;
    MenuNo = MenuStart + find_cursor_vpos();
    if (MenuNo == 0) execmode_change(SYSTEM_TEST);
    else if (MenuNo == 1) sExecStep = REMOTE_SPEED_CHANGE;
    else if (MenuNo == 2) sExecStep = REMOTE_TEST_PING;
    else if (MenuNo == 3) sExecStep = REMOTE_TEST_SPEED;
    else if (MenuNo == 4) sExecStep = REMOTE_TEST_MONIT;
    else if (MenuNo == 5) sExecStep = REMOTE_TEST_RXMONIT;
    else if (MenuNo == 6) sExecStep = REMOTE_TEST_SEND1;
    else if (MenuNo == 7) sExecStep = REMOTE_TEST_SEND2;
    else if (MenuNo == 8) sExecStep = REMOTE_TEST_SEND3;
    else if (MenuNo == 9) sExecStep = REMOTE_TEST_SEND4;
    else if (MenuNo == 10) execmode_change(SYSTEM_TEST);
    else sExecStep = REMOTE_TEST_MAIN; 
    return;

  case REMOTE_SPEED_CHANGE:
    screen_clear();
    printf("<Remote Com Mode>" );
    if (iCom1Speed == 9600) iCom1Speed = 19200;
    else if (iCom1Speed == 19200) iCom1Speed = 38400;
    else if (iCom1Speed == 38400) iCom1Speed = 57600;
    else if (iCom1Speed == 57600) iCom1Speed = 115200;
    else if (iCom1Speed == 115200) iCom1Speed = 9600;
    else iCom1Speed = 9600;
    com1_mode_set( iCom1Speed, 'n', 8, 1);
    printf("\n>Speed   :%6d", iCom1Speed );
    printf("\n>Parity  : NONE");
    printf("\n>Date Bit: 8");
    printf("\n>Stop Bit: 1");
    printf("\n");
    sExecStep++;
    return;
    
  case REMOTE_SPEED_CHANGE+1:
    sExecStep = REMOTE_SAVE_END;
    return;

  case REMOTE_TEST_PING:
    screen_clear();
    printf("<Remote Ping Test>");
    PingNo = 0;
    sExecStep++;
    return;

  case REMOTE_TEST_PING+1:
    rs485_direction(WRITE);
    sample_data_send('KS');
    sWriteTime = calculate_tx_time(TxDataLenth, iCom1Speed);
    printf("\n>Send Ping[%2d:%2d:%2d]", TxDataLenth, sWriteTime, PingNo);
    if (++PingNo > 99) PingNo = 0;
    DelayStep = 0;
    sExecStep++;
    return;    
    
  case REMOTE_TEST_PING+2:
    if (step_delay(sWriteTime)) rs485_direction(READ);
    return;    
 
  case REMOTE_TEST_PING+3:
    if (ENTER_KEY) sExecStep = REMOTE_TEST_END;
    else if (ANY_KEY) sExecStep = REMOTE_TEST_PING+1;
    else if (step_delay(SEC_1*30)) sExecStep = REMOTE_TEST_END;
    return;
    
  case REMOTE_TEST_RXMONIT:
    screen_clear();
    printf("<Remote Rx Monit>");
    rs485_direction(READ);
    sExecStep++;
    return;

  case REMOTE_TEST_RXMONIT+1:
    if (ENTER_KEY) sExecStep = REMOTE_TEST_MAIN;
    else if (remote_detect()) 
    {
      data = get_remote();
      if (data == SV_STX) printf("\n[");
      else if (data == SV_ETX) printf("]"); 
      else printf("%1c", data);
      DelayStep = 0;
     }
    return;

 case REMOTE_TEST_SPEED:
    screen_clear();
    printf("<REMOTE_TEST_SPEED>");
    printf("\n>Under constraction..");
    sExecStep = REMOTE_TEST_END;
    return;

  case REMOTE_TEST_MONIT:
    screen_clear();
    printf("<REMOTE_TEST_MONIT>");
    printf("\n>Under constraction..");
    sExecStep = REMOTE_TEST_END;
    return;

 case REMOTE_TEST_SEND1:
    screen_clear();
    printf("<REMOTE_TEST_SEND1>");
    printf("\n>Under constraction..");
    sExecStep = REMOTE_TEST_END;
    return;    

 case REMOTE_TEST_SEND2:
    screen_clear();
    printf("<REMOTE_TEST_SEND2>");
    printf("\n>Under constraction..");
    sExecStep = REMOTE_TEST_END;
    return;    

 case REMOTE_TEST_SEND3:
    screen_clear();
    printf("<REMOTE_TEST_SEND3>");
    printf("\n>Under constraction..");
    sExecStep = REMOTE_TEST_END;
    return;    

 case REMOTE_TEST_SEND4:
    screen_clear();
    printf("<REMOTE_TEST_SEND4>");
    printf("\n>Under constraction..");
    sExecStep = REMOTE_TEST_END;
    return;  
    
//*************************************************
//   변경된 데이터를 저장하고 
//   3초간 지연 또는 키입력으로 메인메뉴로 복귀
//*************************************************
  case REMOTE_SAVE_END:
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
    
  case REMOTE_SAVE_END+1:
    if (ANY_KEY) sExecStep = REMOTE_TEST_MAIN;
    else if (step_delay(SEC_1*3)) sExecStep = REMOTE_TEST_MAIN;
    return;    
//
//   1Sec 지연후 메인메뉴로 복귀
//
  case REMOTE_TEST_END:
    DelayStep = 0;
    sExecStep = REMOTE_SAVE_END+1;
    return;
    
  default:
    sExecStep = REMOTE_TEST_MAIN;
    return;
 };
}

unsigned char SeverSendData[256];
unsigned short sever_tx_checksum(short length)
{
  unsigned short lp, sum;
  
  sum = 0;
  for (lp = 1; lp < length; lp++)
    sum += SeverSendData[lp];
  
  return sum;
}

short make_short2ascii(short x, unsigned short y)
{
  SeverSendData[x++] = low2ascii((y >> 12) & 0x0F);
  SeverSendData[x++] = low2ascii((y >> 8) & 0x0F);
  SeverSendData[x++] = low2ascii((y >> 4) & 0x0F);
  SeverSendData[x++] = low2ascii(y & 0x0F);
  return x;
}

short make_hex2ascii(short x, unsigned short y)
{
  SeverSendData[x++] = low2ascii((y >> 4) & 0x0F);
  SeverSendData[x++] = low2ascii(y & 0x0F);
  return x;
}

unsigned char make_packet_head(unsigned short msg, unsigned short length)
{
  unsigned short x;
  x = 0;
  SeverSendData[x++] = SV_STX;
  x = make_short2ascii(x, usTxPacketNo);
  x = make_short2ascii(x, usRxDeviceNo);
  SeverSendData[x++] = msg >> 8;
  SeverSendData[x++] = msg & 0x0FF;
  x = make_short2ascii(x, length);
  TxDataLenth = length;
  return x;
}

void make_packet_tail(unsigned short x)
{
  unsigned short sum;
  sum = sever_tx_checksum(x);
  if (ErrGenerate) sum = sum + 1;
  x = make_short2ascii(x, sum);
  SeverSendData[x++] = SV_ETX;
}


void make_sample_data(unsigned short msg)
{
  unsigned char x;
  x = make_packet_head(msg, 22);
  
  x = make_hex2ascii(x, Year);
  x = make_hex2ascii(x, Month);
  x = make_hex2ascii(x, Date);
  x = make_hex2ascii(x, Hour);
  x = make_hex2ascii(x, Minute);

  x = make_short2ascii(x, iVrs);
  x = make_short2ascii(x, iIa);
  x = make_short2ascii(x, iPw);
  
  make_packet_tail(x);
}
 
void sample_data_send(unsigned short msg)
{
  make_sample_data(msg);
  putstring_modem(&SeverSendData[0]);
  usTxPacketNo0 = usTxPacketNo;
  usTxPacketNo++;
}

short calc_checksum(unsigned char *ptr, char size)
{
  short sum;
  char lp;
  sum = 0;
  size = size + 1;
  for (lp = 1; lp < size; lp++) 
  {
    sum = sum + *ptr;
    ptr++;
  }
  return sum;
}

void put_remote(unsigned char c)
{
  putchar1(c);
}

unsigned char get_remote(void)
{
  unsigned char c;
  c = getchar1();
  return c;
}

char remote_detect(void)
{
  char rx;
  if (rx_rd_index1 != rx_wr_index1) rx = 1; else rx = 0;
  return rx;
}

void putstring_modem(unsigned char* ptr)
{
  unsigned char c, end;
  end = 1;
  
  do 
  {
    c = *ptr;
    put_remote(c);
    
    if ((c == SV_ETX)|(c == '\r')|(c == '\n')) end = 0;
    ptr++;
    
      if (Com2Mode)
      {      
        if(c == SV_STX) printf("\n(");
        else if (c == SV_ETX) printf(")");
        else printf("%1c",c);
      }
  } while(end);           
}

