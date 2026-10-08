//===============================================================
// CSLab_Rectifier_reset.c
// 2026-10-08 추가: 리셋 원인 보고 / 웜 리스타트 (Warm Restart)
//   설계: 노션 "1.1 DCS 웜 리스타트 설계 구조" 1~6 항 (7, 8 항은 tools/dcs_uart_viewer.html)
//   스위치/시간 설정: CSLab_Rectifier_Main.h (WARM_RESTART, WARM_*, RST_*)
//===============================================================
//
// [1] warm / cold 구분
//   cold : 일반 부팅. 출력 0 -> 부팅 안내 -> PLC RUN 으로 재기동 -> 소프트스타트 (기존 동작)
//   warm : 리셋 직전 출력/운전 상태로 바로 복귀 (PLC 재기동, 소프트스타트 없음)
//   ※ 리셋 펄스 동안은 하드웨어가 출력을 끊음 (출력 래치 74HCT574 OE = DC24_RDY, 4.7k 풀업)
//     -> PLC 출력(RUN/REMOTE...)과 DA_EN 이 리셋 동안 꺼짐. warm 은 끊긴 뒤 바로 이어서 운전하는 기능
//
// [2] 리셋 종류(RSTC_SR RSTTYP)별 처리
//   0 전원투입  : RAM 이 사라짐 -> 직전 상태 없음 -> cold
//   2 워치독    : RAM 유지 -> 조건 맞으면 warm
//   3 소프트웨어: 현재 코드에서는 발생하지 않음 -> cold
//   4 NRST      : RAM 유지 (DS1233 3.3V 순간 저하 / 리셋선 노이즈 / JTAG) -> 조건 맞으면 warm
//                 ※ 워치독 리셋도 NRST 를 내보내므로 DS1233 이 펄스를 늘이면 4 로 보일 수 있음
//   5 저전압    : 전압이 무너져 RAM 을 믿을 수 없음 -> cold
//
// [3] warm 조건 (하나라도 아니면 cold, 이유를 [RST] 에 표시)
//   리셋 종류 2 또는 4 / RAM 직전 상태 유효(매직+체크값) / 메인루프 실행 중 리셋(stage != 0)
//   / 직전 운전 중 / 직전 REMOTE / 직전 고장(TotalError) 없음 / 연속 웜 횟수 < WARM_MAX_COUNT
//   연속 횟수: 웜 리스타트마다 +1, 웜 후 WARM_CLEAR_SEC 계속 운전하면 0, cold 부팅하면 0
//
// [4][5] UART 보고 (COM1, dbg_printf, 부팅 때 RTC 초기화 직후)
//   [RST] cold reset : <리셋 종류> <리셋 위치> / not warm: <이유>
//   [RST] warm reset : <리셋 종류> <리셋 위치>
//   [RST] info boot=.. type=.. rsr=0x........ rtc=YYYY-MM-DD hh:mm:ss warm=n/max why=..
//   [RST] prev run=.. user=.. mode=.. fault=.. alarm=.. com=.. amp=.. set=.. out=.. ...   (RAM 직전 상태)
//   [RST] fix dacclr=1 input=1 ...  (웜 복귀 보완 스위치 WARM_FIX_* 켜짐/꺼짐)
//   [RST] opt rearm=1 ...           (리모트 기동/설정전류 0 처리 스위치, armwait = 재입력 대기)
//   [RST] warm stable : ...   (warm 이면 WARM_REPORT_SEC 초 뒤 복귀 결과)
//   [RST] last ...            (RST_REPEAT_SEC 마다 요약 반복, 뷰어를 늦게 연결해도 확인 가능)
//
// [6] RAM 에 저장하는 직전 상태 (ResetCtx, __no_init : 시작 코드가 0 으로 지우지 않음)
//   메인루프 매 스캔(2ms) reset_ctx_update() 로 갱신. FLASH 는 쓰기 시간/수명 때문에 사용 안 함
//   운전 여부, LOCAL/REMOTE, CC/CV, 극성, 고장/알람, 출력 래치 값, DAC 코드 2채널, 운전시간,
//   측정 전류/전압, 설정 전류/전압, 출력 지령 전류/전압 + 체크값(XOR)
//   실행 위치(stage)는 ResetStage 에 따로 (메인루프 함수마다 1회 쓰기, 체크값 재계산 없음)
//
// [부팅 순서] (main.c)
//   reset_capture()        main() 첫 줄 : RSTC_SR, 직전 상태 확보, warm 판정
//                          warm 이면 pio_init() 전에 DA_CLR 을 High 로 -> DAC 클리어 방지
//   pio_init()
//   warm_restart_early()   DAC 코드 -> 출력 래치 -> DC24_RDY ON (가능한 빨리 출력 복귀)
//   ... LCD, data_init(), backup_data_read() (FLASH 설정값 로드)
//   warm_restart_late()    운전 상태/제어 상태 복원, 입력 디바운스 미리 채움
//   ... UART, RTC
//   reset_report_boot()    [RST] 출력
//===============================================================

