/****************************************************************************************************/
/**
\file       SHT.c
\brief      ECU Abstraction level - Sensirion Sensor Abstraction
\author     Jesus I�iguez
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
#include "math.h"
#include "pmc.h"
#include "sysclk.h"

//#define InterruptMode
 /*******************************************************************************
 *                               Macro Definitions
 ********************************************************************************/
static const Pin pinsSHTs[] = {PIN_TWI_TWD0, PIN_TWI_TWCK0};
#define TWIHS_CLK     100000

#define noACK 0
#define ACK 1
//adr command r/w
#define STATUS_REG_W  0x06 //000 0011 0
#define STATUS_REG_R  0x07 //000 0011 1
#define MEASURE_TEMP  0x03 //000 0001 1
#define MEASURE_HUMI  0x05 //000 0010 1
#define RESET         0x1e //000 1111 0
#define SDA           0x00
#define SCK           0x01 
////////////////////////////////////////////////////////////////////////////// 
// Command byte values        adr cmd  r/w 
#define Reset         0x1e   // 000 1111  0      
#define MeasureTemp   0x03   // 000 0001  1      
#define MeasureHumi   0x05   // 000 0010  1 
#define WrStatusReg   0x06   // 000 0011  0 
#define RdStatusReg   0x07   // 000 0011  1 

/*******************************************************************************
 *                               Prototypes Definitions
 ********************************************************************************/
  uint8_t  s_write_byte(uint8_t value);
  uint16_t ReadValueFn();
  uint16_t ReadHumidity();
  uint16_t ReadTemperature();
  uint8_t  SwapData(uint8_t data);
  
static SHT_TaskStateType SHT_ReadTemperatureRaw(uint16_t *pData );
static SHT_TaskStateType SHT_ReadHumidityRaw(uint16_t *pData);
static float log_n(float x);
static float power( float dwX, uint32_t dwY );
/*******************************************************************************
 *                               Global Variable Definitions
 ********************************************************************************/
 
  SHTDataType SHTData;
  uint8_t     TimeOut,err; 
  uint8_t     AckConfirmVar=0u;
  uint8_t     SwitchInterrupt=0;
#ifdef InterruptMode
  uint8_t   RequestEnabled;
  uint16_t  MeasuredValues[2];//0 Temp 1 Humidity 
  uint8_t   InterruptVar=0;
#endif
/********************************************************************************
*                               Static Function Declarations
********************************************************************************/

uint32_t ComPins_Configure( uint32_t dwCom )
{
	return ( PIO_Configure( &pinsSHTs[dwCom], 1 ) );
}

void PIN_Set( uint32_t dwCom )
{   
  if(dwCom)
  {
    PIOA->PIO_WPMR &= 0x00;
    PIOA->PIO_PER   = 0x10;
    PIOA->PIO_OER  |= 0x10;
    PIOA->PIO_CODR |= 0x10; 
    PIOA->PIO_OWER |= 0x10;
  }
  else
  {
    PIOA->PIO_WPMR &= 0x00;
    PIOA->PIO_PER   = 0x08;
    PIOA->PIO_OER  |= 0x08;
    PIOA->PIO_CODR |= 0x08; 
    PIOA->PIO_OWER |= 0x08;
  } 
}


void PIN_Clear( uint32_t dwCom )
{
  if(dwCom)
  {
    PIOA->PIO_WPMR &= 0x00;
    PIOA->PIO_PER   = 0x10;
    PIOA->PIO_OER  |= 0x10;
    PIOA->PIO_SODR |= 0x10; 
    PIOA->PIO_OWER |= 0x10;
  }
  else
  {
    PIOA->PIO_WPMR &= 0x00;
    PIOA->PIO_PER   = 0x08;
    PIOA->PIO_OER  |= 0x08;
    PIOA->PIO_SODR |= 0x08; 
    PIOA->PIO_OWER |= 0x08;
  }
}

