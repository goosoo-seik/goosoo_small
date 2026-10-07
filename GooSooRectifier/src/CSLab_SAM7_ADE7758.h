// ----------------------------------------------------------------------------
//         Epiatech co,. LTD  -  Lee Y k  -
// ----------------------------------------------------------------------------
// File Name           : EpiaLiadMonit_ADE7758.h
// Object              : Epia LOAD Monitor ADE7758 register define
// Creation            : 2007-10-29
// ----------------------------------------------------------------------------

//ADE7758 Register listing
#define	ADE_AWATTHR	0x01	// R 16 Watt-Hour Accumulation Reg. for Phase A
#define	ADE_BWATTHR	0x02	// R 16 Watt-Hour Accumulation Reg. for Phase B
#define	ADE_CWATTHR	0x03	// R 16 Watt-Hour Accumulation Reg. for Phase C
#define	ADE_AVARHR	0x04	// R 16 Var-Hour Accumulation Reg. for Phase A
#define	ADE_BVARHR	0x05	// R 16 Var-Hour Accumulation Reg. for Phase B
#define	ADE_CVARHR	0x06	// R 16 Var-Hour Accumulation Reg. for Phase C
#define	ADE_AVAHR	0x07	// R 16 VA-Hour Accumulation Reg. for Phase A
#define	ADE_BVAHR	0x08	// R 16 VA-Hour Accumulation Reg. for Phase B
#define	ADE_CVAHR	0x09	// R 16 VA-Hour Accumulation Reg. for Phase C
#define	ADE_AIRMS	0x0A    // R 24 Current Channel IRMS Reg. for Phase A
#define	ADE_BIRMS	0x0B    // R 24 Current Channel IRMS Reg. for Phase B
#define	ADE_CIRMS	0x0C    // R 24 Current Channel IRMS Reg. for Phase C
#define	ADE_AVRMS	0x0D    // R 24 Voltage Channel IRMS Reg. for Phase A
#define	ADE_BVRMS	0x0E    // R 24 Voltage Channel IRMS Reg. for Phase B
#define	ADE_CVRMS	0x0F    // R 24 Voltage Channel IRMS Reg. for Phase C
#define	ADE_FREQ	0x10	// R 12 Frequency of line input estimated by Zero-Crossing Processing.
#define	ADE_TEMP	0x11    // R  8 Temperature Register
#define	ADE_WAVEFORM	0x12	// R 24 Waveform Register
#define	ADE_OPMODE	0x13    //RW  8 Operational Mode Register
#define	ADE_MMODE	0x14    //RW  8 Measurement Mode Register
#define	ADE_WAVMODE	0x15	//RW  8 Waveform Mode Register
#define	ADE_COMPMODE	0x16	//RW  8 Computation Mode Register
#define	ADE_LCYCMODE	0x17	//RW  8 Line Cycle Mode Register
#define	ADE_IMASK	0x18    //RW 24 IRQ_ Mask Register
#define	ADE_ISTATUS	0x19    // R 24 IRQ_ Status Register
#define	ADE_RSTATUS	0x1A    // R 24 IRQ_ Reset Status Register. Reset to 0 after read operation
#define	ADE_ZXTOUT      0x1B    //RW 16 Zero-Crossing TimeoutRegister
#define	ADE_LINECYC     0x1C    //RW 16 Line-Cycle Register
#define	ADE_SAGCYC      0x1D    //RW  8 SAG Line Cycle Register.
#define	ADE_SAGLVL      0x1E    //RW  8 SAG Voltage Level.
#define	ADE_VPINTLVL    0x1F    //RW  8 Voltage Peak Level Interrupt Threshold
#define	ADE_IPINTLVL    0x20    //RW  8 Current Peak Level Interrupt Threshold
#define	ADE_VPEAK	0x21    // R  8 Voltage Peak Register
#define	ADE_IPEAK       0x22    // R  8 Current Peak Register
#define	ADE_GAIN        0x23    //RW  8 PGA Gain adjust.
#define	ADE_AVRMSGAIN	0x24    //RW 12 Phase A VRMS Gain Register
#define	ADE_BVRMSGAIN	0x25    //RW 12 Phase B VRMS Gain Register
#define	ADE_CVRMSGAIN	0x26    //RW 12 Phase C VRMS Gain Register
#define	ADE_AIGAIN	0x27    //RW 12 Phase A Current Gain Register
#define	ADE_BIGAIN	0x28    //RW 12 Phase B Current Gain Register
#define	ADE_CIGAIN	0x29    //RW 12 Phase C Current Gain Register
#define	ADE_AWGAIN	0x2A    //RW 12 Phase A Watt Gain Register
#define	ADE_BWGAIN	0x2B    //RW 12 Phase B Watt Gain Register
#define	ADE_CWGAIN	0x2C    //RW 12 Phase C Watt Gain Register
#define	ADE_AVARGAIN	0x2D    //RW 12 Phase A VAR Gain Register
#define	ADE_BVARGAIN	0x2E    //RW 12 Phase B VAR Gain Register
#define	ADE_CVARGAIN	0x2F    //RW 12 Phase B VAR Gain Register
#define	ADE_AVAGAIN	0x30    //RW 12 Phase A VA Gain Register
#define	ADE_BVAGAIN	0x31    //RW 12 Phase B VA Gain Register
#define	ADE_CVAGAIN	0x32    //RW 12 Phase C VA Gain Register
#define	ADE_AVRMSOS     0x33    //RW 12 Phase A Voltage RMS Offset Correction Register
#define	ADE_BVRMSOS     0x34    //RW 12 Phase B Voltage RMS Offset Correction Register
#define	ADE_CVRMSOS     0x35    //RW 12 Phase C Voltage RMS Offset Correction Register
#define	ADE_AIRMSOS     0x36    //RW 12 Phase A Current RMS Offset Correction Register                   Channel
#define	ADE_BIRMSOS     0x37    //RW 12 Phase B Current RMS Offset Correction Register                   Channel
#define	ADE_CIRMSOS     0x38    //RW 12 Phase C Current RMS Offset Correction Register                   Channel
#define	ADE_AWATTOS     0x39    //RW 12 Phase A Watt RMS Offset Correction Register                   Channel
#define	ADE_BWATTOS     0x3A    //RW 12 Phase B Watt RMS Offset Correction Register                   Channel
#define	ADE_CWATTOS     0x3B    //RW 12 Phase C Watt RMS Offset Correction Register                   Channel
#define	ADE_AVAROS      0x3C    //RW 12 Phase A Watt VAR Offset Correction Register                   Channel
#define	ADE_BVAROS      0x3D    //RW 12 Phase B Watt VAR Offset Correction Register                   Channel
#define	ADE_CVAROS      0x3E    //RW 12 Phase C Watt VAR Offset Correction Register                   Channel
#define	ADE_APHCAL      0x3F    //RW  7 Phase A Phase Calibration Register.
#define	ADE_BPHCAL      0x40    //RW  7 Phase B Phase Calibration Register.
#define	ADE_CPHCAL      0x41    //RW  7 Phase C Phase Calibration Register.
#define	ADE_WDIV        0x42    //RW  8 Active Energy Register Divider
#define	ADE_VARDIV      0x43    //RW  8 Reactive Energy Register Divider
#define	ADE_VADIV       0x44    //RW  8 Apparent Energy Register Divider
#define	ADE_APCFNUM     0x45    //RW 16 Active power CF Scaling Numerator Register
#define	ADE_APCFDEN     0x46    //RW 12 Active power CF Scaling Denomiantor Register
#define	ADE_VARCFNUM    0x47    //RW 16 Reactive power CF Scaling Numerator Register
#define	ADE_VARCFDEN    0x48    //RW 12 Reactive power CF Scaling Denomiantor Register
#define	ADE_CHKSUM	0x7E	// R  8 Check Register
#define	ADE_VERSION	0x7F	// R  8 Version of the Die

