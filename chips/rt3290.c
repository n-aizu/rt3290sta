/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt3290.c

	Abstract:
	Specific funcitons and configurations for RT3290

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/

#ifdef RT3290

#include	"rt_config.h"

#ifndef RTMP_RF_RW_SUPPORT
#error "You Should Enable compile flag RTMP_RF_RW_SUPPORT for this chip"
#endif /* RTMP_RF_RW_SUPPORT */

UCHAR RT3290_EeBuffer[EEPROM_SIZE] = {};


FREQUENCY_ITEM FreqItems3290[] = {};
UCHAR NUM_OF_3290_CHNL = (sizeof(FreqItems3290) / sizeof(FREQUENCY_ITEM));

REG_PAIR   RT3290_RFRegTable[] = {
	{RF_R01,		0x0F},
	{RF_R02,		0x80},//0504
	{RF_R03,		0x08},//0504
	{RF_R04,		0x00},//0504	
	{RF_R06,		0xA0},//0901, Version B (lower down VCO current for 3.2G spur)
	{RF_R08,		0xF3},
	{RF_R09,		0x02},
	{RF_R10,		0x53},//0513
	{RF_R11,		0x4A},	
	{RF_R12,		0x46},//0504
	{RF_R13,		0x9F},
	{RF_R18,		0x02}, //0513
	{RF_R22,		0x20},
	{RF_R25,		0x83},//0901, Version B (for I/Q offset.)
	{RF_R27,		0x09},
	{RF_R29,		0x10},
	{RF_R30,		0x10}, //20100429
	{RF_R31,		0x80},
	{RF_R32,		0x80},	
	{RF_R33,		0x00}, //0513
	{RF_R34,		0x05}, //0901, Version B (disable VCO ind buffer hi current for 3.2 spur)
	{RF_R35,		0x12},
	{RF_R36,		0x00},	
	{RF_R38,		0x85},
	{RF_R39,		0x1B}, //0x0a->0x0b
	{RF_R40,		0x0B},//0901, Version B (for power consumption)
	{RF_R41,		0xBB},
	{RF_R42,		0xD5}, //1011
	{RF_R43,		0x7B}, //0513 1011 1209
	{RF_R44,		0x0E}, //0511
	{RF_R45,		0xA2}, //0511
	{RF_R46,		0x73},
	{RF_R47,		0x00},
	{RF_R48,		0x10},
	{RF_R49,		0x98}, //0511
	{RF_R52,		0x38}, //0511
	{RF_R53,		0x00}, //0901, Version B (just set drv_bc_cck to 0,it's no function) 1209
	{RF_R54,		0x78}, //0901, Version B
	{RF_R55,		0x43}, //0901, Version B
	{RF_R56,		0x02}, //0901, Version B 1209
	{RF_R57,		0x80}, //0504
	{RF_R58,		0x7F}, //0504 1011
	{RF_R59,		0x09}, //0511
	{RF_R60,		0x45}, //0504
	{RF_R61,		0xC1}, //0901, Version B	(increase gain margin and lower down 3.2G spur.)	
};

UCHAR RT3290_NUM_RF_REG_PARMS = (sizeof(RT3290_RFRegTable) / sizeof(REG_PAIR));

REG_PAIR   RT3290_BBPRegTable[] = {
	{BBP_R31, 0x08}, /* ADC/DAC control */
	{BBP_R65, 0x2C},		// fix rssi issue
	{BBP_R66, 0x38},		// Also set this default value to pAd->BbpTuning.R66CurrentValue at initial
	{BBP_R68, 0x0B},
	{BBP_R69, 0x12},
	{BBP_R70, 0xa},
	{BBP_R73, 0x13}, /* Rx AGC SQ Acorr threshold */
	{BBP_R75, 0x46}, /* Rx high power VGA offset for LNA offset */
	{BBP_R76, 0x28}, /* Rx medium power VGA offset for LNA offset */
	//{BBP_R77, 0x59},
	{BBP_R77, 0x58},
	{BBP_R81, 0x37},
	{BBP_R82, 0x62},
	{BBP_R83, 0x7A},
	//{BBP_R83, 0x6A},
	{BBP_R84, 0x9A},
	//{BBP_R84, 0x99},
	{BBP_R86, 0x38},
	//{BBP_R86, 0x00},
	{BBP_R91, 0x04},
	{BBP_R92, 0x02},
	{BBP_R103, 0xC0},
	//{BBP_R103, 0x00},		// Disable DC-Filter	
	{BBP_R104, 0x92},
	{BBP_R105, 0x1C},
	{BBP_R106, 0x03},
	{BBP_R128, 0x12},
};
UCHAR RT3290_NUM_BBP_REG_PARMS = (sizeof(RT3290_BBPRegTable) / sizeof(REG_PAIR));


RTMP_REG_PAIR	RT3290_MACRegTable[] =	{
	{TX_SW_CFG0, 0x404},
};
UCHAR RT3290_NUM_MAC_REG_PARMS = (sizeof(RT3290_MACRegTable) / sizeof(RTMP_REG_PAIR));


#ifdef RTMP_INTERNAL_TX_ALC
// TODO: shiang, actually, this table is the same as RT5390
ULONG RT3290_TssiRatioTable[][2] = 
	{
/*	{numerator,	denominator}	Power delta (dBm)	Ratio		Index */
	{955,		10000}, 		/* -12			0.0955	0 */
	{1161, 		10000},		/* -11			0.1161	1 */
	{1413,		10000}, 		/* -10			0.1413	2 */
	{1718,		10000},		/* -9			0.1718	3 */
	{2089, 		10000},		/* -8			0.2089	4 */
	{2541, 		10000}, 		/* -7 			0.2541	5 */
	{3090, 		10000}, 		/* -6 			0.3090	6 */
	{3758, 		10000}, 		/* -5 			0.3758	7 */
	{4571, 		10000}, 		/* -4 			0.4571	8 */
	{5559, 		10000}, 		/* -3 			0.5559	9 */
	{6761, 		10000}, 		/* -2 			0.6761	10 */
	{8222, 		10000}, 		/* -1 			0.8222	11 */
	{1, 			1}, 			/* 0	 			1		12 */
	{12162, 		10000}, 		/* 1				1.2162	13 */
	{14791, 		10000}, 		/* 2				1.4791	14 */
	{17989, 		10000}, 		/* 3				1.7989	15 */
	{21878, 		10000}, 		/* 4				2.1878	16 */
	{26607, 		10000}, 		/* 5				2.6607	17 */
	{32359, 		10000}, 		/* 6				3.2359	18 */
	{39355, 		10000}, 		/* 7				3.9355	19 */
	{47863, 		10000}, 		/* 8				4.7863	20 */
	{58210, 		10000}, 		/* 9				5.8210	21 */
	{70795, 		10000}, 		/* 10			7.0795	22 */
	{86099, 		10000}, 		/* 11			8.6099	23 */
	{104713, 		10000}, 		/* 12			10.4713	24 */
};

extern TX_POWER_TUNING_ENTRY_STRUCT *TxPowerTuningTable;

TX_POWER_TUNING_ENTRY_STRUCT RT3290_TxPowerTuningTable[] = {
//	idxTxPowerTable		Tx power control over RF		Tx power control over MAC
//	(zero-based array)		{ RF R49[5:0]: Tx0 ALC},		{MAC 0x1314~0x1324}
/*     0       */				{0x00,					-15}, 
/*     1       */ 				{0x01,					-15}, 
/*     2       */ 				{0x00,					-14}, 
/*     3       */ 				{0x01,					-14}, 
/*     4       */ 				{0x00,					-13}, 
/*     5       */				{0x01,					-13}, 
/*     6       */ 				{0x00,					-12}, 
/*     7       */ 				{0x01,					-12}, 
/*     8       */ 				{0x00,					-11}, 
/*     9       */ 				{0x01,					-11}, 
/*     10     */ 				{0x00,					-10}, 
/*     11     */ 				{0x01,					-10}, 
/*     12     */ 				{0x00,					-9}, 
/*     13     */ 				{0x01,					-9}, 
/*     14     */ 				{0x00,					-8}, 
/*     15     */ 				{0x01,					-8}, 
/*     16     */ 				{0x00,					-7}, 
/*     17     */ 				{0x01,					-7}, 
/*     18     */ 				{0x00,					-6}, 
/*     19     */ 				{0x01,					-6}, 
/*     20     */ 				{0x00,					-5}, 
/*     21     */ 				{0x01,					-5}, 
/*     22     */ 				{0x00,					-4}, 
/*     23     */ 				{0x01,					-4}, 
/*     24     */ 				{0x00,					-3}, 
/*     25     */ 				{0x01,					-3}, 
/*     26     */ 				{0x00,					-2}, 
/*     27     */ 				{0x01,					-2}, 
/*     28     */ 				{0x00,					-1}, 
/*     29     */ 				{0x01,					-1}, 
/*     30     */ 				{0x00,					0}, 
/*     31     */ 				{0x01,					0}, 
/*     32     */ 				{0x02,					0}, 
/*     33     */ 				{0x03,					0}, 
/*     34     */ 				{0x04,					0}, 
/*     35     */ 				{0x05,					0}, 
/*     36     */ 				{0x06,					0}, 
/*     37     */ 				{0x07,					0}, 
/*     38     */ 				{0x08,					0}, 
/*     39     */ 				{0x09,					0}, 
/*     40     */ 				{0x0A,					0}, 
/*     41     */ 				{0x0B,					0}, 
/*     42     */ 				{0x0C,					0}, 
/*     43     */ 				{0x0D,					0}, 
/*     44     */ 				{0x0E,					0}, 
/*     45     */ 				{0x0F,					0}, 
/*     46     */ 				{0x10,					0}, 
/*     47     */ 				{0x11,					0}, 
/*     48     */ 				{0x12,					0}, 
/*     49     */ 				{0x13,					0}, 
/*     50     */ 				{0x14,					0}, 
/*     51     */ 				{0x15,					0}, 
/*     52     */ 				{0x16,					0}, 
/*     53     */ 				{0x17,					0}, 
/*     54     */ 				{0x18,					0}, 
/*     55     */ 				{0x19,					0}, 
/*     56     */ 				{0x1A,					0}, 
/*     57     */ 				{0x1B,					0}, 
/*     58     */ 				{0x1C,					0}, 
/*     59     */ 				{0x1D,					0}, 
/*     60     */ 				{0x1E,					0}, 
/*     61     */ 				{0x1F,					0}, 
/*     62     */                         	{0x20,                                       0}, 
/*     63     */                         	{0x21,                                       0}, 
/*     64     */                         	{0x22,                                       0}, 
/*     65     */                          {0x23,                                       0}, 
/*     66     */                         	{0x24,                                       0}, 
/*     67     */                         	{0x25,                                       0}, 
/*     68     */                         	{0x26,                                       0}, 
/*     69     */                         	{0x27,                                       0}, 
/*     70     */                         	{0x27-1,                                   1}, 
/*     71     */                         	{0x27,                                       1}, 
/*     72     */                         	{0x27-1,                                   2}, 
/*     73     */                         	{0x27,                                       2}, 
/*     74     */                         	{0x27-1,                                   3}, 
/*     75     */                       	{0x27,                                       3}, 
/*     76     */                       	{0x27-1,                                   4}, 
/*     77     */                       	{0x27,                                       4}, 
/*     78     */                       	{0x27-1,                                   5}, 
/*     79     */                       	{0x27,                                       5}, 
/*     80     */                       	{0x27-1,                                   6}, 
/*     81     */                       	{0x27,                                       6}, 
/*     82     */                       	{0x27-1,                                   7}, 
/*     83     */                       	{0x27,                                       7}, 
/*     84     */                       	{0x27-1,                                   8}, 
/*     85     */                       	{0x27,                                       8}, 
/*     86     */                       	{0x27-1,                                   9}, 
/*     87     */                       	{0x27,                                       9}, 
/*     88     */                       	{0x27-1,                                   10}, 
/*     89     */                       	{0x27,                                       10}, 
/*     90     */                       	{0x27-1,                                   11}, 
/*     91     */                       	{0x27,                                       11}, 
/*     92     */                       	{0x27-1,                                   12}, 
/*     93     */                       	{0x27,                                       12}, 
/*     94     */                      	{0x27-1,                                   13}, 
/*     95     */                       	{0x27,                                       13}, 
/*     96     */                       	{0x27-1,                                   14}, 
/*     97     */                       	{0x27,                                       14}, 
/*     98     */                       	{0x27-1,                                   15}, 
/*     99   */                        	{0x27,                                       15}, 
};


/* The desired TSSI over CCK (with extended TSSI information) */
CHAR RT3290_desiredTSSIOverCCKExt[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1][4];

/* The desired TSSI over OFDM (with extended TSSI information) */
CHAR RT3290_desiredTSSIOverOFDMExt[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1][8];

/* The desired TSSI over HT (with extended TSSI information) */
CHAR RT3290_desiredTSSIOverHTExt[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1][8];

/* The desired TSSI over HT using STBC (with extended TSSI information) */
CHAR RT3290_desiredTSSIOverHTUsingSTBCExt[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1][8];

CHAR RT3290_desiredTSSIOverHT40Ext[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1][8];


typedef enum{
	TSSI_INFO_0 = 0,
	TSSI_INFO_1 = 1,
	TSSI_INFO_2 = 2,
}TSSI_REPORT_SEL;

static BOOLEAN RT3290_Asic_Get_TSSI_Info(
	IN RTMP_ADAPTER *pAd,
	IN TSSI_REPORT_SEL report_sel,
	IN UCHAR *report_val)
{
	UCHAR bbp_val = 0;

	if (report_sel > 2)
		return FALSE;
	
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &bbp_val);
	bbp_val = ((bbp_val & ~0x03) | report_sel); 
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, bbp_val);

	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, report_val);
	return TRUE;
}



/*
   Rounding to integer
   e.g., +16.9 ~= 17 and -16.9 ~= -17

   Parameters
	  pAd: The adapter data structure
	  Integer: Integer part
	  Fraction: Fraction part
	  DenominatorOfTssiRatio: The denominator of the TSSI ratio

    Return Value:
	  Rounding result
*/
LONG Rounding(
	IN PRTMP_ADAPTER 			pAd, 
	IN LONG 						Integer, 
	IN LONG 						Fraction, 
	IN LONG 						DenominatorOfTssiRatio)
{
	LONG temp = 0;

	DBGPRINT(RT_DEBUG_INFO, ("%s: Integer = %d, Fraction = %d, DenominatorOfTssiRatio = %d\n", 
		__FUNCTION__, 
		(INT)Integer, 
		(INT)Fraction, 
		(INT)DenominatorOfTssiRatio));

	if (Fraction >= 0)
	{
		if (Fraction < (DenominatorOfTssiRatio / 10))
		{
			return Integer; /* e.g., 32.08059 ~= 32 */
		}
	}
	else
	{
		if (-Fraction < (DenominatorOfTssiRatio / 10))
		{
			return Integer; /* e.g., -32.08059 ~= -32 */
		}
	}

	if (Integer >= 0)
	{
		if (Fraction == 0)
		{
			return Integer;
		}
		else
		{
			do {
				if (Fraction == 0)
				{
					break;
				}
				else
				{
					temp = Fraction / 10;
					if (temp == 0)
					{
						break;
					}
					else
					{
						Fraction = temp;
					}
				}
			} while (1);

			DBGPRINT(RT_DEBUG_INFO, ("%s: [+] temp = %d, Fraction = %d\n", __FUNCTION__, (INT)temp, (INT)Fraction));

			if (Fraction >= 5)
			{
				return (Integer + 1);
			}
			else
			{
				return Integer;
			}
		}
	}
	else
	{
		if (Fraction == 0)
		{
			return Integer;
		}
		else
		{
			do {
				if (Fraction == 0)
				{
					break;
				}
				else
				{
					temp = Fraction / 10;
					if (temp == 0)
					{
						break;
					}
					else
					{
						Fraction = temp;
					}
				}
			} while (1);

			DBGPRINT(RT_DEBUG_INFO, ("%s: [-] temp = %d, Fraction = %d\n", __FUNCTION__, (INT)temp, (INT)Fraction));

			if (Fraction <= -5)
			{
				return (Integer - 1);
			}
			else
			{
				return Integer;
			}
		}
	}
}


CHAR desire_tssi_calc(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR ch,
	IN CHAR tb_idx,
	IN UCHAR tssi_base,
	IN CHAR tssi_ub,
	IN CHAR tssi_lb)
{
	UCHAR index;
	CHAR DesiredTssi;
	PSTRING print_string = "adjusted";
	
	index = GET_TSSI_RATE_TABLE_INDEX(tb_idx + pAd->TxPowerCtrl.PerChTxPwrOffset[ch]);
	DesiredTssi = (SHORT)Rounding(pAd, (tssi_base * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]),
								(tssi_base * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]),
								RT3290_TssiRatioTable[index][1]);
	
	if (DesiredTssi < tssi_lb)
		DesiredTssi = tssi_lb;
	else if (DesiredTssi > tssi_ub)
		DesiredTssi = tssi_ub;
	else
		print_string = "none adjusted";;
	DBGPRINT(RT_DEBUG_TRACE, ("%s DesiredTssi = 0x%02x\n", print_string, DesiredTssi));

	return DesiredTssi;
}


