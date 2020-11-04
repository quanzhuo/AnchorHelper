/*! ------------------------------------------------------------------------------------------------------------------
* @file rtls_interface.h
* @brief	DecaWave RTLS CLE / RTLS Anchor common definitions / APIs
*
* @attention
*
* Copyright 2016 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
* @author
*/

#ifndef __RTLS_INTERFACE_H
#define __RTLS_INTERFACE_H

#include <stdint.h>
// #include "devicedb.h"

#define TS_40B_SIZE (5)
#define ADDR_BYTE_SIZE (8)
#define ADDR_BYTE_SIZE_SHORT (2)
#define STANDARD_FRAME_SIZE (127)
#define DWMINBLINKLEN (12) /* 12 octets for Minimum IEEE ID blink */

// Protocol structs and enums :
enum RTLS_CMD
{
	RTLS_CMD_PARM_STOP = 0,
	RTLS_CMD_PARM_START = 1,

	RTLS_CMD_REQ_CFG = 0x42,	 // Anchor: request configuration
	RTLS_CMD_SET_CFG_CCP = 0x44, // CLE: response on RTLS_CMD_REQ_CFG request
	// {{ [IDL-1103]: Anchor parameters expand.begin
	RTLS_CMD_SET_CFG_EXT_CCP = 0x45, // CLE: response on RTLS_CMD_REQ_EXT_CFG request
	// }} [IDL-1103]: Anchor parameters expand.end

	// Anchor -> LE
	RTLS_CMD_DEBUG = 0x37, // Anchor: Debug Message

	RTLS_CMD_REPORT_TOA = 0x3A,	   // v.3 TDOA
	RTLS_CMD_REPORT_TOA_EX = 0x3B, // v.3 TDOA with extra data (e.g. IMU data)
	RTLS_CMD_REPORT_TX_CS = 0x3E,  // v.3 CCP Tx report
	RTLS_CMD_REPORT_RX_CS = 0x3F,  // v.3 CCP Rx report

	RTLS_CMD_REPORT_TX_CS_V4 = 0x30,			// v.4 CCP Tx report
	RTLS_CMD_REPORT_RX_CS_V4 = 0x31,			// v.4 CCP Rx report
	RTLS_CMD_REPORT_TOA_V4 = 0x32,				// v.4 TDOA with FP
	RTLS_CMD_REPORT_TOA_IMU_V4 = 0x33,			// v.4 TDOA with FP and IMU data
	RTLS_CMD_REPORT_TOA_SOS_LOWPOWER_V4 = 0x34, //add by PPP,receive SOS

	// {{ [IDL-1521]: merlin.wang add data processing logic of TOF.begin
	RTLS_CMD_REPORT_TOF = 0x38, //TOF
	// }} [IDL-1521]: merlin.wang add data processing logic of TOF.end

	/*Changed for new handshake logic, the logic have synced with anchor ,start */
	// {{ [IDL-1038]:Add command “rtls start” handshake mechanism.begin
	// RTLS_CMD_REP_START_STOP = 0x36, //rtls start/stop response from anchor.
	// }} [IDL-1038]:Add command “rtls start” handshake mechanism.end
	RTLS_CMD_REP_CMD = 0x36, // add for cle and anchor handshake
	/*Changed for new handshake logic, the logic have synced with anchor ,end */

	RTLS_CMD_REP_TEMP_HUM = 0x35, // for device tag report temperature and humidity

	// LE -> Anchor
	RTLS_COMM_TEST_START_REQ = 0x53,
	RTLS_COMM_TEST_RESULT_REQ = 0x54,
	RTLS_RANGE_MEAS_REQ = 0x55,
	RTLS_INIT_REQ = 0x56,
	RTLS_START_REQ = 0x57,
	RTLS_POWER_TEST_START_REQ = 0x58,
	RTLS_RESET_REQ = 0x59,
	RTLS_SINGLE_TWR_MODE_REQ = 0x5A,
	RTLS_ASYMM_TWR_MODE_REQ = 0x5B,

	RTLS_CFG_IND = 0x80,
	RTLS_COMM_TEST_DONE_IND = 0x81,
	RTLS_COMM_TEST_RESULT_IND = 0x82,
	RTLS_RANGE_MEAS_IND = 0x83,
	RTLS_RANGE_MEAS_IND_FINAL = 0x84,
	RTLS_POWER_TEST_DONE_IND = 0x85,