// ADE7758 Interrupt mask register(ADE_MASK:0x18), Interrupt status register(ADE_STATUS:0x19) detail
#define	ADE_AEHF	1   	// WATTHR register is half full
#define	ADE_REHF	1<<1	// VARHR register is half full
#define	ADE_VAEHF	1<<2	// VAHR register is half full
#define	ADE_SAGA	1<<3	// SAG on the line voltage of Phase A
#define	ADE_SAGB	1<<4	// SAG on the line voltage of Phase B
#define	ADE_SAGC	1<<5	// SAG on the line voltage of Phase C
#define	ADE_ZXTOA	1<<6	// Zero-crossing timeout detection on Phase A
#define	ADE_ZXTOB	1<<7	// Zero-crossing timeout detection on Phase B
#define	ADE_ZXTOC	1<<8	// Zero-crossing timeout detection on Phase C
#define	ADE_ZXA		1<<9	// Zero-crossing in the voltage channel of Phase A
#define	ADE_ZXB		1<<10	// Zero-crossing in the voltage channel of Phase B 
#define	ADE_ZXC		1<<11	// Zero-crossing in the voltage channel of Phase C
#define	ADE_LENERGY	1<<12	// Energy accumulations over LINECYC are finished
#define	ADE_RESETVED	1<<13	// Reserved
#define	ADE_PKV		1<<14	// Voltage input selected in the MMODE register is above the value in the VPINTLVL register
#define	ADE_PKI		1<<15	// Current input selected in the MMODE register is above the value in the IPINTLVL register
#define	ADE_WFSM	1<<16	// Data is present in the WAVEMODE register
#define	ADE_REVPAP	1<<17	// A sign change in the watt calculation(by TERMSEL bits in the COMPMODE register
#define	ADE_REVPRP	1<<18	// A sign change in the VAR calculation(by TERMSEL bits in the COMPMODE register
#define	ADE_SEQERR	1<<19	// Zero-crossing from Phase A is follow not by Phase C but with that of Phase B
// 순시전압강하, 순시결상, ZeroCross A 검출, 단위시간 전력량 계측, 과전류, 역상입력 검출
#define	ADE_IRQ_FULL	ADE_SAGA|ADE_SAGB|ADE_SAGC|ADE_ZXTOA|ADE_ZXTOB|ADE_ZXTOC|ADE_ZXA|ADE_LENERGY|ADE_PKV|ADE_PKI|ADE_SEQERR
// ZeroCross A 검출, 단위시간 전력량 계측, 과전류, 역상입력 검출
#define	ADE_IRQ_SIMPLE	ADE_ZXA|ADE_LENERGY|ADE_PKI|ADE_SEQERR