void PIN_MakeInputSDA( )
{   
    PIOA->PIO_WPMR&= 0x00;
    PIOA->PIO_OWDR|= 0x08;
    PIOA->PIO_ODR |= 0x08;
    PIOA->PIO_IDR |= ~0x08;     
    PIOA->PIO_ISR ;
    PIOA->PIO_IER |= 0x08;      
    PIOA->PIO_ESR |= 0x08;      
                                
    PIOA->PIO_FELLSR  |= 0x08;  
    PIOA->PIO_IFER    |= 0x08;
    PIOA->PIO_IFSCER  |= 0x04;   
}

void PIN_MakeOutputSDA( )
{        
    PIOA->PIO_WPMR &= 0; 
    //enable IT
    PIOA->PIO_IDR  |= 0x08;     
    //PIOA->PIO_PDR = 0x08;
    //PIOA->PIO_PER = 0x08;
    PIOA->PIO_IFDR |= 0x08; 
    //PIOA->PIO_DIFSR |= 0x08;  
    //PIOA->PIO_SCDR |= 0x04
    PIOA->PIO_OER  |= 0x08;
    PIOA->PIO_OWER |= 0x08;
    
}           

void PIOA_Handler()
{  
  NVIC_DisableIRQ( PIOA_IRQn ) ; 
  AckConfirmVar=1;
  PIOA->PIO_IDR = 0x18; 
  LED_Toggle(1);
#ifdef InterruptMode 
 
  if(1u==InterruptVar)
  {
    if(MeasureTemp == RequestEnabled)
    { 
      RequestEnabled = 0xff;               
      MeasuredValues[0]= ReadValueFn(); 
      
    }                       
    else if(MeasureHumi == RequestEnabled)
    { 
      RequestEnabled = 0xff;                             
      MeasuredValues[1]= ReadValueFn(); 
    }
    InterruptVar=0u;
  }
#endif
  /*PIOA->PIO_IER |= 0x08;     //PIO_IDR
  PIOA->PIO_ESR |= 0x08; //edge PIO_ESR
                        //LSR level
  PIOA->PIO_FELLSR |= 0x08; //falling edge
  PIOA->PIO_IFER |= 0x08;
  PIOA->PIO_IFSCER |= 0x04; 
    */             
  NVIC_EnableIRQ( PIOA_IRQn ) ; 
}