VOID RT3290_InitDesiredTSSITable(
	IN PRTMP_ADAPTER			pAd)
{
	INT			i = 0;
	CHAR 		DesiredTssi = 0;
	CHAR 		BWPowerDelta = 0;
	UCHAR 		ch = 0;
	UCHAR 		index = 0;
	UCHAR 		BbpR47 = 0;
	UCHAR 		RFValue = 0;
	UCHAR 		TSSIBase = 0; /* The TSSI over OFDM 54Mbps */
	USHORT 		Value = 0;
	USHORT 		TxPower = 0, TxPowerOFDM54 = 0;
	BOOLEAN 	bExtendedTssiMode = FALSE;
	EEPROM_TX_PWR_OFFSET_STRUC TxPwrOffset = {{0}};

	if (pAd->TxPowerCtrl.bInternalTxALC == FALSE)
	{
		return;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("---> %s\n", __FUNCTION__));

	
	{
		RT3290_eeprom_access_grant(pAd, TRUE);
		RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_DELTA, Value);
		
		if ((Value & 0xFF) == 0xFF) /* 20M/40M BW Power Delta for 2.4GHz band */
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: Don't considerate 20M/40M BW Delta Power since EEPROM is not calibrated.\n", __FUNCTION__));
		}
		else
		{
			if ((Value & 0xC0) == 0xC0)
			{
				BWPowerDelta += (Value & 0x3F); /* increase 40M BW Tx power with the delta value */
			}
			else if ((Value & 0xC0) == 0x80)
			{
				BWPowerDelta -= (Value & 0x3F); /* decrease 40M BW Tx power with the delta value */
			}
			else
				DBGPRINT(RT_DEBUG_TRACE, ("%s: 20/40M BW Delta Power is not enabled, Value = 0x%X\n", __FUNCTION__, Value));
		}
		
		RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_OVER_OFDM_54, Value);

		TSSIBase = (Value & 0x007F); /* range: bit6~bit0 */	

		RT28xx_EEPROM_READ16(pAd, (EEPROM_TSSI_STEP_OVER_2DOT4G - 1), Value);
		if (((Value >> 8) & 0x80) == 0x80) /* Enable the extended TSSI mode */
		{
			bExtendedTssiMode = TRUE;
		}
		else
		{
			bExtendedTssiMode = FALSE;
		}

		if (bExtendedTssiMode == TRUE) /* Tx power offset for the extended TSSI mode */
		{
			pAd->TxPowerCtrl.bExtendedTssiMode = TRUE;

			/* Get the per-channel Tx power offset */
			
			RT28xx_EEPROM_READ16(pAd, (EEPROM_TX_POWER_OFFSET_OVER_CH_1 - 1), TxPwrOffset.word);
			pAd->TxPowerCtrl.PerChTxPwrOffset[1] = (TxPwrOffset.field.Byte1 & 0x0F); /* Tx power offset over channel 1 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[2] = (((TxPwrOffset.field.Byte1 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 2 */

			RT28xx_EEPROM_READ16(pAd, EEPROM_TX_POWER_OFFSET_OVER_CH_3, TxPwrOffset.word);
			pAd->TxPowerCtrl.PerChTxPwrOffset[3] = (TxPwrOffset.field.Byte0 & 0x0F); /* Tx power offset over channel 3 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[4] = (((TxPwrOffset.field.Byte0 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 4 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[5] = (TxPwrOffset.field.Byte1 & 0x0F); /* Tx power offset over channel 5 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[6] = (((TxPwrOffset.field.Byte1 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 6 */

			RT28xx_EEPROM_READ16(pAd, EEPROM_TX_POWER_OFFSET_OVER_CH_7, TxPwrOffset.word);
			pAd->TxPowerCtrl.PerChTxPwrOffset[7] = (TxPwrOffset.field.Byte0 & 0x0F); /* Tx power offset over channel 7 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[8] = (((TxPwrOffset.field.Byte0 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 8 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[9] = (TxPwrOffset.field.Byte1 & 0x0F); /* Tx power offset over channel 9 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[10] = (((TxPwrOffset.field.Byte1 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 10 */

			RT28xx_EEPROM_READ16(pAd, EEPROM_TX_POWER_OFFSET_OVER_CH_11, TxPwrOffset.word);
			pAd->TxPowerCtrl.PerChTxPwrOffset[11] = (TxPwrOffset.field.Byte0 & 0x0F); /* Tx power offset over channel 11 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[12] = (((TxPwrOffset.field.Byte0 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 12 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[13] = (TxPwrOffset.field.Byte1 & 0x0F); /* Tx power offset over channel 13 */
			pAd->TxPowerCtrl.PerChTxPwrOffset[14] = (((TxPwrOffset.field.Byte1 & 0xF0) >> 4) & 0x0F); /* Tx power offset over channel 14 */

			/* 4-bit representation ==> 8-bit representation (2's complement) */
			
			for (i = 1; i <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; i++)
			{
				if ((pAd->TxPowerCtrl.PerChTxPwrOffset[i] & 0x08) == 0x00) /* Positive number */
				{
					pAd->TxPowerCtrl.PerChTxPwrOffset[i] = (pAd->TxPowerCtrl.PerChTxPwrOffset[i] & ~0xF8);
				}
				else /* 0x08: Negative number */
				{
					pAd->TxPowerCtrl.PerChTxPwrOffset[i] = (pAd->TxPowerCtrl.PerChTxPwrOffset[i] | 0xF0);
				}
			}

			DBGPRINT(RT_DEBUG_TRACE, ("%s: TxPwrOffset[1] = %d, TxPwrOffset[2] = %d, TxPwrOffset[3] = %d, TxPwrOffset[4] = %d\n", 
				__FUNCTION__, 
				pAd->TxPowerCtrl.PerChTxPwrOffset[1], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[2], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[3], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[4]));
			DBGPRINT(RT_DEBUG_TRACE, ("%s: TxPwrOffset[5] = %d, TxPwrOffset[6] = %d, TxPwrOffset[7] = %d, TxPwrOffset[8] = %d\n", 
				__FUNCTION__, 
				pAd->TxPowerCtrl.PerChTxPwrOffset[5], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[6], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[7], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[8]));
			DBGPRINT(RT_DEBUG_TRACE, ("%s: TxPwrOffset[9] = %d, TxPwrOffset[10] = %d, TxPwrOffset[11] = %d, TxPwrOffset[12] = %d\n", 
				__FUNCTION__, 
				pAd->TxPowerCtrl.PerChTxPwrOffset[9], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[10], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[11], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[12]));
			DBGPRINT(RT_DEBUG_TRACE, ("%s: TxPwrOffset[13] = %d, TxPwrOffset[14] = %d\n", 
				__FUNCTION__, 
				pAd->TxPowerCtrl.PerChTxPwrOffset[13], 
				pAd->TxPowerCtrl.PerChTxPwrOffset[14]));
		}
		else
		{
			pAd->TxPowerCtrl.bExtendedTssiMode = FALSE;
			RTMPZeroMemory(pAd->TxPowerCtrl.PerChTxPwrOffset, sizeof (pAd->TxPowerCtrl.PerChTxPwrOffset));
		}
		
		RT28xx_EEPROM_READ16(pAd, (EEPROM_OFDM_MCS6_MCS7 - 1), Value);
		TxPowerOFDM54 = (0x000F & (Value >> 8));

		DBGPRINT(RT_DEBUG_TRACE, ("%s: TSSIBase = 0x%X, TxPowerOFDM54 = 0x%X, , bExtendedTssiMode = %d\n", 
			__FUNCTION__, 
			TSSIBase, 
			TxPowerOFDM54,
			pAd->TxPowerCtrl.bExtendedTssiMode));

		/* The desired TSSI over CCK */

		DBGPRINT(RT_DEBUG_TRACE, ("%s: ------------------------------------------------------\n", __FUNCTION__));
		DBGPRINT(RT_DEBUG_TRACE, ("%s: The desired TSSI over CCK\n", __FUNCTION__));
		RT28xx_EEPROM_READ16(pAd, EEPROM_CCK_MCS0_MCS1, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xDE = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + 3 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));
			
			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverCCKExt[ch][MCS_0] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverCCKExt[ch][MCS_1] = (CHAR)DesiredTssi;
		}
		
		RT28xx_EEPROM_READ16(pAd, (EEPROM_CCK_MCS2_MCS3 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xDF = 0x%X\n", __FUNCTION__, TxPower));		

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + 3 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)			
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverCCKExt[ch][MCS_2] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverCCKExt[ch][MCS_3] = (CHAR)DesiredTssi;
		}

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, RT5390_desiredTSSIOverCCK[%d][0] = %d, RT5390_desiredTSSIOverCCK[%d][1] = %d, RT5390_desiredTSSIOverCCK[%d][2] = %d, RT5390_desiredTSSIOverCCK[%d][3] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverCCKExt[ch][0], 
				ch, 
				RT3290_desiredTSSIOverCCKExt[ch][1], 
				ch, 
				RT3290_desiredTSSIOverCCKExt[ch][2], 
				ch, 
				RT3290_desiredTSSIOverCCKExt[ch][3]));
		}
		
		/* The desired TSSI over OFDM */

		DBGPRINT(RT_DEBUG_TRACE, ("%s: ------------------------------------------------------\n", __FUNCTION__));
		DBGPRINT(RT_DEBUG_TRACE, ("%s: The desired TSSI over OFDM\n", __FUNCTION__));	
		RT28xx_EEPROM_READ16(pAd, EEPROM_OFDM_MCS0_MCS1, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE0 = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverOFDMExt[ch][MCS_0] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverOFDMExt[ch][MCS_1] = (CHAR)DesiredTssi;
		}

		RT28xx_EEPROM_READ16(pAd, (EEPROM_OFDM_MCS2_MCS3 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE1 = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverOFDMExt[ch][MCS_2] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverOFDMExt[ch][MCS_3] = (CHAR)DesiredTssi;
		}
		
		RT28xx_EEPROM_READ16(pAd, EEPROM_OFDM_MCS4_MCS5, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE2 = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverOFDMExt[ch][MCS_4] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverOFDMExt[ch][MCS_5] = (CHAR)DesiredTssi;

			index = GET_TSSI_RATE_TABLE_INDEX(pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));
			
			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverOFDMExt[ch][MCS_6] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverOFDMExt[ch][MCS_7] =(CHAR) DesiredTssi;
		}
		
		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverOFDM[%d][0] = %d, desiredTSSIOverOFDM[%d][1] = %d, desiredTSSIOverOFDM[%d][2] = %d, desiredTSSIOverOFDM[%d][3] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][0], 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][1], 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][2], 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][3]));
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverOFDM[%d][4] = %d, desiredTSSIOverOFDM[%d][5] = %d, desiredTSSIOverOFDM[[%d]6] = %d, desiredTSSIOverOFDM[%d][7] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][4], 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][5], 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][6], 
				ch, 
				RT3290_desiredTSSIOverOFDMExt[ch][7]));
		}

		/* The desired TSSI over HT */

		DBGPRINT(RT_DEBUG_TRACE, ("%s: ------------------------------------------------------\n", __FUNCTION__));
		DBGPRINT(RT_DEBUG_TRACE, ("%s: The desired TSSI over HT\n", __FUNCTION__));		
		RT28xx_EEPROM_READ16(pAd, EEPROM_HT_MCS0_MCS1, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE4 = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET + BWPowerDelta);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));
			
			/* Boundary verification: the desired TSSI value */
		
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTExt[ch][MCS_0] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTExt[ch][MCS_1] = (CHAR)DesiredTssi;
		}
		
		RT28xx_EEPROM_READ16(pAd, (EEPROM_HT_MCS2_MCS3 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE5 = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET + BWPowerDelta);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));
			
			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTExt[ch][MCS_2] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTExt[ch][MCS_3] = (CHAR)DesiredTssi;
		}

		RT28xx_EEPROM_READ16(pAd, EEPROM_HT_MCS4_MCS5, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE6 = 0x%X\n", __FUNCTION__, TxPower));
		
		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET + BWPowerDelta);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTExt[ch][MCS_4] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTExt[ch][MCS_5] = (CHAR)DesiredTssi;
		}

		RT28xx_EEPROM_READ16(pAd, (EEPROM_HT_MCS6_MCS7 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE7 = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET + BWPowerDelta);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTExt[ch][MCS_6] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTExt[ch][MCS_7] = (CHAR)DesiredTssi;
		}
		
		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverHT[%d][0] = %d, desiredTSSIOverHT[%d][1] = %d, desiredTSSIOverHT[%d][2] = %d, desiredTSSIOverHT[%d][3] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][0], 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][1], 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][2], 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][3]));
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverHT[%d][4] = %d, desiredTSSIOverHT[%d][5] = %d, desiredTSSIOverHT[%d][6] = %d, desiredTSSIOverHT[%d][7] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][4], 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][5], 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][6], 
				ch, 
				RT3290_desiredTSSIOverHTExt[ch][7]));
		}

		/* Calcuate the desired TSSI over HT for MCS 5, 6 and 7 in BW40 */
		
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Calculate the desired TSSI over HT for MCS 5, 6 and 7 in BW40\n", __FUNCTION__));
		RT28xx_EEPROM_READ16(pAd, EEPROM_HT_MCS4_MCS5, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE6 = 0x%X\n", __FUNCTION__, TxPower));
		
		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] - 1 + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */

			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHT40Ext[ch][MCS_5] = (CHAR)DesiredTssi;
		}
		
		RT28xx_EEPROM_READ16(pAd, (EEPROM_HT_MCS6_MCS7 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xE7 = 0x%X\n", __FUNCTION__, TxPower));
		
		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] - 1 + TSSI_RATIO_TABLE_OFFSET + BWPowerDelta);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */

			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}

			RT3290_desiredTSSIOverHT40Ext[ch][MCS_6] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHT40Ext[ch][MCS_7] = (CHAR)DesiredTssi;
		}

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverHT40[%d][0] = %d, desiredTSSIOverHT40[%d][1] = %d, desiredTSSIOverHT40[%d][2] = %d, desiredTSSIOverHT40[%d][3] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][0], 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][1], 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][2], 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][3]));
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverHT40[%d][4] = %d, desiredTSSIOverHT40[%d][5] = %d, desiredTSSIOverHT40[%d][6] = %d, desiredTSSIOverHT40[%d][7] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][4], 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][5], 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][6], 
				ch, 
				RT3290_desiredTSSIOverHT40Ext[ch][7]));
		}
		
		/* The desired TSSI over HT using STBC */

		DBGPRINT(RT_DEBUG_TRACE, ("%s: ------------------------------------------------------\n", __FUNCTION__));
		DBGPRINT(RT_DEBUG_TRACE, ("%s: The desired TSSI over HT using STBC\n", __FUNCTION__));
		RT28xx_EEPROM_READ16(pAd, EEPROM_HT_USING_STBC_MCS0_MCS1, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xEC = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));
			
			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_0] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_1] = (CHAR)DesiredTssi;
		}

		RT28xx_EEPROM_READ16(pAd, (EEPROM_HT_USING_STBC_MCS2_MCS3 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);

		RT3290_eeprom_access_grant(pAd, FALSE);

		
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xED = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s:ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_2] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_3] = (CHAR)DesiredTssi;
		}

		RT28xx_EEPROM_READ16(pAd, EEPROM_HT_USING_STBC_MCS4_MCS5, Value);
		TxPower = (Value & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xEE = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_4] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_5] = (CHAR)DesiredTssi;
		}

		RT28xx_EEPROM_READ16(pAd, (EEPROM_HT_USING_STBC_MCS6_MCS7 - 1), Value);
		TxPower = ((Value >> 8) & 0x000F);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: eFuse 0xEF = 0x%X\n", __FUNCTION__, TxPower));

		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			index = GET_TSSI_RATE_TABLE_INDEX(TxPower - TxPowerOFDM54 + pAd->TxPowerCtrl.PerChTxPwrOffset[ch] + TSSI_RATIO_TABLE_OFFSET);
			DesiredTssi = (SHORT)Rounding(pAd, (TSSIBase * RT3290_TssiRatioTable[index][0] / RT3290_TssiRatioTable[index][1]), (TSSIBase * RT3290_TssiRatioTable[index][0] % RT3290_TssiRatioTable[index][1]), RT3290_TssiRatioTable[index][1]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, index = %d, DesiredTssi = 0x%02X\n", __FUNCTION__, ch, index, DesiredTssi));

			/* Boundary verification: the desired TSSI value */
			
			if (DesiredTssi < 0x00)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));
				
				DesiredTssi = 0x00;
			}
			else if (DesiredTssi > 0x7C)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Out of range, DesiredTssi = 0x%02X\n", 
					__FUNCTION__, 
					DesiredTssi));

				DesiredTssi = 0x7C;
			}
			
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_6] = (CHAR)DesiredTssi;
			RT3290_desiredTSSIOverHTUsingSTBCExt[ch][MCS_7] = (CHAR)DesiredTssi;
		}
		
		for (ch = 1; ch <= NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET; ch++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverHTUsingSTBC[%d][0] = %d, desiredTSSIOverHTUsingSTBC[%d][1] = %d, desiredTSSIOverHTUsingSTBC[%d][2] = %d, desiredTSSIOverHTUsingSTBC[%d][3] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][0], 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][1], 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][2], 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][3]));
			
			DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, desiredTSSIOverHTUsingSTBC[%d][4] = %d, desiredTSSIOverHTUsingSTBC[%d][5] = %d, desiredTSSIOverHTUsingSTBC[%d][6] = %d, desiredTSSIOverHTUsingSTBC[%d][7] = %d\n", 
				__FUNCTION__, 
				ch, 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][4], 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][5], 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][6], 
				ch, 
				RT3290_desiredTSSIOverHTUsingSTBCExt[ch][7]));
		}

		/* 5390 RF TSSI configuraiton */
		
		RT30xxReadRFRegister(pAd, RF_R28, (PUCHAR)(&RFValue));
		RFValue = 0;
		RT30xxWriteRFRegister(pAd, RF_R28, RFValue);

		RT30xxReadRFRegister(pAd, RF_R29, (PUCHAR)(&RFValue));
		RFValue = ((RFValue & ~0x03) | 0x00);
		RT30xxWriteRFRegister(pAd, RF_R29, RFValue);

		RT30xxReadRFRegister(pAd, RF_R27, (PUCHAR)(&RFValue));
		RFValue = (RFValue & ~0xFC); /* [7:4] = 0, [3:2] = 0 */
		RFValue = (RFValue | 0x03); /* [1:0] = 0x03 (tssi_gain = 12dB) */
		RT30xxWriteRFRegister(pAd, RF_R27, RFValue);
		
		RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_GAIN_AND_ATTENUATION,Value);
		Value = (Value & 0x00FF);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: EEPROM_TSSI_GAIN_AND_ATTENUATION = 0x%X\n", 
			__FUNCTION__, 
			Value));

		if ((Value != 0x00) && (Value != 0xFF))
		{
			RT30xxReadRFRegister(pAd, RF_R27, (PUCHAR)(&RFValue));
			Value = (Value & 0x000F);
			RFValue = ((RFValue & 0xF0) | Value); /* [3:0] = (tssi_gain and tssi_atten) */
			RT30xxWriteRFRegister(pAd, RF_R27, RFValue);
		}
		
		/* 5390 BBP TSSI configuration */
		
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
		BbpR47 = ((BbpR47 & ~0x80) | 0x80); /* ADC6 on */
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpR47);
		
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
		BbpR47 = ((BbpR47 & ~0x18) | 0x10); /* TSSI_MODE (new averaged TSSI mode for 3290/5390) */
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpR47);

		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
		BbpR47 = ((BbpR47 & ~0x07) | 0x04); /* TSSI_REPORT_SEL (TSSI INFO 0 - TSSI) and enable TSSI INFO udpate */
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpR47);	
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("<--- %s\n", __FUNCTION__));
}


