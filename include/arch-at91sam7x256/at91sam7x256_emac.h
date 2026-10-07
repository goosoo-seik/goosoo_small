/* linux/include/asm-arm/arch-at91sam7x256/at91sam7x256_emac.h
 * 
 * Hardware definition for the emac peripheral in the ATMEL at91sam7x256 processor
 * 
 * Generated  11/02/2005 (15:17:30) AT91 SW Application Group from EMACB_6119A V1.6
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


#ifndef __AT91SAM7X256_EMAC_H
#define __AT91SAM7X256_EMAC_H

/* -------------------------------------------------------- */
/* EMAC ID definitions for  AT91SAM7X256           */
/* -------------------------------------------------------- */
#ifndef AT91C_ID_EMAC
#define AT91C_ID_EMAC  	16 /**< Ethernet MAC id */
#endif /* AT91C_ID_EMAC */

/* -------------------------------------------------------- */
/* EMAC Base Address definitions for  AT91SAM7X256   */
/* -------------------------------------------------------- */
#define AT91C_BASE_EMAC      	0xFFFDC000 /**< EMAC base address */

/* -------------------------------------------------------- */
/* PIO definition for EMAC hardware peripheral */
/* -------------------------------------------------------- */
#define AT91C_PB16_ECOL     	(1 << 16) /**< Ethernet MAC Collision Detected */
#define AT91C_PB4_ECRS     	(1 << 4) /**< Ethernet MAC Carrier Sense/Carrier Sense and Data Valid */
#define AT91C_PB18_EF100    	(1 << 18) /**< Ethernet MAC Force 100 Mbits/sec */
#define AT91C_PB8_EMDC     	(1 << 8) /**< Ethernet MAC Management Data Clock */
#define AT91C_PB9_EMDIO    	(1 << 9) /**< Ethernet MAC Management Data Input/Output */
#define AT91C_PB5_ERX0     	(1 << 5) /**< Ethernet MAC Receive Data 0 */
#define AT91C_PB6_ERX1     	(1 << 6) /**< Ethernet MAC Receive Data 1 */
#define AT91C_PB13_ERX2     	(1 << 13) /**< Ethernet MAC Receive Data 2 */
#define AT91C_PB14_ERX3     	(1 << 14) /**< Ethernet MAC Receive Data 3 */
#define AT91C_PB17_ERXCK    	(1 << 17) /**< Ethernet MAC Receive Clock */
#define AT91C_PB15_ERXDV_ECRSDV 	(1 << 15) /**< Ethernet MAC Receive Data Valid */
#define AT91C_PB7_ERXER    	(1 << 7) /**< Ethernet MAC Receive Error */
#define AT91C_PB2_ETX0     	(1 << 2) /**< Ethernet MAC Transmit Data 0 */
#define AT91C_PB3_ETX1     	(1 << 3) /**< Ethernet MAC Transmit Data 1 */
#define AT91C_PB10_ETX2     	(1 << 10) /**< Ethernet MAC Transmit Data 2 */
#define AT91C_PB11_ETX3     	(1 << 11) /**< Ethernet MAC Transmit Data 3 */
#define AT91C_PB0_ETXCK_EREFCK 	(1 << 0) /**< Ethernet MAC Transmit Clock/Reference Clock */
#define AT91C_PB1_ETXEN    	(1 << 1) /**< Ethernet MAC Transmit Enable */
#define AT91C_PB12_ETXER    	(1 << 12) /**< Ethernet MAC Transmikt Coding Error */