/*****************************************************************************
 *
 * Function:        log_n
 *
 * Description:     Returns the natural logarithm of val
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
static float log_n(float val)
{
    uint8_t n;
    float x, logNat; 
    /*val = (1 + x)*/
    x = (val - 1.0);
    logNat = 0.0;
    if(val > 0 && val <= 1)
    {
        for(n = 0; n < 17; n++)
        {
            logNat += (power((-1),n) * (power(x,(n+1)))/(n+1));
        } 
    }
    return logNat;
}

 /*****************************************************************************
 *
 * Function:        power
 *
 * Description:     Returns the value of dwX powered to the exponential dwY
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
static float power( float dwX, uint32_t dwY )
{
    float dwResult = 1 ;

    while ( dwY > 0 ) {
        dwResult *= dwX ;
        dwY-- ;
    }
    return dwResult ;
}
/********************************************************************************
 *                               Global and Static Function Definitions
 ********************************************************************************/

 /*****************************************************************************
 *
 * Function:        SHT_Init
 *
 * Description:     Initialize the sensor controller
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
  	PMC_EnablePeripheral(ID_PIOA);
  	NVIC_DisableIRQ( PIOA_IRQn ) ;
	NVIC_ClearPendingIRQ( PIOA_IRQn ) ;
	NVIC_SetPriority( PIOA_IRQn, 1 ) ;
  	NVIC_EnableIRQ( PIOA_IRQn ) ;  
}

 /*****************************************************************************
 *
 * Function:        SHT_ObtainTemp
 *
 * Description:     This function calculates the Temperature obtained from the sensor
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
Std_ReturnType SHT_ObtainTemp (void)
{
	uint16_t SO_T;
	Std_ReturnType status = E_NOT_OK;
	if(SHT_TASK_OK == SHT_ReadTemperatureRaw(&SO_T))
	{
		/*Given the Temp raw value we can now calculate the real temperature *
		* the temperature sensor is very linear by design, according to the  *
		* datasheet the formula is T = d1 + (d2 * SOt), where d1 and d2 are  *
		* coefficients and SOt the digital readout                          */
		SHTData.Temp = (SHT_SO_TEMP_D1 + (SHT_SO_TEMP_D2 * SO_T));
    printf("%f\n\r",SHTData.Temp );
    status = E_OK;
	}
	return status;
}

 /*****************************************************************************
 *
 * Function:        SHT_ObtainRH
 *
 * Description:     This function calculates the Relation Humidity from the sensor
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
Std_ReturnType SHT_ObtainRH (void)
{
	uint16_t SO_RH;
	Std_ReturnType status = E_NOT_OK;
	if(SHT_TASK_OK ==SHT_ReadHumidityRaw(&SO_RH))
	{
		float RH_Linear;
		/*Given the RH raw value and having calculated the real temperature  *
		* we can now calculate the real relative humidity. For compensating  *
		* non-linearity of the humidity sensor it is recommended to convert  *
		* the humidity readout (SOrh) wiht the following formula with        *
		* coefficients given by the datasheet:                               *
		*          RH_Linear = c1 + (c2 * SOrh) + (c3 * (SOrh)^2)            *
		* For temperatures significantly different from 25�C (~77�F)         *
		* the humidity signal requires temperature compensation.             *
		* The temperature correction corresponds roughly to                  *
		* 0.12%RH/�C @ 50%RH. Coefficients for the temperature               *
		* compensation are given by the datasheet:                           *
		*          RH = (T[�C] -25) * (t1 + (t2 * SO_RH)) + RH_Linear       */

		
    RH_Linear = (-2.0468 + (SHT_SO_HR_C2 * SO_RH) + 
		            ((-0.0000015955) * SO_RH * SO_RH));

		SHTData.RH = ((SHTData.Temp - 25u) * (SHT_SO_HR_T1 + 
		             (SHT_SO_HR_T2 * SO_RH)) + RH_Linear);
    printf("%f\n\r",SHTData.RH );
		status = E_OK;
	}
	return status;
}

 /*****************************************************************************
 *
 * Function:        SHT_CalculateDP
 *
 * Description:     This function returns the calculated dew point from the sensor
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
Std_ReturnType SHT_CalculateDP (void)
{
  float Tn, m, partial1;
  float  Ln_RH;
  int32_t number, result;
  char ln_result[50];
  /*For dew point (Td) calculations there are various formulas                               *
  * to be applied, most of them quite complicated. For the                                   *
  * temperature range of -40 � 50�C the following                                            *
  * approximation provides good accuracy with parameters given by the                        *
  * datasheet.                                                                               *
  * Td(RH,T) = Tn * (Ln(RH/100%) + (m * T)/(Tn + T) / (m - Ln(RH/100%) - (m * T)/(Tn + T))) */

  if(SHTData.Temp >= 0) /*Above water*/
  {
    Tn = SHT_DP_TN_ABV_WATER;
    m = SHT_DP_M_ABV_WATER;
  }
  else /*Above ice*/
  {
    Tn = SHT_DP_TN_ABV_ICE;
    m = SHT_DP_M_ABV_ICE;
  }

  Ln_RH = log_n(SHTData.RH / 100.0);
  partial1 =(float)((m * SHTData.Temp) / (Tn + SHTData.Temp));

  SHTData.DwPoint = (Tn * ((Ln_RH + partial1) / (m - Ln_RH - partial1)));
    printf("%f\n\r",SHTData.DwPoint);
  return E_OK;
}
 
 /*****************************************************************************
 *
 * Function:        GetData
 *
 * Description:     Returns the value of the structure Data under the parameter 
 *                  as reference
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
void GetData(SHTDataType *Data)
{
	Data = &SHTData;
}

 /*****************************************************************************
 *
 * Function:        SHT_ReadTemperatureRaw
 *
 * Description:     This function make a reading of the raw temperature over 
 *                  the sensor
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
static SHT_TaskStateType SHT_ReadTemperatureRaw(uint16_t *pData)
{  
  uint16_t Data=0;
	SHT_TaskStateType status = SHT_TASK_NOT_OK;
  if(0u==SwitchInterrupt)
  {
    Data=ReadTemperature();
    if((Data&0x8000)==0x8000)
    {
       *pData=Data-0x8000;
       status = SHT_TASK_OK;
    }
    SwitchInterrupt++;
  }
  else
  { 
    #ifdef InterruptMode
    RequestEnabled = MeasureTemp; //RequestEnabled = MeasureHumi;
    #endif
    SwitchInterrupt=0u; 
  }   
  return  status;
}

 /*****************************************************************************
 *
 * Function:        SHT_ReadHumidityRaw
 *
 * Description:     This function make a reading of the raw humidity over 
 *                  the sensor
 *
 * Caveats:
 *   None
 *
 *****************************************************************************/
