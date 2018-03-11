/****************************************************************************************************/
/**
\file       fpu.h
\brief      MCAL abstraction level - Floating Point Unit initialization and configuration
\author     Abraham Tezmol
\version    1.0
\project    Tau 
\date       23/October/2016
*/
/****************************************************************************************************/

#ifndef __FPU_H        /*prevent duplicated includes*/
#define __FPU_H

/*****************************************************************************************************
* Include files
*****************************************************************************************************/

/** Core modules */
#include "compiler.h"
#include "stdint.h"

/*****************************************************************************************************
* Declaration of module wide TYPEs 
*****************************************************************************************************/


/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs 
*****************************************************************************************************/


/*****************************************************************************************************
* Declaration of module wide FUNCTIONS
*****************************************************************************************************/

/** Floatint Point Unit Enable */
void vfnFpu_enable(void);

/****************************************************************************************************/

#endif /* __FPU_H */