/* -------------------------------------------------------- */
/* Register offset definition for EMAC hardware peripheral */
/* -------------------------------------------------------- */
#define EMAC_NCR 	(0x0000) 	/**< Network Control Register */
#define EMAC_NCFGR 	(0x0004) 	/**< Network Configuration Register */
#define EMAC_NSR 	(0x0008) 	/**< Network Status Register */
#define EMAC_TSR 	(0x0014) 	/**< Transmit Status Register */
#define EMAC_RBQP 	(0x0018) 	/**< Receive Buffer Queue Pointer */
#define EMAC_TBQP 	(0x001C) 	/**< Transmit Buffer Queue Pointer */
#define EMAC_RSR 	(0x0020) 	/**< Receive Status Register */
#define EMAC_ISR 	(0x0024) 	/**< Interrupt Status Register */
#define EMAC_IER 	(0x0028) 	/**< Interrupt Enable Register */
#define EMAC_IDR 	(0x002C) 	/**< Interrupt Disable Register */
#define EMAC_IMR 	(0x0030) 	/**< Interrupt Mask Register */
#define EMAC_MAN 	(0x0034) 	/**< PHY Maintenance Register */
#define EMAC_PTR 	(0x0038) 	/**< Pause Time Register */
#define EMAC_PFR 	(0x003C) 	/**< Pause Frames received Register */
#define EMAC_FTO 	(0x0040) 	/**< Frames Transmitted OK Register */
#define EMAC_SCF 	(0x0044) 	/**< Single Collision Frame Register */
#define EMAC_MCF 	(0x0048) 	/**< Multiple Collision Frame Register */
#define EMAC_FRO 	(0x004C) 	/**< Frames Received OK Register */
#define EMAC_FCSE 	(0x0050) 	/**< Frame Check Sequence Error Register */
#define EMAC_ALE 	(0x0054) 	/**< Alignment Error Register */
#define EMAC_DTF 	(0x0058) 	/**< Deferred Transmission Frame Register */
#define EMAC_LCOL 	(0x005C) 	/**< Late Collision Register */
#define EMAC_ECOL 	(0x0060) 	/**< Excessive Collision Register */
#define EMAC_TUND 	(0x0064) 	/**< Transmit Underrun Error Register */
#define EMAC_CSE 	(0x0068) 	/**< Carrier Sense Error Register */
#define EMAC_RRE 	(0x006C) 	/**< Receive Ressource Error Register */
#define EMAC_ROV 	(0x0070) 	/**< Receive Overrun Errors Register */
#define EMAC_RSE 	(0x0074) 	/**< Receive Symbol Errors Register */
#define EMAC_ELE 	(0x0078) 	/**< Excessive Length Errors Register */
#define EMAC_RJA 	(0x007C) 	/**< Receive Jabbers Register */
#define EMAC_USF 	(0x0080) 	/**< Undersize Frames Register */
#define EMAC_STE 	(0x0084) 	/**< SQE Test Error Register */
#define EMAC_RLE 	(0x0088) 	/**< Receive Length Field Mismatch Register */
#define EMAC_TPF 	(0x008C) 	/**< Transmitted Pause Frames Register */
#define EMAC_HRB 	(0x0090) 	/**< Hash Address Bottom[31:0] */
#define EMAC_HRT 	(0x0094) 	/**< Hash Address Top[63:32] */
#define EMAC_SA1L 	(0x0098) 	/**< Specific Address 1 Bottom, First 4 bytes */
#define EMAC_SA1H 	(0x009C) 	/**< Specific Address 1 Top, Last 2 bytes */
#define EMAC_SA2L 	(0x00A0) 	/**< Specific Address 2 Bottom, First 4 bytes */
#define EMAC_SA2H 	(0x00A4) 	/**< Specific Address 2 Top, Last 2 bytes */
#define EMAC_SA3L 	(0x00A8) 	/**< Specific Address 3 Bottom, First 4 bytes */
#define EMAC_SA3H 	(0x00AC) 	/**< Specific Address 3 Top, Last 2 bytes */
#define EMAC_SA4L 	(0x00B0) 	/**< Specific Address 4 Bottom, First 4 bytes */
#define EMAC_SA4H 	(0x00B4) 	/**< Specific Address 4 Top, Last 2 bytes */
#define EMAC_TID 	(0x00B8) 	/**< Type ID Checking Register */
#define EMAC_TPQ 	(0x00BC) 	/**< Transmit Pause Quantum Register */
#define EMAC_USRIO 	(0x00C0) 	/**< USER Input/Output Register */
#define EMAC_WOL 	(0x00C4) 	/**< Wake On LAN Register */
#define EMAC_REV 	(0x00FC) 	/**< Revision Register */

