/* linux/include/asm-arm/arch-at91sam7x256/at91sam7x256_can.h
 * 
 * Hardware definition for the can peripheral in the ATMEL at91sam7x256 processor
 * 
 * Generated  11/02/2005 (15:17:30) AT91 SW Application Group from CAN_6019B V1.1
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __AT91SAM7X256_CAN_H
#define __AT91SAM7X256_CAN_H

/* -------------------------------------------------------- */
/* CAN ID definitions for  AT91SAM7X256           */
/* -------------------------------------------------------- */
#ifndef AT91C_ID_CAN
#define AT91C_ID_CAN   	15 /**< Control Area Network Controller id */
#endif /* AT91C_ID_CAN */

/* -------------------------------------------------------- */
/* CAN Base Address definitions for  AT91SAM7X256   */
/* -------------------------------------------------------- */
#define AT91C_BASE_CAN       	0xFFFD0000 /**< CAN base address */

/* -------------------------------------------------------- */
/* PIO definition for CAN hardware peripheral */
/* -------------------------------------------------------- */
#define AT91C_PA19_CANRX    	(1 << 19) /**< CAN Receive */
#define AT91C_PA20_CANTX    	(1 << 20) /**< CAN Transmit */


/* -------------------------------------------------------- */
/* Register offset definition for CAN hardware peripheral */
/* -------------------------------------------------------- */
#define CAN_MR 	(0x0000) 	/**< Mode Register */
#define CAN_IER 	(0x0004) 	/**< Interrupt Enable Register */
#define CAN_IDR 	(0x0008) 	/**< Interrupt Disable Register */
#define CAN_IMR 	(0x000C) 	/**< Interrupt Mask Register */
#define CAN_SR 	(0x0010) 	/**< Status Register */
#define CAN_BR 	(0x0014) 	/**< Baudrate Register */
#define CAN_TIM 	(0x0018) 	/**< Timer Register */
#define CAN_TIMESTP 	(0x001C) 	/**< Time Stamp Register */
#define CAN_ECR 	(0x0020) 	/**< Error Counter Register */
#define CAN_TCR 	(0x0024) 	/**< Transfer Command Register */
#define CAN_ACR 	(0x0028) 	/**< Abort Command Register */
#define CAN_VR 	(0x00FC) 	/**< Version Register */
#define CAN_MB0 	(0x0200) 	/**< CAN Mailbox 0 */
#define CAN_MB1 	(0x0220) 	/**< CAN Mailbox 1 */
#define CAN_MB2 	(0x0240) 	/**< CAN Mailbox 2 */
#define CAN_MB3 	(0x0260) 	/**< CAN Mailbox 3 */
#define CAN_MB4 	(0x0280) 	/**< CAN Mailbox 4 */
#define CAN_MB5 	(0x02A0) 	/**< CAN Mailbox 5 */
#define CAN_MB6 	(0x02C0) 	/**< CAN Mailbox 6 */
#define CAN_MB7 	(0x02E0) 	/**< CAN Mailbox 7 */
#define CAN_MB8 	(0x0300) 	/**< CAN Mailbox 8 */
#define CAN_MB9 	(0x0320) 	/**< CAN Mailbox 9 */
#define CAN_MB10 	(0x0340) 	/**< CAN Mailbox 10 */
#define CAN_MB11 	(0x0360) 	/**< CAN Mailbox 11 */
#define CAN_MB12 	(0x0380) 	/**< CAN Mailbox 12 */
#define CAN_MB13 	(0x03A0) 	/**< CAN Mailbox 13 */
#define CAN_MB14 	(0x03C0) 	/**< CAN Mailbox 14 */
#define CAN_MB15 	(0x03E0) 	/**< CAN Mailbox 15 */

