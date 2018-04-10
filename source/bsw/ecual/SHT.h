/****************************************************************************************************/
/**
\file       SHT.h
\brief      ECU Abstraction level - Sensirion Sensor Abstraction
\author     Juan Pablo Garcia
\version    1.0
\project    Catalyst Controller
\date       8/March/2018
*/
/****************************************************************************************************/
#ifndef SHT_H
#define SHT_H
/********************************************************************************
 *                               Include Files
 ********************************************************************************/

/********************************************************************************
*                               Macro Definitions
********************************************************************************/
/*Coefficients given by table 9 from datasheet.*/
/*Temperature*/
#define SHT_SO_TEMP_D1      (-40.1f)		/*Using a Vdd = 5V*/
#define SHT_SO_TEMP_D2      (0.04f) 		/*Using the 12bit configuration*/

/*Relative Humidity*/
#define SHT_SO_HR_C1          (2.0468f)	   /*Using the 8bit configuration*/
#define SHT_SO_HR_C2          (0.5872f)	   /*Using the 8bit configuration*/
#define SHT_SO_HR_C3          (4.0845E-4)  /*Using the 8bit configuration*/
#define SHT_SO_HR_T1          (0.01f)      /*Using the 8bit configuration*/
#define SHT_SO_HR_T2          (0.00128f)   /*Using the 8bit configuration*/

/*Dew Point*/
#define SHT_DP_TN_ABV_WATER             (243.12f) /*Temperature range 0 - 50 °C*/
#define SHT_DP_M_ABV_WATER              (17.62f)  /*Temperature range 0 - 50 °C*/
#define SHT_DP_TN_ABV_ICE               (272.62f) /*Temperature range -40 - 0 °C*/
#define SHT_DP_M_ABV_ICE                (22.46f)  /*Temperature range -40 - 0 °C*/

/*Status register Bits*/
#define SHT_STS_HTR_ON 	(0x1u << 2) /*Set heater ON*/
#define SHT_STS_HTR_OFF (0x0u << 2) /*Set heater OFF*/
#define SHT_STS_RES_MIN (0x1u << 0) /*Set min resolution: 8bit RH / 12bit Temp*/
#define SHT_STS_RES_MAX (0x0u << 0) /*Set max resolution: 12bit RH / 14bit Temp*/

/*SHT Spi Commands*/
#define SHT_CMD_MEASURE_TEMP	(0x03u)
#define SHT_CMD_MEASURE_RH		(0x05u)
#define SHT_CMD_STS_WRITE		(0x06u)
#define SHT_CMD_STS_READ		(0x07u)
#define SHT_SFT_RST 			(0x1Eu)	
 
#define noACK 0 
#define ACK   1 
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    
/********************************************************************************
*                               Type Definitions
********************************************************************************/

/********************************************************************************
 *                               Global Variable Definitions
 ********************************************************************************/

/********************************************************************************
*                               Function Declarations
********************************************************************************/
void SHT_Init (void);
Std_ReturnType SHT_CalculateDP (void);
Std_ReturnType SHT_ObtainTemp (void);
Std_ReturnType SHT_ObtainRH (void);

/*Stubbs*/
SHT_TaskStateType SHT_ReadTemperatureRaw(uint16_t *Data );
SHT_TaskStateType SHT_ReadHumidityRaw(uint16_t *Data);
#endif /* SHT_H */

