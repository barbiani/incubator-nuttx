/****************************************************************************
 * boards/renesas/rx65n/rx65n-grrose/src/rx65n.bringup.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/config.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <syslog.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/board.h>

#include "rx65n_grrose.h"
#include <rx65n_definitions.h>
#ifdef CONFIG_LIB_BOARDCTL

#ifdef HAVE_RTC_DRIVER
#  include <nuttx/timers/rtc.h>
#  include "rx65n_rtc.h"
#endif

#ifdef HAVE_RIIC_DRIVER
#  include <nuttx/i2c/i2c_master.h>
#  include "rx65n_riic.h"
#endif
/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rtc_driver_initialize
 *
 * Description:
 *   Initialize and register the RTC driver.
 *
 ****************************************************************************/

#ifdef HAVE_RTC_DRIVER
static int rtc_driver_initialize(void)
{
  FAR struct rtc_lowerhalf_s *lower;
  int ret;

  /* Instantiate the rx65n lower-half RTC driver */

  lower = rx65n_rtc_lowerhalf();
  if (lower == NULL)
    {
      serr("ERROR: Failed to instantiate the RTC lower-half driver\n");
      ret = -ENOMEM;
    }
  else
    {
      /* Bind the lower half driver and register the combined RTC driver
       * as /dev/rtc0
       */

      ret = rtc_initialize(0, lower);
      if (ret < 0)
        {
          serr("ERROR: Failed to bind/register the RTC driver: %d\n", ret);
        }
    }

  return ret;
}

#endif
/****************************************************************************
 * Name: rx65n_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=n && CONFIG_LIB_BOARDCTL=y :
 *     Called from the NSH library
 *
 ****************************************************************************/

int rx65n_bringup(void)
{
  int ret;
#ifdef HAVE_RTC_DRIVER
  ret = rtc_driver_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: rtc_driver_initialize failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_FS_PROCFS

  /* Mount the procfs file system */

  ret = mount(NULL, "/proc", "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR,
             "ERROR: Failed to mount the PROC filesystem: %d (%d)\n",
             ret, errno);
    }

#endif

#ifdef CONFIG_RX65N_SBRAM
  /* Initialize battery-backed RAM */

  (void)rx65n_sbram_int();
#endif
#ifdef HAVE_RIIC_DRIVER
  FAR struct i2c_master_s *i2c;

  /* Get the I2C lower half instance */
#ifdef CONFIG_RX65N_RIIC0
#ifdef CONFIG_RX65N_SCI2
  i2cerr("Port is initialized for RIIC0. SCI2 cannot be used\n");
#endif
  riic0_init_port();
  i2c = rx65n_i2cbus_initialize(0);
  if (i2c == NULL)
    {
      i2cerr("ERROR: Initialization of RIIC Channel 0 failed: %d\n", ret);
    }

  else
    {
      /* Register the I2C character driver */

      ret = i2c_register(i2c, 0);
      if (ret < 0)
        {
          i2cerr("ERROR: Failed to register RIIC device: %d\n", ret);
        }
    }

#endif
#ifdef CONFIG_RX65N_RIIC1
  i2cerr("RIIC1 setting is not supported on GR-ROSE Board\n");
#endif
#ifdef CONFIG_RX65N_RIIC2
  i2cerr("RIIC2 setting is not supported on GR-ROSE Board\n");
  i2cerr("It is used for USB port and cannot be configured\n");
#endif
#endif
  return OK;
}

#if defined (CONFIG_ARCH_HAVE_LEDS) && (CONFIG_ARCH_LEDS)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_autoled1_on
 *
 * Description:
 *   Turns on LED 0
 *
 ****************************************************************************/

void board_autoled1_on(int led)
{
  LED0 = LED_ON;
}

/****************************************************************************
 * Name: board_autoled2_on
 *
 * Description:
 *   Turns on LED 1
 *
 ****************************************************************************/

void board_autoled2_on(int led)
{
  LED1 = LED_ON;
}

/****************************************************************************
 * Name: board_autoled_on
 *
 * Description:
 *   Turns on LED 0 & LED 1
 *
 ****************************************************************************/

void board_autoled_on(int led)
{
  LED0 = LED_ON;
  LED1 = LED_ON;
}

/****************************************************************************
 * Name: board_autoled1_off
 *
 * Description:
 *   Turns off LED 0
 *
 ****************************************************************************/

void board_autoled1_off(int led)
{
  LED0 = LED_OFF;
}

/****************************************************************************
 * Name: board_autoled2_off
 *
 * Description:
 *   Turns off LED 1
 *
 ****************************************************************************/

void board_autoled2_off(int led)
{
  LED1 = LED_OFF;
}

/****************************************************************************
 * Name: board_autoled_off
 *
 * Description:
 *   Turns off LED 0 & LED 1
 *
 ****************************************************************************/

void board_autoled_off(int led)
{
  LED0 = LED_OFF;
  LED1 = LED_OFF;
}

#endif
#endif