/*
   Get the desired TSSI based on the latest packet

   Parameters
	  pAd: The adapter data structure
	  pDesiredTssi: The desired TSSI
	  pCurrentTssi: The current TSSI/
	
   Return Value:
	  Success or failure
*/
BOOLEAN RT3290_GetDesiredTssiAndCurrentTssi(
	IN 		PRTMP_ADAPTER 		pAd,
	IN OUT 	PCHAR 				pDesiredTssi,
	IN OUT 	PCHAR 				pCurrentTssi)
{
	UCHAR BbpR47 = 0;
	UCHAR RateInfo = 0;
	CCK_TSSI_INFO cckTssiInfo = {{0}};
	OFDM_TSSI_INFO ofdmTssiInfo = {{0}};
	HT_TSSI_INFO htTssiInfo = {
			.PartA.value= 0,
			.PartB.value = 0,
		};
	
	UCHAR ch=1;

	DBGPRINT(RT_DEBUG_INFO, ("---> %s\n", __FUNCTION__));


	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
	if ((BbpR47 & 0x04) == 0x04) /* The TSSI INFO is not ready. */
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: BBP TSSI INFO is not ready. (BbpR47 = 0x%X)\n", __FUNCTION__, BbpR47));
		return FALSE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: BBP TSSI INFO ready. (BbpR47 = 0x%X)\n", __FUNCTION__, BbpR47));
	}

	if ((pAd->CommonCfg.CentralChannel >= 1) && (pAd->CommonCfg.CentralChannel <= 14))
		ch = pAd->CommonCfg.CentralChannel;
	else
	{
		ch = 1;
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Incorrect channel #%d, set as default #%d\n", 
					__FUNCTION__, pAd->CommonCfg.CentralChannel, ch));
	}
		
	/* Get TSSI */
	RT3290_Asic_Get_TSSI_Info(pAd, TSSI_INFO_0, pCurrentTssi); /* TSSI INFO 0 - TSSI */
	if ((*pCurrentTssi < 0) || (*pCurrentTssi > 0x7C))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pCurrentTssi(%d) Out of range!\n", __FUNCTION__, *pCurrentTssi));
		*pCurrentTssi = 0;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("%s: *pCurrentTssi = %d\n", __FUNCTION__, *pCurrentTssi));
	
	/* Get packet information */
	RT3290_Asic_Get_TSSI_Info(pAd, TSSI_INFO_1, &RateInfo); /* TSSI INFO 1 - Packet infomation */
	DBGPRINT(RT_DEBUG_INFO, ("%s: RateInfo = 0x%x\n", __FUNCTION__, RateInfo));
	if ((RateInfo & 0x03) == MODE_CCK) /* CCK */
	{
		cckTssiInfo.value = RateInfo;

		DBGPRINT(RT_DEBUG_TRACE, ("%s: CCK, cckTssiInfo.field.Rate = %d\n", 
			__FUNCTION__, cckTssiInfo.field.Rate));
		if (((cckTssiInfo.field.Rate >= 4) && (cckTssiInfo.field.Rate <= 7)) || 
		      (cckTssiInfo.field.Rate > 11)) /* boundary verification */
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: incorrect MCS: cckTssiInfo.field.Rate = %d\n", 
				__FUNCTION__, cckTssiInfo.field.Rate));
			
			return FALSE;
		}

		/* Data rate mapping for short/long preamble over CCK */
		if (cckTssiInfo.field.Rate >= 8)
			cckTssiInfo.field.Rate -= 8;
	
		*pDesiredTssi = RT3290_desiredTSSIOverCCKExt[ch][cckTssiInfo.field.Rate];
	}
	else if ((RateInfo & 0x03) == MODE_OFDM) /* OFDM */
	{
		int rate_idx;

		UCHAR bbp_ofdm_rate[] = {0xb /* 6M */, 0xf /* 9M */, 0xa /* 12M */,
								   0xe /* 18M */, 0x9 /* 24M */, 0xd /* 36M */,
								   0x8 /* 48M */, 0xc /* 54M */};
		UCHAR mac_ofdm_rate[] = {MCS_0, MCS_1, MCS_2, MCS_3, MCS_4, MCS_5, MCS_6, MCS_7};


		/* BBP OFDM rate format ==> MAC OFDM rate format */		
		ofdmTssiInfo.value = RateInfo;
		for (rate_idx = 0 ; rate_idx < sizeof(bbp_ofdm_rate) /sizeof(UCHAR); rate_idx++)
		{
			if (ofdmTssiInfo.field.Rate == bbp_ofdm_rate[rate_idx]) {
				ofdmTssiInfo.field.Rate = mac_ofdm_rate[rate_idx];
				break;
			}
		}

		if (rate_idx == sizeof(bbp_ofdm_rate)/sizeof(UCHAR))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Incorrect OFDM rate = 0x%X\n", __FUNCTION__, ofdmTssiInfo.field.Rate));
			return FALSE;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("%s: OFDM, ofdmTssiInfo.field.Rate = %d\n", 
			__FUNCTION__, ofdmTssiInfo.field.Rate));

		if ((ofdmTssiInfo.field.Rate < 0) || (ofdmTssiInfo.field.Rate > 7)) /* boundary verification */
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: incorrect MCS: ofdmTssiInfo.field.Rate = %d\n", 
				__FUNCTION__, ofdmTssiInfo.field.Rate));

			return FALSE;
		}

		*pDesiredTssi = RT3290_desiredTSSIOverOFDMExt[ch][ofdmTssiInfo.field.Rate];
	}
	else /* Mixed mode or green-field mode */
	{
		htTssiInfo.PartA.value = RateInfo;

		RT3290_Asic_Get_TSSI_Info(pAd, TSSI_INFO_2, &RateInfo); /* TSSI INFO 2 - Packet infomation */
		htTssiInfo.PartB.value = RateInfo;
		DBGPRINT(RT_DEBUG_INFO, ("%s: RateInfo = 0x%X\n", __FUNCTION__, RateInfo));

		DBGPRINT(RT_DEBUG_TRACE, ("%s: HT, htTssiInfo.PartA.field.STBC = %d, htTssiInfo.PartB.field.MCS = %d\n", 
			__FUNCTION__, htTssiInfo.PartA.field.STBC, htTssiInfo.PartB.field.MCS));

		if ((htTssiInfo.PartB.field.MCS < 0) || (htTssiInfo.PartB.field.MCS > 7)) /* boundary verification */
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: incorrect MCS: htTssiInfo.PartB.field.MCS = %d\n", 
				__FUNCTION__, htTssiInfo.PartB.field.MCS));

			return FALSE;
		}

		if (htTssiInfo.PartA.field.STBC == 0)
		{
			if ((htTssiInfo.PartB.field.BW == BW_40) && 
			     ((htTssiInfo.PartB.field.MCS == MCS_5) || (htTssiInfo.PartB.field.MCS == MCS_6) || (htTssiInfo.PartB.field.MCS == MCS_7)))
			{
				*pDesiredTssi = RT3290_desiredTSSIOverHT40Ext[ch][htTssiInfo.PartB.field.MCS];
			}
			else
			{
				*pDesiredTssi = RT3290_desiredTSSIOverHTExt[ch][htTssiInfo.PartB.field.MCS];
			}
		}
		else
		{
			*pDesiredTssi = RT3290_desiredTSSIOverHTUsingSTBCExt[ch][htTssiInfo.PartB.field.MCS];
		}
	}	

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ch = %d, *pDesiredTssi = %d\n", __FUNCTION__, ch, *pDesiredTssi));
	
	if (*pDesiredTssi < 0x00)
	{
		*pDesiredTssi = 0x00;
	}	
	else if (*pDesiredTssi > 0x7c)
	{
		*pDesiredTssi = 0x7c;
	}

	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
	BbpR47 = ((BbpR47 & ~0x07) | 0x04); /* TSSI_REPORT_SEL (TSSI INFO 0 - TSSI) and enable TSSI INFO udpate */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpR47);

	DBGPRINT(RT_DEBUG_INFO, ("<--- %s\n", __FUNCTION__));

	return TRUE;
}


VOID RT3290_AsicTxAlcGetAutoAgcOffset(
	IN PRTMP_ADAPTER 			pAd,
	IN PCHAR					pDeltaPwr,
	IN PCHAR					pTotalDeltaPwr,
	IN PCHAR					pAgcCompensate,
	IN PCHAR 					pDeltaPowerByBbpR1)
{
	const TX_POWER_TUNING_ENTRY_STRUCT *TxPowerTuningTable = pAd->chipCap.TxPowerTuningTable_2G;
	PTX_POWER_TUNING_ENTRY_STRUCT pTxPowerTuningEntry = NULL;
	UCHAR 			RFValue = 0;
	CHAR 			desiredTssi = 0;
	CHAR 			currentTssi = 0;
	CHAR 			TotalDeltaPower = 0; 
	CHAR			TuningTableIndex = 0;
	
	/* Locate the Internal Tx ALC tuning entry */
	if (pAd->TxPowerCtrl.bInternalTxALC == TRUE)
	{
		if ((pAd->Mlme.OneSecPeriodicRound % 4 == 0) && (*pDeltaPowerByBbpR1 == 0))
		{
			if (IS_RT3290(pAd) && (pAd->WlanBTCoexInfo.coexSettingRunning == TRUE)) // RT3290 Coex mode will disable TSSI
			{
				
			}
			else if (RT3290_GetDesiredTssiAndCurrentTssi(pAd, &desiredTssi, &currentTssi) == FALSE)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s: Incorrect desired TSSI or current TSSI\n", __FUNCTION__));
				
				/* Tx power adjustment over RF */
				RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)(&RFValue));
				RFValue = ((RFValue & ~0x3F) | pAd->TxPowerCtrl.RF_TX_ALC);
				if ((RFValue & 0x3F) > 0x27) /* The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x27 */
				{
					RFValue = ((RFValue & ~0x3F) | 0x27);
				}
				RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)(RFValue));

				/* Tx power adjustment over MAC */
				TotalDeltaPower += pAd->TxPowerCtrl.MAC_PowerDelta;
			}
			else
			{
				if (desiredTssi > currentTssi)
				{
					pAd->TxPowerCtrl.idxTxPowerTable++;
				}

				if (desiredTssi < currentTssi)
				{
					pAd->TxPowerCtrl.idxTxPowerTable--;
				}

				TuningTableIndex = pAd->TxPowerCtrl.idxTxPowerTable
#ifdef DOT11_N_SUPPORT				
									+ pAd->TxPower[pAd->CommonCfg.CentralChannel-1].Power;
#else
									+ pAd->TxPower[pAd->CommonCfg.Channel-1].Power;
#endif /* DOT11_N_SUPPORT */

				if (TuningTableIndex < LOWERBOUND_TX_POWER_TUNING_ENTRY)
				{
					TuningTableIndex = LOWERBOUND_TX_POWER_TUNING_ENTRY;
				}

				if (TuningTableIndex > UPPERBOUND_TX_POWER_TUNING_ENTRY(pAd))
				{
					TuningTableIndex = UPPERBOUND_TX_POWER_TUNING_ENTRY(pAd);
				}
				/* Valide pAd->TxPowerCtrl.idxTxPowerTable: -30 ~ 69 */
				pTxPowerTuningEntry = 
					(PTX_POWER_TUNING_ENTRY_STRUCT)&(TxPowerTuningTable[TuningTableIndex + TX_POWER_TUNING_ENTRY_OFFSET]);

				pAd->TxPowerCtrl.RF_TX_ALC = pTxPowerTuningEntry->RF_TX_ALC;
				pAd->TxPowerCtrl.MAC_PowerDelta = pTxPowerTuningEntry->MAC_PowerDelta;

				/* Tx power adjustment over RF */
				RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)(&RFValue));
				RFValue = ((RFValue & ~0x3F) | pAd->TxPowerCtrl.RF_TX_ALC);
				if ((RFValue & 0x3F) > 0x27) /* The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x1F */
				{
					RFValue = ((RFValue & ~0x3F) | 0x27);
				}
				RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)(RFValue));

				/* Tx power adjustment over MAC */
				TotalDeltaPower += pAd->TxPowerCtrl.MAC_PowerDelta;

				DBGPRINT(RT_DEBUG_TRACE, ("%s: desiredTSSI = %d, currentTSSI = %d, idxTxPowerTable = %d, TuningTableIndex = %d, {RF_TX_ALC = 0x%X, MAC_PowerDelta = %d}\n", 
					__FUNCTION__, 
					desiredTssi, 
					currentTssi, 
					pAd->TxPowerCtrl.idxTxPowerTable, 
					TuningTableIndex,
					pTxPowerTuningEntry->RF_TX_ALC, 
					pTxPowerTuningEntry->MAC_PowerDelta));
			}
		}
		else
		{
			if ( IS_RT3290(pAd) && (pAd->WlanBTCoexInfo.coexSettingRunning == FALSE))
			{
				/* Tx power adjustment over RF */
				RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)(&RFValue));
				RFValue = ((RFValue & ~0x3F) | pAd->TxPowerCtrl.RF_TX_ALC);
				if ((RFValue & 0x3F) > 0x27) /* The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x1F */
				{
					RFValue = ((RFValue & ~0x3F) | 0x27);
				}
				RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)(RFValue));
			}
			/* Tx power adjustment over MAC */
			TotalDeltaPower += pAd->TxPowerCtrl.MAC_PowerDelta;
		}
	}

	*pTotalDeltaPwr = TotalDeltaPower;
}


#endif /* RTMP_INTERNAL_TX_ALC */


/*
	==========================================================================
	Description:

	Load RF normal operation-mode setup
	
	==========================================================================
 */
VOID RT3290LoadRFNormalModeSetup(
	IN PRTMP_ADAPTER 	pAd)
{
	UCHAR RfReg, bbpreg = 0;


	// TODO: shiang, is it ok here we just return directly here for Radio on/off case?


	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R31, 0x08);

	/* rx_lo1_en (enable RX LO1, 0: LO1 follows TR switch) */
	RT30xxReadRFRegister(pAd, RF_R38, (PUCHAR)&RfReg);
	RfReg = ((RfReg & ~0x20) | 0x00);
	RT30xxWriteRFRegister(pAd, RF_R38, (UCHAR)RfReg);

	/* rx_lo2_en (enable RX LO2, 0: LO2 follows TR switch) */
	RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&RfReg);
	RfReg = ((RfReg & ~0x80) | 0x00);
	RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)RfReg);

	/* 
		Avoid data lost and CRC error
	*/
	/* MAC interface control (MAC_IF_80M, 1: 80 MHz) */
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &bbpreg);
	bbpreg = ((bbpreg & ~0x40) | 0x40);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, bbpreg);
	/* rxvcm (Rx BB filter VCM) */
	RT30xxReadRFRegister(pAd, RF_R30, (PUCHAR)&RfReg);
	RfReg = ((RfReg & ~0x18) | 0x10);
	RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RfReg);	
}

/*
	==========================================================================
	Description:

	Load RF sleep-mode setup
	
	==========================================================================
 */
VOID RT3290LoadRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd)
{
/*	UCHAR RFValue;*/
/*	UINT32 MACValue;*/
	UCHAR rfreg;

	// TODO: shiang, is it ok here we just return directly here for radio on/off case?
	if (IS_RT3290(pAd))
		return;
	else
	{
		RT30xxReadRFRegister(pAd, RF_R01, &rfreg);
		rfreg = ((rfreg & ~0x01) | 0x00); /* vco_en */
		RT30xxWriteRFRegister(pAd, RF_R01, rfreg);

		RT30xxReadRFRegister(pAd, RF_R06, &rfreg);
		rfreg = ((rfreg & ~0xC0) | 0x00); /* vco_ic (VCO bias current control, 00: off) */
		RT30xxWriteRFRegister(pAd, RF_R06, rfreg);

		RT30xxReadRFRegister(pAd, RF_R22, &rfreg);
		rfreg = ((rfreg & ~0xE0) | 0x00); /* cp_ic (reference current control, 000: 0.25 mA) */
		RT30xxWriteRFRegister(pAd, RF_R22, rfreg);

		RT30xxReadRFRegister(pAd, RF_R42, &rfreg);
		rfreg = ((rfreg & ~0x40) | 0x00); /* rx_ctb_en */
		RT30xxWriteRFRegister(pAd, RF_R42, rfreg);

		RT30xxReadRFRegister(pAd, RF_R20, &rfreg);
		rfreg = ((rfreg & ~0x77) | 0x77); /* ldo_pll_vc and ldo_rf_vc (111: -0.15) */
		RT30xxWriteRFRegister(pAd, RF_R20, rfreg);
	}	
	/* Don't touch LDO_CFG0 for 3090F & 3593, possibly the board is single power scheme */
	
}


