

// Include Standard LIB  files
#include "project.h"

#define ON_MAX  16
#define KEY_QTY 32

/********************************/
/*          key scan            */
/* Generate keycode             */
/********************************/
unsigned char KeyStatus;
char TouchDt;
char  PushKey, PullKey;
char  KeyCount[KEY_QTY] = {0};
char KeyDigit;
char KeyStep;	// key string input step
unsigned int KeyValue;

#define	KEY_BD_GOSOO_V2
#ifdef KEY_BD_GOSOO_V2
const char KeyArray[KEY_QTY] =
{
   KEY_r,  KEY_s, KEY_m,  KEY_y,
       0,      0,     0,      0,
  KEY_CR,      0,     0,      0,
       0,      0,     0,      0,
// Ext Input key       
       0,      0, KEY_R,      0,       
       0,      0,     0,      0,
       0,  KEY_C,     0,      0,
       0,      0,     0,      0,       
};
const char KeyArrayPull[KEY_QTY] =
{
      0,      0, KEY_M,  KEY_x,
      0,      0,     0,      0,
      0   
};
#endif

#ifdef KEY_BD_GOSOO
const char KeyArray[KEY_QTY] =
{
   KEY_y,  KEY_l,  KEY_m,  KEY_s,
   KEY_r,      0,     0,      0,
  KEY_CR,      0,     0,      0,
       0,      0,     0,      0,
// Ext Input key       
   KEY_R,      0,     0,      0,       
       0,      0,     0,      0,
       0,  KEY_C,     0,      0,
       0,      0,     0,      0,       
};
const char KeyArrayPull[KEY_QTY] =
{
  KEY_x,      0,     0,      0,
      0,      0,     0,      0,
      0   
};
#endif

void keyin_start(char digit)
{
    KeyDigit = digit;
    KeyValue = 0;
}

// Key input start & next step
void keyin_dp_start(char digit)
{
    KeyDigit = digit;
    KeyValue = 0;
    sExecStep++;
}

char hex_keyboard(char key)
{
  if (key == '*') key = 'A';
   else if (key == '#') key = 'B';
   else if (key == 'a') key = 'C';
   else if (key == 'b') key = 'D';
   else if (key == 'c') key = 'E';
   else if (key == 0x0D) key = 'F';
  return key;
}

// Hex Key input & display
// if Key input end, goto next step
char keyin_dp_hex(void)
{
  char key, shift;
  unsigned int uimask;
  if (PushKey)
    if (KeyDigit > 0)
      if (KeyDigit <= 8)
      {
        key = hex_keyboard(PushKey);
        key = ascii2hex(key);
	shift = (KeyDigit - 1) << 2;
        uimask = 0x0F << shift;
	uimask ^= 0xFFFFFFFF;
        KeyValue &= uimask;
        KeyValue |= key << shift;
	printf("%01X", key);
        if (--KeyDigit == 0) sExecStep++;;
      }
      else KeyDigit = 0;
  return KeyDigit;
}

// Hex Key input & display
// if Key input end, goto next step
char keyin_dp_bcd(void)
{
  char key, shift;
  unsigned int uimask;
   if (DEC_KEY)
   {
     if (KeyDigit > 0)
       if (KeyDigit <= 8)
       {
         key = ascii2hex(PushKey);
	 shift = (KeyDigit - 1) << 2;
         uimask = 0x0F << shift;
	 uimask ^= 0xFFFFFFFF;
         KeyValue &= uimask;
         KeyValue |= key << shift;
	 printf("%01X", key);
         if (--KeyDigit == 0) sExecStep++;
       }
       else KeyDigit = 0;
   }
   else if (ENTER_KEY) sExecStep++;
  return KeyDigit;
}

