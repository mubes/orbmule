/* File: startup_ARMCM3.c
 * Purpose: startup file for Cortex-M3 devices.
 *          Should be used with GCC 'GNU Tools ARM Embedded'
 * Version: V1.01
 * Date: 12 June 2014
 *
 */
/* Copyright (c) 2011 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include <stdint.h>


/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __copy_table_start__;
extern uint32_t __copy_table_end__;
extern uint32_t __zero_table_start__;
extern uint32_t __zero_table_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );


/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
#ifndef __START
    extern void  _start(void) __attribute__((noreturn));    /* PreeMain (C library entry point) */
#else
    extern int  __START(void) __attribute__((noreturn));    /* main entry point */
#endif

#ifndef __NO_SYSTEM_INIT
    extern void SystemInit (void);            /* CMSIS System Initialization      */
#endif


/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void);                          /* Default empty handler */
void Reset_Handler(void);                            /* Reset Handler */


/*----------------------------------------------------------------------------
  User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
#ifndef __STACK_SIZE
    #define   __STACK_SIZE  0x00000400
#endif
static uint8_t stack[__STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

#ifndef __HEAP_SIZE
    #define   __HEAP_SIZE   0x00000C00
#endif
#if __HEAP_SIZE > 0
    static uint8_t heap[__HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
#endif


/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Cortex-M3 Processor Exceptions */
void NMI_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
const pFunc __Vectors[] __attribute__ ((section(".isr_vector"))) =
{
    /* Cortex-M3 Exceptions Handler */
    (pFunc)&__StackTop,                       /*      Initial Stack Pointer     */
    Reset_Handler,                            /*      Reset Handler             */
    NMI_Handler,                              /*      NMI Handler               */
    HardFault_Handler,                        /*      Hard Fault Handler        */
    MemManage_Handler,                        /*      MPU Fault Handler         */
    BusFault_Handler,                         /*      Bus Fault Handler         */
    UsageFault_Handler,                       /*      Usage Fault Handler       */
    0,                                        /*      Reserved                  */
    0,                                        /*      Reserved                  */
    0,                                        /*      Reserved                  */
    0,                                        /*      Reserved                  */
    SVC_Handler,                              /*      SVCall Handler            */
    DebugMon_Handler,                         /*      Debug Monitor Handler     */
    0,                                        /*      Reserved                  */
    PendSV_Handler,                           /*      PendSV Handler            */
    SysTick_Handler,                          /*      SysTick Handler           */
};


/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    uint32_t *pSrc, *pDest;
    uint32_t *pTable __attribute__((unused));

    /*  Firstly it copies data from read only memory to RAM. There are two schemes
     *  to copy. One can copy more than one sections. Another can only copy
     *  one section.  The former scheme needs more instructions and read-only
     *  data to implement than the latter.
     *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
    /*  Multiple sections scheme.
     *
     *  Between symbol address __copy_table_start__ and __copy_table_end__,
     *  there are array of triplets, each of which specify:
     *    offset 0: LMA of start of a section to copy from
     *    offset 4: VMA of start of a section to copy to
     *    offset 8: size of the section to copy. Must be multiply of 4
     *
     *  All addresses must be aligned to 4 bytes boundary.
     */
    pTable = &__copy_table_start__;

    for (; pTable < &__copy_table_end__; pTable = pTable + 3)
        {
            pSrc  = (uint32_t*)*(pTable + 0);
            pDest = (uint32_t*)*(pTable + 1);
            for (; pDest < (uint32_t*)(*(pTable + 1) + *(pTable + 2)) ; )
                {
                    *pDest++ = *pSrc++;
                }
        }
#else
    /*  Single section scheme.
     *
     *  The ranges of copy from/to are specified by following symbols
     *    __etext: LMA of start of the section to copy from. Usually end of text
     *    __data_start__: VMA of start of the section to copy to
     *    __data_end__: VMA of end of the section to copy to
     *
     *  All addresses must be aligned to 4 bytes boundary.
     */
    pSrc  = &__etext;
    pDest = &__data_start__;

    for ( ; pDest < &__data_end__ ; )
        {
            *pDest++ = *pSrc++;
        }
#endif /*__STARTUP_COPY_MULTIPLE */

    /*  This part of work usually is done in C library startup code. Otherwise,
     *  define this macro to enable it in this startup.
     *
     *  There are two schemes too. One can clear multiple BSS sections. Another
     *  can only clear one section. The former is more size expensive than the
     *  latter.
     *
     *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
     *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
     */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
    /*  Multiple sections scheme.
     *
     *  Between symbol address __copy_table_start__ and __copy_table_end__,
     *  there are array of tuples specifying:
     *    offset 0: Start of a BSS section
     *    offset 4: Size of this BSS section. Must be multiply of 4
     */
    pTable = &__zero_table_start__;

    for (; pTable < &__zero_table_end__; pTable = pTable + 2)
        {
            pDest = (uint32_t*)*(pTable + 0);
            for (; pDest < (uint32_t*)(*(pTable + 0) + *(pTable + 1)) ; )
                {
                    *pDest++ = 0;
                }
        }
#elif defined (__STARTUP_CLEAR_BSS)
    /*  Single BSS section scheme.
     *
     *  The BSS section is specified by following symbols
     *    __bss_start__: start of the BSS section.
     *    __bss_end__: end of the BSS section.
     *
     *  Both addresses must be aligned to 4 bytes boundary.
     */
    pDest = &__bss_start__;

    for ( ; pDest < &__bss_end__ ; )
        {
            *pDest++ = 0ul;
        }
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
    SystemInit();
#endif

#ifndef __START
#define __START _start
#endif
    __START();

}


/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{

    while(1);
}