static SHT_TaskStateType SHT_ReadHumidityRaw(uint16_t *pData)
{                 
  uint16_t Data=0;
	SHT_TaskStateType status = SHT_TASK_NOT_OK;
  if(0u==SwitchInterrupt)
  {
    Data=ReadHumidity();
    if((Data&0x8000)==0x8000)
    {
       *pData=Data-0x8000;
       status = SHT_TASK_OK;
    }
    SwitchInterrupt++;
  }
  else
  { 
    #ifdef InterruptMode
    RequestEnabled = MeasureTemp; //RequestEnabled = MeasureHumi;
    #endif
    SwitchInterrupt=0u; 
  }   
  return  status;
}

void s_delay_2_us()
{
  asm("NOP");
}

void TransmitStart()
{ 
  PIN_Clear( SDA );
  PIN_Set(SCK);     s_delay_2_us();
  PIN_Clear(SCK);   s_delay_2_us();
  PIN_Set(SDA);     s_delay_2_us();
  PIN_Set(SCK);     s_delay_2_us();
  PIN_Clear(SCK);   s_delay_2_us();
  PIN_Clear(SDA);   s_delay_2_us();
  PIN_Set(SCK);     s_delay_2_us();
}

uint16_t ReadValueFn()
{ 
 uint8_t i, ByteHigh=0, ByteLow=0,SwpHigh=0,SwpLow=0; 
 uint16_t Lx=0,ActualValue=0; 

  for(i=1;i<=8;++i){                     // read high byte VALUE from SHT11 
     PIN_Clear(SCK); 
     s_delay_2_us();
     /*adquisition value*/
     ActualValue=(PIOA->PIO_PDSR&PIO_PDSR_P3)>>3;
     ByteHigh|= ((ActualValue)<<i);
     PIN_Set(SCK); 
     s_delay_2_us();
  } 
  PIN_MakeOutputSDA( );
  PIN_Set(SDA); 
  s_delay_2_us();
  PIN_Clear(SCK); 
  s_delay_2_us();
  PIN_Set(SCK);
  PIN_MakeInputSDA( );
  s_delay_2_us();
  ActualValue=0;
  for(i=1;i<=8;++i){                     // read low byte VALUE from SHT11 
     PIN_Clear(SCK); 
     s_delay_2_us();
     ActualValue=(PIOA->PIO_PDSR&PIO_PDSR_P3)>>3;
     ByteLow|= ((ActualValue)<<i);
     /*adquisition value*/
     PIN_Set(SCK); 
     s_delay_2_us();
  }
  PIN_MakeOutputSDA( );
  PIN_Clear(SDA); 
  s_delay_2_us();
  PIN_Clear(SCK); 
  s_delay_2_us();
  PIN_Set(SCK); 
  s_delay_2_us();
  ByteHigh=SwapData(ByteHigh);
  ByteLow=SwapData(ByteLow);
  Lx=(((uint16_t)ByteHigh<<8)|ByteLow)<<1; 
  return(Lx); 
} 