	RTLS_TEMP_VBAT_IND = 0x87,		 // Temperature and VBAT
	RTLS_LOG_ACCUMULATOR_REQ = 0x88, // Request Accumulator for CCP or Blink
	RTLS_LOG_ACCUMULATOR_IND = 0x91, // Accumulator Report for CCP or Blink

	/*Add for OTA, H2604415 start*/
	RTLS_OTA_REQ = 0x5C
	/*Add for OTA, H2604415 end*/

};

/*Add for OTA, H2604415 start*/
#define RTLS_OTA_REQ_LEN sizeof(cmd_ota_t)
/*Add for OTA, H2604415 end*/

#define RTLS_CMD_SET_CFG_CCP_LEN (26)
#define RTLS_POWER_TEST_START_LEN (4)
#define RTLS_COMM_TEST_START_LEN (4)
#define RTLS_COMM_TEST_RESULT_REQ_LEN (1)
#define RTLS_RANGE_MEAS_REQ_LEN (21)
#define RTLS_SINGLE_TWR_MODE_REQ_LEN (17)
#define RTLS_ASYMM_TWR_MODE_REQ_LEN (27)
#define RTLS_START_REQ_LEN (2)
#define RTLS_LOG_ACC_REQ_LEN (4)
#define RTLS_RESET_REQ_LEN (18)

#define RTLS_CMD_REQ_CFG_LEN (64)
#define RTLS_CMD_REPORT_TOA_LEN (16)
#define RTLS_CMD_REPORT_TOA_EX_LEN (17) //minimum length of longer TDoA report, extra byte, specifying extra data length
#define RTLS_CMD_REPORT_TX_CS_LEN (8)
#define RTLS_CMD_REPORT_RX_CS_LEN (16)

/*Changed for new handshake logic, the logic have synced with anchor ,start */
// {{ [IDL-1038]:Add command “rtls start” handshake mechanism.begin
#define RTLS_CMD_REP_START_STOP_LEN (10) //length of rtls start/stop response from anchor.
// }} [IDL-1038]:Add command “rtls start” handshake mechanism.end
#define RTLS_CMD_REP_CMD_LEN (11)
/*Changed for new handshake logic, the logic have synced with anchor, end */

#define RTLS_CMD_REP_TEMP_HUM_LEN (21)

#define RTLS_CMD_REPORT_TOA_V4_LEN (18)
#define RTLS_CMD_REPORT_TX_CS_V4_LEN (8)
#define RTLS_CMD_REPORT_RX_CS_V4_LEN (18)

// Merlin.wang modify length(24) of C6 package.
#ifdef C6_SUPPORT_RSSI
#define RTLS_CMD_REPORT_TOA_RSSI_LEN (4)
#else
#define RTLS_CMD_REPORT_TOA_RSSI_LEN (0)
#endif
#ifdef C6_SUPPORT_FP_POWER
#define RTLS_CMD_REPORT_TOA_FPP_LEN (4)
#else
#define RTLS_CMD_REPORT_TOA_FPP_LEN (0)
#endif
#ifdef C6_SUPPORT_NOISE
#define RTLS_CMD_REPORT_TOA_NOISE_LEN (2)
#else
#define RTLS_CMD_REPORT_TOA_NOISE_LEN (0)
#endif //  C6_SUPPORT_NOISE
#ifdef C6_SUPPORT_FP_AMPL2
#define RTLS_CMD_REPORT_TOA_FP_AMPL2_LEN (2)
#else
#define RTLS_CMD_REPORT_TOA_FP_AMPL2_LEN (0)
#endif																																												 //  C6_SUPPORT_FP_AMPL2
#define RTLS_CMD_REPORT_TOA_SOS_LOWPOWER_V4_LEN (20 + RTLS_CMD_REPORT_TOA_RSSI_LEN + RTLS_CMD_REPORT_TOA_FPP_LEN + RTLS_CMD_REPORT_TOA_NOISE_LEN + RTLS_CMD_REPORT_TOA_FP_AMPL2_LEN) //add by PPP for SOS

// {{ [IDL-1521]: merlin.wang add data processing logic of TOF.begin
#define RTLS_CMD_REPORT_TOF_LEN (27)
// }} [IDL-1521]: merlin.wang add data processing logic of TOF.end

#define RTLS_CMD_DEBUG_LEN (3)