// Operational mode register(ADE_OPMODE:0x13)
#define	ADE_DISHPF	1	// 1: HPFs in all current channel are disable.
#define	ADE_DISLPF	1<<1	// 1: LPFs after the watt and VAR multiplier are disable.
#define	ADE_DISCF	1<<2	// 1: Frequency output APCF and VARCF are disable.
#define	ADE_DISMOD	7<<3	// 1: ADE7758's ADC can be turned off.
#define	ADE_DISMOD0 	0	// Normal Operation
#define	ADE_DISMOD4 	4<<3	// Redirect the voltage inputs to the signal paths for the current channel and the current inputs to the signal paths for the voltage channel.
#define	ADE_DISMOD1 	1<<3	// Switch off only the current channel ADCs. 
#define	ADE_DISMOD5 	5<<3	// Switch off current channel ADCs and redirect the current input signals to the voltage channel signal paths.
#define	ADE_DISMOD2 	2<<3	// Switch off only the voltage channel ADCs.
#define	ADE_DISMOD6 	6<<3	// Switch off voltage channel ADCs and redirect the voltage input signals to the current channel signal paths.
#define	ADE_DISMOD3 	3<<3	// Put the ade7758 in sleep mode
#define	ADE_DISMOD7 	7<<3	// Put the ade7758 in power-down mode(reduces AIdd to 1mA typ.)
#define	ADE_SWRST	1<<6	// 1: Software chip Reset.
#define	ADE_OPMODE_DEFAULT	ADE_DISCF

// Measurement mode register(ADE_MMODE:0x14)
#define	ADE_FREQSELA	0	// Select Phase A FOR the source of voltage line frequency.
#define	ADE_FREQSELB	1	// Select Phase B FOR the source of voltage line frequency.
#define	ADE_FREQSELC	2	// Select Phase C FOR the source of voltage line frequency.
#define	ADE_PEAKSELA	1<<2	// Select Phase A used voltage and current peak register. 
#define	ADE_PEAKSELB	1<<3	// Select Phase B used voltage and current peak register. 
#define	ADE_PEAKSELC	1<<4	// Select Phase C used voltage and current peak register. 
#define	ADE_PKIRQSELA	1<<5	// Select Phase A used for the peak interrupt detection.
#define	ADE_PKIRQSELB	1<<6	// Select Phase B used for the peak interrupt detection.
#define	ADE_PKIRQSELC	1<<7	// Select Phase C used for the peak interrupt detection.
#define	ADE_MMODE_DEFAULT	ADE_FREQSEL

