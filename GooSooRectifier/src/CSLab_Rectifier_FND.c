

// Include Standard LIB  files
#include "project.h"

// FND 코드 정의
// 편의상 기호는 세가지만 처리
#define	FND_MI  36	// '-'
#define	FND_SP  37
#define FND_UN	38	// '_'

/************************************************************
*  Ddfinition for  FND 7 segments                           *
*************************************************************
*    -A-    |   b7 b6 b5 b4 <=> SEG 0,2,4,6 <=> ADDR 0,2,4,6
*   F   B   |    B  G  C  x 
*    -G-    |
*   E   C   |   b3 b2 b1 b0 <=> SEG 1,3,5,7 <=> ADDR 1,3,5,7
*    -D-    |    A  F  E  D
************************************************************/  
const unsigned char FND_IMAGE[] = 
{
	0x3f/* 0 */,0x06/* 1 */,0x5B/* 2 */,0x4F/* 3 */,0x66/* 4 */,
	0x6D/* 5 */,0x7D/* 6 */,0x07/* 7 */,0x7F/* 8 */,0x6F/* 9 */,
	0x77/* A */,0x7C/* B */,0x39/* C */,0x5E/* D */,0x79/* E */,
	0x71/* F */,0x3D/* G */,0x76/* H */,0x30/* I */,0x0E/* J */,
	0x78/* K */,0x38/* L */,0x37/* M */,0x54/* N */,0x5C/* O */,
	0x73/* P */,0x67/* Q */,0x50/* R */,0x6D/* S */,0x31/* T */,
	0x1C/* U */,0x3E/* V */,0x3E/* W */,0x52/* X */,0x72/* Y */,
	0x0F/* Z */,0x40/* - */,0x00/*' '*/,0x08/* _ */
};

#define MODE_VADP 0
unsigned char FndBuf[10];
short sOutVolt;
short sOutVolt0;
int iOutCurrent;
int iOutCurrent0;
char MinusLamp;
char FndDpMode;
short FndScanTime;
unsigned char FndPPbuf;

/* ------ write one byte to the device ------- */
void fnd_byte_write(uchar W_Byte, uchar no)	
{
  uchar i, mask;
  mask = 1 << no;
  W_Byte = FND_IMAGE[W_Byte] ^ 0xFF;
  // FND의 point 점등여부 처리
  if ((FndPPbuf & mask) != 0) W_Byte &= 0x7F; else W_Byte |= 0x80;
  for(i = 0; i < 8; ++i)
  {    
    //set data port by read data
    pio_clear(PIOA, FND_CK);	//SCLK = 0;
    if(W_Byte & 0x80) pio_set(PIOA, FND_DT);
      else pio_clear(PIOA, FND_DT);    
    delay_us(2);
    pio_set(PIOA, FND_CK); 	//SCLK = 1;
    W_Byte <<= 1;
  }
}

void fnd_point_write(uchar W_Byte)	
{
  uchar i;
  W_Byte = FND_IMAGE[W_Byte] ^ 0xFF;
  if (MinusLamp) W_Byte &= 0x7F; else W_Byte |= 0x80;
  for(i = 0; i < 8; ++i)
  {
    pio_clear(PIOA, FND_CK);	//SCLK = 0;
    //set data port by read data
    if(W_Byte & 0x80) pio_set(PIOA, FND_DT);
      else pio_clear(PIOA, FND_DT);
    //delay_us(5);
    pio_set(PIOA, FND_CK); 	//SCLK = 1;
    W_Byte <<= 1;
  }
}

void fnd_all_write(void)
{
  char lp;
  pio_clear(PIOA, FND_LD);
  //fnd_point_write(MinusLamp);
  pio_clear(PIOA, FND_CK);	//SCLK = 0;
  for (lp = 0; lp < 8; lp++) fnd_byte_write(FndBuf[7-lp], lp);
  pio_set(PIOA, FND_LD);
  delay_us(3);
  pio_clear(PIOA, FND_LD);
}

void fnd_point_onoff(char no, char onoff)
//0:off, 1:on, 2: toggle
{
  unsigned char mask;
  no &= 7;
  mask = 0x80 >> no;
  if (onoff == ON) FndPPbuf |= mask;
  else if (onoff == OFF) FndPPbuf &= (mask ^ 0xFF);
}

