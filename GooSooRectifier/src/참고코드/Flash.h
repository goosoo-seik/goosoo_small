//  ----------------------------------------------------------------------------
//          ATMEL Microcontroller Software Support  -  ROUSSET  -
//  ----------------------------------------------------------------------------
//  DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
//  DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  ----------------------------------------------------------------------------
//* File Name           : Flash.h
//* Object              : Flash constan description
//* Creation            : JPP  30/Jun/2004
//*
//*----------------------------------------------------------------------------
#ifndef Flash_h
#define Flash_h

/*-------------------------------*/
/* Flash Status Field Definition */
/*-------------------------------*/

#define 	AT91C_MC_FSR_MVM 		((unsigned int) 0xFF << 8)		// (MC) Status Register GPNVMx: General-purpose NVM Bit Status
#define 	AT91C_MC_FSR_LOCK 		((unsigned int) 0xFFFF << 16)	// (MC) Status Register LOCKSx: Lock Region x Lock Status


#define	 ERASE_VALUE 		0xFFFFFFFF

/*------------------------------*/
/* External function Definition */
/*------------------------------*/

/* Flash function */
extern void AT91F_Flash_Init(void);

#ifdef ADS_COMPIL
extern int AT91F_Flash_Check_Erase(unsigned int * start, unsigned int size);
extern int AT91F_Flash_Erase_All(void);
extern int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char MemSet);
extern int AT91F_Flash_Write_all( unsigned int Flash_Address ,int size ,unsigned int * buff);
/* Lock Bits functions */
extern int AT91F_Flash_Lock_Status(void);
extern int AT91F_Flash_Lock (unsigned int Flash_Lock);
extern int AT91F_Flash_Unlock(unsigned int Flash_Lock);
/* NVM bits functions */
extern int AT91F_NVM_Status(void);
extern int AT91F_NVM_Set (unsigned char NVM_Number);
extern int AT91F_NVM_Clear(unsigned char NVM_Number);
#else
extern __ramfunc int AT91F_Flash_Check_Erase(unsigned int * start, unsigned int size);
extern __ramfunc int AT91F_Flash_Erase_All(void);
extern __ramfunc int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char MemSet);
extern __ramfunc int AT91F_Flash_Write_all( unsigned int Flash_Address ,int size ,unsigned int * buff);
/* Lock Bits functions */
extern __ramfunc int AT91F_Flash_Lock_Status(void);
extern __ramfunc int AT91F_Flash_Lock (unsigned int Flash_Lock);
extern __ramfunc int AT91F_Flash_Unlock(unsigned int Flash_Lock);
/* NVM bits functions */
extern __ramfunc int AT91F_NVM_Status(void);
extern __ramfunc int AT91F_NVM_Set (unsigned char NVM_Number);
extern __ramfunc int AT91F_NVM_Clear(unsigned char NVM_Number);
#endif


/* Security bit function */
extern int AT91F_SET_Security_Status (void);
extern int AT91F_SET_Security (void);


#endif /* Flash_h */
