/*
 * Copyright (C) 2020 Bixin
 */

#include <libopencm3/stm32/gpio.h>
#include <string.h>

#include "bitmaps.h"
#include "buttons.h"
#include "oled.h"
#include "si2c.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"

uint8_t g_ucWorkMode = 0;
uint8_t g_ucFlag = 0;
uint8_t s_usPower_Button_Status = POWER_BUTTON_UP;

extern void vDISP_DeviceInfo(void);

/*
 * ble mac get ble name
 */
void vCalu_BleName(uint8_t *pucMac, uint8_t *pucName) {
  uint8_t i;
  memcpy(pucName, BLE_ADV_NAME, BLE_ADV_NAME_LEN);
  for (i = 0; i < 4; i++) {
    pucName[BLE_ADV_NAME_LEN - 4 + i] += pucMac[i] % 20;
  }
}

/*
 * display ble message
 */
bool bBle_DisPlay(uint8_t ucIndex, uint8_t *ucStr) {
  uint8_t ucDelayFalg;
  oledClear();
  ucDelayFalg = 0x00;
  switch (ucIndex) {
    case BT_LINK:
      oledDrawStringCenter(60, 30, "Connect by Bluetooth", FONT_STANDARD);
      break;
    case BT_UNLINK:
      oledDrawStringCenter(60, 30, "BLE unLink", FONT_STANDARD);
      break;
    case BT_DISPIN:
      ucStr[BT_PAIR_LEN] = '\0';
      oledDrawStringCenter(60, 30, "BLE Pair Pin", FONT_STANDARD);
      oledDrawStringCenter(60, 50, (char *)ucStr, FONT_STANDARD);
      ucDelayFalg = 1;
      break;
    case BT_PINERROR:
      oledDrawStringCenter(60, 30, "Pair Pin Error", FONT_STANDARD);
      break;
    case BT_PINTIMEOUT:
      oledDrawStringCenter(60, 30, "Pair Pin Timeout", FONT_STANDARD);
      break;
    case BT_PAIRINGSCESS:
      oledDrawStringCenter(60, 30, "Pair Pin Success", FONT_STANDARD);
      break;
    case BT_PINCANCEL:
      oledDrawStringCenter(60, 30, "Pair Pin Cancel", FONT_STANDARD);
      break;
    default:
      break;
  }
  oledRefresh();
#if !EMULATOR
  uint8_t ucSw[4];
  ucSw[0] = 0x90;
  ucSw[1] = 0x00;
  vSI2CDRV_SendResponse(ucSw, 2);
#endif
  if (0x00 == ucDelayFalg) {
    delay_time(2000);
    return true;
  } else {
    return false;
  }
}

#if !EMULATOR
/*
 * battery power on/off
 */
void vPower_Control(uint8_t ucMode) {
  uint32_t uiCount = 0;

  if (BUTTON_POWER_ON == ucMode) {
    while (1) {
      if (GET_BUTTON_CANCEL()) {
        delay_time(10);
        uiCount++;
        if (uiCount > 150) {
          POWER_ON();
          g_ucWorkMode = WORK_MODE_BLE;
          s_usPower_Button_Status = POWER_BUTTON_DOWN;
          break;
        }

      } else {
        delay_time(2);
        if (0x00 == GET_BUTTON_CANCEL()) {
          POWER_OFF();
          while (1)
            ;
        }
      }
    }

  } else {
    if ((WORK_MODE_USB != g_ucWorkMode) && (GET_BUTTON_CANCEL())) {
      // no usb and button down and no nfc
      if ((0x00 == GET_USB_INSERT()) &&
          (POWER_BUTTON_UP == s_usPower_Button_Status) && (GET_NFC_INSERT())) {
        while (GET_BUTTON_CANCEL()) {
          delay_time(10);
          uiCount++;
          if (uiCount > 150) {
            vDisp_PromptInfo(DISP_PRESSKEY_POWEROFF);
            POWER_OFF();
            while (1)
              ;
          }
        }
      }
    } else {
      s_usPower_Button_Status = POWER_BUTTON_UP;
      vDISP_DeviceInfo();
    }
  }
}

/*
 * check usb/nfc/ble
 */
void vCheckMode(void) {
  g_ucWorkMode = 0;

  // nfc mode
  if (0x00 == GET_NFC_INSERT()) {
    delay_time(2);
    if (0x00 == GET_NFC_INSERT()) {
      g_ucWorkMode = WORK_MODE_NFC;
      POWER_ON();
      return;
    }
  } else {
    // usb mode
    if (GET_USB_INSERT()) {
      delay_time(2);
      if (GET_USB_INSERT()) {
        g_ucWorkMode = WORK_MODE_USB;
        POWER_ON_BLE();
        return;
      }
    } else {
      // 2s power on
      vPower_Control(BUTTON_POWER_ON);
    }
  }
}
#endif
