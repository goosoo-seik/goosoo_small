

// Include Standard LIB  files
#include "project.h"

#define GM12641		1

#ifdef GM12641
// LCD GM12641 Command Listing
#define LCD_DELAY       12
#define LCD_ON        	0x3F  // LCD display ON
#define LCD_OFF       	0x3E  // LCD display OFF
#define LCD_DP_START  	0xC0  // 0 - 63 : RAM line corresponding to top line of display 
#define LCD_PAGE_ADD  	0xB8  // 0 -  7 : Set RAM page address in page address register
#define LCD_COL_ADD   	0x40  // 0 - 63 : Set display RAM column address in page column register

#define PAGE_SIZE       64	// LCD page size 
#define LCD_LINE_SIZE   128	// LCD`line length
#define LCD_BUF_SIZE    LCD_LINE_SIZE * 8
#define DPBUF_SIZE      336   // 16 lines(font 0), 21 lines(font 1)
#define DP_SCAN_TIME    32    // 1 page scan time : 32 x 2 = 32 ms
// charactor q'ty per line
#define FONT6_LINE	21
#define FONT8_LINE	16
#define FONT16_LINE	16
// charactor q'ty per page
#define FONT6_PAGE	168	
#define FONT8_PAGE	64
#define FONT16_PAGE	64
#endif

unsigned char  bLCDbuf[LCD_BUF_SIZE + 8];
unsigned short sDPbuf[DPBUF_SIZE];
unsigned char  DPtype[DPBUF_SIZE];
unsigned short sDpBufSize;
char LCDpage;
short LcdLine;          // LCD line no
char  LcdBlock;         // LCD block no
unsigned char LoadIndex; // LCDbuf sacn index
short sCurPos;      // cursor position
BYTE  bCursorDP;    // cursor display on/off
BYTE  bLCDpage;
short sLCDpos;
short sDPstart;
char  FontWidth;
char  DPcode;
char  Blink;
char  LineBlink;
short sBlinkTime;
BYTE  bDpStep;
char  FontStyle;
//char  DebugDP;
char  DpUpdate;
char  DpScanNo;
char  CursorUse;
char  DumpDisplay; // 1: if col = 0, space is not display
char  ImageMode;
char  ImageNo;
unsigned short sOldCode;

char LcdCol, LcdRow;

// LCD module Reset by RESET signal
void LCD_hard_reset(void)
{
  pio_set( PIOB, LCD_A0|LCD_CS1|LCD_CS2|LCD_RW|LCD_RST ) ;
  pio_clear( PIOB, LCD_RST ) ;
  delay_us(50);
  pio_set( PIOB, LCD_RST ) ;
}

void delay_lcd (short time )
{
    short    i ;
    for ( i = 0 ;(i < time );i++ ) ;
}

/********************************/
/* LCD Data IN/OUT Functionc    */
/* For DataImage GM12641 series */
/********************************/
#ifdef GM12641
// 1 byte write to LCD module
void LCD_write(BYTE data)
{
  pio_set( PIOB, LCD_EN);
  data &= 0xFF;
  pio_set( PIOB, data );            // data out
  pio_clear( PIOB, data ^ 0xFF );   // data out
  delay_lcd(LCD_DELAY);
  pio_clear( PIOB, LCD_EN|LCD_CS1|LCD_CS2);
  delay_lcd(LCD_DELAY);
  pio_set( PIOB, LCD_EN);
}

// 1 byte CMD write to LCD module master
void LCD_cmd_write1(BYTE cmd)
{
  pio_clear( PIOB, LCD_EN);
  delay_lcd(LCD_DELAY);
  pio_clear( PIOB, LCD_CS2|LCD_A0|LCD_RW);
  pio_set( PIOB, LCD_CS1);
  delay_lcd(LCD_DELAY);
  LCD_write(cmd);
}

// 1 byte CMD write to LCD module slave
void LCD_cmd_write2(BYTE cmd)
{
  pio_clear( PIOB, LCD_EN);
  delay_lcd(LCD_DELAY);
  pio_clear( PIOB, LCD_CS1|LCD_A0|LCD_RW|LCD_EN);
  pio_set( PIOB, LCD_CS2);
  delay_lcd(LCD_DELAY);
  LCD_write(cmd);
}

