/* linux/include/asm-arm/arch-at91sam7x256/at91sam7x256_can_mb.h
 * 
 * Hardware definition for the can_mb peripheral in the ATMEL at91sam7x256 processor
 * 
 * Generated  11/02/2005 (15:17:30) AT91 SW Application Group from  V
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


#ifndef __AT91SAM7X256_CAN_MB_H
#define __AT91SAM7X256_CAN_MB_H

/* -------------------------------------------------------- */
/* CAN_MB ID definitions for  AT91SAM7X256           */
/* -------------------------------------------------------- */

/* -------------------------------------------------------- */
/* CAN_MB Base Address definitions for  AT91SAM7X256   */
/* -------------------------------------------------------- */
#define AT91C_BASE_CAN_MB0   	0xFFFD0200 /**< CAN_MB0 base address */
#define AT91C_BASE_CAN_MB1   	0xFFFD0220 /**< CAN_MB1 base address */
#define AT91C_BASE_CAN_MB2   	0xFFFD0240 /**< CAN_MB2 base address */
#define AT91C_BASE_CAN_MB3   	0xFFFD0260 /**< CAN_MB3 base address */
#define AT91C_BASE_CAN_MB4   	0xFFFD0280 /**< CAN_MB4 base address */
#define AT91C_BASE_CAN_MB5   	0xFFFD02A0 /**< CAN_MB5 base address */
#define AT91C_BASE_CAN_MB6   	0xFFFD02C0 /**< CAN_MB6 base address */
#define AT91C_BASE_CAN_MB7   	0xFFFD02E0 /**< CAN_MB7 base address */

/* -------------------------------------------------------- */
/* PIO definition for CAN_MB hardware peripheral */
/* -------------------------------------------------------- */

/* -------------------------------------------------------- */
/* Register offset definition for CAN_MB hardware peripheral */
/* -------------------------------------------------------- */
#define CAN_MB_MMR 	(0x0000) 	/**< MailBox Mode Register */
#define CAN_MB_MAM 	(0x0004) 	/**< MailBox Acceptance Mask Register */
#define CAN_MB_MID 	(0x0008) 	/**< MailBox ID Register */
#define CAN_MB_MFID 	(0x000C) 	/**< MailBox Family ID Register */
#define CAN_MB_MSR 	(0x0010) 	/**< MailBox Status Register */
#define CAN_MB_MDL 	(0x0014) 	/**< MailBox Data Low Register */
#define CAN_MB_MDH 	(0x0018) 	/**< MailBox Data High Register */
#define CAN_MB_MCR 	(0x001C) 	/**< MailBox Control Register */

/* -------------------------------------------------------- */
/* Bitfields definition for CAN_MB hardware peripheral */
/* -------------------------------------------------------- */
/* --- Register CAN_MMR */
#define AT91C_CAN_MTIMEMARK   (0xFFFF << 0 ) /**< (CAN_MB) Mailbox Timemark */
#define AT91C_CAN_PRIOR       (0xF << 16) /**< (CAN_MB) Mailbox Priority */
#define AT91C_CAN_MOT         (0x7 << 24) /**< (CAN_MB) Mailbox Object Type */
#define 	AT91C_CAN_MOT_DIS                  (0x0 << 24) /**< (CAN_MB)  */
#define 	AT91C_CAN_MOT_RX                   (0x1 << 24) /**< (CAN_MB)  */
#define 	AT91C_CAN_MOT_RXOVERWRITE          (0x2 << 24) /**< (CAN_MB)  */
#define 	AT91C_CAN_MOT_TX                   (0x3 << 24) /**< (CAN_MB)  */
#define 	AT91C_CAN_MOT_CONSUMER             (0x4 << 24) /**< (CAN_MB)  */
#define 	AT91C_CAN_MOT_PRODUCER             (0x5 << 24) /**< (CAN_MB)  */
/* --- Register CAN_MAM */
#define AT91C_CAN_MIDvB       (0x3FFFF << 0 ) /**< (CAN_MB) Complementary bits for identifier in extended mode */
#define AT91C_CAN_MIDvA       (0x7FF << 18) /**< (CAN_MB) Identifier for standard frame mode */
#define AT91C_CAN_MIDE        (0x1 << 29) /**< (CAN_MB) Identifier Version */
/* --- Register CAN_MID */
#define AT91C_CAN_MIDvB       (0x3FFFF << 0 ) /**< (CAN_MB) Complementary bits for identifier in extended mode */
#define AT91C_CAN_MIDvA       (0x7FF << 18) /**< (CAN_MB) Identifier for standard frame mode */
#define AT91C_CAN_MIDE        (0x1 << 29) /**< (CAN_MB) Identifier Version */
/* --- Register CAN_MFID */
/* --- Register CAN_MSR */
#define AT91C_CAN_MTIMESTAMP  (0xFFFF << 0 ) /**< (CAN_MB) Timer Value */
#define AT91C_CAN_MDLC        (0xF << 16) /**< (CAN_MB) Mailbox Data Length Code */
#define AT91C_CAN_MRTR        (0x1 << 20) /**< (CAN_MB) Mailbox Remote Transmission Request */
#define AT91C_CAN_MABT        (0x1 << 22) /**< (CAN_MB) Mailbox Message Abort */
#define AT91C_CAN_MRDY        (0x1 << 23) /**< (CAN_MB) Mailbox Ready */
#define AT91C_CAN_MMI         (0x1 << 24) /**< (CAN_MB) Mailbox Message Ignored */
/* --- Register CAN_MDL */
/* --- Register CAN_MDH */
/* --- Register CAN_MCR */
#define AT91C_CAN_MDLC        (0xF << 16) /**< (CAN_MB) Mailbox Data Length Code */
#define AT91C_CAN_MRTR        (0x1 << 20) /**< (CAN_MB) Mailbox Remote Transmission Request */
#define AT91C_CAN_MACR        (0x1 << 22) /**< (CAN_MB) Abort Request for Mailbox */
#define AT91C_CAN_MTCR        (0x1 << 23) /**< (CAN_MB) Mailbox Transfer Command */

#endif /* __AT91SAM7X256_CAN_MB_H */