// Include Standard LIB  files
#include "project.h"

#define RCTX_MAGIC        0x43545821      // 직전 상태 유효 표시
#define RSTAGE_KEY        0x5A000000      // ResetStage 유효 표시 (상위 8비트)
#define WARM_MAGIC        0x5741524D      // 'WARM' : WarmCount/BootNo 유효 표시

// 직전 상태 플래그
#define RCTX_RUN          0x01
#define RCTX_REMOTE       0x02
#define RCTX_COM_OK       0x04
#define RCTX_FAULT        0x08
#define RCTX_ALARM        0x10
#define RCTX_WARM         0x20            // 그 부팅이 웜 리스타트였음

// 리셋 종류 (RSTC_SR RSTTYP)
#define RTYPE_POWERUP     0
#define RTYPE_WATCHDOG    2
#define RTYPE_SOFTWARE    3
#define RTYPE_NRST        4
#define RTYPE_BROWNOUT    5

// cold 이유 (why)
#define WHY_WARM_OK       0
#define WHY_DISABLED      1
#define WHY_TYPE          2
#define WHY_NO_STATE      3
#define WHY_BOOTING       4
#define WHY_STOPPED       5
#define WHY_LOCAL         6
#define WHY_FAULT         7
#define WHY_LIMIT         8

typedef struct
{
  unsigned int magic;
  unsigned int flags;
  unsigned int mode;        // OperUser<<16 | OperPole<<8 | OperMode
  unsigned int extout;      // 출력 래치(74HCT574) 값 ExtOutBuf
  unsigned int dacamp;      // DAC 전류 채널 코드
  unsigned int dacvolt;     // DAC 전압 채널 코드
  int          runtime;     // 운전시간 iRunTime [초]
  float        ampin;       // 측정 전류 fAmpInput [A]
  float        voltin;      // 측정 전압 fVoltInput [V]
  float        setamp;      // 설정 전류 fOperAmp [A] (REMOTE 에서는 PLC 수신값)
  float        setvolt;     // 설정 전압 fOperVolt [V]
  float        outamp;      // 출력 지령 전류 fOutAmp [A]
  float        outvolt;     // 출력 지령 전압 fOutVolt [V]
  unsigned int check;       // 위 항목 전체 XOR
} RESET_CTX;

#define RCTX_WORDS        (sizeof(RESET_CTX) / 4)

__no_init RESET_CTX ResetCtx;             // 리셋되어도 지워지지 않는 RAM : 리셋 직전 상태
__no_init unsigned int ResetStage;        // 리셋되어도 유지 : 메인루프 실행 위치
__no_init unsigned int WarmMagic;         // 리셋되어도 유지
__no_init unsigned int WarmCount;         // 연속 웜 리스타트 횟수
__no_init unsigned int BootNo;            // 전원투입 후 부팅 횟수 (전원투입 = 1)

