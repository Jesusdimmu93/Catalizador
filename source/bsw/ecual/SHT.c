/****************************************************************************************************/
/**
\file       SHT.c
\brief      ECU Abstraction level - Sensirion Sensor Abstraction
\author     Jesus Iñiguez
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

#define InterruptMode
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
  PMC_EnablePeripheral(ID_PIOA);
  NVIC_DisableIRQ( PIOA_IRQn ) ;
	NVIC_ClearPendingIRQ( PIOA_IRQn ) ;
	NVIC_SetPriority( PIOA_IRQn, 1 ) ;
  NVIC_EnableIRQ( PIOA_IRQn ) ;  
  return 0;
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
  return 0;
}
  
SHT_TaskStateType SHT_GetTemperatureRaw(uint16_t *pData)
{  
  
	SHT_TaskStateType status = SHT_TASK_OK;
  if(0u==SwitchInterrupt)
  {
    ReadTemperature();
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

SHT_TaskStateType SHT_GetHumidityRaw(uint16_t *pData)
{
	SHT_TaskStateType status = SHT_TASK_OK;
  ReadHumidity();
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

uint16_t ReadValueFn(){ 
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
  Lx=((uint16_t)ByteHigh<<8)|ByteLow; 
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
  long Lx=0u,Value; 
  TransmitStart(); 
  SendCommand(MeasureTemp); 
  while(0u==AckConfirmVar);/*wait for acknolwledge*/ 
  AckConfirmVar=0u;
  s_delay_2_us();
  PIN_MakeInputSDA();
  while(0u==AckConfirmVar);/*wait for acknolwledge*/ 
  AckConfirmVar=0u;
  Lx=ReadValueFn();                        
  //Value=CalcTempValues(Lx); 
  return(Value); 
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
  return(Value); 
} 
#endif
///////////////////////////////////////////////////////// 
// 
///////////////////////////////////////////////////////// 

///////////////////////////////////////////////////////// 
// 
/*//////////////////////////////////////////////////////// 
long CalcTempValues(long Lx){ 
 long value; 
 float Fx; 

  Fx=0.01*(float)Lx; 
  Fx=Fx-40; 
  Value=Fx*10; 
  return(value); 
} 
/*//////////////////////////////////////////////////////// 
// 
/*//////////////////////////////////////////////////////// 
long CalcHumiValues(long Lx){ 
 long Value; 
 float Fx, Fy; 

  Fx=(float)Lx*(float)Lx; 
  Fx=Fx*(-0.0000028); 
  Fy=(float)Lx*0.0405; 
  Fx=Fx+Fy; 
  Fx=Fx-4; 
  Value=Fx*10; 
  return(value); 
} 
/*///////////////
   
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
