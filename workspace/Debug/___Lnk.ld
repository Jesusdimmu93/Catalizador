/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2014, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

INPUT (C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\dbg_console.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\board_lowlevel.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\board_memories.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\MemAlloc.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\xdma_hardware_interface.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\xdmac.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\xdmad.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\fpu.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\mcan.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\exceptions.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\mpu.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\pio_it.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\pio.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\pmc.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\supc.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\tc.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\twihs.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\wdt.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\uart.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\syscalls.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\system_sam.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\timetick.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\app_scheduler.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\Tasks.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\sysclk.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\CanNm.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\CanIf.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\CanIf_Cfg.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\led.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\serial_ctrl.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\SHT.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\main.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\SensingEnvironment.o 
C:\Users\uid42696\Desktop\Iteso_Projects\Catalist_Project_Git\Catalizador\workspace\Debug\startup_sam.o)

/*------------------------------------------------------------------------------
 *      Linker script for running in internal FLASH on the ATSAMV71Q21
 *----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
  rom (rx)  :      ORIGIN = 0x00400000, LENGTH = 0x00200000
  ram (rwx) :      ORIGIN = 0x20400000, LENGTH = 0x00050000
  ram_heap (rwx) : ORIGIN = 0x20450000, LENGTH = 0x00010000
  sdram(rwx): ORIGIN = 0x70000000, LENGTH = 0x00200000
}

/* The stack size used by the application. NOTE: you need to adjust according to your application. */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 0x2000;

/* The heapsize used by the application. NOTE: you need to adjust according to your application. */
HEAP_SIZE = DEFINED(HEAP_SIZE) ? HEAP_SIZE : 0x1000;

MY_HEAP_SIZE = DEFINED(MY_HEAP_SIZE) ? MY_HEAP_SIZE : 0x10000;

INCLUDE ..\source\infrastructure\toolset\gcc\sam_flash.ld
INCLUDE ..\source\infrastructure\toolset\gcc\sam_sdram.ld