/*
	==========================================================================
	Description:

	Reverse RF sleep-mode setup
	
	==========================================================================
 */
VOID RT3290ReverseRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd,
	IN BOOLEAN			FlgIsInitState)
{
	UCHAR rfreg;

	RT30xxReadRFRegister(pAd, RF_R22, &rfreg);
	rfreg = ((rfreg & ~0xE0) | 0x20); // cp_ic (reference current control, 001: 0.33 mA)
	RT30xxWriteRFRegister(pAd, RF_R22, rfreg);

	RT30xxReadRFRegister(pAd, RF_R42, &rfreg);
	rfreg |= 0x40; // rx_ctb_en
	RT30xxWriteRFRegister(pAd, RF_R42, rfreg);

	RT30xxReadRFRegister(pAd, RF_R20, &rfreg);
	rfreg &= (~0x77); // ldo_rf_vc and ldo_pll_vc
	RT30xxWriteRFRegister(pAd, RF_R20, rfreg);


	RT30xxReadRFRegister(pAd, RF_R03, &rfreg);
	rfreg = ((rfreg & ~0x80) | 0x80); // vcocal_en (initiate VCO calibration (reset after completion))
	RT30xxWriteRFRegister(pAd, RF_R03, rfreg);
}


VOID RT3290HaltAction(
	IN PRTMP_ADAPTER 	pAd)
{
	// TODO: shiang, check about PCIe power saving in 3290
	pAd->bPCIclkOff = TRUE;
	RTMPEnableWlan(pAd, FALSE, FALSE);
	RTMP_SET_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND);
}


VOID RT3290_AsicResetBbpAgent(
	IN PRTMP_ADAPTER pAd)
{
	// TODO:
}


VOID NICInitRT3290RFRegisters(IN PRTMP_ADAPTER pAd)
{
	INT i;
	UCHAR RfReg = 0;
	ULONG data;
	
	
	/* Init RF calibration, toggle bit 7 before init RF registers */
	RT30xxReadRFRegister(pAd, RF_R02, (PUCHAR)&RfReg);
	RfReg = ((RfReg & ~0x80) | 0x80);
	RT30xxWriteRFRegister(pAd, RF_R02, (UCHAR)RfReg);
	RTMPusecDelay(1000);
	RfReg = ((RfReg & ~0x80) | 0x00);
	RT30xxWriteRFRegister(pAd, RF_R02, (UCHAR)RfReg);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Initialize the RF registers to the default values", __FUNCTION__));
		
	/* Initialize RF register to default value */
	for (i = 0; i < RT3290_NUM_RF_REG_PARMS; i++)
	{
		if (IS_RT3290LE(pAd) && (RT3290_RFRegTable[i].Register == RF_R25))
			RT3290_RFRegTable[i].Value |= 0x03; /* set RF_R25 bit_1:0=11 */

		RT30xxWriteRFRegister(pAd,
				RT3290_RFRegTable[i].Register,
				RT3290_RFRegTable[i].Value);
	}

 	
	/*
		Set bbp filter initial value, because we does 
		not need RTMPFilterCalibration.
	*/
	pAd->Mlme.CaliBW20RfR24 = 0x1F;
	pAd->Mlme.CaliBW40RfR24 = 0x2F; /* Bit[5] must be 1 for BW 40 */

	RTMPFilterCalibration(pAd);
	
	/* set led open drain enable */
	RTMP_IO_READ32(pAd, OPT_14, &data);
	data |= 0x01;
	RTMP_IO_WRITE32(pAd, OPT_14, data);

	RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0);
	RTMP_IO_WRITE32(pAd, TX_SW_CFG2, 0x0);


	/* enable DC filter */
	//RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R103, 0xc0);

	RT3290LoadRFNormalModeSetup(pAd);
}


/*
========================================================================
Routine Description:
	Initialize specific MAC registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/



/*
========================================================================
Routine Description:
	Initialize specific BBP registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID NICInitRT3290BbpRegisters(
	IN	PRTMP_ADAPTER pAd)
{
	BBP_R105_STRUC BBPR105 = {.byte = 0};
	UCHAR BbpReg = 0;
	UCHAR BbpValue = 0;
	UCHAR RFValue = 0;
	
	DBGPRINT(RT_DEBUG_TRACE, ("--> %s\n", __FUNCTION__));
	
	/* The channel estimation updates based on remodulation of L-SIG and HT-SIG symbols. */
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R105, &BBPR105.byte);
	BBPR105.field.MLDFor2Stream = 0;
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R105, BBPR105.byte);

	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BbpReg);
	BbpReg = ((BbpReg & ~0x40) | 0x40); // MAC interface control (MAC_IF_80M, 1: 80 MHz)
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpReg);

	DBGPRINT(RT_DEBUG_ERROR, ("BBP_R105: BBPR105.field.EnableSIGRemodulation = %d, BBPR105.field.MLDFor2Stream = %d\n",  
		BBPR105.field.EnableSIGRemodulation, 
		BBPR105.field.MLDFor2Stream));
	
	// TODO: shiang, following code may remove due to I already add these registers in RT3290_BBPRegTable
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R31, 0x08); // ADC/DAC control

	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R68, 0x0B);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x13); // Rx AGC SQ Acorr threshold	
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46); // Rx high power VGA offset for LNA offset
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R76, 0x28); // Rx medium power VGA offset for LNA offset		
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R77, 0x58);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R83, 0x7A);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R84, 0x9A);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0x38);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R91, 0x04);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R92, 0x02);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R103, 0xC0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R104, 0x92);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R105, 0x1C);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R106, 0x03);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R128, 0x12);
	// TODO: move end

	if (IS_RT3290LE(pAd) && ((pAd->CommonCfg.HWAntennaSwitch == 1) || ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE))))
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x14);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x06);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x2F);	

		RT30xxReadRFRegister(pAd, RF_R43, (PUCHAR)&BbpValue);
		BbpValue &= ~0x07;
		RT30xxWriteRFRegister(pAd, RF_R43, (UCHAR)BbpValue);				
	}	
	else if (IS_RT3290LE(pAd))
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x18);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x09);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x33);	
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R74, 0x0b);	
	}
	else
	{
		// patch RSSI inaccurate issue, due to design change
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x13);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x05);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x33);	
	}

		//Enable RX antenna switch
		RT30xxReadRFRegister(pAd, RF_R29, (PUCHAR)(&RFValue));
		RFValue = ((RFValue & ~0xc0) | 0xc0);
		RT30xxWriteRFRegister(pAd, RF_R29, RFValue);

		
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, 0x30); //disable OFDM HW diversity
		DBGPRINT(RT_DEBUG_TRACE, ("!!! EN BBP_R150 = %d\n",pAd->BbpWriteLatch[BBP_R150]));
		
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, 0x2e);

		if (IS_RT3290LE(pAd) && ((pAd->CommonCfg.HWAntennaSwitch == 1) || ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE))))
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, 0xa4);
		}		
		else if ((pAd->CommonCfg.HWAntennaSwitch == 1) || ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE)))
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, 0xa0);
		}
		else
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, 0x20);
		}
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R153, 0x34);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R154, 0x40);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R155, 0x3b);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R253, 0x04);

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R142, 0x04);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R143, 153);

		if (IS_RT3290LE(pAd))
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x24);	
		}

		RT30xxReadRFRegister(pAd, RF_R47, (PUCHAR)(&RFValue));
		RFValue = ((RFValue & ~0x80) | 0x80);
		RT30xxWriteRFRegister(pAd, RF_R47, RFValue);
		
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpReg);
	BbpReg &= (~0xc0);
	BbpReg |= 0xc0; //Use 5bit ADC for Acquistion
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpReg);		
	
	DBGPRINT(RT_DEBUG_TRACE, ("<-- %s\n", __FUNCTION__));
}


VOID RT3290_AsicAntennaDefaultReset(
	IN struct _RTMP_ADAPTER	*pAd,
	IN EEPROM_ANTENNA_STRUC *pAntenna)
{
	pAntenna->word = 0;
	pAntenna->field.RfIcType = 0xf;
	pAntenna->field.TxPath = 1;
	pAntenna->field.RxPath = 1;
}


VOID RT3290_AsicSetRxAnt(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Ant)
{
	if ((pAd->bPCIclkOff == FALSE) && 
		(((pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode == TRUE) && 
		   (pAd->NicConfig2.field.AntDiversity == TRUE))
		   || (pAd->NicConfig2.field.AntOpt == 1)))
	{
		WLAN_FUN_CTRL_STRUC	WlanFunCtrl = {.word = 0};

		RTMP_SEM_LOCK(&pAd->WlanEnLock);
		RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);

		if (WlanFunCtrl.field.WLAN_EN == TRUE)
		{	
				// 3290LE  
				//TR_SW0 = 0   BT = main antenna Wlan = aux antenna
				//TR_SW0 = 1   BT = aux antenna   Wlan = main antenna
				//this the reason we reverse the antenna 						

			if (Ant == 0) // 0: Main antenna
			{
				if (WlanFunCtrl.field.INV_TR_SW0)
				{
					WlanFunCtrl.field.INV_TR_SW0 = 0;			
					RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
				}
				printk("RT3290_AsicSetRxAnt 0: Main antenna\n");
			}
			else
			{
				if (WlanFunCtrl.field.INV_TR_SW0 == 0)
				{
					WlanFunCtrl.field.INV_TR_SW0 = 1;		
					RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
				}
				printk("RT3290_AsicSetRxAnt 1: Aux antenna\n");
			}
			DBGPRINT(RT_DEBUG_TRACE, ("%s(): switch to %s antenna\n", 
						__FUNCTION__, (Ant == 0 ? "Main" : "Aux")));
			pAd->WlanFunCtrl.word = WlanFunCtrl.word;	
		}

		RTMP_SEM_UNLOCK(&pAd->WlanEnLock);
	}
}


VOID RT3290_ChipBBPAdjust(
	IN RTMP_ADAPTER			*pAd)
{
	// TODO: shiang, Fix me

}






VOID RT3290_RxSensitivityTuning(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR R66 = 0x1C + 2*GET_LNA_GAIN(pAd);

		AsicBBPWriteWithRxChain(pAd, BBP_R66, R66, RX_CHAIN_ALL);

	DBGPRINT(RT_DEBUG_TRACE,("turn off R17 tuning, restore to 0x%02x\n", R66));
}

#ifdef CONFIG_STA_SUPPORT
VOID RT3290_NetDevNickNameInit(
	IN struct _RTMP_ADAPTER *pAd)
{
	snprintf((PSTRING) pAd->nickname, sizeof(pAd->nickname), "RT3290STA");
}

UCHAR RT3290_ChipAGCAdjust(
	IN PRTMP_ADAPTER	pAd,
	IN CHAR				Rssi,
	IN UCHAR				OrigR66Value)
{
	UCHAR R66 = 0x1C + 2*GET_LNA_GAIN(pAd);

	if (Rssi > RSSI_FOR_MID_LOW_SENSIBILITY)
	{
		if (IS_RT3290LE(pAd) && ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE)))
		{
			R66 = 0x14 + 2*GET_LNA_GAIN(pAd) + 0x10;
		}
		else if (IS_RT3290LE(pAd) && (pAd->WlanBTCoexInfo.coexSettingRunning == TRUE))
		{
			R66 = 0x14 + 2*GET_LNA_GAIN(pAd) + 0x10;
		}
		else if (IS_RT3290(pAd))
		{
			R66 = 0x1C + 2*GET_LNA_GAIN(pAd) + 0x10;
		}
		else
		{
			R66 = 0x1C + 2*GET_LNA_GAIN(pAd) + 0x10;
		}

		
		if (OrigR66Value != R66)
		{
			AsicBBPWriteWithRxChain(pAd, BBP_R66, R66, RX_CHAIN_ALL);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R83, 0x4A); // Rx AGC LNA MM select threshold in log2
		}
	}
	else
	{
		if (IS_RT3290(pAd))
		{
//
			if (pAd->WlanBTCoexInfo.coexSettingRunning == FALSE)
			{
				if (IS_RT3290LE(pAd) && ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE)))
				{
					//WLAN only
					if (Rssi >= -75)
					{
						R66 = 0x14 + 2*GET_LNA_GAIN(pAd) + 0x08;
					}
					else
					{
						R66 = 0x14 + 2*GET_LNA_GAIN(pAd);
					}
				}						
				else if (IS_RT3290LE(pAd))
				{
					//WLAN only
					if (Rssi >= -80)
					{
						R66 = 0x1C + 2*GET_LNA_GAIN(pAd) + 0x08;
					}
					else
					{
						R66 = 0x1C + 2*GET_LNA_GAIN(pAd);
					}
				}
				else
				{
					R66 = 0x1C + 2*GET_LNA_GAIN(pAd) + 0x08;
				}
			}
			else
			{
				if (IS_RT3290LE(pAd))
				{						
					//Enter Coex. Mode
					if (Rssi >= -75)
					{
						R66 = 0x14 + 2*GET_LNA_GAIN(pAd) + 0x10;
					}
					else
					{
						R66 = 0x14 + 2*GET_LNA_GAIN(pAd);
					}
				}
				else
				{
					//Enter Coex. Mode
					if (Rssi >= -75)
					{
						R66 = 0x1C + 2*GET_LNA_GAIN(pAd) + 0x08;
					}
					else
					{
						R66 = 0x1C + 2*GET_LNA_GAIN(pAd);
					}						
				}
			}
		}

		if (OrigR66Value != R66)
		{
			AsicBBPWriteWithRxChain(pAd, BBP_R66, R66, RX_CHAIN_ALL);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R83, 0x7A); // Rx AGC LNA MM select threshold in log2

		}
	}	

	DBGPRINT(RT_DEBUG_TRACE, 
				("%s():LnaGain=0x%x, set R66 as 0x%x \n", 
				__FUNCTION__,
				GET_LNA_GAIN(pAd), R66));
		//Customer-Samsung: <1m distance throughput issue
	if (IS_RT3290(pAd))
	{
		if (Rssi >= -36)
		{
			if (pAd->BbpWriteLatch[BBP_R77] != 0x27)
			{
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x52);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R77, 0x27);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x1C);
			}
		}
		else
		{
			if (pAd->BbpWriteLatch[BBP_R77] != 0x58)
			{
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R77, 0x58);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);	
			}
		}
	}

	
	return R66;
}
#endif /* CONFIG_STA_SUPPORT */


VOID RT3290_ChipAGCInit(
	IN PRTMP_ADAPTER		pAd,
	IN UCHAR				BandWidth)
{
	UCHAR R66 = 0x1C + 2*GET_LNA_GAIN(pAd);
	
	AsicBBPWriteWithRxChain(pAd, BBP_R66, R66, RX_CHAIN_ALL);
	
}


/*
	========================================================================
	
	Routine Description:
		Read initial Tx power per MCS and BW from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note: RT3290 EEPROM V0.1
		
	========================================================================
*/
VOID	RTMPRT3290ReadTxPwrPerRate(
	IN	PRTMP_ADAPTER	pAd)
{
	// TODO: shiang, Fix me

}

/*
	========================================================================
	
	Routine Description:
		Read initial channel power parameters from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	RTMPRT3290ReadChannelPwr(
	IN	PRTMP_ADAPTER	pAd)
{
	// TODO: shiang, Fix me

}

/* ATE will also call this function to set GPIO, channel=36 to set low and channel=1 to set high */
VOID	RTMPRT3290ABandSel(
	IN	UCHAR	Channel)
{
		// TODO: shiang, Fix me

}




VOID RT3290_ChipSetBW(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR bandwidth)
{
	

}


VOID RT3290_ChipLDOCoreLvAdjust(
	IN RTMP_ADAPTER *pAd,
	IN BOOLEAN bHalt)
{
	CMB_CTRL_STRUC CmbCtrl={.word = 0};
	UCHAR core_level;

	if (IS_RT3290LE(pAd))
		return;
			
	RTMP_IO_READ32(pAd, CMB_CTRL, &CmbCtrl.word);
	if (CmbCtrl.field.LDO3_EN == FALSE)
		return;

	core_level = CmbCtrl.field.LDO_CORE_LEVEL;
	CmbCtrl.field.LDO_CORE_LEVEL = 0;
	if ((!bHalt) && 
		(pAd->CommonCfg.BBPCurrentBW == BW_20) &&
		((pAd->CommonCfg.Channel == 13) || (pAd->CommonCfg.Channel == 14)) &&
		(core_level < 2))
		CmbCtrl.field.LDO_CORE_LEVEL = 2;
	if (CmbCtrl.field.LDO_CORE_LEVEL != core_level)
		RTMP_IO_WRITE32(pAd, CMB_CTRL, CmbCtrl.word);

}