RESET_CTX ResetBoot;                      // 부팅 때 확보한 직전 상태 사본
unsigned int uiBootRsr;                   // 부팅 때 읽은 RSTC_SR
unsigned int uiBootStage;                 // 부팅 때 확보한 ResetStage
unsigned int uiBootWarmCount;             // 이번 부팅 판정 때의 연속 웜 횟수 (보고용)
char BootStateValid;                      // 1: 직전 상태 유효
char WarmStart;                           // 1: 이번 부팅은 웜 리스타트
char ColdWhy;                             // cold 이유 (WHY_*)
unsigned char BootRtc[6];                 // 부팅 시각 BCD 년 월 일 시 분 초
unsigned short WarmHoldScan;              // 웜 복귀 후 제어 계산 보류 남은 스캔 (ADC.c)
unsigned short WarmSpHoldScan;            // 웜 복귀 후 PLC 설정전류 0 무시 남은 스캔 (profi.c)
unsigned int uiWarmRunScan;               // 웜 후 연속 운전 스캔 수 (연속 횟수 초기화용)
unsigned int uiRstScan;                   // 메인루프 시작 후 스캔 수
unsigned int uiRstRepeat;                 // [RST] last 반복 카운터
char WarmStableSent;

static unsigned int rctx_check(RESET_CTX *ctx)
{
  unsigned int *p, sum;
  char lp;
  p = (unsigned int *)ctx;
  sum = 0xA5A5A5A5;
  for (lp = 0; lp < RCTX_WORDS - 1; lp++) sum ^= p[lp];
  return sum;
}

//---------------------------------------------------------------
// 메인루프 실행 위치 이름 (main.c 의 reset_mark(n) 바로 다음 함수)
//---------------------------------------------------------------
static const char *stage_name(unsigned int stage)
{
  switch (stage)
  {
  case 0:  return "boot init";
  case 1:  return "ext_in_read";
  case 2:  return "panel_key_scan";
  case 3:  return "rotary_key_scan";
  case 4:  return "scroll_key_generate";
  case 5:  return "key_function";
  case 6:  return "AnyBusDP_transive";
  case 7:  return "operate_by_local_data";
  case 8:  return "modbus_message_exchange";
  case 9:  return "AV_measure_ADE7758";
  case 10: return "extin_ararm_deside";
  case 11: return "system_operate";
  case 12: return "adc_read_ad7705";
  case 13: return "sam_adc_converting";
  case 14: return "output_level_control_SCR";
  case 15: return "extout_ararm_deside";
  case 16: return "running_pole_indicate";
  case 17: return "ext_out_update";
  case 18: return "date_time_update";
  case 19: return "execute_per_sec";
  case 20: return "remote_live_check";
  case 21: return "all_FND_test";
  case 22: return "FND_display_update";
  case 23: return "display_scan";
  case 24: return "event_indicating";
  case 25: return "event_clear";
  case 26: return "exec_time_check/dbg";
  case 99: return "idle (wait scan)";
  default: return "?";
  }
}

static const char *type_name(unsigned int type)
{
  switch (type)
  {
  case RTYPE_POWERUP:  return "Power-up (RAM lost)";
  case RTYPE_WATCHDOG: return "Watchdog";
  case RTYPE_SOFTWARE: return "Software";
  case RTYPE_NRST:     return "NRST pin (DS1233 3.3V dip/noise/JTAG)";
  case RTYPE_BROWNOUT: return "Brownout";
  default:             return "Unknown";
  }
}

static const char *why_name(char why)
{
  switch (why)
  {
  case WHY_WARM_OK:  return "ok";
  case WHY_DISABLED: return "WARM_RESTART off";
  case WHY_TYPE:     return "reset type (RAM not kept)";
  case WHY_NO_STATE: return "no valid RAM state";
  case WHY_BOOTING:  return "reset during boot init";
  case WHY_STOPPED:  return "was stopped";
  case WHY_LOCAL:    return "was LOCAL";
  case WHY_FAULT:    return "fault active";
  case WHY_LIMIT:    return "warm limit reached";
  default:           return "?";
  }
}

