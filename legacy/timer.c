/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2016 Saleem Rashid <trezor@saleemrashid.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "timer.h"

#include "buttons.h"
//#include "oled.h"

#include <libopencm3/cm3/systick.h>
#if !EMULATOR
#include <libopencm3/cm3/vector.h>
#endif
#include <libopencm3/stm32/rcc.h>

/* 1 tick = 1 ms */
extern volatile uint32_t system_millis;
uint8_t ucTimeFlag;
uint8_t g_ucLanguageFlag = 0;

/*poweroff */
volatile uint32_t system_millis_poweroff_start;

/*
 * delay time
 */
void delay_time(uint32_t uiDelay_Ms) {
  uint32_t uiTimeout = uiDelay_Ms * 30000;
  while (uiTimeout--) {
    __asm__("nop");
  }
}
void delay_us(uint32_t uiDelay_us) {
  uint32_t uiTimeout = uiDelay_us * 30;
  while (uiTimeout--) {
    __asm__("nop");
  }
}

#if !EMULATOR
/*
 * Initialise the Cortex-M3 SysTick timer
 */
void timer_init(void) {
  system_millis = 0;
  ucTimeFlag = 0;

  /*
   * MCU clock (120 MHz) as source
   *
   *     (120 MHz / 8) = 15 clock pulses
   *
   */
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
  STK_CVR = 0;

  /*
   * 1 tick = 1 ms @ 120 MHz
   *
   *     (15 clock pulses * 1000 ms) = 15000 clock pulses
   *
   * Send an interrupt every (N - 1) clock pulses
   */
  systick_set_reload(14999);

  /* SysTick as interrupt */
  systick_interrupt_enable();

  systick_counter_enable();
}

void sys_tick_handler(void) {
  system_millis++;
  if (POWER_OFF_TIMER_READY()) {
    system_millis_poweroff_start++;
    if (system_millis_poweroff_start > autoPowerOffMsDefault) {
      // vDisp_PromptInfo(DISP_PRESSKEY_POWEROFF);
      POWER_OFF();
      while (1)
        ;
    }
  }
}
#endif