// 1 byte data write to LCD module master
void LCD_data_write1(unsigned char data)
{
  pio_clear( PIOB, LCD_EN);
  delay_lcd(LCD_DELAY);
  pio_clear( PIOB, LCD_CS2|LCD_RW);
  pio_set( PIOB, LCD_CS1|LCD_A0);
  delay_lcd(2);
  LCD_write(data);
}

// 1 byte data write to LCD module slave
void LCD_data_write2(unsigned char data)
{
  pio_clear( PIOB, LCD_EN);
  delay_lcd(LCD_DELAY);
  pio_clear( PIOB, LCD_CS1|LCD_RW);
  pio_set( PIOB, LCD_CS2|LCD_A0);
  delay_lcd(2);
  LCD_write(data);
} 
void LCD_fill(char data)
{
  unsigned char page, lp;
 
  LCD_cmd_write1(LCD_DP_START);  
  for (page = 0; page < 8; page++)
   {
     LCD_cmd_write1(LCD_PAGE_ADD+page);
     LCD_cmd_write1(LCD_COL_ADD);
     for (lp = 0; lp < 64; lp++) LCD_data_write1(data);
   }
  
  LCD_cmd_write2(LCD_DP_START);
  for (page = 0; page < 8; page++)
   {
     LCD_cmd_write2(LCD_PAGE_ADD+page);
     LCD_cmd_write2(LCD_COL_ADD);
     for (lp = 0; lp < 64; lp++) LCD_data_write2(data);
   }
}

//LCD_GM12641_pagetest
void LCD_test(void)
{
  unsigned char page, lp, data;
  page = LCDpage;
  
  if (page < 8)
   {
     LCD_cmd_write1(LCD_DP_START);
     LCD_cmd_write1(LCD_PAGE_ADD+page);
     LCD_cmd_write1(LCD_COL_ADD);
     data = 1;
     for (lp = 0; lp < 64; lp++) 
     {
       LCD_data_write1(data);
       data = data << 1;
       if (data == 0) data = 1;
     }
   }
   else if (page < 16)
   {
     page &= 7;
     LCD_cmd_write2(LCD_DP_START);
     LCD_cmd_write2(LCD_PAGE_ADD+page);
     LCD_cmd_write2(LCD_COL_ADD);
     data = 0x80;
     for (lp = 0; lp < 64; lp++) 
     {
       LCD_data_write2(data);
       data = data >> 1;
       if (data == 0) data = 0x80;
     }
   }
   else if (page == 16)
   {
     LCD_fill(0xFF);
   }
   else if (page == 17)
   {
     LCD_fill(0);
   }
  if (++LCDpage > 17) LCDpage = 0;
}
#endif

unsigned char display_size(void)
{
  unsigned char dpsize;
  if (FontStyle == 0) dpsize = FONT6_LINE; 
   else  if (FontStyle == 1) dpsize = FONT8_LINE;
   else  if (FontStyle == 2) dpsize = FONT16_LINE;
 return dpsize;
}

unsigned char display_page(void)
{
  unsigned char page;
  if (FontStyle == 0) page = FONT6_PAGE; 
   else  if (FontStyle == 1) page = FONT8_PAGE;
   else  if (FontStyle == 2) page = FONT16_PAGE;
 return page;
}

// 1개의 6x7 font image를 bLCDbuf에 로드
void LCD_font6x7_load(short code, short curpos)
{
  short line, bline, pos, pos_h, lp, dpsize, mask;
  dpsize = LCD_LINE_SIZE / 6;
  line = curpos / dpsize; 
  pos = (curpos % dpsize) * 6;
  pos = line * LCD_LINE_SIZE + pos;
  pos_h = pos - (line * LCD_LINE_SIZE);
  
  // 남은 LCD width가 font size 보다 작으면 다음 line의 처음으로 위치 변경
  if (pos_h > (LCD_LINE_SIZE - 6)) 
  {
   // LCD line 끝 부분 clear
    for (lp = pos_h; lp < LCD_LINE_SIZE; lp++) bLCDbuf[pos++] = 0;
    if (++line > 3) line = 0;
    pos = line * LCD_LINE_SIZE;
  }
  // LCD buffer에 1 font image를 copy
  mask = 0;
  //if (sCurPos == curpos) if (Blink) mask = 0xFF;
  if (Blink) 
    if (LineBlink) 
    {
      bline = sCurPos / FONT6_LINE;
      if (line == bline) mask = 0xFF; 
    }      
    else if (sCurPos == curpos)  mask = 0xFF;
  for (lp = 0; lp < 6; lp++) 
    bLCDbuf[pos++] = ASCII_FONT6x7[code][lp] ^ mask;
  sLCDpos = pos;
}