/* -------------------------------------------------------- */
/* Bitfields definition for CAN hardware peripheral */
/* -------------------------------------------------------- */
/* --- Register CAN_MR */
#define AT91C_CAN_CANEN       (0x1 << 0 ) /**< (CAN) CAN Controller Enable */
#define AT91C_CAN_LPM         (0x1 << 1 ) /**< (CAN) Disable/Enable Low Power Mode */
#define AT91C_CAN_ABM         (0x1 << 2 ) /**< (CAN) Disable/Enable Autobaud/Listen Mode */
#define AT91C_CAN_OVL         (0x1 << 3 ) /**< (CAN) Disable/Enable Overload Frame */
#define AT91C_CAN_TEOF        (0x1 << 4 ) /**< (CAN) Time Stamp messages at each end of Frame */
#define AT91C_CAN_TTM         (0x1 << 5 ) /**< (CAN) Disable/Enable Time Trigger Mode */
#define AT91C_CAN_TIMFRZ      (0x1 << 6 ) /**< (CAN) Enable Timer Freeze */
#define AT91C_CAN_DRPT        (0x1 << 7 ) /**< (CAN) Disable Repeat */
/* --- Register CAN_IER */
#define AT91C_CAN_MB0         (0x1 << 0 ) /**< (CAN) Mailbox 0 Flag */
#define AT91C_CAN_MB1         (0x1 << 1 ) /**< (CAN) Mailbox 1 Flag */
#define AT91C_CAN_MB2         (0x1 << 2 ) /**< (CAN) Mailbox 2 Flag */
#define AT91C_CAN_MB3         (0x1 << 3 ) /**< (CAN) Mailbox 3 Flag */
#define AT91C_CAN_MB4         (0x1 << 4 ) /**< (CAN) Mailbox 4 Flag */
#define AT91C_CAN_MB5         (0x1 << 5 ) /**< (CAN) Mailbox 5 Flag */
#define AT91C_CAN_MB6         (0x1 << 6 ) /**< (CAN) Mailbox 6 Flag */
#define AT91C_CAN_MB7         (0x1 << 7 ) /**< (CAN) Mailbox 7 Flag */
#define AT91C_CAN_MB8         (0x1 << 8 ) /**< (CAN) Mailbox 8 Flag */
#define AT91C_CAN_MB9         (0x1 << 9 ) /**< (CAN) Mailbox 9 Flag */
#define AT91C_CAN_MB10        (0x1 << 10) /**< (CAN) Mailbox 10 Flag */
#define AT91C_CAN_MB11        (0x1 << 11) /**< (CAN) Mailbox 11 Flag */
#define AT91C_CAN_MB12        (0x1 << 12) /**< (CAN) Mailbox 12 Flag */
#define AT91C_CAN_MB13        (0x1 << 13) /**< (CAN) Mailbox 13 Flag */
#define AT91C_CAN_MB14        (0x1 << 14) /**< (CAN) Mailbox 14 Flag */
#define AT91C_CAN_MB15        (0x1 << 15) /**< (CAN) Mailbox 15 Flag */
#define AT91C_CAN_ERRA        (0x1 << 16) /**< (CAN) Error Active Mode Flag */
#define AT91C_CAN_WARN        (0x1 << 17) /**< (CAN) Warning Limit Flag */
#define AT91C_CAN_ERRP        (0x1 << 18) /**< (CAN) Error Passive Mode Flag */
#define AT91C_CAN_BOFF        (0x1 << 19) /**< (CAN) Bus Off Mode Flag */
#define AT91C_CAN_SLEEP       (0x1 << 20) /**< (CAN) Sleep Flag */
#define AT91C_CAN_WAKEUP      (0x1 << 21) /**< (CAN) Wakeup Flag */
#define AT91C_CAN_TOVF        (0x1 << 22) /**< (CAN) Timer Overflow Flag */
#define AT91C_CAN_TSTP        (0x1 << 23) /**< (CAN) Timestamp Flag */
#define AT91C_CAN_CERR        (0x1 << 24) /**< (CAN) CRC Error */
#define AT91C_CAN_SERR        (0x1 << 25) /**< (CAN) Stuffing Error */
#define AT91C_CAN_AERR        (0x1 << 26) /**< (CAN) Acknowledgment Error */
#define AT91C_CAN_FERR        (0x1 << 27) /**< (CAN) Form Error */
#define AT91C_CAN_BERR        (0x1 << 28) /**< (CAN) Bit Error */
/* --- Register CAN_IDR */
#define AT91C_CAN_MB0         (0x1 << 0 ) /**< (CAN) Mailbox 0 Flag */
#define AT91C_CAN_MB1         (0x1 << 1 ) /**< (CAN) Mailbox 1 Flag */
#define AT91C_CAN_MB2         (0x1 << 2 ) /**< (CAN) Mailbox 2 Flag */
#define AT91C_CAN_MB3         (0x1 << 3 ) /**< (CAN) Mailbox 3 Flag */
#define AT91C_CAN_MB4         (0x1 << 4 ) /**< (CAN) Mailbox 4 Flag */
#define AT91C_CAN_MB5         (0x1 << 5 ) /**< (CAN) Mailbox 5 Flag */
#define AT91C_CAN_MB6         (0x1 << 6 ) /**< (CAN) Mailbox 6 Flag */
#define AT91C_CAN_MB7         (0x1 << 7 ) /**< (CAN) Mailbox 7 Flag */
#define AT91C_CAN_MB8         (0x1 << 8 ) /**< (CAN) Mailbox 8 Flag */
#define AT91C_CAN_MB9         (0x1 << 9 ) /**< (CAN) Mailbox 9 Flag */
#define AT91C_CAN_MB10        (0x1 << 10) /**< (CAN) Mailbox 10 Flag */
#define AT91C_CAN_MB11        (0x1 << 11) /**< (CAN) Mailbox 11 Flag */
#define AT91C_CAN_MB12        (0x1 << 12) /**< (CAN) Mailbox 12 Flag */
#define AT91C_CAN_MB13        (0x1 << 13) /**< (CAN) Mailbox 13 Flag */
#define AT91C_CAN_MB14        (0x1 << 14) /**< (CAN) Mailbox 14 Flag */
#define AT91C_CAN_MB15        (0x1 << 15) /**< (CAN) Mailbox 15 Flag */
#define AT91C_CAN_ERRA        (0x1 << 16) /**< (CAN) Error Active Mode Flag */
#define AT91C_CAN_WARN        (0x1 << 17) /**< (CAN) Warning Limit Flag */
#define AT91C_CAN_ERRP        (0x1 << 18) /**< (CAN) Error Passive Mode Flag */
#define AT91C_CAN_BOFF        (0x1 << 19) /**< (CAN) Bus Off Mode Flag */
#define AT91C_CAN_SLEEP       (0x1 << 20) /**< (CAN) Sleep Flag */
#define AT91C_CAN_WAKEUP      (0x1 << 21) /**< (CAN) Wakeup Flag */
#define AT91C_CAN_TOVF        (0x1 << 22) /**< (CAN) Timer Overflow Flag */
#define AT91C_CAN_TSTP        (0x1 << 23) /**< (CAN) Timestamp Flag */
#define AT91C_CAN_CERR        (0x1 << 24) /**< (CAN) CRC Error */
#define AT91C_CAN_SERR        (0x1 << 25) /**< (CAN) Stuffing Error */
#define AT91C_CAN_AERR        (0x1 << 26) /**< (CAN) Acknowledgment Error */
#define AT91C_CAN_FERR        (0x1 << 27) /**< (CAN) Form Error */
#define AT91C_CAN_BERR        (0x1 << 28) /**< (CAN) Bit Error */
/* --- Register CAN_IMR */
#define AT91C_CAN_MB0         (0x1 << 0 ) /**< (CAN) Mailbox 0 Flag */
#define AT91C_CAN_MB1         (0x1 << 1 ) /**< (CAN) Mailbox 1 Flag */
#define AT91C_CAN_MB2         (0x1 << 2 ) /**< (CAN) Mailbox 2 Flag */
#define AT91C_CAN_MB3         (0x1 << 3 ) /**< (CAN) Mailbox 3 Flag */
#define AT91C_CAN_MB4         (0x1 << 4 ) /**< (CAN) Mailbox 4 Flag */
#define AT91C_CAN_MB5         (0x1 << 5 ) /**< (CAN) Mailbox 5 Flag */
#define AT91C_CAN_MB6         (0x1 << 6 ) /**< (CAN) Mailbox 6 Flag */
#define AT91C_CAN_MB7         (0x1 << 7 ) /**< (CAN) Mailbox 7 Flag */
#define AT91C_CAN_MB8         (0x1 << 8 ) /**< (CAN) Mailbox 8 Flag */
#define AT91C_CAN_MB9         (0x1 << 9 ) /**< (CAN) Mailbox 9 Flag */
#define AT91C_CAN_MB10        (0x1 << 10) /**< (CAN) Mailbox 10 Flag */
#define AT91C_CAN_MB11        (0x1 << 11) /**< (CAN) Mailbox 11 Flag */
#define AT91C_CAN_MB12        (0x1 << 12) /**< (CAN) Mailbox 12 Flag */
#define AT91C_CAN_MB13        (0x1 << 13) /**< (CAN) Mailbox 13 Flag */
#define AT91C_CAN_MB14        (0x1 << 14) /**< (CAN) Mailbox 14 Flag */
#define AT91C_CAN_MB15        (0x1 << 15) /**< (CAN) Mailbox 15 Flag */
#define AT91C_CAN_ERRA        (0x1 << 16) /**< (CAN) Error Active Mode Flag */
#define AT91C_CAN_WARN        (0x1 << 17) /**< (CAN) Warning Limit Flag */
#define AT91C_CAN_ERRP        (0x1 << 18) /**< (CAN) Error Passive Mode Flag */
#define AT91C_CAN_BOFF        (0x1 << 19) /**< (CAN) Bus Off Mode Flag */
#define AT91C_CAN_SLEEP       (0x1 << 20) /**< (CAN) Sleep Flag */
#define AT91C_CAN_WAKEUP      (0x1 << 21) /**< (CAN) Wakeup Flag */
#define AT91C_CAN_TOVF        (0x1 << 22) /**< (CAN) Timer Overflow Flag */
#define AT91C_CAN_TSTP        (0x1 << 23) /**< (CAN) Timestamp Flag */
#define AT91C_CAN_CERR        (0x1 << 24) /**< (CAN) CRC Error */
#define AT91C_CAN_SERR        (0x1 << 25) /**< (CAN) Stuffing Error */
#define AT91C_CAN_AERR        (0x1 << 26) /**< (CAN) Acknowledgment Error */
#define AT91C_CAN_FERR        (0x1 << 27) /**< (CAN) Form Error */
#define AT91C_CAN_BERR        (0x1 << 28) /**< (CAN) Bit Error */
/* --- Register CAN_SR */
#define AT91C_CAN_MB0         (0x1 << 0 ) /**< (CAN) Mailbox 0 Flag */
#define AT91C_CAN_MB1         (0x1 << 1 ) /**< (CAN) Mailbox 1 Flag */
#define AT91C_CAN_MB2         (0x1 << 2 ) /**< (CAN) Mailbox 2 Flag */
#define AT91C_CAN_MB3         (0x1 << 3 ) /**< (CAN) Mailbox 3 Flag */
#define AT91C_CAN_MB4         (0x1 << 4 ) /**< (CAN) Mailbox 4 Flag */
#define AT91C_CAN_MB5         (0x1 << 5 ) /**< (CAN) Mailbox 5 Flag */
#define AT91C_CAN_MB6         (0x1 << 6 ) /**< (CAN) Mailbox 6 Flag */
#define AT91C_CAN_MB7         (0x1 << 7 ) /**< (CAN) Mailbox 7 Flag */
#define AT91C_CAN_MB8         (0x1 << 8 ) /**< (CAN) Mailbox 8 Flag */
#define AT91C_CAN_MB9         (0x1 << 9 ) /**< (CAN) Mailbox 9 Flag */
#define AT91C_CAN_MB10        (0x1 << 10) /**< (CAN) Mailbox 10 Flag */
#define AT91C_CAN_MB11        (0x1 << 11) /**< (CAN) Mailbox 11 Flag */
#define AT91C_CAN_MB12        (0x1 << 12) /**< (CAN) Mailbox 12 Flag */
#define AT91C_CAN_MB13        (0x1 << 13) /**< (CAN) Mailbox 13 Flag */
#define AT91C_CAN_MB14        (0x1 << 14) /**< (CAN) Mailbox 14 Flag */
#define AT91C_CAN_MB15        (0x1 << 15) /**< (CAN) Mailbox 15 Flag */
#define AT91C_CAN_ERRA        (0x1 << 16) /**< (CAN) Error Active Mode Flag */
#define AT91C_CAN_WARN        (0x1 << 17) /**< (CAN) Warning Limit Flag */
#define AT91C_CAN_ERRP        (0x1 << 18) /**< (CAN) Error Passive Mode Flag */
#define AT91C_CAN_BOFF        (0x1 << 19) /**< (CAN) Bus Off Mode Flag */
#define AT91C_CAN_SLEEP       (0x1 << 20) /**< (CAN) Sleep Flag */
#define AT91C_CAN_WAKEUP      (0x1 << 21) /**< (CAN) Wakeup Flag */
#define AT91C_CAN_TOVF        (0x1 << 22) /**< (CAN) Timer Overflow Flag */
#define AT91C_CAN_TSTP        (0x1 << 23) /**< (CAN) Timestamp Flag */
#define AT91C_CAN_CERR        (0x1 << 24) /**< (CAN) CRC Error */
#define AT91C_CAN_SERR        (0x1 << 25) /**< (CAN) Stuffing Error */
#define AT91C_CAN_AERR        (0x1 << 26) /**< (CAN) Acknowledgment Error */
#define AT91C_CAN_FERR        (0x1 << 27) /**< (CAN) Form Error */
#define AT91C_CAN_BERR        (0x1 << 28) /**< (CAN) Bit Error */
#define AT91C_CAN_RBSY        (0x1 << 29) /**< (CAN) Receiver Busy */
#define AT91C_CAN_TBSY        (0x1 << 30) /**< (CAN) Transmitter Busy */
#define AT91C_CAN_OVLY        (0x1 << 31) /**< (CAN) Overload Busy */
/* --- Register CAN_BR */
#define AT91C_CAN_PHASE2      (0x7 << 0 ) /**< (CAN) Phase 2 segment */
#define AT91C_CAN_PHASE1      (0x7 << 4 ) /**< (CAN) Phase 1 segment */
#define AT91C_CAN_PROPAG      (0x7 << 8 ) /**< (CAN) Programmation time segment */
#define AT91C_CAN_SYNC        (0x3 << 12) /**< (CAN) Re-synchronization jump width segment */
#define AT91C_CAN_BRP         (0x7F << 16) /**< (CAN) Baudrate Prescaler */
#define AT91C_CAN_SMP         (0x1 << 24) /**< (CAN) Sampling mode */
/* --- Register CAN_TIM */
#define AT91C_CAN_TIMER       (0xFFFF << 0 ) /**< (CAN) Timer field */
/* --- Register CAN_TIMESTP */
#define AT91C_CAN_MTIMESTAMP  (0xFFFF << 0 ) /**< (CAN) Timer Value */
/* --- Register CAN_ECR */
#define AT91C_CAN_REC         (0xFF << 0 ) /**< (CAN) Receive Error Counter */
#define AT91C_CAN_TEC         (0xFF << 16) /**< (CAN) Transmit Error Counter */
/* --- Register CAN_TCR */
#define AT91C_CAN_MB0         (0x1 << 0 ) /**< (CAN) Mailbox 0 Flag */
#define AT91C_CAN_MB1         (0x1 << 1 ) /**< (CAN) Mailbox 1 Flag */
#define AT91C_CAN_MB2         (0x1 << 2 ) /**< (CAN) Mailbox 2 Flag */
#define AT91C_CAN_MB3         (0x1 << 3 ) /**< (CAN) Mailbox 3 Flag */
#define AT91C_CAN_MB4         (0x1 << 4 ) /**< (CAN) Mailbox 4 Flag */
#define AT91C_CAN_MB5         (0x1 << 5 ) /**< (CAN) Mailbox 5 Flag */
#define AT91C_CAN_MB6         (0x1 << 6 ) /**< (CAN) Mailbox 6 Flag */
#define AT91C_CAN_MB7         (0x1 << 7 ) /**< (CAN) Mailbox 7 Flag */
#define AT91C_CAN_MB8         (0x1 << 8 ) /**< (CAN) Mailbox 8 Flag */
#define AT91C_CAN_MB9         (0x1 << 9 ) /**< (CAN) Mailbox 9 Flag */
#define AT91C_CAN_MB10        (0x1 << 10) /**< (CAN) Mailbox 10 Flag */
#define AT91C_CAN_MB11        (0x1 << 11) /**< (CAN) Mailbox 11 Flag */
#define AT91C_CAN_MB12        (0x1 << 12) /**< (CAN) Mailbox 12 Flag */
#define AT91C_CAN_MB13        (0x1 << 13) /**< (CAN) Mailbox 13 Flag */
#define AT91C_CAN_MB14        (0x1 << 14) /**< (CAN) Mailbox 14 Flag */
#define AT91C_CAN_MB15        (0x1 << 15) /**< (CAN) Mailbox 15 Flag */
#define AT91C_CAN_TIMRST      (0x1 << 31) /**< (CAN) Timer Reset Field */
/* --- Register CAN_ACR */
#define AT91C_CAN_MB0         (0x1 << 0 ) /**< (CAN) Mailbox 0 Flag */
#define AT91C_CAN_MB1         (0x1 << 1 ) /**< (CAN) Mailbox 1 Flag */
#define AT91C_CAN_MB2         (0x1 << 2 ) /**< (CAN) Mailbox 2 Flag */
#define AT91C_CAN_MB3         (0x1 << 3 ) /**< (CAN) Mailbox 3 Flag */
#define AT91C_CAN_MB4         (0x1 << 4 ) /**< (CAN) Mailbox 4 Flag */
#define AT91C_CAN_MB5         (0x1 << 5 ) /**< (CAN) Mailbox 5 Flag */
#define AT91C_CAN_MB6         (0x1 << 6 ) /**< (CAN) Mailbox 6 Flag */
#define AT91C_CAN_MB7         (0x1 << 7 ) /**< (CAN) Mailbox 7 Flag */
#define AT91C_CAN_MB8         (0x1 << 8 ) /**< (CAN) Mailbox 8 Flag */
#define AT91C_CAN_MB9         (0x1 << 9 ) /**< (CAN) Mailbox 9 Flag */
#define AT91C_CAN_MB10        (0x1 << 10) /**< (CAN) Mailbox 10 Flag */
#define AT91C_CAN_MB11        (0x1 << 11) /**< (CAN) Mailbox 11 Flag */
#define AT91C_CAN_MB12        (0x1 << 12) /**< (CAN) Mailbox 12 Flag */
#define AT91C_CAN_MB13        (0x1 << 13) /**< (CAN) Mailbox 13 Flag */
#define AT91C_CAN_MB14        (0x1 << 14) /**< (CAN) Mailbox 14 Flag */
#define AT91C_CAN_MB15        (0x1 << 15) /**< (CAN) Mailbox 15 Flag */

#endif /* __AT91SAM7X256_CAN_H */