//---------------------------------------------------------------
// main() 첫 줄 : 리셋 원인, 직전 상태 확보 + warm 판정
//---------------------------------------------------------------
void reset_capture(void)
{
  unsigned int type, stage;

  uiBootRsr = AT91C_BASE_RSTC->RSTC_RSR;
  type = (uiBootRsr >> 8) & 0x07;
  ResetBoot = ResetCtx;
  uiBootStage = ResetStage;

  // 전원투입 직후(또는 처음) : 연속 횟수, 부팅 횟수 초기화
  if ((WarmMagic != WARM_MAGIC) | (type == RTYPE_POWERUP))
  {
    WarmMagic = WARM_MAGIC;
    WarmCount = 0;
    BootNo = 0;
  }
  if (BootNo != 0xFFFFFFFF) BootNo++;

  // 직전 상태 유효 : 매직 + 체크값 + RAM 이 유지되는 리셋 종류
  BootStateValid = 0;
  if ((type != RTYPE_POWERUP) & (type != RTYPE_BROWNOUT))
    if ((ResetBoot.magic == RCTX_MAGIC) & (ResetBoot.check == rctx_check(&ResetBoot)))
      BootStateValid = 1;
  if ((uiBootStage & 0xFF000000) == RSTAGE_KEY) stage = uiBootStage & 0xFF; else stage = 0xFF;

  // 이후 부팅 초기화 중에 다시 리셋되면 '부팅 초기화 중(stage 0)' + '직전 상태 없음' 으로 보이도록
  ResetCtx.magic = 0;
  ResetStage = RSTAGE_KEY | 0;

  // warm 판정
  uiBootWarmCount = WarmCount;
  WarmStart = 0;
#ifdef WARM_RESTART
  if ((type != RTYPE_WATCHDOG) & (type != RTYPE_NRST)) ColdWhy = WHY_TYPE;
  else if (stage == 0) ColdWhy = WHY_BOOTING;
  else if (!BootStateValid) ColdWhy = WHY_NO_STATE;
  else if ((ResetBoot.flags & RCTX_RUN) == 0) ColdWhy = WHY_STOPPED;
  else if (((ResetBoot.mode >> 16) & 0xFF) != REMOTE) ColdWhy = WHY_LOCAL;
  else if (ResetBoot.flags & RCTX_FAULT) ColdWhy = WHY_FAULT;
  else if (WarmCount >= WARM_MAX_COUNT) ColdWhy = WHY_LIMIT;
  else ColdWhy = WHY_WARM_OK;
#else
  ColdWhy = WHY_DISABLED;
#endif

  if (ColdWhy == WHY_WARM_OK)
  {
    WarmCount++;
    WarmStart = 1;
#ifdef WARM_FIX_DACCLR
    // (1) pio_init() 이 DA_CLR(PA14)을 출력으로 바꿀 때 Low 로 나가면 AD5663 이 0 으로 클리어됨
    // -> 미리 출력값을 High 로 써 두어 DAC 출력이 리셋 직전 값 그대로 남게 함
    AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << AT91C_ID_PIOA);
    AT91C_BASE_PIOA->PIO_SODR = DA_CLR;
#endif
  }
  else WarmCount = 0;     // cold 부팅이면 연속이 끊김
}

//---------------------------------------------------------------
// pio_init() 직후 : DAC 코드 -> 출력 래치 -> 출력 허가
//---------------------------------------------------------------
void warm_restart_early(void)
{
  if (!WarmStart) return;
  // 1) DAC (AD5663) 클리어 해제 후 리셋 직전 코드 (ch1 전류, ch0 전압)
#ifdef WARM_FIX_DACCLR
  DAclear(OFF);   // (1) DA_CLR High : Low 상태면 DAC 를 써도 출력이 바뀌지 않음
#endif
  DAout_ad5663(0, 1, ResetBoot.dacamp & 0xFFFF);
  DAout_ad5663(0, 0, ResetBoot.dacvolt & 0xFFFF);
  // 2) 출력 래치: pio_init() 의 EXT_E1 상승 에지로 버스 값이 들어갔을 수 있으므로 다시 씀
  ExtOutBuf = ResetBoot.extout & 0xFFFF;
  ext_out_update();
  // 3) 래치 출력 허가 (OE) -> PLC 출력(RUN/REMOTE ...), DA_EN 복귀
  DC24power(ON);
}

