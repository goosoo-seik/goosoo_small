

#define BYTE  unsigned short
// Include Standard LIB  files
#include "project.h"

#define USART_INTERRUPT_LEVEL		7

int iCom1Speed;
//* \fn    AT91F_US_Baudrate
//* \brief Calculate the baudrate
//* Standard Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )

// USART1 Receiver buffer
#define RX_BUFFER_SIZE1 256
BYTE  rx_buffer1[RX_BUFFER_SIZE1];
short rx_wr_index1,rx_rd_index1,rx_counter1;
// This flag is set on USART1 Receiver buffer overflow
char ErrUsart1;


// USART1 Transmitter buffer
#define TX_BUFFER_SIZE1 1024	//256
BYTE  tx_buffer1[TX_BUFFER_SIZE1];
short tx_wr_index1,tx_rd_index1,tx_counter1;

//*------------------------- Internal Function --------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : Usart1_c_irq_handler
//* Object              : C handler interrupt function calAT91B_LED by the interrupts
//*                       assembling routine
//* Input Parameters    : <RTC_pt> time rtc descriptor
//* Output Parameters   : increment count_timer1_interrupt
//*----------------------------------------------------------------------------

__ramfunc void Usart1_c_irq_handler(void)
{
unsigned char  data;
        //GRNlamp(ON);
	AT91PS_USART USART_pt = AT91C_BASE_US1;
	unsigned int status;
	//* get Usart status register
	status = USART_pt->US_CSR;
	if ( status & AT91C_US_RXRDY)   // (DBGU) RXRDY Interrupt
         {    
          data = AT91F_US_GetChar(USART_pt);
         // AT91F_US_PutChar (USART_pt, data);    // echo return
          rx_buffer1[rx_wr_index1]=data;
          ++rx_wr_index1;
          rx_wr_index1 &= 0xFF;
          if (rx_wr_index1 == rx_rd_index1) ErrUsart1 = 'B';
	}
        
        if ( status & AT91C_US_TXRDY)
         {
          if ( tx_rd_index1 != tx_wr_index1 ) 
           {
            AT91F_US_PutChar (USART_pt, tx_buffer1[tx_rd_index1]);
            if (++tx_rd_index1 == TX_BUFFER_SIZE1) tx_rd_index1=0;
           }
          else 
           AT91F_US_DisableIt(AT91C_BASE_US1, AT91C_US_TXRDY);
        }
	if ( status & AT91C_US_OVRE) {
		//* clear US_RXRDY
		 AT91F_US_GetChar(USART_pt);
                 ErrUsart1 = 'O';
		 //AT91F_US_PutChar (USART_pt, 'O');
	}

	//* Check error
	if ( status & AT91C_US_PARE) {
                 ErrUsart1 = 'P';
		 //AT91F_US_PutChar (USART_pt, 'P');
	}

	if ( status & AT91C_US_FRAME) {
                 ErrUsart1 = 'P';
		 //AT91F_US_PutChar (USART_pt, 'F');
	}

	if ( status & AT91C_US_TIMEOUT){
		USART_pt->US_CR = AT91C_US_STTTO;
		ErrUsart1 = 'T';
                //AT91F_US_PutChar (USART_pt, 'T');
	}
	//* Reset the satus bit
	 USART_pt->US_CR = AT91C_US_RSTSTA;
}