VOID RT3290_ChipSwitchChannel(
	IN PRTMP_ADAPTER 			pAd,
	IN UCHAR					Channel,
	IN BOOLEAN					bScan) 
{
	ULONG			MacValue = 0;
	CHAR			TxPwer = 0, TxPwer2 = DEFAULT_RF_TX_POWER;
	UCHAR			index;
	UCHAR Tx0FinePowerCtrl = 0, Tx1FinePowerCtrl = 0;
	UCHAR Value = 0, TxRxh20M = 0;
	/* UCHAR BBPR4 = 0; */

	// TODO: shiang, Fix me
	
	// Search Tx power value
	for (index = 0; index < MAX_NUM_OF_CHANNELS; index++)
	{
		if (Channel == pAd->TxPower[index].Channel)
		{
			TxPwer = pAd->TxPower[index].Power;
			TxPwer2 = pAd->TxPower[index].Power2;

			if (IS_RT3090A(pAd))
			{
				Tx0FinePowerCtrl = pAd->TxPower[index].Tx0FinePowerCtrl;
				Tx1FinePowerCtrl = pAd->TxPower[index].Tx1FinePowerCtrl;
			}

			break;
		}
	}
	if (index == MAX_NUM_OF_CHANNELS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Cant find the Channel#%d \n",
				__FUNCTION__, Channel));
		return;
	}

	// TODO: shiang, here we need a better place to call this function
	RT3290_ChipLDOCoreLvAdjust(pAd, FALSE);
	
	for (index = 0; index < NUM_OF_3020_CHNL; index++)
	{
		if (Channel == FreqItems3020[index].Channel)
		{
			// Programming channel parameters
			RT30xxWriteRFRegister(pAd, RF_R08, FreqItems3020[index].N); // N
			RT30xxWriteRFRegister(pAd, RF_R09, (FreqItems3020[index].K & 0x0F)); // K, N<11:8> is set to zero

			RT30xxReadRFRegister(pAd, RF_R11, (PUCHAR)&Value);
			Value = ((Value & ~0x03) | (FreqItems3020[index].R & 0x03)); // R
			RT30xxWriteRFRegister(pAd, RF_R11, (UCHAR)Value);

			RT30xxReadRFRegister(pAd, RF_R49, &Value);

			Value = ((Value & ~0x3F) | (TxPwer & 0x3F)); // tx0_alc
			if ((Value & 0x3F) > 0x27) // The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x27
			{
				Value = ((Value & ~0x3F) | 0x27);
			}
			RT30xxWriteRFRegister(pAd, RF_R49, Value);			
		
			RT30xxReadRFRegister(pAd, RF_R17, &Value);
#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
			if (pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration == TRUE && INFRA_ON(pAd))
			{	// Update the frequency offset from the adaptive frequency offset
				Value = ((Value & ~0x7F) | (pAd->FreqCalibrationCtrl.AdaptiveFreqOffset & 0x7F)); // xo_code (C1 value control) - Crystal calibration
			}
			else
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
			{	// Update the frequency offset from EEPROM
				Value = ((Value & ~0x7F) | (pAd->RfFreqOffset & 0x7F)); // xo_code (C1 value control) - Crystal calibration
			}
			if (Value > 0x5f)
				Value = 0x5F;
			RT30xxWriteRFRegister(pAd, RF_R17, Value);

				if ((!bScan) && (pAd->CommonCfg.BBPCurrentBW == BW_40)) // BW 40
				{
					TxRxh20M = ((pAd->Mlme.CaliBW40RfR24 & 0x20) >> 5); // Tx/Rx h20M
					
                                    RT30xxReadRFRegister(pAd, RF_R32, &Value);
                                    Value &= ~0xF8;      //tx_agc_fc<4:0>
                                    Value |= (pAd->Mlme.CaliBW40RfR24 << 3);
                                    RT30xxWriteRFRegister(pAd, RF_R32, Value);

                                    RT30xxReadRFRegister(pAd, RF_R31, &Value);
                                    Value &= ~0xF8;      //rx_agc_fc<4:0>
                                    Value |= (pAd->Mlme.CaliBW40RfR24 << 3);
                                    RT30xxWriteRFRegister(pAd, RF_R31, Value);	
				}
				else // BW 20
				{
					TxRxh20M = ((pAd->Mlme.CaliBW20RfR24 & 0x20) >> 5); // Tx/Rx h20M

					RT30xxReadRFRegister(pAd, RF_R32, &Value);
					Value &= ~0xF8;      //tx_agc_fc<4:0>
					Value |= (pAd->Mlme.CaliBW20RfR24 << 3);
					RT30xxWriteRFRegister(pAd, RF_R32, Value);

					RT30xxReadRFRegister(pAd, RF_R31, &Value);
					Value &= ~0xF8;      //rx_agc_fc<4:0>
					Value |= (pAd->Mlme.CaliBW20RfR24 << 3);
					RT30xxWriteRFRegister(pAd, RF_R31, Value);
				}

			Value = (Channel == 6) ? 0x0c : 0x0b;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd,BBP_R68, Value);			
				
			/* pa2_cc_ofdm<3:0> (PA2 Cascode Bias OFDM mode) */
			Value = 0;
			if ((Channel >= 1) && (Channel <= 6))
				Value = 0x0f;	
			else if ((Channel >= 7) && (Channel <= 11))
				Value = 0x0e;
			else if ((Channel >= 12) && (Channel <= 14))
				Value = 0x0d;
			if (Value)
				RT30xxWriteRFRegister(pAd, RF_R59, Value); 
			
			RT30xxReadRFRegister(pAd, RF_R30, &Value);
			Value = ((Value & ~0x06) | (TxRxh20M << 1) | (TxRxh20M << 2));
			RT30xxWriteRFRegister(pAd, RF_R30, Value);

			/* vcocal_en (initiate VCO calibration (reset after completion)) - It should be at the end of RF configuration. */
			RT30xxReadRFRegister(pAd, RF_R03, &Value);
			Value = ((Value & ~0x80) | 0x80);
			RT30xxWriteRFRegister(pAd, RF_R03, Value);

			/* Channel latch */
			pAd->LatchRfRegs.Channel = Channel;

			DBGPRINT(RT_DEBUG_TRACE, ("%s: SwitchChannel#%d(RF=%d, Pwr0=%d, Pwr1=%d, %dT), N=0x%02x, K=0x%02x, R=0x%02x\n",
				__FUNCTION__, Channel, pAd->RfIcType, 
				TxPwer, TxPwer2, pAd->Antenna.field.TxPath, 
				FreqItems3020[index].N, FreqItems3020[index].K, FreqItems3020[index].R));

			break;
		}
	}
	
	/* Change BBP setting during siwtch from a->g, g->a */
	if (Channel <= 14)
	{
		ULONG	TxPinCfg = 0x00050F0A;
		CHAR lanGain = GET_LNA_GAIN(pAd);
		
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - lanGain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - lanGain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - lanGain));

		/* 5G band selection PIN, bit1 and bit2 are complement */
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &MacValue);
		MacValue &= (~0x6);
		MacValue |= (0x04);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, MacValue);

		/* Turn off unused PA or LNA when only 1T or 1R */
		if (pAd->Antenna.field.TxPath == 1)
			TxPinCfg &= 0xFFFFFFF3;
		if (pAd->Antenna.field.RxPath == 1)
			TxPinCfg &= 0xFFFFF3FF;

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}

	/* R66 should be set according to Channel and use 20MHz when scanning */
	RTMPSetAGCInitValue(pAd, BW_20);

	/*
		On 11A, We should delay and wait RF/BBP to be stable
		and the appropriate time should be 1000 micro seconds 
		2005/06/05 - On 11G, We also need this delay time. Otherwise it's difficult to pass the WHQL.
	*/
	RTMPusecDelay(1000);  
}


#define ANTENNA_TRAINNING_ROUNDS 2