// Hex Key input
char key_input_hex(void)
{
    char key;
    if (PushKey) 
    {
      key = hex_keyboard(PushKey);
      key = ascii2hex(key);
      
      if (KeyDigit == 8) 
      {
        KeyValue &= 0x0FFFFFFF;
	KeyValue |= key << 28;
        KeyDigit--;
      }
      else if (KeyDigit == 7) 
      {
        KeyValue &= 0xF0FFFFFF;
	KeyValue |= key << 24;
        KeyDigit--;
      }
      else if (KeyDigit == 6) 
      {
        KeyValue &= 0xFF0FFFFF;
	KeyValue |= key << 20;
        KeyDigit--;
      }
      else if (KeyDigit == 5) 
      {
        KeyValue &= 0xFFF0FFFF;
	KeyValue |= key << 16;
        KeyDigit--;
      }
      else if (KeyDigit == 4) 
      {
	KeyValue &= 0xFFFF0FFF;
	KeyValue |= key << 12;
        KeyDigit--;
      }
      else if (KeyDigit == 3) 
      {
        KeyValue &= 0xFFFFF0FF;
	KeyValue |= key << 8;
        KeyDigit--;
      }
      else if (KeyDigit == 2) 
      {
        KeyValue &= 0xFFFFFF0F;
	KeyValue |= key << 4;
        KeyDigit--;
      }
      else if (KeyDigit == 1) 
      {
        KeyValue &= 0xFFFFFFF0;
	KeyValue |= key;
        KeyDigit--;
      }
  }
  return KeyDigit;
}

char key_input_bcd(void)
{
  char key;
  if (DEC_KEY)
  {
      key = ascii2hex(PushKey);
      
      if (KeyDigit == 6) 
      {
        KeyValue &= 0xFF0FFFFF;
	KeyValue |= key << 20;
        KeyDigit--;
      }
      else if (KeyDigit == 5) 
      {
        KeyValue &= 0xFFF0FFFF;
	KeyValue |= key << 16;
        KeyDigit--;
      }
      else if (KeyDigit == 4) 
      {
	KeyValue &= 0xFFFF0FFF;
	KeyValue |= key << 12;
        KeyDigit--;
      }
      else if (KeyDigit == 3) 
      {
        KeyValue &= 0xFFFFF0FF;
	KeyValue |= key << 8;
        KeyDigit--;
      }
      else if (KeyDigit == 2) 
      {
        KeyValue &= 0xFFFFFF0F;
	KeyValue |= key << 4;
        KeyDigit--;
      }
      else if (KeyDigit == 1) 
      {
        KeyValue &= 0xFFFFFFF0;
	KeyValue |= key;
        KeyDigit--;
	KeyStep = 0;
      }
  }
  return KeyDigit;
}

void inc_key(char no)
{
 char byte;
 byte = KeyCount[no];
 if ((byte & 0x3F) < ON_MAX) byte++;
 if ((byte & 0x3F) >= ON_MAX) 
 {
  if ((byte & 0x80) == 0) 
  {
   byte = byte | 0xC0; 
   PushKey = KeyArray[no];
   }
   else if ((byte & 0xC0) == 0xC0) byte = byte & 0xBF;
  };
 KeyCount[no] = byte;                            
}

void dec_key(char no) 
{
char byte;
byte = KeyCount[no];
if ((byte & 0x3F) > 0) byte--;
if ((byte & 0x3F) == 0) 
{
  if ((byte & 0x80) == 0x80) 
  {
   byte = 0x40;
   PushKey = KeyArrayPull[no];
   }
   else if ((byte & 0x40) == 0x40) byte = 0;
  };
 KeyCount[no] = byte;
}  

// Emergency Button Push check
// Local/Remote selector check
//#define STOP_BT     1
#define REMOTE_SEL  2
#define EMEG_BT     3
void select_switch_check(void)
{
  if (KeyCount[EMEG_BT] & 0x80) EmegStop = OFF;
  else EmegStop = ON;
  // 시스템 작동중에는 리모트/로컬 전환을 금지한다. 2007/10/27
  if (SystemRun == 0)
  {
    //if (RemoteReady == OFF) OperUser = LOCAL;
    if (KeyCount[REMOTE_SEL] & 0x80) OperUser = REMOTE;
    else OperUser = LOCAL;
  }
}

char  EmegStop, EmegStop0; 
void panel_key_scan(void)
{
  char no, lp;
  unsigned char  data, mask;

  PushKey = 0;
  PullKey = 0;
  no = 0;
  mask = 0x01;
  data = pio_read(PIOA) >> 19;
  if (ExtIn[EXTIN_EMEG_STOP] == ON) data &= 0xF7; else data |= 0x08;

  mask = 0x01;
  for (lp = 0; lp < 4; lp++)
  { 
    if (data & mask) dec_key(no); else inc_key(no);
    mask <<= 1;
    no++;
  }
  // Emergency Button Push check
  select_switch_check();   
}  