// Get a character from the USART1 Receiver buffer
char getchar1(void)
{
 char data;
      while (rx_rd_index1 == rx_wr_index1);
      data=rx_buffer1[rx_rd_index1];
      if (++rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
return data;
}

void putchar1(char c)
{
      AT91PS_USART USART_pt = AT91C_BASE_US1;
      unsigned int status;
      //* get Usart status register
      status = USART_pt->US_CSR;
      if ((tx_rd_index1 != tx_wr_index1) | ((status & AT91C_US_TXRDY) == 0 ))
       {
        tx_buffer1[tx_wr_index1]=c;
        if (++tx_wr_index1 == TX_BUFFER_SIZE1) tx_wr_index1=0;
        AT91F_US_EnableIt(AT91C_BASE_US1, AT91C_US_TXRDY);
         // AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_TXRDY);
       }
      else
      {
       AT91F_US_PutChar (USART_pt, c); 
      }
}


//*-------------------------- External Function -------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : Usart_init
//* Object              : USART initialization
//* Input Parameters    : none
//* Output Parameters   : TRUE
//*----------------------------------------------------------------------------
void Usart1_init ( void )
{
	AT91PS_USART COM1 = AT91C_BASE_US1;

        //* Configure PIO controllers to periph mode
 	AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA,
 		((unsigned int) AT91C_PA5_RXD1    ) |
 		((unsigned int) AT91C_PA6_TXD1    ), // Peripheral A
                 0); // Peripheral B
 	//	((unsigned int) AT91C_PA8_RTS1    ) |
 	//	((unsigned int) AT91C_PA9_CTS1    ), // Peripheral A
 	//	0); // Peripheral B

   	// First, enable the clock of the USART
    	AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_US1 ) ;
	// Usart Configure
        AT91F_US_Configure (COM1, AT91B_MCK, AT91C_US_ASYNC_MODE, iCom1Speed, 0);

	// Enable usart
	COM1->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;

    //* Enable USART IT error and RXRDY
    	AT91F_US_EnableIt(COM1,AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_RXRDY);

    	//* open Usart 1 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_US1, USART_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, Usart1_c_irq_handler);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US1);
}

void com1_mode_set( int rate, char parity, char bit, char stop)
{
  AT91PS_USART COM1 = AT91C_BASE_US1;
  unsigned int mode, bit_mode, stop_mode, p_mode;

  if (bit == 8) bit_mode = AT91C_US_CHRL_8_BITS;
    else if (bit == 7) bit_mode = AT91C_US_CHRL_7_BITS;
    
  if (stop == 1) stop_mode = AT91C_US_NBSTOP_1_BIT;
    else if (stop == 2) stop_mode = AT91C_US_NBSTOP_2_BIT;
    
  if ((parity == 'N')|(parity == 'n')) p_mode = AT91C_US_PAR_NONE;
    else if ((parity == 'E')|(parity == 'e')) p_mode = AT91C_US_PAR_EVEN;
    else if ((parity == 'O')|(parity == 'o')) p_mode = AT91C_US_PAR_ODD;
 
  mode = AT91C_US_USMODE_NORMAL + stop_mode + p_mode + bit_mode + AT91C_US_CLKS_CLOCK;

        // Usart Configure
        AT91F_US_Configure (COM1, AT91B_MCK, mode, rate, 0);
	// Enable usart
	COM1->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
        //* Enable USART IT error and RXRDY
    	AT91F_US_EnableIt(COM1,AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_RXRDY);
    	//* open Usart 1 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_US1, USART_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, Usart1_c_irq_handler);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US1);
        //printf("\nCOM0:%6d,%1d,%1d,", rate, bit, stop); 
        //putchar_dp(parity);
}

void com1_buffer_clear(void)
{
  rx_rd_index1 = 0;
  rx_wr_index1 = 0;
  tx_rd_index1 = 0;
  tx_wr_index1 = 0;
}


//*----------------------------------------------------------------------------
//* UART 디버그 출력 (2026-10 추가)
//*   REMOTE_Tx (PA6 / TxD1, MCU 90번 핀) → Nu-Link VCOM Rx → PC 터미널
//*   COM1 설정은 기존 그대로 (iCom1Speed, 기본 38400 8N1) - 다시 설정하지 않음
//*
//*   ※ 스택이 매우 작음 (CSTACK 400 바이트, at91SAM7X256_FLASH.xcl)
//*     → 라이브러리 printf/vsprintf 대신 자체 포맷터(dbg_printf) 사용, 큰 지역 버퍼 없음
//*   - 줄을 쓰기 전에 dbg_reserve() 로 송신 버퍼 자리를 확인. 자리가 없으면
//*     그 줄은 통째로 버리고 DbgDropCount 증가 (putchar1 은 넘쳐도 덮어쓰기 때문)
//*   - 기다리지 않음 → 2ms 루프 / 워치독에 영향 없음
//*   - 인터럽트 핸들러 안에서는 호출하지 말 것
//*----------------------------------------------------------------------------
char DbgOutEnable = 1;              // 0 이면 디버그 출력 끔
unsigned short DbgDropCount;        // 버퍼 부족으로 버린 줄 수

