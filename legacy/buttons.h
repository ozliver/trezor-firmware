/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
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

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <libopencm3/stm32/gpio.h>
#include <stdbool.h>

#define OLD_PCB 0

struct buttonState {
  volatile bool YesUp;
  volatile int YesDown;
  volatile bool NoUp;
  volatile int NoDown;
  volatile bool UpUp;
  volatile int UpDown;
  volatile bool DownUp;
  volatile int DownDown;
};

extern struct buttonState button;

uint16_t buttonRead(void);
void buttonUpdate(void);
bool hasbutton(void);

#ifndef BTN_PORT
#define BTN_PORT GPIOC
#endif

#ifndef BTN_PIN_YES
#define BTN_PIN_YES GPIO2
#endif

#ifndef BTN_PIN_NO
#define BTN_PIN_NO GPIO0
#endif

#ifndef BTN_PIN_UP
#define BTN_PIN_UP GPIO3
#endif

#ifndef BTN_PIN_DOWN
#define BTN_PIN_DOWN GPIO5
#endif

#if !EMULATOR
#define GPIO_POWER_ON GPIO4
// power control
#define POWER_ON() (gpio_set(GPIOC, GPIO_POWER_ON))
#define POWER_OFF() (gpio_clear(GPIOC, GPIO_POWER_ON))
#endif

#endif