#define RTLS_POWER_TEST_DONE_IND_LEN (1)
#define RTLS_COMM_TEST_DONE_IND_LEN (1)
#define RTLS_COMM_TEST_RESULT_IND_LEN (5)
#define RTLS_RANGE_MEAS_IND_LEN (14)
#define RTLS_TEMP_VBAT_IND_LEN (12)

#define RTLS_ACC_LOG_HEADER_LEN (15)			  //1 byte (MSG ID) + 1 byte frame type (CCP or Blink) + 1 byte seq Num + 4 bytes log Num + 8 bytes of source address (CCP Master or Blink Tag)
#define RTLS_ACC_LOG_REG_LEN (16)				  //fpIndex, maxNoise, firstPathAmp1, stdNoise, firstPathAmp2, firstPathAmp3, maxGrowthCIR, rxPreamCount
#define RTLS_DW_ACCUMULATOR_LEN_16 (992 * 4 + 1)  //16M PRF is 992*4+1
#define RTLS_DW_ACCUMULATOR_LEN_64 (1016 * 4 + 1) //64M PRF is 1016*4+1

#define RTLS_LOG_ACCUMULATOR_IND_LEN_16 (RTLS_ACC_LOG_HEADER_LEN + RTLS_ACC_LOG_REG_LEN + RTLS_DW_ACCUMULATOR_LEN_16)
#define RTLS_LOG_ACCUMULATOR_IND_LEN_64 (RTLS_ACC_LOG_HEADER_LEN + RTLS_ACC_LOG_REG_LEN + RTLS_DW_ACCUMULATOR_LEN_64)
#define RTLS_LOG_ACCUMULATOR_IND_LEN ((RTLS_LOG_ACCUMULATOR_IND_LEN_16 < RTLS_LOG_ACCUMULATOR_IND_LEN_64) ? RTLS_LOG_ACCUMULATOR_IND_LEN_16 : RTLS_LOG_ACCUMULATOR_IND_LEN_64) //minimumn of 2

#define ACCUM_TYPE_BLINK 0xA1 //accumulator reading is only used to read blinks and CCPs
#define ACCUM_TYPE_CCP 0xA2	  //accumulator reading is only used to read blinks and CCPs

#define DWT_DIAGNOSTIC_LOG_REV_5 (5)
#define DWT_DIAGNOSTIC_LOG_V_5 (5)
#define DWT_SIZEOFDIAGNOSTICDATA_5 (66)
#define DWT_SIZE_OF_IMUDATA (30)

#define DWT_LOG_NUM_SIZE (4)

/* The data is framed as follows :
*<STX><LENlsb><LENmsb><DATA : <FC><XX>.....><CRClsb><CRCmsb><ETX>
* STX = 0x2
* LEN is the length of data message(16 bits)
* CRC is the 16 - bit CRC of the data bytes
* ETX = 0x3
* FC = is the function code(API code)
*
*/
#define FRAME_HEADER_LEN (6)
#define FRAME_START_IDX (0)
#define FRAME_LENGTH_IDX (1)
#define FRAME_DATA_IDX (3)

enum RTLS_DATA
{
	RTLS_DATA_ANCHOR_REQ = 0x41,

	RTLS_DATA_ANCHOR = 0x61,
	RTLS_DATA_BLINK = 0x62,
	RTLS_DATA_STATS = 0x63,
	RTLS_DATA_IMU = 0x64,
	RTLS_DATA_BLINK_EXT = 0x65,
	RTLS_DATA_TEMP_HUM = 0X66,
	RTLS_DATA_BLINK_SOS = 0xc8,
	RTLS_DATA_BLINK_EXT_DEBUG = 0xc9,
};

//subtype: 0 - statics / 1 - SOS / 2 - LowPower / 3 - SOS + LowPower / 4 - move
enum
{
	BLINK_STATUS_STATIC = 0,
	BLINK_STATUS_STATIC_SOS = 1,
	BLINK_STATUS_STATIC_LV = 2,
	BLINK_STATUS_STATIC_SOS_LV = 3,
	BLINK_STATUS_MOVING = 4,
	BLINK_STATUS_MOVING_SOS = 5,
	BLINK_STATUS_MOVING_LV = 6,
	BLINK_STATUS_MOVING_SOS_LV = 7,
};

/* Network commands
* use byte access if in doubt
*/

#define UN16(x)         \
	union               \
	{                   \
		uint16_t x##16; \
		uint8_t x[2];   \
	}