VOID MlmeAntSelection(
	IN PRTMP_ADAPTER	pAd,
	IN ULONG			AccuTxTotalCnt,
	IN ULONG			TxErrorRatio,
	IN ULONG			TxSuccess,
	IN CHAR				Rssi)
{
		if (IS_RT3290(pAd) &&
			(pAd->NicConfig2.field.AntDiversity == TRUE) &&
			((AccuTxTotalCnt > 150) || (pAd->AntennaDiversityInfo.AntennaDiversityState==1)) &&
			(pAd->StaCfg.AntS == 1))		
		{
			static	ULONG			AvgTxErrorRatio = 5;
			

			DBGPRINT(RT_DEBUG_TRACE,("AntS: AvgTxErrorRatio = %lu , TxErrorRatio = %lu, AccuTxTotalCnt = %lu, Rssi = %d\n",AvgTxErrorRatio, TxErrorRatio, AccuTxTotalCnt, Rssi));

			if(pAd->AntennaDiversityInfo.RateUp)
			{
				pAd->AntennaDiversityInfo.RateUp = FALSE;
				AvgTxErrorRatio = TxErrorRatio;// = 0;
			}
			
			AvgTxErrorRatio = AvgTxErrorRatio * 2 + TxErrorRatio ;
			AvgTxErrorRatio /=3;

			//Ave PER
			pAd->AntennaDiversityInfo.AntennaDiversityPER[pAd->WlanFunCtrl.field.INV_TR_SW0] += TxErrorRatio;
			pAd->AntennaDiversityInfo.AntennaDiversityPER[pAd->WlanFunCtrl.field.INV_TR_SW0] /= 2;

			// trainning phase
			if ((pAd->AntennaDiversityInfo.AntennaDiversityState == 1) && (pAd->AntennaDiversityInfo.AntennaDiversityCount < ANTENNA_TRAINNING_ROUNDS))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("AntS: Train State: AntennaDiversityCount = %lu\n", pAd->AntennaDiversityInfo.AntennaDiversityCount));

				
				//Sum total Tx-Success packet
				//pAd->AntennaDiversityTxPacketCount[pAd->WlanFunCtrl.field.INV_TR_SW0] += TxTotalCnt;
				pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[pAd->WlanFunCtrl.field.INV_TR_SW0] += TxSuccess;
				//Sum total Rx-Success packet
				pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[pAd->WlanFunCtrl.field.INV_TR_SW0] += pAd->RalinkCounters.OneSecRxOkCnt2;
				pAd->RalinkCounters.OneSecRxOkCnt2 = 0;


				//Ave RSSI
				//pAd->Rssi[pAd->WlanFunCtrl.field.INV_TR_SW0] += pAd->StaCfg.AvgRssi0;
				//pAd->Rssi[pAd->WlanFunCtrl.field.INV_TR_SW0] /= 2;
			
				DBGPRINT(RT_DEBUG_TRACE,("AntS, Ant 0, Tx: %lu, Rx: %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[0],pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[0]));
				DBGPRINT(RT_DEBUG_TRACE,("AntS, Ant 1, Tx: %lu, Rx: %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[1],pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[1]));

				DBGPRINT(RT_DEBUG_TRACE,("AntS, current antenna = %d\n",pAd->WlanFunCtrl.field.INV_TR_SW0));
				DBGPRINT(RT_DEBUG_TRACE,("AntS: Train State: PER[Main] = %lu, PER[Aux] = %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityPER[0],pAd->AntennaDiversityInfo.AntennaDiversityPER[1])); 
				DBGPRINT(RT_DEBUG_TRACE,("AntS: TotalTxCount[Main] = %lu, TotalTxCount[Aux] = %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[0],pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[1])); 
				DBGPRINT(RT_DEBUG_TRACE,("AntS: TotalRxCount[Main] = %lu, TotalRxCount[Aux] = %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[0],pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[1])); 

				if (pAd->AntennaDiversityInfo.AntennaDiversityCount == 0)
				{
					AsicSetRxAnt(pAd, !pAd->WlanFunCtrl.field.INV_TR_SW0);
				}
				else if (pAd->AntennaDiversityInfo.AntennaDiversityCount == 1)
				{
					/*
					  *	We will use packet count to select antenna if PER are the same
					  */
					CHAR diffRssi = 0, diffPER = 0;						  
					UINT Ant0TotalPacket = pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[0] + pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[0];
					UINT Ant1TotalPacket = pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[1] + pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[1];

					Ant1TotalPacket += pAd->StaCfg.AntSAuxDelta;

					DBGPRINT(RT_DEBUG_TRACE,("AntS: AntSAuxDelta = %lu, AntSRssiFactor = %lu, AntSPERFactor = %lu\n",
							pAd->StaCfg.AntSAuxDelta,
							pAd->StaCfg.AntSRssiFactor,
							pAd->StaCfg.AntSPERFactor)); 

					if (Rssi >= -75)
					{
						if (Rssi < -35)
						{
							if (pAd->AntennaDiversityInfo.Rssi[0] > pAd->AntennaDiversityInfo.Rssi[1])
							{
								diffRssi = pAd->AntennaDiversityInfo.Rssi[0] - pAd->AntennaDiversityInfo.Rssi[1];
								Ant0TotalPacket +=  (diffRssi*Ant0TotalPacket/pAd->StaCfg.AntSRssiFactor);
					
								DBGPRINT(RT_DEBUG_TRACE,("AntS: Good Rssi is at Main !! ++%lu\n",(diffRssi*Ant0TotalPacket/pAd->StaCfg.AntSRssiFactor))); 
							}
							else
							{
								diffRssi = pAd->AntennaDiversityInfo.Rssi[1] - pAd->AntennaDiversityInfo.Rssi[0];
								Ant1TotalPacket +=  (diffRssi*Ant1TotalPacket/pAd->StaCfg.AntSRssiFactor);
								DBGPRINT(RT_DEBUG_TRACE,("AntS: Good Rssi is at Aux !!++%lu\n",(diffRssi*Ant1TotalPacket/pAd->StaCfg.AntSRssiFactor))); 
							}
						}

						if (pAd->AntennaDiversityInfo.AntennaDiversityPER[0] < pAd->AntennaDiversityInfo.AntennaDiversityPER[1])
						{
							diffPER = (CHAR)(pAd->AntennaDiversityInfo.AntennaDiversityPER[1] - pAd->AntennaDiversityInfo.AntennaDiversityPER[0]);
							Ant0TotalPacket +=  (diffPER*Ant0TotalPacket/pAd->StaCfg.AntSPERFactor);
				
							DBGPRINT(RT_DEBUG_TRACE,("AntS: Good PER is at Main !! ++%lu\n", (diffPER*Ant0TotalPacket/pAd->StaCfg.AntSPERFactor))); 
						}
						else
						{
							diffPER = (CHAR)(pAd->AntennaDiversityInfo.AntennaDiversityPER[0] - pAd->AntennaDiversityInfo.AntennaDiversityPER[1]);
							Ant1TotalPacket +=  (diffPER*Ant1TotalPacket/pAd->StaCfg.AntSPERFactor);
				
							DBGPRINT(RT_DEBUG_TRACE,("AntS: Good PER is at Aux !! ++%lu\n", (diffPER*Ant1TotalPacket/pAd->StaCfg.AntSPERFactor))); 
						}						
						DBGPRINT(RT_DEBUG_TRACE,("AntS: Delta Packet = %lu,%lu, diffRssi = %d, diffPER = %d\n",(diffRssi*Ant0TotalPacket/pAd->StaCfg.AntSRssiFactor),(diffPER*Ant1TotalPacket/pAd->StaCfg.AntSPERFactor),diffRssi, diffPER)); 
					}
					AsicSetRxAnt(pAd, (Ant0TotalPacket > Ant1TotalPacket)?(0):(1));
					
					if ((pAd->CommonCfg.HWAntennaSwitch == 1) || ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE)))
					{
						// HW antenna will not enable SW antenna selection based on CCK beacon
					}
					else
					{
					}
					
					DBGPRINT(RT_DEBUG_TRACE,("AntS: Stop Train State: PER[Main] = %lu, PER[Aux] = %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityPER[0],pAd->AntennaDiversityInfo.AntennaDiversityPER[1])); 
					DBGPRINT(RT_DEBUG_TRACE,("AntS: TotalTxCount[Main] = %lu, TotalTxCount[Aux] = %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[0],pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[1])); 
					DBGPRINT(RT_DEBUG_TRACE,("AntS: TotalRxCount[Main] = %lu, TotalRxCount[Aux] = %lu\n",pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[0],pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[1])); 
					DBGPRINT(RT_DEBUG_TRACE,("AntS: Select Ant = %d, Main=%u, Aux=%d\n",(Ant0TotalPacket > Ant1TotalPacket)?(0):(1), Ant0TotalPacket,Ant1TotalPacket)); 
					
					pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[0] = pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[1] = 0;
					pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[0] = pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[1] = 0;
					pAd->AntennaDiversityInfo.AntennaDiversityState = 0;
					pAd->AntennaDiversityInfo.AntennaDiversityCount = 0;		
					
					// the next antenna-slection can start only after 5 seconds later
					pAd->AntennaDiversityInfo.TrainCounter = 50;// 5 second;						
					
				}
				pAd->AntennaDiversityInfo.AntennaDiversityCount ++;
			}
			
			if ((pAd->AntennaDiversityInfo.AntennaDiversityTrigger == 1) && ((AccuTxTotalCnt > SWANT_THRESHOLD)))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("AntS: First Startup\n")); 
				pAd->AntennaDiversityInfo.AntennaDiversityState = 1; //Go into training state
				pAd->AntennaDiversityInfo.AntennaDiversityTrigger = 0;
				pAd->RalinkCounters.OneSecRxOkCnt2 = 0; // reset the counter for success-RX packets.
				pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[0] = 0;
				pAd->AntennaDiversityInfo.AntennaDiversityTxPacketCount[1] = 0;
				pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[0] = 0;
				pAd->AntennaDiversityInfo.AntennaDiversityRxPacketCount[1] = 0;
				pAd->AntennaDiversityInfo.AntennaDiversityCount = 0;

				if ((Rssi >= -63) || (pAd->WlanBTCoexInfo.coexSettingRunning == FALSE))
					pAd->WlanBTCoexInfo.TxQualityFlag = 1;
			}
		}

}


/*
	BT and WALN can't access EEPROM at the same time.
	WLAN need to check bit31 of WLAN_FUN_INFO, then write bit31 of BT_FUN_INFO.
	After finish reading EEPROM, clear bit31 of BT_FUN_INFO.
*/
INT RT3290_eeprom_access_grant(
	IN RTMP_ADAPTER *pAd,
	IN BOOLEAN bGetCtrl)
{
	ULONG	btFunInfo = 0;

	RT3290_CHECK_SW_EEP_BUSY(pAd);
	RTMP_IO_FORCE_READ32(pAd, BT_FUN_INFO, &btFunInfo);
	if (bGetCtrl)
		btFunInfo |= 0x80000000;
	else
		btFunInfo &= ~(0x80000000);
	RTMP_IO_FORCE_WRITE32(pAd, BT_FUN_INFO, btFunInfo);

	return TRUE;
}


/*
	MAC idle Power saving mode enable.
	00:disable MAC idle power saving
	01: 1/2 MAC clock
	10: 1/4 MAC clock
	11: 1/8 MAC clock
	Access HW register(BBP) via FW will delay because MAC idle. So, driver will disable MAC idle before R/W BBP and will enable MAC idle after R/W BBP.
*/
VOID RTMP_MAC_PWRSV_EN(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN EnterIdle,
	IN BOOLEAN use40M)
{
	if (IS_RT3290(pAd))
	{
		UINT32 value;
		DBGPRINT(RT_DEBUG_TRACE,
				("RTMP_MAC_PWRSV_EN EnterIdle %d  bPCIclkOff = %d\n",
				EnterIdle, pAd->bPCIclkOff));
		RTMP_IO_FORCE_READ32(pAd, PBF_SYS_CTRL, &value);

		if (EnterIdle)
			value |= 0x100000;/* Enable MAC new features idle power save */
		else
			value &= ~(0x100000);			

		RTMP_IO_FORCE_WRITE32(pAd, PBF_SYS_CTRL, value);

	}
}


VOID RT3290_LinkDown(
	IN RTMP_ADAPTER *pAd)
{
	WLAN_FUN_CTRL_STRUC WlanFunCtrl = {.word=0};
	CMB_CTRL_STRUC CmbCtrl={.word=0};

	
	RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);
	// RT3290 WLAN BT Coex will modify BBP_R65. Restore it here.
	if (pAd->BbpWriteLatch[BBP_R65] != 0x2c)
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2c);	

	if (IS_RT3290LE(pAd))
	{
		if (pAd->BbpWriteLatch[BBP_R67] != 0x24)
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x24);				
	}
	else
	{
		if (pAd->BbpWriteLatch[BBP_R67] != 0x20)
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x20);			
	}				

	// TODO: shiang, for BaLimit, now not apply it in linux driver!
	pAd->CommonCfg.BaLimit = 14;
	pAd->WlanBTCoexInfo.ampduOff = FALSE;

	RTMP_IO_READ32(pAd, CMB_CTRL, &CmbCtrl.word);
	if ((CmbCtrl.field.LDO3_EN == TRUE) && (CmbCtrl.field.LDO_CORE_LEVEL == 2) && !IS_RT3290LE(pAd))
	{
		// use 1.2v
		CmbCtrl.field.LDO_CORE_LEVEL = 0;
		RTMP_IO_WRITE32(pAd, CMB_CTRL, CmbCtrl.word);
	}
}


VOID RT3290_LinkUp(
	IN RTMP_ADAPTER *pAd)
{

			UCHAR Value;
			UCHAR Data;
			if (IS_RT3290(pAd))
			{
				CMB_CTRL_STRUC CmbCtrl={.word = 0};		
						RTMP_IO_READ32(pAd, CMB_CTRL, &CmbCtrl.word);
						if (CmbCtrl.field.LDO3_EN == TRUE)
						{
							if (IS_RT3290LE(pAd))
							{
								if (pAd->CommonCfg.BBPCurrentBW == BW_20)
								{
									if ((pAd->CommonCfg.Channel == 6) ||(pAd->CommonCfg.Channel == 7)||(pAd->CommonCfg.Channel == 13))
									{
										// use 1.15v
										CmbCtrl.field.LDO_CORE_LEVEL = 5;
									}
									else if (CmbCtrl.field.LDO_CORE_LEVEL == 5)
									{
										// use 1.2v
										CmbCtrl.field.LDO_CORE_LEVEL = 0;								
									}
								}
								else
								{
									if (CmbCtrl.field.LDO_CORE_LEVEL != 5)
									{
										// use 1.15v
										CmbCtrl.field.LDO_CORE_LEVEL = 5;
									}
								}						
							}
							else if (pAd->CommonCfg.BBPCurrentBW == BW_20)
							{			
								if ((CmbCtrl.field.LDO_CORE_LEVEL != 2) && ((pAd->CommonCfg.Channel == 13) || (pAd->CommonCfg.Channel == 14)))
								{
									// Increase LDO_CORE_LEVEL from 1.2v to 1.3v to patch ch-13/14 spur
									CmbCtrl.field.LDO_CORE_LEVEL = 2;
								}
								else if (CmbCtrl.field.LDO_CORE_LEVEL == 2)
								{
									// use 1.2v
									CmbCtrl.field.LDO_CORE_LEVEL = 0;
								}
							}
							RTMP_IO_WRITE32(pAd, CMB_CTRL, CmbCtrl.word);
						}

						//BtAFHCtl(pAd, pAd->CommonCfg.BBPCurrentBW, pAd->CommonCfg.Channel, pAd->CommonCfg.CentralChannel, FALSE);
			}

			//Profile_TwoAnt(pAd);

			if (IS_RT3290(pAd) && (pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode == FALSE))
			{
				RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
				Data  &= 0xFFFFFF00;
				RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);

				RTMP_IO_WRITE32(pAd, PBF_MAX_PCNT, 0x1F3F7F9F);
				DBGPRINT(RT_DEBUG_TRACE, ("Txburst 1\n"));
			}

	// RT3290 WLAN BT Coex will modify BBP_R65. Restore it here.
	if (pAd->BbpWriteLatch[BBP_R65] != 0x2c)
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2c);	

	// TODO: shiang, for BaLimit, now not apply it in linux driver!
	pAd->CommonCfg.BaLimit = 14;//woody
	pAd->WlanBTCoexInfo.ampduOff = FALSE;

	Value = 0x0;
	RT30xxWriteRFRegister(pAd, RF_R47, (UCHAR)Value);
	pAd->WlanBTCoexInfo.alc = Value;
}


/*
	========================================================================
	
	Routine Description:
		Enable Wlan function. this action will enable wlan clock so that chip can accept command. So MUST put in the 
		very beginning of Initialization. And put in the very LAST in the Halt function.

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None

	IRQL <= DISPATCH_LEVEL
	
	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
VOID RTMPEnableWlan(
	IN PRTMP_ADAPTER	pAd,
	IN BOOLEAN bOn,
	IN BOOLEAN bResetWLAN)
{
	WLAN_FUN_CTRL_STRUC WlanFunCtrl = {.word=0};
	CMB_CTRL_STRUC CmbCtrl;
	WPDMA_GLO_CFG_STRUC GloCfg={.word=0};
	UCHAR index,PatchPLLCount = 0;


	RTMP_SEM_LOCK(&pAd->WlanEnLock);

	DBGPRINT(RT_DEBUG_OFF,
				("CacheVal-OnOff:%d, bPCIclkOff:%d, WlanFunCtrl:0x%x\n",
				bOn, pAd->bPCIclkOff, pAd->WlanFunCtrl.word));
	RTMP_IO_FORCE_READ32(pAd, WLAN_FUN_CTRL, &pAd->WlanFunCtrl.word);

	if (bResetWLAN == TRUE)
	{
		pAd->WlanFunCtrl.field.GPIO0_OUT_OE_N = 0xFF;
		pAd->WlanFunCtrl.field.FRC_WL_ANT_SET = TRUE;
	}
	
	DBGPRINT(RT_DEBUG_OFF,
				("==> RTMPWlan %d, bPCIclkOff = %d, pAd->WlanFunCtrl.word = 0x%x\n",
				bOn, pAd->bPCIclkOff, pAd->WlanFunCtrl.word));

	WlanFunCtrl.word = pAd->WlanFunCtrl.word;
	if (bOn == TRUE)
	{
		if ((WlanFunCtrl.field.WLAN_EN == 1) && (bResetWLAN == FALSE))
		{
			RTMP_SEM_UNLOCK(&pAd->WlanEnLock);
			DBGPRINT(RT_DEBUG_OFF,
				("<== RTMPWlan bPCIclkOff = %d, Exit because WLAN EN is already ON \n",
				pAd->bPCIclkOff));			
			return;
		}
		WlanFunCtrl.field.WLAN_CLK_EN = 0;
		WlanFunCtrl.field.WLAN_EN = 1;
	}
	else
	{
		UINT i;

		/* Change Interrupt bitmask. */
		RTMP_IO_FORCE_WRITE32(pAd, INT_MASK_CSR, 0);
		DBGPRINT(RT_DEBUG_ERROR, ("Set INT_MASK_CSR = 0\n"));

		RTMP_IO_FORCE_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		GloCfg.field.EnableRxDMA = 0;
		RTMP_IO_FORCE_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);

		/* wait RX DMA idle */
		i = 0;
		do
		{
			RTMP_IO_FORCE_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
			if ((GloCfg.field.RxDMABusy == 0) || (GloCfg.word == 0xFFFFFFFF))
				break;

			RTMPusecDelay(1000);
			i++;
		}while(i < 200);

		if (i >= 200)
		{
			RTMP_SEM_UNLOCK(&pAd->WlanEnLock);
			DBGPRINT(RT_DEBUG_ERROR, ("[boundary]DMA Rx keeps busy.  %d\n", i ));
#ifdef CONFIG_STA_SUPPORT
			AsicForceWakeup(pAd, DOT11POWERSAVE);
#endif /*CONFIG_STA_SUPPORT*/
			return;
		}	
		
		WlanFunCtrl.field.WLAN_EN = 0;
		/* clear it here, after PLL_LD write it to TRUE */
		WlanFunCtrl.field.WLAN_CLK_EN = 0;
	}
	
	DBGPRINT(RT_DEBUG_ERROR,
		("==> RTMPWlan %d, bPCIclkOff = %d, WlanFunCtrl.word = 0x%x\n",
		bOn, pAd->bPCIclkOff, WlanFunCtrl.word));
	RTMP_IO_FORCE_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
	pAd->WlanFunCtrl.word = WlanFunCtrl.word;
	RTMPusecDelay(2);

	do
	{
		index = 0;
		CmbCtrl.word = 0;
		if (bOn == TRUE)
		{
			do 
			{
				RTMP_IO_FORCE_READ32(pAd, CMB_CTRL, &CmbCtrl.word);

				/* HW issue: Must check PLL_LD&XTAL_RDY when setting EEP to disable PLL power down */
				if ((CmbCtrl.field.PLL_LD == 1) && (CmbCtrl.field.XTAL_RDY == 1))
					break;

				RTMPusecDelay(20);
			} while (index++ < 100);

			DBGPRINT(RT_DEBUG_TRACE, ("[CMB_CTRL=0x%08x]. index = %d.\n", CmbCtrl.word, index));
			if (index>=100)
			{
				DBGPRINT(RT_DEBUG_ERROR, 
						("Lenny:[boundary]Check PLL_LD ..CMB_CTRL 0x%08x, index=%d,PatchPLLCount=%d !\n",
						CmbCtrl.word, index,PatchPLLCount));

				if (PatchPLLCount>=10)
					break;

				/* Patch PLL issue(Lenny) */
				RTMP_IO_FORCE_WRITE32(pAd, 0x58, 0x018);
				RTMPusecDelay(10);			
				RTMP_IO_FORCE_WRITE32(pAd, 0x58, 0x418); /* reset */
				RTMPusecDelay(10);			
				RTMP_IO_FORCE_WRITE32(pAd, 0x58, 0x618);/* reset and clock out enable */

				PatchPLLCount++;
				
			}
			else
			{
				RTMP_IO_FORCE_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);		
				PatchPLLCount = 0; /* for exit do while loop */
			}
		
			pAd->WlanFunCtrl.field.PCIE_APP0_CLK_REQ = 0;
			pAd->WlanFunCtrl.field.WLAN_CLK_EN = 1;

			if (bResetWLAN)
			{
				RTMP_IO_FORCE_WRITE32(pAd, WLAN_FUN_CTRL, pAd->WlanFunCtrl.word | 0x8);	
				RTMPusecDelay(2);
			}


			if (bResetWLAN)
			{
				pAd->WlanFunCtrl.word &= (~0x8);
				RTMP_IO_FORCE_WRITE32(pAd, WLAN_FUN_CTRL, pAd->WlanFunCtrl.word);		
			}
			else
			{
				RTMP_IO_FORCE_WRITE32(pAd, WLAN_FUN_CTRL, pAd->WlanFunCtrl.word);
			}
			
			RTMP_IO_FORCE_WRITE32(pAd, INT_SOURCE_CSR, 0x7fffffff);  /* clear garbage interrupts */
		}
	}while(PatchPLLCount != 0);

	RTMP_SEM_UNLOCK(&pAd->WlanEnLock);	
	DBGPRINT(RT_DEBUG_ERROR,
		("<== RTMPWlan %d  bPCIclkOff = %d pAd->WlanFunCtrl.word = 0x%x\n",
		bOn, pAd->bPCIclkOff, pAd->WlanFunCtrl.word));
}


/*
========================================================================
Routine Description:
	Initialize RT305x.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RT3290_Init(
	IN PRTMP_ADAPTER		pAd)
{
	// TODO: shiang, Fix me
	
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;


	/* 
		Init chip capabilities
	*/
	pChipCap->SnrFormula = SNR_FORMULA3;
	pChipCap->FlgIsHwWapiSup = TRUE;
	pChipCap->FlgIsVcoReCalMode = VCO_CAL_MODE_2;
	pChipCap->FlgIsHwAntennaDiversitySup = FALSE;	// TODO: shiang, RT3290 support this??
#ifdef STREAM_MODE_SUPPORT
	pChipCap->FlgHwStreamMode = FALSE;
#endif /* STREAM_MODE_SUPPORT */
#ifdef FIFO_EXT_SUPPORT
	pChipCap->FlgHwFifoExtCap = FALSE;
#endif /* FIFO_EXT_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	pChipCap->HW_PCIE_PS_SUPPORT = TRUE;
	pChipCap->HW_PCIE_PS_L3_ENABLE = TRUE;
#endif /* CONFIG_STA_SUPPORT */

	pChipCap->RfReg17WtMethod= RF_REG_WT_METHOD_STEP_ON;
		
	pChipCap->MaxNumOfRfId = 63;
	pChipCap->pRFRegTable = RT3290_RFRegTable;

	pChipCap->MaxNumOfBbpId = 255;	
	pChipCap->pBBPRegTable = RT3290_BBPRegTable;
	pChipCap->bbpRegTbSize = RT3290_NUM_BBP_REG_PARMS;

	pChipCap->MaxNss = 1;

	pChipCap->TXWISize = 16;
	pChipCap->RXWISize = 16;
	pChipCap->WPDMABurstSIZE = 3;
#ifdef RTMP_EFUSE_SUPPORT
	pChipCap->EFUSE_USAGE_MAP_START = 0x1e0;
	pChipCap->EFUSE_USAGE_MAP_END = 0x1fd;      
	pChipCap->EFUSE_USAGE_MAP_SIZE = 30;
	DBGPRINT(RT_DEBUG_TRACE, ("Efuse Size=0x%x [%x-%x] \n",
				pAd->chipCap.EFUSE_USAGE_MAP_SIZE,
				pAd->chipCap.EFUSE_USAGE_MAP_START,
				pAd->chipCap.EFUSE_USAGE_MAP_END));
#endif /* RTMP_EFUSE_SUPPORT */

	/*
		Following function configure beacon related parameters
		in pChipCap
			FlgIsSupSpecBcnBuf / BcnMaxHwNum / 
			WcidHwRsvNum / BcnMaxHwSize / BcnBase[]
	*/
	RtmpChipBcnInit(pAd);

	/*
		init operator
	*/
	
	/* BBP adjust */
	pChipOps->ChipBBPAdjust = RT3290_ChipBBPAdjust;
#ifdef CONFIG_STA_SUPPORT
	pChipOps->ChipAGCAdjust = RT3290_ChipAGCAdjust;
#endif /* CONFIG_STA_SUPPORT */

	/* Channel */
	pChipOps->ChipSwitchChannel = RT3290_ChipSwitchChannel;
	pChipOps->ChipAGCInit = RT3290_ChipAGCInit;

	pChipOps->AsicMacInit = NICInitRT3290MacRegisters;
	pChipOps->AsicBbpInit = NICInitRT3290BbpRegisters;
	pChipOps->AsicRfInit = NICInitRT3290RFRegisters;
	pChipOps->AsicRfTurnOn = NULL;

	pChipOps->AsicHaltAction = RT3290HaltAction;
	pChipOps->AsicRfTurnOff = RT3290LoadRFSleepModeSetup;
	pChipOps->AsicReverseRfFromSleepMode = RT3290ReverseRFSleepModeSetup;
	pChipOps->AsicResetBbpAgent = RT3290_AsicResetBbpAgent;
	
	/* MAC */

	/* EEPROM */
	pChipOps->NICInitAsicFromEEPROM = NULL;
#ifdef RTMP_FLASH_SUPPORT
	pChipCap->eebuf = RT3290_EeBuffer;
	/* pChipOps->AsicEeBufferInit = RT3290_AsicEeBufferInit; */
#endif /* RTMP_FLASH_SUPPORT */

	/* Antenna */
	pChipOps->AsicAntennaDefaultReset = RT3290_AsicAntennaDefaultReset;
	pChipOps->SetRxAnt = RT3290_AsicSetRxAnt;
	
	/* Frequence Calibration */
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
#ifdef CONFIG_STA_SUPPORT
	pChipOps->AsicFreqCalInit = InitFrequencyCalibration;
	pChipOps->AsicFreqCalStop = StopFrequencyCalibration;
	pChipOps->AsicFreqCal = FrequencyCalibration;
	pChipOps->AsicFreqOffsetGet = GetFrequencyOffset;
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
	/* Others */
#ifdef CONFIG_STA_SUPPORT
	pChipOps->NetDevNickNameInit = RT3290_NetDevNickNameInit;
#endif /* CONFIG_STA_SUPPORT */

	/* Chip tuning */
	pChipOps->RxSensitivityTuning = RT3290_RxSensitivityTuning;

	pChipOps->ATEReadExternalTSSI = NULL;
	pChipOps->TSSIRatio = NULL;