uint8_t SwapData(uint8_t data)
{
  uint8_t tempData=0x00;
  tempData |= (data&0x80)>>7;  
  tempData |= (data&0x40)>>5;
  tempData |= (data&0x20)>>3; 
  tempData |= (data&0x10)>>1;
  tempData |= (data&0x08)<<1; 
  tempData |= (data&0x04)<<3;
  tempData |= (data&0x02)<<5;
  tempData |= (data&0x01)<<7;
  return tempData;
}
///////////////////////////////////////////////////////// 
void SendCommand(int Command){ 
 uint8_t i; 

  for(i=128;i>0;i/=2){ 
     if(i & Command) 
     {
       PIN_Clear(SDA);
     }
     else            
     {
       PIN_Set(SDA);
     } 
     PIN_Clear(SCK); 
     s_delay_2_us(); 
     PIN_Set(SCK); 
     s_delay_2_us(); 
  } 
 /*Change pin mode to input*/
  PIN_MakeInputSDA();
  PIN_Clear(SCK); 
  s_delay_2_us();
  PIN_Set(SCK); 
  s_delay_2_us();
  
} 

/////////////////////////////////////////////////////////
#ifdef InterruptMode
 
uint16_t ReadTemperature()
{ 
  uint8_t AcknowPINge; 
  uint16_t Lx=0u,Value,DummyCounter; 
  TransmitStart(); 
  SendCommand(MeasureTemp); 
  DummyCounter=0x0000;
  while(0u==AckConfirmVar||DummyCounter<0xffff)
  {
    DummyCounter++;
  }/*wait for acknolwledge*/ 
  if(DummyCounter!=0xffff)
  {
    AckConfirmVar=0u;
    s_delay_2_us();
    PIN_MakeInputSDA();
    InterruptVar=1u;
  }
  
  //Value=CalcTempValues(Lx); 
  return(Value);  
} 

uint16_t ReadHumidity()
{ 
  uint16_t Lx=0u,Value,DummyCounter;  
  TransmitStart(); 
  SendCommand(MeasureHumi); 
 
  while(0u==AckConfirmVar);       /*wait for acknolwledge*/ 
  AckConfirmVar=0u;
  s_delay_2_us();
  PIN_MakeInputSDA();
  DummyCounter=0;
  while(0u==AckConfirmVar||DummyCounter<0xffff)
  {
    DummyCounter++;
  }/*wait for acknolwledge*/ 
  if(DummyCounter!=0xffff)
  {
    AckConfirmVar=0u;
    s_delay_2_us();
    PIN_MakeInputSDA();
    InterruptVar=1u;
  }
  return(Value); 
}

#else    //polling mode
  
uint16_t ReadTemperature()
{ 
  uint8_t AcknowPINge; 
  uint16_t DummyCounter=0;
  long Lx=0u,Value; 
  TransmitStart(); 
  SendCommand(MeasureTemp); 
  while(0u==AckConfirmVar);/*wait for acknolwledge*/ 
  //while(0u==AckConfirmVar||DummyCounter<0xffff)
  //{
  //  DummyCounter++;
  //}/*wait for acknolwledge*/ 
  AckConfirmVar=0u;
  s_delay_2_us();
  PIN_MakeInputSDA();
  //while(0u==AckConfirmVar||DummyCounter<0xffff)
  //{
  //  DummyCounter++;
  //}/*wait for acknolwledge*/ 
  while(0u==AckConfirmVar);/*wait for acknolwledge*/ 
  AckConfirmVar=0u;
  Lx=ReadValueFn();              
  //Value=CalcTempValues(Lx); 
  return(Lx+0x8000); 
} 
  
uint16_t ReadHumidity(){ 
  uint8_t AcknowPINge; 
  long Lx=0u,Value; 
  TransmitStart(); 
  SendCommand(MeasureHumi); 
  while(0==AckConfirmVar);/*wait for acknolwledge*/ 
  AckConfirmVar=0;
  s_delay_2_us();
  PIN_MakeInputSDA();
  while(0u==AckConfirmVar);/*wait for acknolwledge*/ 
  AckConfirmVar=0u;
  Lx=ReadValueFn();                        
  //Value=CalcTempValues(Lx); 
  return(Lx+0x8000); 
} 
#endif
///////////////////////////////////////////////////////// 
// 
///////////////////////////////////////////////////////// 

void RST_Connection(){ 
 int i; 

  PIN_Clear(SDA); 
  for(i=1;i<=10;++i) { 
      PIN_Clear(SCK); s_delay_2_us();
      PIN_Set(SCK);  s_delay_2_us();
  } 
  TransmitStart(); 
} 