// 1개의 8x16 font image를 bLCDbuf에 로드
void LCD_font8x16_load(short code, short curpos)
{
  short line, bline, pos, pos_h, lp, dpsize, mask;
  dpsize = LCD_LINE_SIZE >> 3;	// /8
  line = curpos / dpsize;
  pos = (curpos % dpsize) * 8;
  pos = line * LCD_LINE_SIZE * 2 + pos;
  pos_h = pos - (line * LCD_LINE_SIZE * 2);
  
  // 남은 LCD width가 font size 보다 작으면 다음 line의 처음으로 위치 변경
  if (pos_h > (LCD_LINE_SIZE - 8)) 
  {
    // LCD line 끝 부분 clear
    for (lp = pos_h; lp < LCD_LINE_SIZE; lp++) 
    {
      bLCDbuf[pos] = 0;
      bLCDbuf[pos + LCD_LINE_SIZE] = 0;
      pos++;
    }
    if (++line > 1) line = 0;
    pos = line * LCD_LINE_SIZE * 2;
  }

  // Inverse, Blink 처리
  mask = 0;
  if (Blink) 
  {
    if (LineBlink == 1) 
    {
      bline = sCurPos / FONT16_LINE;
      if (line == bline) mask = 0xFFFF; 
    }
    else if (LineBlink == 2)
    {
      if (DPtype[curpos] == BLINK) mask = 0xFFFF;
    }
    else if (CursorUse)
    {
      if (sCurPos == curpos)  mask = 0xFFFF;
    }
  }
  
  // LCD buffer에 1 font image를 copy
  for (lp = 0; lp < 8; lp++) 
  {
    bLCDbuf[pos] = ASCII_FONT8x16[code][lp] ^ mask;
    bLCDbuf[pos + LCD_LINE_SIZE] = ASCII_FONT8x16[code][lp + 8] ^ mask;
    pos++;
  }
  sLCDpos = pos;
}

// Decide font mask for inverse display
short fontmask_decide(short curpos)
{
  char linesize, blinkline, dpline;
  unsigned short mask;
  
  mask = 0;
  
  if (LineBlink == 0)
  {
    if (sCurPos == curpos)  mask = 0xFFFF;
  }
  else if (LineBlink == 1) 
  {
    linesize = display_size();
    blinkline = sCurPos / linesize;
    dpline = curpos / linesize;    
    if (blinkline == dpline) mask = 0xFFFF; 
  }
  else if (LineBlink == 2)
  {
   if ( DPtype[sCurPos] == BLINK) mask = 0xFFFF;
  }
  return mask;
}    
    
// 1개의 16x16 hangul font image를 bLCDbuf에 로드
void LCD_hangul_font16_load(unsigned short code, short curpos)
{
  short line, bline, pos, pos_h, lp, dpsize, data , mask;
  
  dpsize = LCD_LINE_SIZE >> 3;
  line = curpos / dpsize;
  pos = (curpos % dpsize) * 8;
  pos = line * LCD_LINE_SIZE * 2 + pos;
  pos_h = pos - (line * LCD_LINE_SIZE * 2);

  // 남은 LCD width가 font size 보다 작으면 다음 line의 처음으로 위치 변경
  if (pos_h > (LCD_LINE_SIZE - 16)) 
  {
    // LCD line 끝 부분 clear
    for (lp = pos_h; lp < LCD_LINE_SIZE; lp++) 
    {
      bLCDbuf[pos] = 0;
      bLCDbuf[pos + LCD_LINE_SIZE] = 0;
      pos++;
    }
    if (++line > 1) line = 0;
    pos = line * LCD_LINE_SIZE * 2;
  }
  
  // Inverse, Blink 처리
  mask = 0;
  if (Blink) 
  {
    if (LineBlink == 1) 
    {
      bline = sCurPos / FONT16_LINE;
      if (line == bline) mask = 0xFFFF; 
    }
    else if (LineBlink == 2)
    {
      if (DPtype[curpos] == BLINK) mask = 0xFFFF;
    }
    else if (CursorUse)
    {
      if (sCurPos == curpos)  mask = 0xFFFF;
    }
  }

  // LCD buffer에 1 font image를 copy  
  for (lp = 0; lp < 16; lp++) 
  {
    data = HANGUL_FONT16x16[code][lp] ^ mask;
    bLCDbuf[pos] = data & 0xFF;
    bLCDbuf[pos + LCD_LINE_SIZE] = data >> 8;
    pos++;
  }
  sLCDpos = pos;
}