void ext_remote_scan(void)
{
  char no, lp;
  unsigned short  data, mask;

  no = 16;
  data = usExtInBuf;

  mask = 0x8000;
  for (lp = 0; lp < 16; lp++)
  { 
    if (data & mask) dec_key(no); else inc_key(no);
    mask >>= 1;
    no++;
  }
}  

/***********************************/
/*  Wheel interface Functions      */
/*  Rotary excoder switch drive    */
/***********************************/
char RotPulse, RotPulse0;
unsigned char RotFw, RotRev, RotTime, RotSpeed;
#define ROT_BT    8
void rotary_key_test(void)
{
  if ((pio_read(PIOA) & JOG_BT) == 0) inc_key(ROT_BT); else dec_key(ROT_BT);
  RotPulse = (pio_read(PIOA)>>8) & 3;
  
  if (RotPulse != RotPulse0) 
  {
    //if (RotPulse & 2) printf("1"); else printf("0");
    //if (RotPulse & 1) printf("1 "); else printf("0 ");
    //printf("%d", RotPulse);
    if ((RotPulse == 0)&(RotPulse0 == 2)) RotFw++; 
     else if ((RotPulse == 0)&(RotPulse0 == 1)) RotRev++; 
     else if ((RotPulse == 3)&(RotPulse0 == 1)) RotFw++; 
     else if ((RotPulse == 3)&(RotPulse0 == 2)) RotRev++;     
    
    if ((RotPulse == 3)|(RotPulse == 0))
    {
      RotSpeed = (SEC_1/2)/RotTime;
      printf("\rFW:%3d RW:%3d S:%3d", RotFw, RotRev, RotSpeed);
      RotTime = 0;
    }
    RotPulse0 = RotPulse;
    //printf("\nFW:%3d REV:%3d", RotFw, RotRev);
  }
  if (RotTime < 99) RotTime++;
} 

char JogKey, JogKey0;
unsigned char JogFw, JogRev, JogTime, JogSpeed;
void rotary_key_scan(void)
{
  if (!(pio_read(PIOA) & JOG_BT)) inc_key(8); else dec_key(8);
  JogKey = (pio_read(PIOA)>>8) & 3;
  if (JogKey != JogKey0) 
  {
    if ((JogKey == 0)&(JogKey0 == 2)) PushKey = KEY_d; 
     else if ((JogKey == 0)&(JogKey0 == 1)) PushKey = KEY_u; 
     else if ((JogKey == 3)&(JogKey0 == 1)) PushKey = KEY_d;
     else if ((JogKey == 3)&(JogKey0 == 2)) PushKey = KEY_u;     
    
    if ((JogKey == 3)|(JogKey == 0))
    {
      JogSpeed = (SEC_1/2)/JogTime;
      JogTime = 0;
    }
    JogKey0 = JogKey;
  }
  if (JogTime < 99) JogTime++;
}  

char RollUpNo, RollDnNo;
short sRollClearTime;
#define ROLL_COUNT  2
void scroll_key_generate(void)
{
  if (++sRollClearTime > SEC_1*2) 
  {
    sRollClearTime = 0;
    RollUpNo = 0;
    RollDnNo = 0;
  }
  
  if (ROLL_UP)
  {
    RollDnNo = 0;
    sRollClearTime = 0;
    if (++RollUpNo > ROLL_COUNT) 
    {
      RollUpNo = 0;
      PushKey = 'g';
    }
  }
  else if (ROLL_DN)
  {
    RollUpNo = 0;
    sRollClearTime = 0;
    if (++RollDnNo > ROLL_COUNT) 
    {
      RollDnNo = 0;
      PushKey = 'h' ;
    }
  }
}
          