#define UN32(x)         \
	union               \
	{                   \
		uint32_t x##32; \
		uint8_t x[4];   \
	}

/* Wire format of messages from Anchor to CLE */
#pragma pack(push, 1)

/*Add for OTA, H2604415 start*/
/*RTLS_OTA_REQ*/
typedef struct
{
	uint8_t command;  //0
	uint32_t tftp_ip; //1-4
	char fileName[24];
} cmd_ota_t;
/*Add for OTA, H2604415 end*/

/* RTLS_CMD_REPORT_TOA_V4 */
typedef struct
{
	uint8_t type;				   //0		: type = TOA report
	uint8_t seqNum;				   //1		: Blink message sequence number
	uint8_t tagID[ADDR_BYTE_SIZE]; //2-9	: tag ID
	uint8_t rawTOA[TS_40B_SIZE];   //10-14	: raw timestamp
	UN16(firstPath);			   //15-16	: raw Firstpath
	uint8_t extLen;				   //17	: length of ext below
	union
	{
		uint8_t ext[DWT_SIZE_OF_IMUDATA + sizeof(uint32_t) + DWT_SIZEOFDIAGNOSTICDATA_5];
		struct
		{
			uint8_t dataIMU[DWT_SIZE_OF_IMUDATA]; // : IMU data sent from the tag(2 (ench + exth) + 1 (dwh)+27 (dwp))
			UN32(logNum);
			uint8_t diagnostics[DWT_SIZEOFDIAGNOSTICDATA_5];
		};
	};
} report_toa_imu_v4_t;

/* RTLS_CMD_REPORT_TOA_V4 */
typedef struct
{
	uint8_t type;				   //0		: type = TOA report
	uint8_t seqNum;				   //1		: Blink message sequence number
	uint8_t tagID[ADDR_BYTE_SIZE]; //2-9	: tag ID
	uint8_t rawTOA[TS_40B_SIZE];   //10-14	: raw timestamp
	UN16(firstPath);			   //15-16	: raw Firstpath
	uint8_t extLen;				   //17	: length of ext below
	union
	{
		uint8_t ext[sizeof(uint32_t) + DWT_SIZEOFDIAGNOSTICDATA_5];
		struct
		{
			UN32(logNum);
			uint8_t diagnostics[DWT_SIZEOFDIAGNOSTICDATA_5];
		};
	};
} report_toa_v4_t;

/*
  *add by ppp for SOS
  *RTLS_CMD_REPORT_TOA_SOS_LOWPOWER_V4
*/
typedef struct
{
	uint8_t type;	 //0     : type = TOA report
	uint8_t subType; //1 : 0-C5/1-SOS/2-LowPower/3-SOS+LowPower/4-move
	uint8_t msg;
	uint8_t seqNum;				   //3     : Blink message sequence number
	uint8_t tagID[ADDR_BYTE_SIZE]; //4-11   : tag ID
	uint8_t rawTOA[TS_40B_SIZE];   //12-16 : raw timestamp
	UN16(firstPath);			   //17-18 : raw Firstpath
#ifdef C6_SUPPORT_FP_POWER
	float firstPathPower;
#endif
	// {{ [IDL-623]:Add rssi vaule for C6.begin
#ifdef C6_SUPPORT_RSSI
	float rssi; //19-22 : rssi value of anchor received.
#endif
	// }} [IDL-623]:Add rssi vaule for C6.end

	//{{ [IDL-1208]:Add STD_NOISE and FP_AMPL2 vaule for C6.begin
#ifdef C6_SUPPORT_NOISE
	UN16(stdNoise); //23-24
#endif				// C6_SUPPORT_NOISE
#ifdef C6_SUPPORT_FP_AMPL2
	UN16(FP_AMPL2); //25-26
#endif				// C6_SUPPORT_ \
	//{{ [IDL-1208]:Add STD_NOISE and FP_AMPL2 vaule for C6.end

	uint8_t extLen; //27    : length of ext below
	union
	{
		uint8_t ext[sizeof(uint32_t) + DWT_SIZEOFDIAGNOSTICDATA_5];
		struct
		{
			UN32(logNum);
			uint8_t diagnostics[DWT_SIZEOFDIAGNOSTICDATA_5];
		};
	};
} report_toa_sos_lowpower_v4_t;