// Waveform mode register(ADE_WAVMODE:0x15)
#define	ADE_PHSELA	0	// Select Phase A used waveform sample. 
#define	ADE_PHSELB	1	// Select Phase B used waveform sample. 
#define	ADE_PHSELC	2	// Select Phase C used waveform sample. 
#define	ADE_DTRT2604	0<<5	// Selest Date update rate : 26.04kSPS(CLKIN/3/128)
#define	ADE_DTRT1302    1<<5    // Selest Date update rate : 13.02kSPS(CLKIN/3/256)
#define	ADE_DTRT0651    2<<5    // Selest Date update rate :  6.51kSPS(CLKIN/3/512)
#define	ADE_DTRT0325    3<<5    // Selest Date update rate :  3.51kSPS(CLKIN/3/1024)
#define	ADE_VACF	1<<7	// 1: VARCF is proportional to total apparent power(VA), 0: total reactive power(VAR)
#define	ADE_WAVMODE_DEFAULT	0

// Computation Mode Register(ADE_COMPMODE:0x16)
#define	ADE_CONSEL		// Select the input to the energy accumulation register
#define	ADE_CONSEL0	0	// VA x IA	  VB x IB    VC x IC
#define	ADE_CONSEL1     1       // VA x (IA-IB)	     0	     VC x (IC-IB)
#define	ADE_CONSEL2     2       // VA x (IA-IB)	     0	     VC x IC
#define	ADE_TERMSELA	1<<2	// Phase A included in the APCH and VARCF
#define	ADE_TERMSELB	1<<3	// Phase B included in the APCH and VARCF
#define	ADE_TERMSELC	1<<4	// Phase C included in the APCH and VARCF
#define	ADE_ABS		1<<5	// APCF pin in sum of absolute(AWATTHR, BWATTHR, CWATTHR) 
#define	ADE_SAVAR	1<<6	// VARCF pin in sum of absolute(AVARHR, BVARHR, CVARHR)
#define	ADE_NOLOAD	1<<7	// Activate the no load threshold in the ADE7758
#define	ADE_COMPMODE_DEFAULT	ADE_TERMSELA|ADE_TERMSELB|ADE_TERMSELC|ADE_NOLOAD

// Line Cycle Register(ADE_LCYCMODE:0x17)
#define	ADE_LWATT	1	// (AWATTHR,BWATTHR,CWATTHR) is line-cycle accumulation mode
#define	ADE_LVAR	1<<1	// (AVARHR,BVARHR,CVARHR) is line-cycle accumulation mode
#define	ADE_LVA		1<<2	// (AVAHR,BVAHR,CVAHR) is line-cycle accumulation mode
#define	ADE_ZXSELA	1<<3	// Select Phase A used for counting the number of zero crossing in the line-cycle accumulation mode
#define	ADE_ZXSELB	1<<4	// Select Phase B used for counting the number of zero crossing in the line-cycle accumulation mode
#define	ADE_ZXSELC	1<<5	// Select Phase C used for counting the number of zero crossing in the line-cycle accumulation mode
#define	ADE_RSTREAD	1<<6	// Enable read-with-reset for all the WATTHR, VARHR, VAHR for all three phases.
#define	ADE_FREQSEL	1<<7	// 1: FREQ(0X10) register is Period, 0: Ffrquency.
#define	ADE_LCYCMODE_DEFAULT	ADE_ZXSELA|ADE_ZXSELB|ADE_ZXSELB|ADE_RSTREAD

