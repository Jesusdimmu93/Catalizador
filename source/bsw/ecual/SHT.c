/****************************************************************************************************/
/**
\file       SHT.c
\brief      ECU Abstraction level - Sensirion Sensor Abstraction
\author     Juan Pablo Garcia
\version    1.0
\project    Catalyst Controller
\date       8/March/2018
*/
/****************************************************************************************************/

/********************************************************************************
 *                               Include Files
 ********************************************************************************/
#include "Std_Types.h"
#include "SHT_Types.h"
#include "SHT.h"
#include "twihs.h"
#include "board.h"
#include "usart.h"
#include "pmc.h"
#include "sysclk.h"

 /*******************************************************************************
 *                               Macro Definitions
 ********************************************************************************/

#define TWIHS_CLK     400000

 /*******************************************************************************
 *                               Global Variable Definitions
 ********************************************************************************/
  Twihs p_twihs;
  twihs_options_t p_opt;
  twihs_packet_t packet_tx, packet_rx;
  SHTDataType SHTData;
/********************************************************************************
*                               Static Function Declarations
********************************************************************************/

/********************************************************************************
 *                               Global and Static Function Definitions
 ********************************************************************************/

 /*****************************************************************************
 *
 * Function:        
 *
 * Description:     
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
SHT_InitState SHT_Init (void)
{
  SHT_InitState InitStatusReturn = TWIHS_INVALID_ARGUMENT;
	/*Initializing Variables*/
	SHTData.RH = 0.0;
	SHTData.Temp = 0.0;
	SHTData.DwPoint = 0.0;
  
  PMC_EnablePeripheral(ID_TWIHS0);
  p_opt.master_clk = sysclk_get_peripheral_hz();
	p_opt.speed      = TWIHS_CLK;
  InitStatusReturn = twihs_master_init(ID_TWIHS0, &p_opt);
  return InitStatusReturn;
}

Std_ReturnType SHT_ObtainTemp (void)
{
	uint16_t Temp_SO_T;
	Std_ReturnType status = E_NOT_OK;
	if(SHT_TASK_OK == SHT_GetTemperatureRaw(&Temp_SO_T))
	{
		/*Given the Temp raw value we can now calculate the real temperature *
		* the temperature sensor is very linear by design, according to the  *
		* datasheet the formula is T = d1 + (d2 * SOt), where d1 and d2 are  *
		* coefficients and SOt the digital readout                          */
		SHTData.Temp = (SHT_TEMP_D1 + (SHT_TEMP_D2 * Temp_SO_T));
		status = E_OK;
	}
	return status;
}

Std_ReturnType SHT_ObtainRH (void)
{
  uint16_t RH_SO_T;
	Std_ReturnType status = E_NOT_OK;
	if(SHT_TASK_OK == SHT_GetHumidityRaw(&RH_SO_T))
	{
		/*Given the Temp raw value we can now calculate the real temperature *
		* the temperature sensor is very linear by design, according to the  *
		* datasheet the formula is T = d1 + (d2 * SOt), where d1 and d2 are  *
		* coefficients and SOt the digital readout                          */
		SHTData.RH = 0u;//(SHT_RH_D1 + (SHT_RH_D2 * RH_SO_T));
		status = E_OK;
	}
	return status;
}

Std_ReturnType SHT_CalculateDP (void)
{

}

/*Stubbs*/
SHT_TaskStateType SHT_GetTemperatureRaw(uint16_t *Data)
{
	SHT_TaskStateType status = SHT_TASK_OK;
  
  packet_tx.chip        = 0u;//AT24C_ADDRESS;
	packet_tx.addr[0]     = 0u;//EEPROM_MEM_ADDR >> 8;
	packet_tx.addr[1]     = 0u;//EEPROM_MEM_ADDR;
	packet_tx.addr_length = 0u;//EEPROM_MEM_ADDR_LENGTH;
	packet_tx.buffer      = 0u;//(uint8_t *) test_data_tx;
	packet_tx.length      = 0u;//TEST_DATA_LENGTH;

	/* Configure the data packet to be received */
	packet_rx.chip        = 0u;//packet_tx.chip;
	packet_rx.addr[0]     = 0u;//packet_tx.addr[0];
	packet_rx.addr[1]     = 0u;//packet_tx.addr[1];
	packet_rx.addr_length = 0u;//packet_tx.addr_length;
	packet_rx.buffer      = 0u;//gs_uc_test_data_rx;
	packet_rx.length      = 0u;//packet_tx.length;

  twihs_master_write(ID_TWIHS0, &packet_tx);
  
  twihs_master_read(ID_TWIHS0, &packet_rx);
	return  status;
}

SHT_TaskStateType SHT_GetHumidityRaw(uint16_t *Data)
{
	SHT_TaskStateType status = SHT_TASK_OK;
	return  status;
}