/* RTLS_CMD_REPORT_RX_CS_V4 */
typedef struct
{
	uint8_t type;					  //0		: type = CCP RX timestamp
	uint8_t seqNum;					  //1		: CCP Seq Num
	uint8_t masterID[ADDR_BYTE_SIZE]; //2-9	: master anchor ID (that sent the CCP frame)
	uint8_t csRxTime[TS_40B_SIZE];	  //10-14	: CCP Rx time
	UN16(firstPath);				  //15-16	: raw Firstpath
	uint8_t extLen;					  //17	: length of ext below
	union
	{
		uint8_t ext[sizeof(uint32_t) + DWT_SIZEOFDIAGNOSTICDATA_5];
		struct
		{
			UN32(logNum);
			uint8_t diagnostics[DWT_SIZEOFDIAGNOSTICDATA_5];
		};
	};
} report_ccp_rx_v4_t;

/* RTLS_CMD_REPORT_TX_CS_V4 */
typedef struct
{
	uint8_t type;				   //0			: type = CCP TX timestamp
	uint8_t seqNum;				   //1			: CCP Seq Num
	uint8_t csTxTime[TS_40B_SIZE]; //2-6		: CCP Tx time
	uint8_t extLen;				   //7 		: length of ext below
	union
	{
		uint8_t ext[sizeof(uint32_t)];
		UN32(logNum);
	};
} report_ccp_tx_v4_t;

/* RTLS_TEMP_VBAT_IND */
typedef struct
{
	uint8_t type;	   //0				: type = Battery abnd Voltage Level report
	uint8_t bat;	   //1				: battery voltage value
	uint8_t temp;	   //2				: temperature value
	UN16(atemp);	   //3-4			: ambient Sensor temperature value
	uint8_t apress[3]; //5-7			: ambient Sensor pressure value
	UN32(logNum);	   //8-11			: log number
} report_temp_vbat_t;

/* to be structured:
RTLS_COMM_TEST_RESULT_REQ
RTLS_START_REQ
RTLS_LOG_ACCUMULATOR_REQ
RTLS_RESET_REQ
*/

/* RTLS_CMD_SET_CFG_CCP */
typedef struct
{
	uint8_t command;			//0
	uint8_t id;					//1
	uint8_t master;				//2
	uint8_t prf_ch;				//3
	uint8_t datarate;			//4
	uint8_t code;				//5
	uint8_t txPreambLength;		//6
	uint8_t nsSFD_rxPAC;		//7
	UN16(delay_rx);				//8-9
	UN16(delay_tx);				//10-11
	uint8_t free_12;			//12
	uint8_t debug_logs;			//13
	uint8_t eui64_to_follow[8]; //14-21
	UN32(lag_delay);			//22-25
} cmd_config_t;

/* RTLS_SINGLE_TWR_MODE_REQ */
typedef struct
{
	uint8_t command;	   //0 	: RTLS_SINGLE_TWR_MODE_REQ
	uint8_t role;		   //1 	: initiator==1 responder==0
	uint8_t use_ant_delay; //2
	UN16(delay_tx);		   //3-4
	UN16(delay_rx);		   //5-6
	UN16(response_delay);  //7-8
	UN16(addr);			   //9-10
	uint8_t lna_on;		   //11
	UN32(power);		   //12-15
	uint8_t log_all;	   //16
} cmd_twr_single_t;

/* RTLS_RANGE_MEAS_REQ */
typedef struct
{
	uint8_t command;	   //0  	: RTLS_RANGE_MEAS_REQ
	uint8_t log_all;	   //1
	uint8_t role;		   //2
	UN16(num_ranges);	   //3-4
	uint8_t use_ant_delay; //5
	UN16(delay_tx);		   //6-7
	UN16(delay_rx);		   //8-9
	UN16(response_delay);  //10-11
	UN16(initiator_addr);  //12-13
	UN16(responder_addr);  //14-15
	uint8_t lna_on;		   //16
	UN32(power);		   //17-20
} cmd_twr_t;

/* RTLS_RANGE_MEAS_REQ */
typedef struct
{
	uint8_t command;	   //0  	: RTLS_RANGE_MEAS_REQ
	uint8_t log_all;	   //1
	uint8_t role;		   //2
	UN16(num_ranges);	   //3-4
	uint8_t use_ant_delay; //5
	UN16(delay_tx);		   //6-7
	UN16(delay_rx);		   //8-9
	UN16(response_delay);  //10-11
	UN16(final_delay);	   //12-13
	UN16(report_delay);	   //14-15
	UN16(poll_period);	   //16-17
	UN16(initiator_addr);  //18-19
	UN16(responder_addr);  //20-21
	uint8_t lna_on;		   //22
	UN32(power);		   //23-26
} cmd_twr_asymm_t;