/***********************************/
/*   POP UP/DOWN Menu select       */
/*  UP, DOWN 키를 사용한 메뉴 처리 */
/***********************************/
char MenuNo, MenuStart, MenuEnd, MenuSize;
void menu_display(char menu, char no)
{
  switch(menu)
  {
    case MAIN_MENU:   menu_display_main(no);      return;
    case SYSTEM_TEST: menu_display_test(no);      return;
    case RTC_SET:     menu_display_rtc_set(no);   return;
    case PROFI_SET:   menu_display_profi(no);     return;
    case REMOTE_SET:  menu_display_remote(no);    return;
    case METER_ADJUST: menu_display_adjust(no);   return;
    case EXTOUT_TEST: menu_display_extout(no);    return;
    case DAC_TEST:    menu_display_dactest(no);   return;
    case ADC_TEST:    menu_display_adctest(no);   return;
    case ADE_TEST:    menu_display_adetest(no);   return;
    case ADEIO_TEST:  menu_display_adeiotest(no); return;
  }
}

void popup_menu_update(char menu, char dir)
{
  char line, end, lp;
  if (FontStyle == 0) end = 7; else end = 3;
  if (end >= MenuSize) end = MenuSize-1;
  line = find_cursor_vpos();
  
  if (dir == UP)
  {
    if (line > 0) cursor_move_up();
    else 
    {
      if ((MenuStart > 0)&(MenuEnd < MenuSize))
      {
        MenuStart--;
        MenuEnd--;
        screen_clear();
        for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display(menu, lp);
        cursor_move_home();
      }       
    }
  }
  else if (dir == DOWN)
  {
    if (MenuEnd < MenuSize)
    {
      if (line < end) cursor_move_down();
       else if (MenuEnd < MenuSize-1) 
       {
         MenuStart++;
         MenuEnd++;
         menu_display(menu, MenuEnd);
       }
    }
  }   
  sExecStep++;
}

//
// Wheel을 돌려 숫자 입력 받기
// 주의: sExecStep 을 사용하는 KeyFunction에서만 사용할 수 있음
//
char wheel_input(int min, int max)
{
  char end;
  
    end = 0;
    if (COUNT_UP) 
    {
      if (JogSpeed < 4) JogSpeed = 1; 
      if (iTempSet < max) iTempSet+= JogSpeed;
      if (iTempSet > max) iTempSet = max;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      if (JogSpeed < 4) JogSpeed = 1; 
      if (iTempSet > min) iTempSet -= JogSpeed;
      if (iTempSet < min) iTempSet = min;
      sExecStep++;
    }
    else if (ENTER_KEY) end = 1;
  return end;
}

//
// Wheel을 돌려 숫자 입력 받기
// 속도 제한 변수 부가
// 주의: sExecStep 을 사용하는 KeyFunction에서만 사용할 수 있음
//
char wheel_input_speed(int min, int max, char speed)
{
  char end;
  if (speed > JogSpeed) speed = JogSpeed; 
    end = 0;
    if (COUNT_UP) 
    {
      if (speed < 4) speed = 1; 
      if (iTempSet < max) iTempSet+= speed;
      if (iTempSet > max) iTempSet = max;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      if (speed < 4) speed = 1; 
      if (iTempSet > min) iTempSet -= speed;
      if (iTempSet < min) iTempSet = min;
      sExecStep++;
    }
    else if (ENTER_KEY) end = 1;
  return end;
}

//
// Wheel을 돌려 숫자 입력 받기
// 변수값을 직접적으로 운전설정치로 반영
// 주의: sExecStep 을 사용하는 KeyFunction에서만 사용할 수 있음
//
void running_wheel_input(int min, int max)
{
    if (COUNT_UP) 
    {
      if (JogSpeed < 4) JogSpeed = 1; 
      if (iTempSet < max) iTempSet+= JogSpeed;
      if (iTempSet > max) iTempSet = max;
      DelayStep = 0;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      if (JogSpeed < 4) JogSpeed = 1; 
      if (iTempSet > min) iTempSet -= JogSpeed;
      if (iTempSet < min) iTempSet = min;
      DelayStep = 0;
      sExecStep++;
    }
}

//
// Key Counter Display
// 버튼 디버깅용 코드
//
void key_count_monit(char no)
{
  short pos;
  pos = sCurPos;
  if (FontStyle == 0) goto_cursor(19,0);
  else goto_cursor(14,0);
  printf("%02X",KeyCount[no]);
  sCurPos = pos;
}