//---------------------------------------------------------------
// FLASH 설정값 로드 후 : 운전 상태 / 제어 상태 복원
//---------------------------------------------------------------
void warm_restart_late(void)
{
  unsigned int m;
#ifdef WARM_FIX_INPUT
  char lp;
#endif

  if (!WarmStart) return;
  m = ResetBoot.mode;
  op_warm_resume((m >> 16) & 0xFF, m & 0xFF, (m >> 8) & 0xFF, ResetBoot.runtime);
  fOperAmp = ResetBoot.setamp;              // PLC 설정전류 (통신이 다시 붙기 전까지 유지)
  ctrl_warm_resume(ResetBoot.outamp, ResetBoot.outvolt, ResetBoot.ampin, ResetBoot.voltin,
                   ResetBoot.dacamp & 0xFFFF, ResetBoot.dacvolt & 0xFFFF);
  ExtOutBuf = ResetBoot.extout & 0xFFFF;    // data_init() 에서 0 으로 지워진 값 복원
  DC24power(ON);

#ifdef WARM_FIX_INPUT
  // (2) 입력 디바운스를 미리 채움
  //   부팅 직후 비상정지 입력은 ExtIn(60ms) + 키(32ms) 디바운스가 끝날 때까지 '비상정지 눌림'으로
  //   판정되어(EmegStop = ON) 첫 스캔에서 system_stop() 이 됨 -> 실제 입력 상태로 미리 채움
  //   (SystemRun = ON 이므로 select_switch_check() 는 OperUser 를 바꾸지 않음)
  for (lp = 0; lp < 60; lp++)
  {
    ext_in_read();
    panel_key_scan();
  }
  PushKey = 0;
#endif

#ifdef WARM_FIX_HOLD
  WarmHoldScan = WARM_HOLD_MS / 2;              // (6)
#endif
#ifdef WARM_FIX_PLC_SP0
  WarmSpHoldScan = WARM_SP_HOLD_SEC * SEC_1;    // (3)
#endif
}

//---------------------------------------------------------------
// [RST] 보고
//---------------------------------------------------------------
static void reset_report_print(void)
{
  unsigned int type, stage, flags, m;
  char cold;

  type = (uiBootRsr >> 8) & 0x07;
  cold = !WarmStart;
  if ((uiBootStage & 0xFF000000) == RSTAGE_KEY) stage = uiBootStage & 0xFF; else stage = 0xFF;

  // 1줄: 요약
  if ((type == RTYPE_POWERUP) | (type == RTYPE_BROWNOUT) | (stage == 0xFF))
  {
    if (cold) dbg_printf("[RST] cold reset : %s / not warm: %s\r\n", type_name(type), why_name(ColdWhy));
    else      dbg_printf("[RST] warm reset : %s\r\n", type_name(type));
  }
  else
  {
    if (cold) dbg_printf("[RST] cold reset : %s at stage %u %s / not warm: %s\r\n",
                         type_name(type), stage, stage_name(stage), why_name(ColdWhy));
    else      dbg_printf("[RST] warm reset : %s at stage %u %s\r\n", type_name(type), stage, stage_name(stage));
  }

  // 2줄: 리셋 정보
  dbg_printf("[RST] info boot=%u type=%u rsr=0x%08X rtc=20%02X-%02X-%02X %02X:%02X:%02X warm=%u/%u why=%s\r\n",
             BootNo, type, uiBootRsr, BootRtc[0], BootRtc[1], BootRtc[2], BootRtc[3], BootRtc[4], BootRtc[5],
             WarmStart ? uiBootWarmCount + 1 : uiBootWarmCount, WARM_MAX_COUNT, why_name(ColdWhy));

  // 3줄: 리셋 직전 상태 (RAM)
  if (BootStateValid)
  {
    flags = ResetBoot.flags;
    m = ResetBoot.mode;
    dbg_printf("[RST] prev run=%d user=%s mode=%s fault=%d alarm=%d com=%d amp=%d set=%d out=%d volt=%.2f dacA=%u dacV=%u runtime=%d\r\n",
               (flags & RCTX_RUN) ? 1 : 0,
               (((m >> 16) & 0xFF) == REMOTE) ? "REMOTE" : "LOCAL",
               ((m & 0xFF) == CC_MODE) ? "CC" : "CV",
               (flags & RCTX_FAULT) ? 1 : 0, (flags & RCTX_ALARM) ? 1 : 0, (flags & RCTX_COM_OK) ? 1 : 0,
               (int)ResetBoot.ampin, (int)ResetBoot.setamp, (int)ResetBoot.outamp, ResetBoot.voltin,
               ResetBoot.dacamp & 0xFFFF, ResetBoot.dacvolt & 0xFFFF, ResetBoot.runtime);
  }
  else dbg_printf("[RST] prev none (RAM lost or not valid)\r\n");

  // 4줄: 웜 복귀 보완 스위치 상태 (CSLab_Rectifier_Main.h WARM_FIX_*, 1 = 켜짐) - 시험 로그 구분용
  dbg_printf("[RST] fix dacclr=%d input=%d plcsp0=%d inold=%d dacref=%d hold=%d\r\n",
#ifdef WARM_FIX_DACCLR
             1,
#else
             0,
#endif
#ifdef WARM_FIX_INPUT
             1,
#else
             0,
#endif
#ifdef WARM_FIX_PLC_SP0
             1,
#else
             0,
#endif
#ifdef WARM_FIX_INOLD
             1,
#else
             0,
#endif
#ifdef WARM_FIX_DACREF
             1,
#else
             0,
#endif
#ifdef WARM_FIX_HOLD
             1);
#else
             0);