// 송신 버퍼에 n 글자 넣을 자리가 있으면 1
char dbg_reserve(short n)
{
  short free;
  if (!DbgOutEnable) return 0;
  free = tx_rd_index1 - tx_wr_index1 - 1;
  if (free < 0) free += TX_BUFFER_SIZE1;
  if (n > free) { DbgDropCount++; return 0; }
  return 1;
}

void dbg_putc(char c)
{
  putchar1(c);
}

// 문자열 출력 (자리 확인은 하지 않음 - dbg_reserve 뒤에 사용)
void dbg_str(const char *s)
{
  while (*s) putchar1(*s++);
}

// 부호 없는 정수 출력 (지역 버퍼 11 바이트)
void dbg_uint(unsigned int v)
{
  char buf[11];
  char n = 0;
  do { buf[n++] = (char)('0' + (v % 10)); v /= 10; } while (v && (n < 10));
  while (n) putchar1(buf[--n]);
}

// 부호 있는 정수 출력
void dbg_int(int v)
{
  if (v < 0) { putchar1('-'); dbg_uint((unsigned int)(-v)); }
  else dbg_uint((unsigned int)v);
}

// 자리 확인 + 문자열 한 줄 출력
char dbg_puts(const char *s)
{
  short n = 0;
  while (s[n]) n++;
  if (!dbg_reserve(n)) return 0;
  dbg_str(s);
  return 1;
}

//*----------------------------------------------------------------------------
//* dbg_printf : printf 처럼 쓰는 COM1 디버그 출력
//*   vsprintf 를 쓰지 않는 자체 포맷터 (스택 사용 약 100 바이트 이내)
//*   - 한 줄을 정적 버퍼(DbgLine)에 만든 뒤, 송신 버퍼 자리가 있으면 통째로 보냄
//*     자리가 없으면 그 줄은 버리고 DbgDropCount 증가. DBG_LINE_MAX 를 넘는 글자는 잘림
//*   - 지원: %d %i %u %x %X %c %s %f %%   플래그 '-' '0'   폭(%5d)   정밀도(%.2f)
//*           'l' 'h' 는 무시 (ARM 에서 int == long)
//*   - %f 정밀도 기본 6자리(printf 와 같음), 최대 6자리. 정수부는 약 42억까지. 0.5 는 올림
//*   - 정적 버퍼를 쓰므로 인터럽트 핸들러 안에서 호출하지 말 것
//*----------------------------------------------------------------------------
#include <stdarg.h>

#define DBG_LINE_MAX    160         // dbg_printf 한 번에 만들 수 있는 최대 글자 수

static char  DbgLine[DBG_LINE_MAX];
static short DbgLen;

static void dbg_lput(char c)
{
  if (DbgLen < DBG_LINE_MAX) DbgLine[DbgLen++] = c;
}

// 부호 + 본문(s, n 글자)을 폭/정렬에 맞춰 출력
static void dbg_lfield(char sign, const char *s, short n, short width, char left, char zero)
{
  short pad = width - n - (sign ? 1 : 0);
  if (!left && !zero) while (pad-- > 0) dbg_lput(' ');
  if (sign) dbg_lput(sign);
  if (!left && zero) while (pad-- > 0) dbg_lput('0');
  while (n-- > 0) dbg_lput(*s++);
  if (left) while (pad-- > 0) dbg_lput(' ');
}

// 부호 없는 정수를 버퍼 끝(end)부터 거꾸로 채움. 시작 위치를 돌려줌
static char *dbg_utoa(char *end, unsigned long v, char base, char upper)
{
  const char *dig = upper ? "0123456789ABCDEF" : "0123456789abcdef";
  do { *--end = dig[v % base]; v /= base; } while (v);
  return end;
}