char FndPos;
/****************************************************/
/*  Charactor store to  FND buffer(FndBuf[])        */
/*  if (DebugMonit & FND_OUT) GOSOO 8 FND display   */
/****************************************************/
void putchar_fnd(unsigned short code)
{
  if (FndPos >= 8) FndPos = 0;
  code &= 0xFF;
  if (code == '\r')  FndPos = 0;
  else if (code == '\n') FndPos = 0;
  else if (code == ' ')	// 스페이스 처리		 
  {
    FndBuf[FndPos] = FND_SP;
    fnd_point_onoff(FndPos, OFF);
    if (++FndPos > 7) FndPos = 0;
  }   
  else if ((code >= '0')&(code <= '9'))
  { 
    FndBuf[FndPos] = code - 0x30;
    fnd_point_onoff(FndPos, OFF);
    if (++FndPos > 7) FndPos = 0;
  }
  else if ((code >= 'A')&(code <= 'Z'))
  { 
    FndBuf[FndPos] = code + 10 - 'A';
    fnd_point_onoff(FndPos, OFF);
    if (++FndPos > 7) FndPos = 0;
  }
  else if ((code >= 'a')&(code <= 'z'))
  { 
    FndBuf[FndPos] = code + 10 - 'a';
    fnd_point_onoff(FndPos, OFF);
    if (++FndPos > 7) FndPos = 0;
  }
  else if (code == '-')
  { 
    FndBuf[FndPos] = FND_MI;
    fnd_point_onoff(FndPos, OFF);
    if (++FndPos > 7) FndPos = 0;
  }
  else if (code == '_')
  { 
    FndBuf[FndPos] = FND_UN;
    fnd_point_onoff(FndPos, OFF);
    if (++FndPos > 7) FndPos = 0;
  }
  else if ((code == '.')|(code == ','))
  {
    if (FndPos > 0) fnd_point_onoff(FndPos-1, ON);
  }
}

void volt_display_float(float val)
{
  char buf;
  buf = DebugMonit;
  DebugMonit = FND_OUT;

  FndPos = 0;
  if (val < 0) val *= -1;  
  if (val > 999) printf("%3.0f", val);
  else if (val > 99.9) printf("%3.0f", val);
  else if (val > 9.9) printf("%3.1f", val); 
  else printf(" %2.1f", val); // 2008-12-03 김태곤과장 요청으로 다시 수정
  DebugMonit = buf;
}

void volt_display_test(float val)
{
  char buf;
  unsigned short word;

  buf = DebugMonit;
  DebugMonit = FND_OUT;
  FndPos = 0;
  if (val < 0) val *= -1;  
  word = val * 10;
  printf("%3d", word);
  DebugMonit = buf;
}

void current_display(int i)
{
  char buf;
  buf = DebugMonit;
  DebugMonit = FND_OUT;
  
  if ( i < -9999) i *= -1;
  // 2008-11-26
  // 정지상태에서 전류측정값이 정격최대전류값의 0.2%이내이면 '0'으로 표시
  if (!SystemRun) 
    if ( i < (fMaxOperAmp * 0.02)) i = 0; 
  FndPos = 3;
  printf("%5d", i);
  
  DebugMonit = buf;
}


char FndTestStep;
char FndTestData;
short FndDleay;
void all_FND_test(void)
{
  char data, lp;
 switch(FndTestStep)
 {
  case 0: return;
  case 1:
   pio_clear(PIOA, FND_LD);
   FndTestData = 0;
   MinusLamp = 0;
   FndPPbuf = 0;
   FndTestStep++;
   return;
   
  case 2:
   //pio_set(PIOA, FND_CLR);
   FndTestStep++;
   return;
 
  case 3:
   data = FndTestData;
   for (lp = 0; lp < 8; lp++) FndBuf[lp] = data;
   fnd_point_onoff(FndTestData, ON);
   fnd_all_write();
   FndDleay = 0;
   FndTestStep++;
   return;
  
  case 4:
   if (++FndDleay > SEC_1) FndTestStep++;
   return;
   
  case 5:
   fnd_point_onoff(FndTestData, OFF);
   if (++FndTestData > 15) FndTestStep++; else FndTestStep = 3;
   return;
   
  case 6:   
   volt_display_float(fVoltInput);
   current_display(iAmpInput);
   fnd_all_write();
   FndTestStep = 0; 
   return;
   
 default: 
   FndTestStep = 0; 
   return;
 };
}