/* -------------------------------------------------------- */
/* Bitfields definition for EMAC hardware peripheral */
/* -------------------------------------------------------- */
/* --- Register EMAC_NCR */
#define AT91C_EMAC_LB         (0x1 << 0 ) /**< (EMAC) Loopback. Optional. When set, loopback signal is at high level. */
#define AT91C_EMAC_LLB        (0x1 << 1 ) /**< (EMAC) Loopback local.  */
#define AT91C_EMAC_RE         (0x1 << 2 ) /**< (EMAC) Receive enable.  */
#define AT91C_EMAC_TE         (0x1 << 3 ) /**< (EMAC) Transmit enable.  */
#define AT91C_EMAC_MPE        (0x1 << 4 ) /**< (EMAC) Management port enable.  */
#define AT91C_EMAC_CLRSTAT    (0x1 << 5 ) /**< (EMAC) Clear statistics registers.  */
#define AT91C_EMAC_INCSTAT    (0x1 << 6 ) /**< (EMAC) Increment statistics registers.  */
#define AT91C_EMAC_WESTAT     (0x1 << 7 ) /**< (EMAC) Write enable for statistics registers.  */
#define AT91C_EMAC_BP         (0x1 << 8 ) /**< (EMAC) Back pressure.  */
#define AT91C_EMAC_TSTART     (0x1 << 9 ) /**< (EMAC) Start Transmission.  */
#define AT91C_EMAC_THALT      (0x1 << 10) /**< (EMAC) Transmission Halt.  */
#define AT91C_EMAC_TPFR       (0x1 << 11) /**< (EMAC) Transmit pause frame  */
#define AT91C_EMAC_TZQ        (0x1 << 12) /**< (EMAC) Transmit zero quantum pause frame */
/* --- Register EMAC_NCFGR */
#define AT91C_EMAC_SPD        (0x1 << 0 ) /**< (EMAC) Speed.  */
#define AT91C_EMAC_FD         (0x1 << 1 ) /**< (EMAC) Full duplex.  */
#define AT91C_EMAC_JFRAME     (0x1 << 3 ) /**< (EMAC) Jumbo Frames.  */
#define AT91C_EMAC_CAF        (0x1 << 4 ) /**< (EMAC) Copy all frames.  */
#define AT91C_EMAC_NBC        (0x1 << 5 ) /**< (EMAC) No broadcast.  */
#define AT91C_EMAC_MTI        (0x1 << 6 ) /**< (EMAC) Multicast hash event enable */
#define AT91C_EMAC_UNI        (0x1 << 7 ) /**< (EMAC) Unicast hash enable.  */
#define AT91C_EMAC_BIG        (0x1 << 8 ) /**< (EMAC) Receive 1522 bytes.  */
#define AT91C_EMAC_EAE        (0x1 << 9 ) /**< (EMAC) External address match enable.  */
#define AT91C_EMAC_CLK        (0x3 << 10) /**< (EMAC)  */
#define 	AT91C_EMAC_CLK_HCLK_8               (0x0 << 10) /**< (EMAC) HCLK divided by 8 */
#define 	AT91C_EMAC_CLK_HCLK_16              (0x1 << 10) /**< (EMAC) HCLK divided by 16 */
#define 	AT91C_EMAC_CLK_HCLK_32              (0x2 << 10) /**< (EMAC) HCLK divided by 32 */
#define 	AT91C_EMAC_CLK_HCLK_64              (0x3 << 10) /**< (EMAC) HCLK divided by 64 */
#define AT91C_EMAC_RTY        (0x1 << 12) /**< (EMAC)  */
#define AT91C_EMAC_PAE        (0x1 << 13) /**< (EMAC)  */
#define AT91C_EMAC_RBOF       (0x3 << 14) /**< (EMAC)  */
#define 	AT91C_EMAC_RBOF_OFFSET_0             (0x0 << 14) /**< (EMAC) no offset from start of receive buffer */
#define 	AT91C_EMAC_RBOF_OFFSET_1             (0x1 << 14) /**< (EMAC) one byte offset from start of receive buffer */
#define 	AT91C_EMAC_RBOF_OFFSET_2             (0x2 << 14) /**< (EMAC) two bytes offset from start of receive buffer */
#define 	AT91C_EMAC_RBOF_OFFSET_3             (0x3 << 14) /**< (EMAC) three bytes offset from start of receive buffer */
#define AT91C_EMAC_RLCE       (0x1 << 16) /**< (EMAC) Receive Length field Checking Enable */
#define AT91C_EMAC_DRFCS      (0x1 << 17) /**< (EMAC) Discard Receive FCS */
#define AT91C_EMAC_EFRHD      (0x1 << 18) /**< (EMAC)  */
#define AT91C_EMAC_IRXFCS     (0x1 << 19) /**< (EMAC) Ignore RX FCS */
/* --- Register EMAC_NSR */
#define AT91C_EMAC_LINKR      (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_MDIO       (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_IDLE       (0x1 << 2 ) /**< (EMAC)  */
/* --- Register EMAC_TSR */
#define AT91C_EMAC_UBR        (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_COL        (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_RLES       (0x1 << 2 ) /**< (EMAC)  */
#define AT91C_EMAC_TGO        (0x1 << 3 ) /**< (EMAC) Transmit Go */
#define AT91C_EMAC_BEX        (0x1 << 4 ) /**< (EMAC) Buffers exhausted mid frame */
#define AT91C_EMAC_COMP       (0x1 << 5 ) /**< (EMAC)  */
#define AT91C_EMAC_UND        (0x1 << 6 ) /**< (EMAC)  */
/* --- Register EMAC_RSR */
#define AT91C_EMAC_BNA        (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_REC        (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_OVR        (0x1 << 2 ) /**< (EMAC)  */
/* --- Register EMAC_ISR */
#define AT91C_EMAC_MFD        (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_RCOMP      (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_RXUBR      (0x1 << 2 ) /**< (EMAC)  */
#define AT91C_EMAC_TXUBR      (0x1 << 3 ) /**< (EMAC)  */
#define AT91C_EMAC_TUNDR      (0x1 << 4 ) /**< (EMAC)  */
#define AT91C_EMAC_RLEX       (0x1 << 5 ) /**< (EMAC)  */
#define AT91C_EMAC_TXERR      (0x1 << 6 ) /**< (EMAC)  */
#define AT91C_EMAC_TCOMP      (0x1 << 7 ) /**< (EMAC)  */
#define AT91C_EMAC_LINK       (0x1 << 9 ) /**< (EMAC)  */
#define AT91C_EMAC_ROVR       (0x1 << 10) /**< (EMAC)  */
#define AT91C_EMAC_HRESP      (0x1 << 11) /**< (EMAC)  */
#define AT91C_EMAC_PFRE       (0x1 << 12) /**< (EMAC)  */
#define AT91C_EMAC_PTZ        (0x1 << 13) /**< (EMAC)  */
/* --- Register EMAC_IER */
#define AT91C_EMAC_MFD        (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_RCOMP      (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_RXUBR      (0x1 << 2 ) /**< (EMAC)  */
#define AT91C_EMAC_TXUBR      (0x1 << 3 ) /**< (EMAC)  */
#define AT91C_EMAC_TUNDR      (0x1 << 4 ) /**< (EMAC)  */
#define AT91C_EMAC_RLEX       (0x1 << 5 ) /**< (EMAC)  */
#define AT91C_EMAC_TXERR      (0x1 << 6 ) /**< (EMAC)  */
#define AT91C_EMAC_TCOMP      (0x1 << 7 ) /**< (EMAC)  */
#define AT91C_EMAC_LINK       (0x1 << 9 ) /**< (EMAC)  */
#define AT91C_EMAC_ROVR       (0x1 << 10) /**< (EMAC)  */
#define AT91C_EMAC_HRESP      (0x1 << 11) /**< (EMAC)  */
#define AT91C_EMAC_PFRE       (0x1 << 12) /**< (EMAC)  */
#define AT91C_EMAC_PTZ        (0x1 << 13) /**< (EMAC)  */
/* --- Register EMAC_IDR */
#define AT91C_EMAC_MFD        (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_RCOMP      (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_RXUBR      (0x1 << 2 ) /**< (EMAC)  */
#define AT91C_EMAC_TXUBR      (0x1 << 3 ) /**< (EMAC)  */
#define AT91C_EMAC_TUNDR      (0x1 << 4 ) /**< (EMAC)  */
#define AT91C_EMAC_RLEX       (0x1 << 5 ) /**< (EMAC)  */
#define AT91C_EMAC_TXERR      (0x1 << 6 ) /**< (EMAC)  */
#define AT91C_EMAC_TCOMP      (0x1 << 7 ) /**< (EMAC)  */
#define AT91C_EMAC_LINK       (0x1 << 9 ) /**< (EMAC)  */
#define AT91C_EMAC_ROVR       (0x1 << 10) /**< (EMAC)  */
#define AT91C_EMAC_HRESP      (0x1 << 11) /**< (EMAC)  */
#define AT91C_EMAC_PFRE       (0x1 << 12) /**< (EMAC)  */
#define AT91C_EMAC_PTZ        (0x1 << 13) /**< (EMAC)  */
/* --- Register EMAC_IMR */
#define AT91C_EMAC_MFD        (0x1 << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_RCOMP      (0x1 << 1 ) /**< (EMAC)  */
#define AT91C_EMAC_RXUBR      (0x1 << 2 ) /**< (EMAC)  */
#define AT91C_EMAC_TXUBR      (0x1 << 3 ) /**< (EMAC)  */
#define AT91C_EMAC_TUNDR      (0x1 << 4 ) /**< (EMAC)  */
#define AT91C_EMAC_RLEX       (0x1 << 5 ) /**< (EMAC)  */
#define AT91C_EMAC_TXERR      (0x1 << 6 ) /**< (EMAC)  */
#define AT91C_EMAC_TCOMP      (0x1 << 7 ) /**< (EMAC)  */
#define AT91C_EMAC_LINK       (0x1 << 9 ) /**< (EMAC)  */
#define AT91C_EMAC_ROVR       (0x1 << 10) /**< (EMAC)  */
#define AT91C_EMAC_HRESP      (0x1 << 11) /**< (EMAC)  */
#define AT91C_EMAC_PFRE       (0x1 << 12) /**< (EMAC)  */
#define AT91C_EMAC_PTZ        (0x1 << 13) /**< (EMAC)  */
/* --- Register EMAC_MAN */
#define AT91C_EMAC_DATA       (0xFFFF << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_CODE       (0x3 << 16) /**< (EMAC)  */
#define AT91C_EMAC_REGA       (0x1F << 18) /**< (EMAC)  */
#define AT91C_EMAC_PHYA       (0x1F << 23) /**< (EMAC)  */
#define AT91C_EMAC_RW         (0x3 << 28) /**< (EMAC)  */
#define AT91C_EMAC_SOF        (0x3 << 30) /**< (EMAC)  */
/* --- Register EMAC_USRIO */
#define AT91C_EMAC_RMII       (0x1 << 0 ) /**< (EMAC) Reduce MII */
#define AT91C_EMAC_CLKEN      (0x1 << 1 ) /**< (EMAC) Clock Enable */
/* --- Register EMAC_WOL */
#define AT91C_EMAC_IP         (0xFFFF << 0 ) /**< (EMAC) ARP request IP address */
#define AT91C_EMAC_MAG        (0x1 << 16) /**< (EMAC) Magic packet event enable */
#define AT91C_EMAC_ARP        (0x1 << 17) /**< (EMAC) ARP request event enable */
#define AT91C_EMAC_SA1        (0x1 << 18) /**< (EMAC) Specific address register 1 event enable */
#define AT91C_EMAC_MTI        (0x1 << 19) /**< (EMAC) Multicast hash event enable */
/* --- Register EMAC_REV */
#define AT91C_EMAC_REVREF     (0xFFFF << 0 ) /**< (EMAC)  */
#define AT91C_EMAC_PARTREF    (0xFFFF << 16) /**< (EMAC)  */

#endif /* __AT91SAM7X256_EMAC_H */