int dbg_printf(const char *fmt, ...)
{
  va_list ap;
  char  tmp[24];                    // 숫자 변환용 (32비트 정수 + 소수부 6자리 충분)
  char  *p, *end = tmp + sizeof(tmp);
  char  c, sign, left, zero;
  short width, prec;
  long  sv;
  unsigned long uv, scale;
  double dv;

  if (!DbgOutEnable) return 0;
  DbgLen = 0;
  va_start(ap, fmt);
  while ((c = *fmt++) != 0)
  {
    if (c != '%') { dbg_lput(c); continue; }

    // 플래그
    left = 0; zero = 0;
    for (;;)
    {
      if (*fmt == '-') left = 1;
      else if (*fmt == '0') zero = 1;
      else break;
      fmt++;
    }
    // 폭
    width = 0;
    while (*fmt >= '0' && *fmt <= '9') width = (short)(width * 10 + (*fmt++ - '0'));
    // 정밀도
    prec = -1;
    if (*fmt == '.')
    {
      fmt++; prec = 0;
      while (*fmt >= '0' && *fmt <= '9') prec = (short)(prec * 10 + (*fmt++ - '0'));
    }
    // 길이 (무시)
    while (*fmt == 'l' || *fmt == 'h') fmt++;

    sign = 0;
    switch (c = *fmt++)
    {
    case 'd':
    case 'i':
      sv = va_arg(ap, int);
      if (sv < 0) { sign = '-'; uv = 0UL - (unsigned long)sv; } else uv = (unsigned long)sv;
      p = dbg_utoa(end, uv, 10, 0);
      dbg_lfield(sign, p, (short)(end - p), width, left, zero);
      break;

    case 'u':
      p = dbg_utoa(end, (unsigned long)va_arg(ap, unsigned int), 10, 0);
      dbg_lfield(0, p, (short)(end - p), width, left, zero);
      break;

    case 'x':
    case 'X':
      p = dbg_utoa(end, (unsigned long)va_arg(ap, unsigned int), 16, (char)(c == 'X'));
      dbg_lfield(0, p, (short)(end - p), width, left, zero);
      break;

    case 'c':
      tmp[0] = (char)va_arg(ap, int);
      dbg_lfield(0, tmp, 1, width, left, 0);
      break;

    case 's':
      p = va_arg(ap, char *);
      if (p == 0) p = "(null)";
      for (sv = 0; p[sv] && (prec < 0 || sv < prec); sv++);
      dbg_lfield(0, p, (short)sv, width, left, 0);
      break;

    case 'f':
      dv = va_arg(ap, double);
      if (prec < 0) prec = 6;
      if (prec > 6) prec = 6;
      for (scale = 1, sv = 0; sv < prec; sv++) scale *= 10;
      if (dv < 0) { sign = '-'; dv = -dv; }
      dv += 0.5 / (double)scale;                    // 반올림
      if (dv > 4294967295.0) dv = 4294967295.0;     // 정수부 범위 제한
      uv = (unsigned long)dv;                       // 정수부
      p = end;
      if (prec > 0)
      {
        unsigned long fr = (unsigned long)((dv - (double)uv) * (double)scale);
        if (fr >= scale) fr = scale - 1;
        for (sv = 0; sv < prec; sv++) { *--p = (char)('0' + fr % 10); fr /= 10; }
        *--p = '.';
      }
      p = dbg_utoa(p, uv, 10, 0);
      dbg_lfield(sign, p, (short)(end - p), width, left, zero);
      break;

    case '%':
      dbg_lput('%');
      break;

    case 0:                         // 문자열이 '%' 로 끝남
      fmt--;
      break;

    default:                        // 모르는 형식은 그대로 출력
      dbg_lput('%');
      dbg_lput(c);
      break;
    }
  }
  va_end(ap);

  if (DbgLen == 0) return 0;
  if (!dbg_reserve(DbgLen)) return 0;
  for (sv = 0; sv < DbgLen; sv++) putchar1(DbgLine[sv]);
  return DbgLen;
}

// 부팅 메시지 (main 에서 com1_mode_set 다음에 호출)
void dbg_init(void)
{
  DbgDropCount = 0;
  dbg_printf("\r\n\r\n[DBG] GooSoo rectifier boot, COM1 %d 8N1\r\n", iCom1Speed);
}