void LCD_hangul_font_load(unsigned short code, short curpos)
{
  unsigned short hcode;
  if (code < 0xA0) LCD_font8x16_load(code, curpos);
   else if (sOldCode == 0) sOldCode = code;
   else 
      {
        hcode = (sOldCode - 0xB0) * 94 + (code - 0xA0);
	//code |= sOldCode;
	//code -= 0xB0A0;
        sOldCode = 0;
	LCD_hangul_font16_load(hcode, curpos-1);
      }
}
         
// if display buffer over, 1 line scroll
void display_buffer_scroll(char size)
{
  char line, lp, lp0;
  short s_add, d_add;
  line = sDpBufSize / size - 1;
  for (lp = 0; lp < line; lp++)
  {
    d_add = lp * size;
    s_add = d_add + size;
    for (lp0 = 0; lp0 < size; lp0++)
    {
      sDPbuf[d_add + lp0] = sDPbuf[s_add + lp0];
    }
  }
  d_add = sDpBufSize - size;
  for (lp = 0; lp < size; lp++) sDPbuf[d_add + lp] = 0;
}

void goto_cursor(unsigned char x,unsigned char y)
{
  short pos, dpsize, dppage;
  dpsize = display_size();
  dppage = display_page();
  pos = dpsize * y + x;
  if (pos > dppage) pos = dppage;
  sCurPos = pos;
}

void putchar_pos(short pos, unsigned char code)
{
  sDPbuf[pos] = code;
}

char find_cursor_vpos(void)
{
  char line;
  line = sCurPos / display_size();
  return line;
}

void set_cursor_h(unsigned char x)
{
  short pos, dpsize;
  dpsize = display_size();
  if (x >= dpsize) x = dpsize;
  pos = sCurPos / dpsize;
  sCurPos = pos * dpsize + x;
}
   
void cursor_move_left(void)
{
  char dpsize, remain;
  dpsize = display_size();
  remain = sCurPos % dpsize;
  if (remain) sCurPos--;
  DpUpdate = 1;
}

void cursor_move_up(void)
{
  char dpsize;
  
  dpsize = display_size();
  if (sCurPos < dpsize) 
  {
    if (sDPstart >= dpsize) sDPstart = sDPstart - dpsize;
  }
  else sCurPos = sCurPos - dpsize;
  DpUpdate = 1;
}

void cursor_move_right(void)
{
  char dpsize, page;
  
  dpsize = display_size();
  page = display_page();
  
  if (++sCurPos >= page) 
    {
      sDPstart = sDPstart + dpsize;
      //display buffer의 끝이라면, 모든 라인을 스크롤 업
      if (sDPstart >= (sDpBufSize - page)) 
      {
        display_buffer_scroll(dpsize);
        sDPstart = sDpBufSize - page;
      }
      sCurPos = page - dpsize;
    }
  DpUpdate = 1;
}

// 커서를 한줄 아래로 내리기
void cursor_move_down(void)
{
  char dpsize, page;
  
  dpsize = display_size();
  page = display_page();
  
  sCurPos = sCurPos + dpsize;
  // 페이지 바닥이면, 페이지 전체를 한줄 아래로 이동
  if (sCurPos >= page) 
  {
    sCurPos = sCurPos - dpsize;
    sDPstart = sDPstart + dpsize;
    //display buffer의 끝이라면, 모든 라인을 스크롤 업
    if (sDPstart >= (sDpBufSize - page)) 
    {
      display_buffer_scroll(dpsize);
      sDPstart = sDpBufSize - page;
    }
  }
  DpUpdate = 1;
}