// TODO: shiang, following setting is get from RT5390 and not verified yet!!
	/* TxPower related setting */
	pChipOps->AsicGetTxPowerOffset = AsicGetTxPowerOffset;
#ifdef RTMP_INTERNAL_TX_ALC
	pChipCap->TxAlcTxPowerUpperBound_2G = 69;
	pChipCap->TxPowerTuningTable_2G = RT3290_TxPowerTuningTable;
	pChipOps->InitDesiredTSSITable = RT3290_InitDesiredTSSITable;
	pChipOps->AsicTxAlcGetAutoAgcOffset = RT3290_AsicTxAlcGetAutoAgcOffset;
#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef CARRIER_DETECTION_SUPPORT
	pAd->chipCap.carrier_func = TONE_RADAR_V2;
	pChipOps->ToneRadarProgram = ToneRadarProgram_v2;
#endif /* CARRIER_DETECTION_SUPPORT */


//++++++++++++++++++++++++++++++++++++++++++++

#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
#ifdef CONFIG_STA_SUPPORT
	pChipCap->FreqCalibrationSupport = TRUE;
	pChipCap->FreqCalInitMode = FREQ_CAL_INIT_MODE2;
	pChipCap->FreqCalMode = FREQ_CAL_MODE1;
	pChipCap->RxWIFrqOffset = RXWI_FRQ_OFFSET_FIELD1;
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */

pAd->StaCfg.AntS=1;


//--------------------------------------------------------
// TODO: shiang, upper setting is get from RT5390 and not verified yet!!
	

/* Following callback functions already initiailized in RtmpChipOpsEepromHook( ) */
	/*  Calibration access related callback functions */
/*
	int (*eeinit)(struct _RTMP_ADAPTER *pAd);
	int (*eeread)(struct _RTMP_ADAPTER *pAd, USHORT offset, PUSHORT pValue);
	int (*eewrite)(struct _RTMP_ADAPTER *pAd, USHORT offset, USHORT value);
*/
	/* MCU related callback functions */
/*
	int (*loadFirmware)(struct _RTMP_ADAPTER *pAd);
	int (*eraseFirmware)(struct _RTMP_ADAPTER *pAd);
	int (*sendCommandToMcu)(struct _RTMP_ADAPTER *pAd, UCHAR cmd, UCHAR token, UCHAR arg0, UCHAR arg1, BOOLEAN FlgIsNeedLocked);
*/

/* Following callback functions already initiailized in RtmpChipOpsHook() */
/*
	// Power save
	VOID (*EnableAPMIMOPS)(IN struct _RTMP_ADAPTER *pAd, IN BOOLEAN ReduceCorePower);
	VOID (*DisableAPMIMOPS)(IN struct _RTMP_ADAPTER *pAd);
*/
}


//
// Only for RT3290
// MAC	:	0x40 (COEXCFG0)
// BBP	:	65, 47
// RF	:	RF_R39, RF_R47 and RF_R49
// SW	:	BaLimit, ampduOff
//
VOID BtCoexSetting(
			IN RTMP_ADAPTER *pAd,
			IN CHAR				Rssi,
			IN PCHAR			pTotalDeltaPower)

{
	if (!IS_RT3290(pAd) ||
		(pAd->bPCIclkOff == TRUE) ||
		RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF) ||
		RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
	{
		return;
	}	
	else
	{
		WLAN_FUN_INFO_STRUC wlanFunInfo = {.word = 0};
		BT_FUN_CTRL_STRUC btFunCtrl = {.word = 0};
		CMB_CTRL_STRUC CmbCtrl={.word = 0};		
		ULONG	MacValueCoexCfg0;//, COEXCFG2Value;
		UCHAR	RfR47Value, RfR49Value, RfR27Value, rfvalue;		
		UCHAR	BbpR65 = 0, BbpR49 = 0, BbpR47 = 0, BbpR67 = 0x20;	
		UCHAR	index;
		CHAR	TxPwer = 0;
		
		RTMP_IO_READ32(pAd, WLAN_FUN_INFO, &wlanFunInfo.word);
		RTMP_IO_READ32(pAd, BT_FUN_CTRL, &btFunCtrl.word);		
		//RTMP_IO_READ32(pAd, COEXCFG2, &COEXCFG2Value);		

		pAd->WlanFunInfo.word = wlanFunInfo.word;
		pAd->BtFunCtrl.word = btFunCtrl.word;

		// Search Tx power value
		for (index = 0; index < MAX_NUM_OF_CHANNELS; index++)
		{
			if (pAd->CommonCfg.Channel == pAd->TxPower[index].Channel)
			{
				TxPwer = pAd->TxPower[index].Power;
				break;
			}
		}	
		
		if (TxPwer > 5)// TxPwer would be minus 5 later
		{
			RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)(&RfR49Value));
			RfR49Value = ((RfR49Value & ~0x3F) | TxPwer);
			
			if ((RfR49Value & 0x3F) > 0x27) // The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x27
			{
				RfR49Value = ((RfR49Value & ~0x3F) | 0x27);
			}
		}
		
	
		if (pAd->WlanBTCoexInfo.DisableRssiRangeTunning == TRUE)
		{
			/* Reg:0x77f0 Value:0xa will turn this to disable*/
		}
		else if (((wlanFunInfo.field.COEX_MODE&0x3E) != 0) && 
				(btFunCtrl.field.BT_EN == TRUE) && 
				(pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode == TRUE))		
		{
			ULONG CoexMode = wlanFunInfo.field.COEX_MODE;
			DBGPRINT(RT_DEBUG_TRACE, ("%s:COEX_MODE is active(Two Antenna Mode)!!! CoexMode = %lu, BBPCurrentBW = %d, Rssi = %d, RfR49Value = 0x%x\n", 
				__FUNCTION__,
				CoexMode,
				pAd->CommonCfg.BBPCurrentBW,
				Rssi,
				RfR49Value));
	
			//Disable: halt low priority Tx when wl_rx_busy asserted
			//if ((COEXCFG2Value&0x02000000) == 0x02000000)
			//{
			//	COEXCFG2Value &= ~(0x02000000);
			//	RTMP_IO_WRITE32(pAd, COEXCFG2, COEXCFG2Value);				
			//}

			// use Temp as adc_insel
			RT30xxReadRFRegister(pAd, RF_R27, (PUCHAR)&RfR27Value);
			if ((RfR27Value & 0xc0) != 0x40)
			{
				RfR27Value &= ~0xc0;
				RfR27Value |= 0x40;
				RT30xxWriteRFRegister(pAd, RF_R27, (UCHAR)RfR27Value);
			}

			// TSSI Mode: 0: averaged TSSI (default) 
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
			if ((BbpR47 & 0x18) != 0x00)
			{
				BbpR47 &= ~0x18;
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpR47);
			}
			
			RT30xxReadRFRegister(pAd, RF_R47, (PUCHAR)&RfR47Value);
			RfR47Value &= ~0x40;

			if (pAd->CommonCfg.BBPCurrentBW == BW_40)
			{
                            //
                            // Rx AGC/CCA and Initial LNA Setting
                            // Manual LNA
                            // bit 3:2 - 10 - LNA2
                            //  
				BbpR65 = 0x29;					

				if ( Rssi >= -45 )
				{
                                    //
                                    // Rx AGC/CCA and Initial LNA Setting
                                    // Manual LNA
                                    // bit 3:2 - 01 - LNA1(lowest LNA)
                                    //
					BbpR65 = 0x25;		 
					
					RfR47Value |= 0x40;

					if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
					{
						*pTotalDeltaPower += -6;					
					}
					else
					{
						
					}

					pAd->CommonCfg.BaLimit = 14;
					pAd->WlanBTCoexInfo.ampduOff = FALSE;

					MacValueCoexCfg0 = 0x4e008947;
				}
				else if((Rssi <= -45) && (Rssi >= -55))
				{
					RfR47Value |= 0x40;

					if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
					{
						*pTotalDeltaPower += -6;					
					}
					else
					{
						
					}

					pAd->CommonCfg.BaLimit = 14;
					pAd->WlanBTCoexInfo.ampduOff = FALSE;
					if (Rssi < -50 && IS_RT3290LE(pAd))
						MacValueCoexCfg0 = 0x4e008047;
					else
						MacValueCoexCfg0 = 0x4e008947;
					
				}	
				else if((Rssi <= -55) && (Rssi >= -60))
				{
					RfR47Value |= 0x40;
					
					if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
					{
					}
					else
					{
						RfR49Value += 5;							
					}

					pAd->CommonCfg.BaLimit = 14;
					pAd->WlanBTCoexInfo.ampduOff = FALSE;
					if (IS_RT3290LE(pAd))
					{
						MacValueCoexCfg0 = 0x4e008047;
					}
					else
					{
						MacValueCoexCfg0 = 0x4e008947;
					}
				}					
				else if((Rssi <= -60) && (Rssi >= -70))
				{
					if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
					{
						pAd->CommonCfg.BaLimit = 4;
						MacValueCoexCfg0 = 0x6e008047;
					}
					else
					{
						pAd->CommonCfg.BaLimit = 4;
						MacValueCoexCfg0 = 0x4e008847;

						if (IS_RT3290LE(pAd))
						{
							MacValueCoexCfg0 = 0x9e108047;
						}
						else
						{					
							RfR47Value |= 0x40;
							RfR49Value += 5;
						}						
					}
					pAd->WlanBTCoexInfo.ampduOff = FALSE;
				}
				else if((Rssi <= -70) && (Rssi >= -75))
				{
					if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
					{
						pAd->CommonCfg.BaLimit = 4;
						MacValueCoexCfg0 = 0x6e008047;
					}
					else
					{
						pAd->CommonCfg.BaLimit = 4;
						MacValueCoexCfg0 = 0x4e008847;

						if (IS_RT3290LE(pAd))
						{
							MacValueCoexCfg0 = 0x9e108047;
						}
						else
						{
							if ((pAd->RalinkCounters.OneSecTransmittedByteCount > ((pAd->RalinkCounters.OneSecReceivedByteCount * 2) + 5000)))
							{
								RfR47Value |= 0x40;
								RfR49Value += 5;
							}						
						}
					}
					pAd->WlanBTCoexInfo.ampduOff = FALSE;			
					
				}
				else// Rssi < -75
				{
					BbpR65 = 0x2d;

					if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
					{
						pAd->WlanBTCoexInfo.ampduOff = TRUE;
						MacValueCoexCfg0 = 0x6e008047;						
					}
					else
					{
						pAd->WlanBTCoexInfo.ampduOff = TRUE;
						MacValueCoexCfg0 = 0x4e008847;

						if (IS_RT3290LE(pAd) && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BT_ON) && Rssi <= -80)
						{
							MacValueCoexCfg0 = 0x9e10BF47;
							//*pTotalDeltaPower += -6;	
							DBGPRINT(RT_DEBUG_TRACE, ("BTCoexistence: In BT3.0 Mode, release BT Power Upbound limit to Max value\n"));
						}
						else if (IS_RT3290LE(pAd))
						{
							MacValueCoexCfg0 = 0x9e108047;
						}
						else
						{
							if ((pAd->RalinkCounters.OneSecTransmittedByteCount > ((pAd->RalinkCounters.OneSecReceivedByteCount * 2) + 5000)))
							{
								RfR47Value |= 0x40;
								RfR49Value += 5;
							}
						}
					}

				}			
			}
			else // BW = 20
                     {
                     	                 	       
                     	CHAR BBPR65threshold = -45;
				if (IS_RT3290LE(pAd))
					BBPR65threshold = -45;
				
	               	 pAd->WlanBTCoexInfo.MCSupbound= FALSE;	
							
                             if (Rssi >= BBPR65threshold)
                                {
                                        //
                                        // Rx AGC/CCA and Initial LNA Setting
                                        // Manual LNA
                                        // bit 3:2 - 01 - LNA1(lowest LNA)
                                        //  
                                        BbpR65 = 0x25;

                                        MacValueCoexCfg0 = 0x4e008947;

                                        //control Tx gain for concurrent BT/Wi-Fi
                                        RfR47Value |= 0x40;

                                        //*pTotalDeltaPower += -6;
                                        
                                        // control APMDU aggregation packet size
                                        pAd->CommonCfg.BaLimit = 14;
                                        pAd->WlanBTCoexInfo.ampduOff = FALSE;
					     DBGPRINT(RT_DEBUG_TRACE,("Coexistence:R65 = %d, RSSI = %d\n",BbpR65,Rssi));
                                }
                                else if((Rssi <= BBPR65threshold) && (Rssi >= -60))
                                {         
                                		//
						// Rx AGC/CCA and Initial LNA Setting
						// Manual LNA
						// bit 3:2 - 10 - LNA2
						//  
						
						BbpR65 = 0x29;
					/*
						if (IS_RT3290LE(pAd))
							MacValueCoexCfg0 = 0x4e008047;
						else*/
						if (Rssi < -50 && IS_RT3290LE(pAd))
						{
							MacValueCoexCfg0 = 0x4e008047;
						}
						else
						{
							MacValueCoexCfg0 = 0x4e008947;
						}
						//control Tx gain for concurrent BT/Wi-Fi
						RfR47Value |= 0x40;
						RfR49Value += 5;

						// control APMDU aggregation packet size
						pAd->CommonCfg.BaLimit = 14;
						pAd->WlanBTCoexInfo.ampduOff = FALSE;
						DBGPRINT(RT_DEBUG_TRACE,("Coexistence:R65 = %d, RSSI = %d\n",BbpR65,Rssi));
                                }
                                else if((Rssi <= -60) && (Rssi >= -65))
                                {                                       
						//
						// Rx AGC/CCA and Initial LNA Setting
						// Manual LNA
						// bit 3:2 - 10 - LNA2
						//                    
						BbpR65 = 0x29;

						if (IS_RT3290LE(pAd))
							MacValueCoexCfg0 = 0x4e008047;
						else						 
							MacValueCoexCfg0 = 0x4e008947;


						if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
						{
							//control Tx gain for concurrent BT/Wi-Fi
							RfR47Value |= 0x40;
							RfR49Value += 5;
						}
						else
						{
							*pTotalDeltaPower += -6;		
						}						

						// control APMDU aggregation packet size
						pAd->CommonCfg.BaLimit = 14;
						pAd->WlanBTCoexInfo.ampduOff = FALSE;
                                }	
                                else if((Rssi <= -65) && (Rssi >= -70))
                                {                                       
						//
						// Rx AGC/CCA and Initial LNA Setting
						// Manual LNA
						// bit 3:2 - 10 - LNA2
						//                    
						BbpR65 = 0x29;
						if (IS_RT3290LE(pAd))
							MacValueCoexCfg0 = 0x4e008047;
						else		          
							MacValueCoexCfg0 = 0x4e008947;


						if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
						{
							BbpR67 = 0x30;
							RfR47Value |= 0x40;
							RfR49Value += 5;		
						}
						else
						{
//							if (IS_RT3290LE(pAd))
//							{
//								MacValueCoexCfg0 = 0xde108947;
//							}
//							else
							{
								*pTotalDeltaPower += -6;		
							}
						}						

						// control APMDU aggregation packet size
					/*	if (IS_RT3290LE(pAd))
							pAd->CommonCfg.BaLimit = 4;
						else*/
							pAd->CommonCfg.BaLimit = 9;
						pAd->WlanBTCoexInfo.ampduOff = FALSE;
                                }									
                                else if((Rssi <= -70) && (Rssi >= -75))
                                {
                                        //
                                        // Rx AGC/CCA and Initial LNA Setting
                                        // Manual LNA
                                        // bit 3:2 - 10 - LNA2
                                        //                         

                                        // control APMDU aggregation packet size

						if ((wlanFunInfo.field.COEX_MODE&0x18) != 0)
						{
							BbpR65 = 0x29; 
                                        		pAd->CommonCfg.BaLimit = 9;
							if (IS_RT3290LE(pAd))
								MacValueCoexCfg0 = 0x4e008047;
							else												
								MacValueCoexCfg0 = 0x4e008947;
							BbpR67 = 0x30;

							RfR47Value |= 0x40;
							RfR49Value += 5;							
						}
						else
						{
						       BbpR65 = 0x29; 
						/*	if (IS_RT3290LE(pAd))
								pAd->CommonCfg.BaLimit = 4;
							else*/
								pAd->CommonCfg.BaLimit = 9;
							
							if (IS_RT3290LE(pAd))
								MacValueCoexCfg0 = 0x4e008047;
							else
								MacValueCoexCfg0 = 0x4e008947;

//							if (IS_RT3290LE(pAd))
//							{
//								MacValueCoexCfg0 = 0xde108947;
//							}
//							else
							{
								*pTotalDeltaPower += -6;		
							}
						}
						pAd->WlanBTCoexInfo.ampduOff = FALSE;
                                }
                                else if((Rssi <= -75) && (Rssi >= -80)) 
                                {
						//
						// Rx AGC/CCA and Initial LNA Setting
						// disable Manual LNA
						// bit 3:2 - 11 - LNA3(Highest LNA)
						//                    
						BbpR65 = 0x2d;

						// Disable APMDU aggregation                              
						if (0)//(wlanFunInfo.field.COEX_MODE&0x18) != 0)
						{
							pAd->WlanBTCoexInfo.ampduOff = TRUE;
							BbpR67 = 0x30;
							MacValueCoexCfg0 = 0x4e008807;
						}
						else
						{
						       BbpR65 = 0x2d; 
							if (IS_RT3290LE(pAd))
								MacValueCoexCfg0 = 0x4e008047;
							else							   
								MacValueCoexCfg0 = 0x4e008847;
							pAd->WlanBTCoexInfo.ampduOff = TRUE;

							//if (IS_RT3290LE(pAd))
							//{
							//	MacValueCoexCfg0 = 0x9e108947;
							//}
							//else
							{
								if ((pAd->RalinkCounters.OneSecTransmittedByteCount > ((pAd->RalinkCounters.OneSecReceivedByteCount * 2) + 5000)))
									*pTotalDeltaPower += -6;		
							}
							/*
							else if ((pAd->RalinkCounters.OneSecReceivedByteCount > ((pAd->RalinkCounters.OneSecTransmittedByteCount * 5) + 5000)))
								MacValueCoexCfg0 = 0x4e008047;
							*/
						}
                                }
                                else if((Rssi <= -80) && (Rssi >= -85)) 
                                {
						//
						// Rx AGC/CCA and Initial LNA Setting
						// disable Manual LNA
						// bit 3:2 - 11 - LNA3(Highest LNA)
						//                    
						BbpR65 = 0x2d;

						// Disable APMDU aggregation                              
						if (0)//(wlanFunInfo.field.COEX_MODE&0x18) != 0)
						{
							pAd->WlanBTCoexInfo.ampduOff = TRUE;
							BbpR67 = 0x30;
							MacValueCoexCfg0 = 0x6e008807;
						}
						else
						{
							pAd->WlanBTCoexInfo.ampduOff = TRUE;
							BbpR67 = 0x30;
							if (IS_RT3290LE(pAd) && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BT_ON))
							{
								MacValueCoexCfg0 = 0x4e00BF07;
								DBGPRINT(RT_DEBUG_TRACE, ("BTCoexistence: In BT3.0 Mode, release BT Power Upbound limit to Max value\n"));
							}
							else if (IS_RT3290LE(pAd))
							{
								MacValueCoexCfg0 = 0x4e008007;
							}
							else
							{
								MacValueCoexCfg0 = 0x4e008807;
								if ((pAd->RalinkCounters.OneSecTransmittedByteCount > ((pAd->RalinkCounters.OneSecReceivedByteCount * 2) + 500)))
									*pTotalDeltaPower += -6;
							}
						}
                                }    
		                  else 
                                {
                                		if (Rssi <= -85)
							pAd->WlanBTCoexInfo.MCSupbound= TRUE;	
						//
						// Rx AGC/CCA and Initial LNA Setting
						// disable Manual LNA
						// bit 3:2 - 11 - LNA3(Highest LNA)
						//                            
						BbpR65 = 0x2d;               


						// Disable APMDU aggregation
						if (0)//(wlanFunInfo.field.COEX_MODE&0x18) != 0)
						{
							pAd->WlanBTCoexInfo.ampduOff = TRUE;
							BbpR67 = 0x30;
							MacValueCoexCfg0 = 0x6e008807;

						}
						else
						{
							pAd->WlanBTCoexInfo.ampduOff = TRUE;
							BbpR67 = 0x30;
							if (IS_RT3290LE(pAd) && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BT_ON))
							{
								MacValueCoexCfg0 = 0x4e00BF07;
								DBGPRINT(RT_DEBUG_TRACE, ("BTCoexistence: In BT3.0 Mode, release BT Power Upbound limit to Max value\n"));
							}
							else if (IS_RT3290LE(pAd))
							{
								MacValueCoexCfg0 = 0x4e008007;
							}
							else
							{
								MacValueCoexCfg0 = 0x4e008807;
								if ((pAd->RalinkCounters.OneSecTransmittedByteCount > ((pAd->RalinkCounters.OneSecReceivedByteCount * 2) + 500)))
									*pTotalDeltaPower += -6;							
							}
						}
                                }
                        }

                        MacValueCoexCfg0 |= 0x10100002; // BT_TRSW=1 and Force WLAN Rx to diversity antenna(FIX_WL_DI_ANT)
			   
                        RTMP_IO_WRITE32(pAd, COEXCFG0, MacValueCoexCfg0);
						
                        if (pAd->BbpWriteLatch[BBP_R65] != BbpR65)
                                RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BbpR65);

                        if (pAd->BbpWriteLatch[BBP_R67] != BbpR67)
                                RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, BbpR67);

                        
                        if (pAd->WlanBTCoexInfo.alc  != RfR47Value )
                        {
                                RT30xxWriteRFRegister(pAd, RF_R47, (UCHAR)RfR47Value);       
                                pAd->WlanBTCoexInfo.alc = RfR47Value;
                        }

                     /*   if ((pAd->CommonCfg.BBPCurrentBW == BW_40) && (Rssi <= -60) && (Rssi >= -65))
                        {
                                if (pAd->WlanBTCoexInfo.slna != 0x6b )
                                {
                                        RT30xxWriteRFRegister(pAd, RF_R39, 0x6b);
                                        pAd->WlanBTCoexInfo.slna = 0x6b;
                                }                                       
                        }
                        else if (pAd->WlanBTCoexInfo.slna != 0x2b )
                        {
                                RT30xxWriteRFRegister(pAd, RF_R39, 0x2b);
                                pAd->WlanBTCoexInfo.slna = 0x2b;
                        } */
			   if (pAd->WlanBTCoexInfo.slna != 0x2b )
			   {
                                RT30xxWriteRFRegister(pAd, RF_R39, 0x2b);
                                pAd->WlanBTCoexInfo.slna = 0x2b;
                        } 

			//ALC_RF = ALC_E2P + BBP_R49>>2 + 2.
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpR49);
			RfR49Value = RfR49Value + (BbpR49>>2) + 2;
			
			if (RfR49Value >= 0x9f)
				RfR49Value = 0x9f;
			
			DBGPRINT(RT_DEBUG_TRACE, ("%s: Final FNew RfR49Value = 0x%x, BbpR49 = %d\n",
					__FUNCTION__,
					RfR49Value,
					BbpR49
					));
			
			RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)RfR49Value);  

			if (IS_RT3290LE(pAd))
			{
				if(Rssi <-45 && Rssi > -62)
				{
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x14);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x06);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x2F);	
					
					RT30xxReadRFRegister(pAd, RF_R40, (PUCHAR)&rfvalue);
					rfvalue &= ~0x07;
					rfvalue |= 0x02;
					RT30xxWriteRFRegister(pAd, RF_R40, (UCHAR)rfvalue);	

					RT30xxReadRFRegister(pAd, RF_R43, (PUCHAR)&rfvalue);
					rfvalue &= ~0x07;
					RT30xxWriteRFRegister(pAd, RF_R43, (UCHAR)rfvalue);	
				}
				else
				{
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x18);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x09);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x33);	
					
					RT30xxReadRFRegister(pAd, RF_R40, (PUCHAR)&rfvalue);
					rfvalue &= ~0x07;
					rfvalue |= 0x03;
					RT30xxWriteRFRegister(pAd, RF_R40, (UCHAR)rfvalue);	

					RT30xxReadRFRegister(pAd, RF_R43, (PUCHAR)&rfvalue);
					rfvalue &= ~0x07;
					rfvalue |= 0x03;
					RT30xxWriteRFRegister(pAd, RF_R43, (UCHAR)rfvalue);	


				}
			}

                     pAd->WlanBTCoexInfo.coexSettingRunning = TRUE;			 
              }
		else if (((wlanFunInfo.field.COEX_MODE&0x3E) != 0) && 
				(btFunCtrl.field.BT_EN == TRUE) && 
				(pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode == 0))
		{
			
			if (Rssi > -65)
			{
				pAd->CommonCfg.BaLimit = 4;
				pAd->WlanBTCoexInfo.ampduOff = FALSE;
				
			}
			else if (Rssi > -75)
			{
				pAd->CommonCfg.BaLimit = 2;
				pAd->WlanBTCoexInfo.ampduOff = FALSE;
								
			}
			else
			{
				pAd->WlanBTCoexInfo.ampduOff = TRUE;				
			}

			/*
			// Not HID, Not A2DP, Not SCO
			if (((wlanFunInfo.field.COEX_MODE&BTCOEX_STATUS_HID_CONN) == 0) &&
				((wlanFunInfo.field.COEX_MODE&BTCOEX_STATUS_A2DP_BASIC_CONN) == 0) &&
				((wlanFunInfo.field.COEX_MODE&BTCOEX_STATUS_SYNC_CONN) == 0) &&
				((wlanFunInfo.field.COEX_MODE&BTCOEX_STATUS_LOWLAT_CONN) == 0) &&
				(pAd->RalinkCounters.OneSecReceivedByteCount > ((pAd->RalinkCounters.OneSecTransmittedByteCount * 2) + 2000)))
			{
				if ((COEXCFG2Value&0x00080000)!=0x00080000)
				{
					RTMP_IO_WRITE32(pAd, COEXCFG2, 0x0208F700);
				}
			}
			else
			{
				if ((COEXCFG2Value&0x00080000)==0x00080000)
				{
					RTMP_IO_WRITE32(pAd, COEXCFG2, 0x0212F7F7);
				}			
			}
			*/
			/*
			if ((wlanFunInfo.field.COEX_MODE&BTCOEX_STATUS_HID_CONN) == BTCOEX_STATUS_HID_CONN)
			{
				// comment for single ant debug
				//RTMP_IO_WRITE32(pAd, COEXCFG2, 0x0212F7F7);
				DBGPRINT(RT_DEBUG_TRACE, ("WHY I AM HERE ~~~ BECAUSE HID !!!\n"));	
			}
			else
			{
				//BT want to control this register at this time 
				//Therefore we release control right for BT
				//RTMP_IO_WRITE32(pAd, COEXCFG2, 0x0210F730);
				DBGPRINT(RT_DEBUG_TRACE, ("I SHOULD BE HERE!!!\n"));	
			}
			*/
			
		}
		else if (pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode == 0)
		{
			pAd->CommonCfg.BaLimit = 14;
			pAd->WlanBTCoexInfo.ampduOff = FALSE;
			
			/*if ((COEXCFG2Value&0x00080000)!=0x00080000)
			{
				RTMP_IO_WRITE32(pAd, COEXCFG2, 0x00099300);
				DBGPRINT(RT_DEBUG_TRACE, ("BT is idle!!!\n"));		
			}
			*/
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s:COEX_MODE is idle [%d] or is single antenna solution AUX_OPT_Bit15_Two_AntennaMode[%d]!!!\n", __FUNCTION__,wlanFunInfo.field.COEX_MODE,pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode));		

			// use TSSI as adc_insel
			RT30xxReadRFRegister(pAd, RF_R27, (PUCHAR)&RfR27Value);
			if ((RfR27Value & 0xc0) != 0x00)
			{
				RfR27Value &= ~0xc0;
				RT30xxWriteRFRegister(pAd, RF_R27, (UCHAR)RfR27Value);
			}

			// TSSI Mode: 2: new averaged TSSI mode
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpR47);
			if ((BbpR47 & 0x18) != 0x10)
			{
				BbpR47 = ((BbpR47 & ~0x18) | 0x10); // TSSI_MODE (new averaged TSSI mode for 3290/5390)
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpR47);			
			}

			RT30xxReadRFRegister(pAd, RF_R47, (PUCHAR)&RfR47Value);
			RfR47Value &= ~0x40;

			if (pAd->WlanBTCoexInfo.alc  != RfR47Value )
			{
				RT30xxWriteRFRegister(pAd, RF_R47, (UCHAR)RfR47Value);	
				pAd->WlanBTCoexInfo.alc = RfR47Value;
			}


			RTMP_IO_READ32(pAd, CMB_CTRL, &CmbCtrl.word);

			// Init  BBP R65		
			if ( (Rssi > -55) && (CmbCtrl.field.LDO3_EN == TRUE) && (CmbCtrl.field.LDO_CORE_LEVEL == 2) && !IS_RT3290LE(pAd))
			{
				if (pAd->BbpWriteLatch[BBP_R65] != 0x28)
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x28);			
			}
			else
			{
				if (pAd->BbpWriteLatch[BBP_R65] != 0x2c)
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2c);
			}

			// Init  BBP R67
			if (IS_RT3290LE(pAd))
			{
				if (pAd->BbpWriteLatch[BBP_R67] != 0x24)
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x24);
			}
			else
			{
				if (pAd->BbpWriteLatch[BBP_R67] != 0x20)
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x20);
			}

			if (pAd->WlanBTCoexInfo.slna != 0x1b )
			{
				RT30xxWriteRFRegister(pAd, RF_R39, 0x1b);
				pAd->WlanBTCoexInfo.slna = 0x1b;
			}

			if ((pAd->WlanBTCoexInfo.coexSettingRunning == TRUE) && (Rssi <= -80))
			{
				pAd->WlanBTCoexInfo.RateSelectionForceToUseRSSI = TRUE;
			}

			if (IS_RT3290LE(pAd) && ((pAd->CommonCfg.HWAntennaSwitch == 1) || ((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE))))
			{
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x14);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x06);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x2F);	

				RT30xxReadRFRegister(pAd, RF_R43, (PUCHAR)&rfvalue);
				rfvalue &= ~0x07;
				RT30xxWriteRFRegister(pAd, RF_R43, (UCHAR)rfvalue);				
			}	
			else if (IS_RT3290LE(pAd))
			{
				RT30xxWriteRFRegister(pAd, RF_R43, (UCHAR)0x7B);	
			
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R79, 0x18);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R80, 0x09);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R81, 0x33);	
				
				RT30xxWriteRFRegister(pAd, RF_R40, (UCHAR)0x0B);
							
			}

			pAd->WlanBTCoexInfo.coexSettingRunning = FALSE;

			if (Rssi <= -65)
			{
				pAd->CommonCfg.BaLimit = 9;
			}
			else
			{
				pAd->CommonCfg.BaLimit = 14;
			}

			pAd->WlanBTCoexInfo.ampduOff = FALSE;		

			//Enable: halt low priority Tx when wl_rx_busy asserted
			//if ((COEXCFG2Value&0x02000000) != 0x02000000)
			//{
			//	COEXCFG2Value |= 0x02000000;
			//	RTMP_IO_WRITE32(pAd, COEXCFG2, COEXCFG2Value);
			//}
		}

		BtCoexSettingBACapability(pAd, (BOOLEAN)btFunCtrl.field.BT_EN);

	}
}