int iAmpInput0;
int iVoltInput0;
char FndSkipNo;
void FND_display_update(void)
{
  //float err;
  if (++FndScanTime > SEC_1>>4)
  {
    FndScanTime = 0;
    FndSkipNo++;
    if (!FndTestStep)
    {
      if (FndDpMode == MODE_VADP)
      {
        if ((iAmpInput != iAmpInput0)|(iVoltInput != iVoltInput0))
        {
          volt_display_float(fVoltAvrInput);
          current_display(iAmpInput);
          fnd_all_write();
          iAmpInput0 = iAmpInput;
          iVoltInput0 = iVoltInput;
        } 
        else if (++FndSkipNo > 32) 
        {
          FndSkipNo = 0;
          if(!SystemRun) volt_display_float(fVoltInput);
          if(!SystemRun) current_display(iAmpInput);
          fnd_point_onoff(7, OperPole);
          fnd_all_write();
        }                   
      }
    }
  }
}

unsigned int AverExecTime;
void FND_display_exectime(void)
{
  AverExecTime += iExecTime;
  if (++FndScanTime > SEC_1)
  {
    FndScanTime = 0;
    if (!FndTestStep)
    {
      //volt_display(iMaxExecTime/10,1);
      volt_display_float(iMaxExecTime/10);
      current_display(AverExecTime/SEC_1);
      fnd_all_write();
      iMaxExecTime = 0;
      AverExecTime = 0;
    }
  }
}

/*
void volt_display(int v, short div)
{
  char d0, d1, d2;
  int dv, mod;
  dv = v / div;
  mod = (v<<1) % div;
  if (mod > div) dv++;   
  dv = hex2bcd3(dv);
  d0 = dv & 0x000F;
  dv >>= 4;
  d1 = dv & 0x000F;
  dv >>= 4;
  d2 = dv & 0x000F;
  
  // ZERO blanking
  if (div != 1)
    if (d2 == 0) 
    {
      d2 = FND_SP;
      if (d1 == 0) d1 = FND_SP;
    }
  
  // float point decide
  FndPPbuf = 0;
  if (div == 1) fnd_point_onoff(7, ON);
    else if (div == 10) fnd_point_onoff(6, ON);
  FndBuf[0] = d2;
  FndBuf[1] = d1;
  FndBuf[2] = d0;
  //fnd_all_write();
}

void current_display(int i)
{
  char d0, d1, d2, d3, d4, p;
  p = 0;
  if (i < 0) 
  {
    i *= -1;
    p = 1;
  }
  i = hex2bcd5(i);
  //i = hex2bcd5(98765);
  d0 = i & 0x000F;
  i >>= 4;
  d1 = i & 0x000F;
  i >>= 4;
  d2 = i & 0x000F;
  i >>= 4;
  d3 = i & 0x000F;
  i >>= 4;
  d4 = i & 0x000F;
  
  // ZERO blanking
  if (d4 == 0) 
  {
    if (p == 1) d4 = FND_MI; else d4 = FND_SP;
    if (d3 == 0) 
    {
      if (p == 1) d3 = FND_0; else d3 = FND_SP;
      if (d2 == 0)
      {
        if (p == 1) d2 = FND_0; else d2 = FND_SP;
        if (d1 == 0) d1 = FND_SP;
      }
    }
  }

  FndBuf[3] = d4;
  FndBuf[4] = d3;
  FndBuf[5] = d2;
  FndBuf[6] = d1;
  FndBuf[7] = d0;
  //fnd_all_write();
}


void float_current_display(float fi)
{
  float rate;
  int i;
  char d0, d1, d2, d3, d4;
  if (fi < 100) rate = 100;
  else if (fi < 1000) rate = 10;
  else rate = 1;
  
  i = fi * rate;  
  i = hex2bcd5(i);

  d0 = i & 0x000F;
  i >>= 4;
  d1 = i & 0x000F;
  i >>= 4;
  d2 = i & 0x000F;
  i >>= 4;
  d3 = i & 0x000F;
  i >>= 4;
  d4 = i & 0x000F;
  
  // ZERO blanking
  if (d4 == 0) 
  {
    d4 = FND_SP;
    if (d3 == 0) 
    {
      d3 = FND_SP;
      if ((d2 == 0)&(rate != 100))
      {
        d2 = FND_SP;
        if ((d1 == 0)&(rate == 1)) d1 = FND_SP;
      }
    }
  }
  
  if (rate == 100) fnd_point_onoff(2, ON);
    else if (rate == 10) fnd_point_onoff(1, ON);  
  FndBuf[3] = d4;
  FndBuf[4] = d3;
  FndBuf[5] = d2;
  FndBuf[6] = d1;
  FndBuf[7] = d0;
  //fnd_all_write();
}
*/