void cursor_move_home(void)
{
  sCurPos = 0;
  DpUpdate = 1;
}

void cursor_move_start(void)
{
  sCurPos = 0;
  sDPstart = 0;
  DpUpdate = 1;
}
   
void LCD_line_feed(void)
{
  char line, dpsize;
  dpsize = display_size();
  line = sCurPos / dpsize;
  sCurPos = line * dpsize;
  cursor_move_down();
  cursor_move_left();
}

void LCD_carrige_return(void)
{
  char line, dpsize;
  dpsize = display_size();
  line = sCurPos / dpsize;
  sCurPos = line * dpsize;
  cursor_move_left();
}

/****************************************************/
/*  Charactor store to display buffer(DPbuf[])      */
/* b0-11: ASCII code                                */
/* b12-15: Charactor attribute                      */
/* 0: 8x6 ASCII FONT                                */
/* 1: 16x8 ASCII FONT                               */
/****************************************************/
char CodeOdd, dpsize;
void putchar_dp(unsigned short code)
{
  DpUpdate = 1;
  if (code == '\r')  //0x0D 
  {
    CodeOdd = 0;
    LCD_carrige_return();
  }
  else if (code == '\n') 
  {
    CodeOdd = 0;
    LCD_line_feed();
  }
  else if (code < 0x20)		// control code 는 표시하지 않음
  {
    CodeOdd = 0;
  }
  else if (code == ' ')	// 스페이스 처리		 
  {
    // DumpDisplay 이면 첫 스페이스를 무시
    CodeOdd = 0;
    dpsize = display_size();
    if ((!DumpDisplay)|(code != 0x20)|((sCurPos % dpsize)!= 0)) 
    {
      sDPbuf[sDPstart + sCurPos] = code;
      cursor_move_right();
    }
  }   
  else if ((code < 0xA0)|(FontStyle != 2))
  { 
    CodeOdd = 0;
    sDPbuf[sDPstart + sCurPos] = code;
    cursor_move_right();
  }
  // 한글모드이고 code가 한글 영역인 경우
  else if ((FontStyle == 2)&(code >= 0xA0))
  {
    if (CodeOdd == 0) 
    {
      // 한글전각/영문반각 표시모드일때 
      // 라인의 남은 칸이 반각 한칸뿐이면  스페이스를 삽입
      if ((sCurPos % FONT16_LINE) == (FONT16_LINE-1)) 
      {
	sDPbuf[sDPstart + sCurPos] = 0x20;
	cursor_move_right();
      }
      CodeOdd = 1;
      sDPbuf[sDPstart + sCurPos] = code;
      cursor_move_right();
    }
    else 
    {
      CodeOdd = 0;
      sDPbuf[sDPstart + sCurPos] = code;
      cursor_move_right();
    }
  }
}

// bLCDbuf[] clear
void LCDbuf_clear(void)
{
  short lp;
  for (lp = 0; lp < LCD_BUF_SIZE; lp++) bLCDbuf[lp] = 0;
  sLCDpos = 0;
  sCurPos = 0;
}

// LCD module ON/OFF
void LCD_onoff(BYTE onoff)
{
  if (onoff == ON) onoff = LCD_ON; else onoff = LCD_OFF;
  LCD_cmd_write1(onoff);
  LCD_cmd_write2(onoff);
}

// Display Type clear
void DPtype_clear(void)
{
  short lp;
  for (lp = 0; lp < sDpBufSize; lp++) DPtype[lp] = 0;
  LineBlink = 0;
}

void dp_type_assign(char type, short start, short end)
{
  short size, lp;
  size = display_page();
  if (end > size) end = size;
  for (lp = start; lp <= end; lp++) DPtype[lp] = type;
}

const unsigned char CURSOR_AREA[] = 
{
//  11, 12,   // CC/CV
//  14, 14,   // P/N
  20, 23,     // VOLT
  26, 30,     // AMP
//  52, 59,     // SET TIME
  46, 47      // MENU
};