#endif

  // 5줄: 리모트 기동/설정전류 0 처리 스위치 (노션 5.3) + 이번 부팅의 재입력 대기 상태
  dbg_printf("[RST] opt rearm=%d faultrearm=%d sp0hold=%d zerosp=%d armwait=%d\r\n",
#ifdef REMOTE_START_REARM
             1,
#else
             0,
#endif
#ifdef REARM_FAULT_STOP
             1,
#else
             0,
#endif
#ifdef REMOTE_SP0_HOLD
             1,
#else
             0,
#endif
#ifdef CTRL_FIX_ZERO_SP
             1,
#else
             0,
#endif
             RemArmWait ? 1 : 0);

  // 6줄: 측정 필터/안정 판정 튜닝 값 [TUN] (CSLab_Rectifier_ADC.c)
  adc_tune_print();
}

// RTC 초기화 후 1회 (main.c)
void reset_report_boot(void)
{
  rtc_time_read();
  BootRtc[0] = Year;  BootRtc[1] = Month;  BootRtc[2] = Date;
  BootRtc[3] = Hour;  BootRtc[4] = Minute; BootRtc[5] = Sec;
  reset_report_print();
}

//---------------------------------------------------------------
// 메인루프 매 스캔 : 직전 상태 갱신 (main.c 스캔 시작)
//---------------------------------------------------------------
void reset_ctx_update(void)
{
  unsigned int flags;

  flags = 0;
  if (SystemRun)          flags |= RCTX_RUN;
  if (OperUser == REMOTE) flags |= RCTX_REMOTE;
  if (RemoteReady)        flags |= RCTX_COM_OK;
  if (TotalError)         flags |= RCTX_FAULT;
  if (TotalAlarm)         flags |= RCTX_ALARM;
  if (WarmStart)          flags |= RCTX_WARM;
  ResetCtx.magic   = RCTX_MAGIC;
  ResetCtx.flags   = flags;
  ResetCtx.mode    = ((unsigned int)OperUser << 16) | ((unsigned int)OperPole << 8) | (unsigned int)OperMode;
  ResetCtx.extout  = ExtOutBuf;
  ResetCtx.dacamp  = iDacCodeAmp & 0xFFFF;
  ResetCtx.dacvolt = iDacCodeVolt & 0xFFFF;
  ResetCtx.runtime = iRunTime;
  ResetCtx.ampin   = fAmpInput;
  ResetCtx.voltin  = fVoltInput;
  ResetCtx.setamp  = fOperAmp;
  ResetCtx.setvolt = fOperVolt;
  ResetCtx.outamp  = fOutAmp;
  ResetCtx.outvolt = fOutVolt;
  ResetCtx.check   = rctx_check(&ResetCtx);

  // 연속 웜 횟수: 웜 후 WARM_CLEAR_SEC 동안 계속 운전하면 0
  if ((WarmCount) & (SystemRun))
  {
    if (++uiWarmRunScan >= (unsigned int)WARM_CLEAR_SEC * SEC_1) { WarmCount = 0; uiWarmRunScan = 0; }
  }
  else uiWarmRunScan = 0;

  // PLC 설정전류 0 무시 시간 (REMOTE 운전이 아니면 즉시 끝)
  if (WarmSpHoldScan)
  {
    if ((OperUser != REMOTE) | (!SystemRun)) WarmSpHoldScan = 0;
    else WarmSpHoldScan--;
  }
}

