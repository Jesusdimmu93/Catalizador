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
#include "board.h"
#include "usart.h"

 /*******************************************************************************
 *                               Macro Definitions
 ********************************************************************************/

 /*******************************************************************************
 *                               Global Variable Definitions
 ********************************************************************************/
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
void SHT_Init (void)
{
	/*Initializing Variables*/
	SHTData.RH = 0.0;
	SHTData.Temp = 0.0;
	SHTData.DwPoint = 0.0;

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

}

Std_ReturnType SHT_CalculateDP (void)
{

}

/*Stubbs*/
SHT_TaskStateType SHT_GetTemperatureRaw(uint16_t *Data)
{
	SHT_TaskStateType status = SHT_TASK_OK;
	return  status;
}