/* RTLS_POWER_TEST_START_REQ */
typedef struct
{
	uint8_t command; //0
	uint8_t start;	 //1
	UN16(duration);	 //2-3
} cmd_power_test_t;

/*RTLS_CMD_REP_CMD = 0x36
*cmd_handshank_t->responsecmd = 0x57, it is rtls start or stop, value = 1 = start, value = 0 = stop
*cmd_handshank_t->responsecmd = 0x59, it is anchor reset, value = 1 = reset success.
*/
/*RTLS_CMD_REP_CMD*/
typedef struct
{
	uint8_t head;				 //0
	uint8_t responsecmd;		 //1
	uint8_t uid[ADDR_BYTE_SIZE]; //2 - 9
	uint8_t value;				 //10
} cmd_handshank_t;

/* RTLS_COMM_TEST_START_REQ */
typedef struct
{
	uint8_t command;	 //0
	uint8_t transmit[1]; //1
	UN16(data);			 //2-3
} cmd_comm_test_t;

// {{ [IDL-1103]: Anchor parameters expand.begin
typedef struct
{
	uint8_t follow_master_id[8];
	UN16(channelDelay);
} follow_anc_list_t;

/* RTLS_CMD_SET_CFG_EXT_CCP */
typedef struct
{
	uint8_t command;			//0
	uint8_t id;					//1
	uint8_t master;				//2
	uint8_t prf_ch;				//3
	uint8_t datarate;			//4
	uint8_t code;				//5
	uint8_t txPreambLength;		//6
	uint8_t nsSFD_rxPAC;		//7
	UN16(delay_rx);				//8-9
	UN16(delay_tx);				//10-11
	uint8_t free_12;			//12
	uint8_t debug_logs;			//13
	uint8_t eui64_to_follow[8]; //14-21
	UN32(lag_delay);			//22-25
	UN32(ccp_tx_cycle);			//26-29
	uint8_t anchorNum;			//30
	follow_anc_list_t follow_anc_list[16];
} cmd_config_ext_t;
// }} [IDL-1103]: Anchor parameters expand.end

using report_temp_hum = struct
{
	uint8_t type;				   //0     : type = TOA report
	uint8_t subType;			   //1     :   0-C5/1-SOS/2-LowPower/3-SOS+LowPower/4-move
	uint8_t seqNum;				   //2     : sequence number
	uint8_t tagID[ADDR_BYTE_SIZE]; //3-10  : tag ID
	uint8_t rawTOA[TS_40B_SIZE];   //11-15 : raw timestamp
	uint16_t temp;				   //16-17 : temperature
	uint16_t hum;				   //18-19 : humidity
	uint8_t extLen;				   //20    : length of ext below
	union
	{
		uint8_t ext[sizeof(uint32_t) + DWT_SIZEOFDIAGNOSTICDATA_5];
		struct
		{
			UN32(logNum);
			uint8_t diagnostics[DWT_SIZEOFDIAGNOSTICDATA_5];
		};
	};
};

// {{ [IDL-1521]: merlin.wang add data processing logic of TOF.begin
typedef struct
{
	uint8_t type;	  //0: type = TOF report
	uint8_t subType;  //1: 0-statics/1-SOS/2-LowPower/3-SOS+LowPower/4-move
	uint8_t seqNum;	  //2: Blink message sequence number
	uint8_t tagID[8]; //3-10: tag ID
					  //float distance;                 //11-14 : distance
	UN32(tofdistance);

	float firstPathPower; //15-18: FPP
	float rssi;			  //19-22 : rssi value of anchor received.
	UN16(stdNoise);		  //23-24
	UN16(FP_AMPL2);		  //25-26

	uint8_t extLen; //27    : length of ext below
	union
	{
		uint8_t ext[sizeof(uint32_t) + DWT_SIZEOFDIAGNOSTICDATA_5];
		struct
		{
			UN32(logNum);
			uint8_t diagnostics[DWT_SIZEOFDIAGNOSTICDATA_5];
		};
	};
} report_tof_t;
// }} [IDL-1521]: merlin.wang add data processing logic of TOF.end

#pragma pack(pop)

#undef UN16
#undef UN32

#endif //__RTLS_INTERFACE_H
