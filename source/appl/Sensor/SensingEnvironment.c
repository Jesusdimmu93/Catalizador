/****************************************************************************************************/
/**
\file       SensingEnvironment.c
\brief      Application level - Sensing Relative Humidity and 
*                               Temperature of catalyst environment
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
#include "SensingEnvironment.h"
#include "SHT_Types.h"
#include "SHT.h"
//#include "Can.h"
//#include "SD_Mem.h"



 /*******************************************************************************
 *                               Macro Definitions
 ********************************************************************************/
typedef enum 
{
	SM_SENSENV_INIT = 0,
	SM_SENSENV_OBTAINING_TEMP,
	SM_SENSENV_OBTAINING_RH,
	SM_SENSENV_CALCULATING_DP,
	SM_SENSENV_SPREAD_RESULT

} SM_SensEnv_Type;

 /*******************************************************************************
 *                               Global Variable Definitions
 ********************************************************************************/

SM_SensEnv_Type SensingState;

uint16_t RealTemp;
uint16_t RealRH;

/********************************************************************************
*                               Static Function Declarations
********************************************************************************/
Std_ReturnType get_Temp (void);
Std_ReturnType get_RH (void);
Std_ReturnType Calculate_DP (void);
Std_ReturnType Send_Results (void);
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
void SensEnv_Init (void)
{
	SensingState = SM_SENSENV_INIT;
}

void Process_Sensing_Env (void)
{
	switch (SensingState)
	{
		case SM_SENSENV_INIT:
			if(SHT_Inited==SHT_Init())
      {
        SensingState = SM_SENSENV_OBTAINING_TEMP;
      }
      else
      {
        /*Nothing to do*/
      }
      
		break;
		case SM_SENSENV_OBTAINING_TEMP:
			if(E_OK == get_Temp())
			{
				SensingState = SM_SENSENV_OBTAINING_TEMP;//SM_SENSENV_OBTAINING_RH;
			}
		break;
		case SM_SENSENV_OBTAINING_RH:
			if(E_OK == get_RH())
			{
				SensingState = SM_SENSENV_CALCULATING_DP;
			}
		break;
		case SM_SENSENV_CALCULATING_DP:
			if(E_OK == Calculate_DP())
			{
				SensingState = SM_SENSENV_SPREAD_RESULT;
			}
      else
      {
        SensingState = SM_SENSENV_OBTAINING_TEMP;
      }
		break;
		case SM_SENSENV_SPREAD_RESULT:
			if(E_OK == Send_Results())
			{
				SensingState = SM_SENSENV_OBTAINING_TEMP;
			}
      else
      {
        SensingState = SM_SENSENV_INIT;
      }  
		break;
		default:
			SensingState = SM_SENSENV_INIT;
		break;
	}
}


Std_ReturnType get_Temp (void)
{
	Std_ReturnType status = E_NOT_OK;
	if(E_OK == SHT_ObtainTemp())
	{
		status = E_OK;
	}

	return status;
}

Std_ReturnType get_RH (void)
{
	Std_ReturnType status = E_NOT_OK;
	if(E_OK == SHT_ObtainRH())
	{
		status = E_OK;
	}
	return status;
}

Std_ReturnType Calculate_DP (void)
{
	Std_ReturnType status = E_NOT_OK;
	if (E_OK == SHT_CalculateDP())
	{
		status = E_OK;
	}
	return status;
}

Std_ReturnType Send_Results (void)
{
	Std_ReturnType status = E_NOT_OK;
	//Can_SetSignal(SensorData, );
	return E_OK;
}