// 메인루프 실행 위치 표시 (워치독 리셋 때 멈춘 함수 확인용)
void reset_mark(unsigned char stage)
{
  ResetStage = RSTAGE_KEY | stage;
}

//---------------------------------------------------------------
// 시험용 UART 명령 (COM1 수신, CSLab_SAM7_Usart1.c 수신 링버퍼)
//---------------------------------------------------------------
#if defined(RST_TEST_CMD) || defined(ADC_TUNE_CMD)    // 2026-10-08 : TUN 튜닝 명령도 같은 파서
extern BYTE  rx_buffer1[];
extern short rx_wr_index1, rx_rd_index1;
static char RstCmd[40];             // 2026-10-08 8 -> 40 : 'TUN 이름=값'
static char RstCmdLen;

static char cmd_is(const char *s)
{
  char lp;
  for (lp = 0; s[lp]; lp++) if ((lp >= RstCmdLen) | (RstCmd[lp] != s[lp])) return 0;
  return (lp == RstCmdLen);
}

static void reset_rx_command(void)
{
  char c, n;
  if (ExecMode == REMOTE_SET) return;       // 시스템 메뉴 Remote 시험이 COM1 수신을 사용 중
  n = 0;
  while ((rx_rd_index1 != rx_wr_index1) & (n < 16))
  {
    c = getchar1();
    n++;
    if ((c == '\r') | (c == '\n'))
    {
      if (cmd_is("RST?")) reset_report_print();
#ifdef DBG_PLCCMD
      else if (cmd_is("PLC?")) plc_cmd_force();     // [PLCCMD] 다시 출력
#endif
#ifdef ADC_TUNE_CMD
      else if (adc_tune_command(RstCmd, RstCmdLen)) ;   // 2026-10-08 추가: TUN? / TUN DEF / TUN 이름=값
#endif
#ifdef RST_TEST_CMD
      else if (cmd_is("!WDT"))
      {
        dbg_printf("[RST] test : main loop stall -> watchdog reset\r\n");
        for (;;);                           // 송신은 인터럽트로 계속됨, 워치독(200ms)이 리셋
      }
#endif
      RstCmdLen = 0;
    }
    else if (RstCmdLen < sizeof(RstCmd)) RstCmd[RstCmdLen++] = c;
    else RstCmdLen = 0;
  }
}
#endif

//---------------------------------------------------------------
// 메인루프 매 스캔 (main.c dbg_status_log() 다음)
//---------------------------------------------------------------
void reset_scan_tick(void)
{
  float err;

  if (uiRstScan != 0xFFFFFFFF) uiRstScan++;

  // 웜 복귀 결과 (1회)
  if ((WarmStart) & (!WarmStableSent) & (uiRstScan >= (unsigned int)WARM_REPORT_SEC * SEC_1))
  {
    WarmStableSent = 1;
    if (fOperAmp > 0) err = (fAmpInput - fOperAmp) * 100 / fOperAmp; else err = 0;
    dbg_printf("[RST] warm stable : t=%ums run=%d amp=%d set=%d out=%d err=%.1f%% com=%d plc_sp=%s\r\n",
               uiRstScan * 2, SystemRun ? 1 : 0, (int)fAmpInput, (int)fOperAmp, (int)fOutAmp, err,
               RemoteReady ? 1 : 0, WarmSpHoldScan ? "held" : "live");
  }

#if RST_REPEAT_SEC > 0
  // 요약 반복
  if (++uiRstRepeat >= (unsigned int)RST_REPEAT_SEC * SEC_1)
  {
    uiRstRepeat = 0;
    dbg_printf("[RST] last boot=%u %s type=%u why=%s rtc=20%02X-%02X-%02X %02X:%02X:%02X up=%us\r\n",
               BootNo, WarmStart ? "warm" : "cold", (uiBootRsr >> 8) & 0x07, why_name(ColdWhy),
               BootRtc[0], BootRtc[1], BootRtc[2], BootRtc[3], BootRtc[4], BootRtc[5], uiRstScan / SEC_1);
  }
#endif

#if defined(RST_TEST_CMD) || defined(ADC_TUNE_CMD)
  reset_rx_command();
#endif
}