//
// Single Antenna case will let BA TX to 8 and BA RX to 4
// If we set BA to 64 when there is no BT links, it is more difficult for BT to create new connections
//
VOID BtCoexSettingBACapability( 
 IN RTMP_ADAPTER *pAd,
 IN BOOLEAN BTEnable)
{
	//PRTMP_PORT	pPort = RTMPGetActivePort(pAd);

	if (!IS_RT3290(pAd))
	{
		return;
	}

	if (pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode == TRUE)
	{
		//Two Antenna return
		return;
	}

	if (pAd == NULL)
	{
		return;
	}	

	if (BTEnable == TRUE)
	{
		// Update BAWinLimit
		if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != 4)
		{
			pAd->CommonCfg.BACapability.field.RxBAWinLimit = 4;
			pAd->CommonCfg.BACapability.field.TxBAWinLimit = 8;
			
			pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	

			BAOriSessionTearDown(pAd, BSSID_WCID,0, TRUE, TRUE);
			BARecSessionTearDown(pAd, BSSID_WCID,0, TRUE);

			//BATableTearRECEntry(pAd, pPort, 0, BSSID_WCID,TRUE);
			//BATableTearORIEntry(pAd, pPort, 0, BSSID_WCID, TRUE, TRUE);
		}
	}
	else
	{
		// Update BAWinLimit
		if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != 0x40)
		{		
			pAd->CommonCfg.BACapability.field.RxBAWinLimit = 0x40;
			pAd->CommonCfg.BACapability.field.TxBAWinLimit = 0x40;
			
			pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	

			BAOriSessionTearDown(pAd, BSSID_WCID,0, TRUE, TRUE);
			BARecSessionTearDown(pAd, BSSID_WCID,0, TRUE);
			//BATableTearRECEntry(pAd, pPort, 0, BSSID_WCID,TRUE);
			//BATableTearORIEntry(pAd, pPort, 0, BSSID_WCID, TRUE, TRUE);
		}
	}
}


VOID Profile_TwoAnt(
	IN  PRTMP_ADAPTER pAd
)
{

	ULONG	MACValue = 0;
	CMB_CTRL_STRUC cmbCtrl = {.word = 0};
	//WLAN_FUN_CTRL_STRUC	WlanFunCtrl = {.word = 0};

	DBGPRINT(RT_DEBUG_ERROR, ("SSDA:(Two) Antenna Configuration\n"));
	pAd->CmbCtrl.field.AUX_OPT_Bit15_Two_AntennaMode = TRUE;

	RTMP_IO_FORCE_READ32(pAd, CMB_CTRL, &cmbCtrl.word);
	if (!(cmbCtrl.word & 0x8000))
	{
		cmbCtrl.word |= (0x9000);
		RTMP_IO_FORCE_WRITE32(pAd, CMB_CTRL, cmbCtrl.word);
	}
	printk("cmbCtrl = %x\n",cmbCtrl.word);
	
	// Overwrite default Coex Parameter
	RTMP_IO_FORCE_READ32(pAd, COEXCFG0, &MACValue);
	MACValue &= ~(0xFF000000);
	MACValue |= 0x5E000000;
	RTMP_IO_FORCE_WRITE32(pAd, COEXCFG0, MACValue);	
	RTMP_IO_FORCE_WRITE32(pAd, COEXCFG2, 0x0017937F);
	if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != 0x40)
	{		
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = 0x40;
		pAd->CommonCfg.BACapability.field.TxBAWinLimit = 0x40;
		pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	
	}
}


VOID NICInitRT3290MacRegisters(
	IN	PRTMP_ADAPTER pAd)
{
	UINT32 IdReg;


	for(IdReg=0; IdReg<RT3290_NUM_MAC_REG_PARMS; IdReg++)
	{
		RTMP_IO_WRITE32(pAd, RT3290_MACRegTable[IdReg].Register,
								RT3290_MACRegTable[IdReg].Value);
	}
}
#endif /* RT3290 */