void cursor_block(char no)
{
  char start, end;
  no <<= 1;
  start =CURSOR_AREA[no];
  no++;
  end = CURSOR_AREA[no];
  DPtype_clear();
  dp_type_assign(BLINK, start, end);
  CursorUse = 0;
  LineBlink = 2;
}

// bLCDbuf[] & bDPbuf[] clear
void DPbuf_clear(void)
{
  short lp;
  for (lp = 0; lp < sDpBufSize; lp++) 
  {
    sDPbuf[lp] = 0;
    DPtype[lp] = 0;
  }
  LCDbuf_clear();
  cursor_move_start();
  sOldCode = 0;
  CodeOdd = 0;
}

// LCD module GM123210 초기화
void LCD_init(void)
{
  pio_set(PIOB, LCD_RW|LCD_EN|LCD_A0|LCD_LP);
  pio_clear(PIOB, LCD_RST|LCD_CS1|LCD_CS2);
  delay_us(5000);
  pio_set(PIOB, LCD_RST);
  delay_us(1000);
  LCD_onoff(ON);
  LCDbuf_clear();
  display_mode(0);
} 

// bLCDbuf[] & bDPbuf[] clear
void screen_clear(void)
{
  short lp;
  for (lp = 0; lp < sDpBufSize; lp++) 
  {
    sDPbuf[lp] = 0;
    DPtype[lp] = 0;
  }
  cursor_move_start();
  sOldCode = 0;
  CodeOdd = 0;
}

void display_mode(char mode)
{
  FontStyle = mode;
  sDpBufSize = display_page();
  DPbuf_clear(); 
  sOldCode = 0;
  CodeOdd = 0;
}  

/*****************************************/
/* code에 해당하는 이미지를 LCD에 표시   */ 
/* fonr 0: 8x6 ASCII FONT                */ 
/* font 1: 16x8 ASCII FONT               */ 
/*****************************************/
//unsigned short testcode;
void putchar_lcd(unsigned short code, short curpos)
{
  if (FontStyle == 0) LCD_font6x7_load(code, curpos);
   else if (FontStyle== 1) LCD_font8x16_load(code, curpos);
   else if (FontStyle== 2) LCD_hangul_font_load(code, curpos);
}

// bDPbuf[]중 1line의 font image를 bLCDbuf에 로드
void LCD_line_load(char line, char font)
{
  short dpsize, add, start, end;
  unsigned short code;
  //  if font = 0 , 4 line load, else 2 line
  if (font != 0) line &= 1; else line &= 3;
  dpsize = display_size();
 
  start = dpsize * line;
  end = start + dpsize;
  
  for (add = start; add < end; add++)
  { 
    code = sDPbuf[sDPstart + add];
    //font = code >> 12;
    //code &= 0xFF;
    putchar_lcd(code, add);
  }
}

/********************************************/
/* 1/16 page의 font image를 bLCDbuf에 로드  */
/* For 6x8 font only(FontStyle = 0)         */
/********************************************/
void LCD_line_load_6x8(void)
{
  short add, start, end;
  
  if (!LcdBlock) 
   {
    start = LcdLine * FONT6_LINE;
    end = start + 11;
   }
   else if (LcdBlock == 1) 
   {
    start = (LcdLine * FONT6_LINE) + 11;
    end = start + 10;
   }
  
  for (add = start; add < end; add++) 
    putchar_lcd(sDPbuf[sDPstart + add], add);
}

/**********************************************/
/* 1/16 page의 font image를 bLCDbuf에 로드    */
/* For 8x16 & 16x16 font (FontStyle = 1 or 2) */
/**********************************************/ 
void LCD_line_load_8x16(void)
{
  unsigned char add;
  
  add = LoadIndex;
  
  putchar_lcd(sDPbuf[sDPstart + add], add);
  if (++add < FONT8_PAGE) 
        putchar_lcd(sDPbuf[sDPstart + add], add);
  if (++add < FONT8_PAGE) 
        putchar_lcd(sDPbuf[sDPstart + add], add);
  if (++add < FONT8_PAGE) 
        putchar_lcd(sDPbuf[sDPstart + add], add);
  add++;
  LoadIndex = add;
}