//
//default value difines
//
#define PT1_VOLT_DEFAULT        3300
#define PT2_VOLT_DEFAULT        110
#define CT1_AMP_DEFAULT         200   
#define CT2_AMP_DEFAULT         5
#define VOLT_OFFSET_RATE        64
#define	AVRMS_OFFSET_DEFAULT	0xFFF - 336     //22200/64 = 346
#define	BVRMS_OFFSET_DEFAULT	0xFFF - 505     //33000/64 = 515
#define	CVRMS_OFFSET_DEFAULT	0xFFF - 474     //31000/64 = 484
// (683-548) / 9 = 15
// (683-387) / 21 =
#define AMP_OFFSET_RATE         15
#define	AIRMS_OFFSET_DEFAULT	0
#define	BIRMS_OFFSET_DEFAULT	0
#define	CIRMS_OFFSET_DEFAULT	0
#define	AWATT_OFFSET_DEFAULT	0
#define	BWATT_OFFSET_DEFAULT	0
#define	CWATT_OFFSET_DEFAULT	0
#define	AVAR_OFFSET_DEFAULT	0
#define	BVAR_OFFSET_DEFAULT     0	
#define	CVAR_OFFSET_DEFAULT	0
#define VAR_OFFSET_RATE         1

#define	ADE_GAIN_DEFAULT	0
#define	AVRMS_GAIN_DEFAULT      0
#define	BVRMS_GAIN_DEFAULT      0
#define	CVRMS_GAIN_DEFAULT      0
#define	AIRMS_GAIN_DEFAULT      0
#define	BIRMS_GAIN_DEFAULT      0
#define	CIRMS_GAIN_DEFAULT      0
#define	AWATT_GAIN_DEFAULT      0
#define	BWATT_GAIN_DEFAULT      0
#define	CWATT_GAIN_DEFAULT      0
#define	AVAR_GAIN_DEFAULT       0
#define	BVAR_GAIN_DEFAULT       0
#define	CVAR_GAIN_DEFAULT       0
#define	AVA_GAIN_DEFAULT        0
#define	BVA_GAIN_DEFAULT        0
#define	CVA_GAIN_DEFAULT	0

#define	ADE_APHCAL_DEFAULT	0
#define	ADE_BPHCAL_DEFAULT      0	
#define	ADE_CPHCAL_DEFAULT      0
#define	ADE_WDIV_DEFAULT        0
#define	ADE_VARDIV_DEFAULT      0
#define	ADE_VADIV_DEFAULT	0
#define	AVRMS_DIVIDER_DEFAULT   8855    // 9412-207.5:220
#define	BVRMS_DIVIDER_DEFAULT   8855    // 9412-212.5:228
#define	CVRMS_DIVIDER_DEFAULT   8855    // 9412-215.0:227
// 602700 / 12 = 30135
#define	AIRMS_DIVIDER_DEFAULT   31000   // 56845
#define	BIRMS_DIVIDER_DEFAULT   31000   // 24.6:26.5 
#define	CIRMS_DIVIDER_DEFAULT   31000   // 26.3:26.5

#define	WATT_DIVIDER_DEFAULT     3437     
#define	VAR_DIVIDER_DEFAULT      3437   
#define	VA_DIVIDER_DEFAULT       3437   

#define	ADE_PW_DIVIDER_DEFAULT  0      // ADE7758  default ADE_WDIV, ADE_VADIV, ADE_VARDIV
#define WATT_RATE_DEFAULT       1000
#define VAR_RATE_DEFAULT        1000
#define VA_RATE_DEFAULT         1000
#define VRS_RATE_DEFAULT        1000
#define VST_RATE_DEFAULT        1000
#define VTR_RATE_DEFAULT        1000

#define IA_RATE_DEFAULT         1000
#define IB_RATE_DEFAULT         1000
#define IC_RATE_DEFAULT         1000

#define MIN_ADJUST_IRMS         200000
#define MIN_ADJUST_VRMS         150000
#define MIN_ADJUST_WATT         3000
#define MIN_ADJUST_VAR          300
#define MAX_ZERO_IRMS           64000
#define MAX_ZERO_VRMS           64000

const unsigned char ADE_REG_LENGTH[] = 
{
//0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
  0, 16, 16, 16, 16, 16, 16, 16, 16, 16, 24, 24, 24, 24, 24, 24,
 12,  8, 24,  8,  8,  8,  8,  8, 24, 24, 24, 16, 16,  8,  8,  8,
  8,  8,  8,  8, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  7,
  7,  7,  8,  8,  8, 16, 12, 16, 12
};
const unsigned char ADE_WRITE_ENABLE[] =
{ 
//0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  1,  1,  1,
  1,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1
};