void cursor_blink_control(void)
{
  if (++sBlinkTime > SEC_1/2) 
  {
    sBlinkTime = 0;
    Blink ^= 1;
    if (!ImageMode) DpUpdate = 1;
  }
}

/********************************************/
/*  LCD module control Functions            */
/*  For 128x64 Graphic LCD Module GM12641   */
/********************************************/
#ifdef GM12641
unsigned char ImageWidth;
unsigned char ImageHeight;
/**********************************************/
/* 1/16 page의 graphic image를 bLCDbuf에 로드 */
/* For Graphic Image only                     */
/**********************************************/
void LCD_image_scan(char image)
{
  short add, start, end, imsi;
  const unsigned char* ptr = 0;
  
  ImageWidth = 122;
  ImageHeight = 4;
  if (LcdBlock < 2)
  {
   if (LcdBlock) start = 61; else start = 0;
   start = start + (LcdLine * LCD_LINE_SIZE);
   end = start + 61;
   
   imsi = LcdLine * ImageWidth;
   if (LcdBlock) imsi = imsi + 61;
   ptr = pImagePtr[image];
   ptr = ptr + imsi;
  
   for (add = start; add < end; add++) bLCDbuf[add] = *ptr++;
  };
}

/********************************************/
/*  Trasnfer video data for LCD scan        */
/*  Data transfer to LCD module from LCDbuf */
/*  For 128x64 Graphic LCD Module GM12641   */
/*  1/16 page transfer per one execute      */
/*  Scaning duration = 16 x 2 = 32 ms       */
/********************************************/ 
char LcdScanNo;
char LcdScanLine;
short LcdPos;
void LCD_GM12641_scan_new(void)
{
  unsigned char lp, page;
  
  page = (unsigned char)(LCD_PAGE_ADD + LcdScanLine);  
  
  switch(LcdScanNo) 
  {
  case 0:
    LcdPos = LcdScanLine * LCD_LINE_SIZE;
    LCD_cmd_write1(LCD_DP_START);
    LCD_cmd_write1(page);
    LCD_cmd_write1(LCD_COL_ADD);    // column = 0
    for (lp = 0; lp < 32; lp++) 
      LCD_data_write1(bLCDbuf[LcdPos++]);
    return;
    
  case 1:
    for (lp = 0; lp < 32; lp++) 
      LCD_data_write1(bLCDbuf[LcdPos++]);
    return;

  case 2:
    LcdPos = LcdScanLine * LCD_LINE_SIZE + 64;
    LCD_cmd_write2(LCD_DP_START);
    LCD_cmd_write2(page);
    LCD_cmd_write2(LCD_COL_ADD);
    for (lp = 0; lp < 32; lp++) 
      LCD_data_write2(bLCDbuf[LcdPos++]);
    return;
    
  case 3:
    for (lp = 0; lp < 32; lp++) 
      LCD_data_write2(bLCDbuf[LcdPos++]);
    return;
  }
}

void display_scan(void)
{ 
 //if (CursorUse) cursor_blink_control(); else Blink = 0;
 
 cursor_blink_control();
 //if ((DpScanNo == 0)&(DpUpdate != 0))
 //if (DpUpdate) 
 if (DpScanNo == 0)
  {
   DpUpdate = 0;
   LcdScanNo = 0;
   LcdBlock = 0;
   LcdLine = 0; 
   LcdScanLine = 0;
   DpScanNo = 32;
   LoadIndex = 0;
   sOldCode = 0;
  }  
  //else 
  {
   if (DpScanNo > 16)
   {
    DpScanNo--;
    if (ImageMode) LCD_image_scan(ImageNo); 
     else if (!FontStyle) LCD_line_load_6x8();
     else LCD_line_load_8x16();
   }
   else if (DpScanNo > 0)
   {
     DpScanNo--;
     //LCD_GM12641_scan();
   }
   
   LCD_GM12641_scan_new();
   if (++LcdScanNo > 3) 
   {
     LcdScanNo = 0;
     if (++LcdScanLine > 7) LcdScanLine = 0;
   }
     
   if (++LcdBlock > 1) 
    {
     LcdBlock = 0;
     if (++LcdLine > 7) LcdLine = 0;
    };
  };
}
#endif

