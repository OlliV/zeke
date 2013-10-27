/**
 *******************************************************************************
 * @file    tty.c
 * @author  Olli Vanhoja
 * @brief   Device driver for dev tty.
 * @section LICENSE
 * Copyright (c) 2013 Olli Vanhoja <olli.vanhoja@cs.helsinki.fi>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */

/** @addtogroup Dev
  * @{
  */

/** @addtogroup tty
  * @{
  */

#include <hal/uart.h>
#include "dev.h"

void devtty_init(void) __attribute__((constructor));
int devtty_cwrite(uint32_t ch, osDev_t dev);
int devtty_cread(uint32_t * ch, osDev_t dev);

void devtty_init(void)
{
    uart_init_t uart_conf = {
        .baud_rate  = UART_BAUDRATE_9600,
        .stop_bits  = UART_STOPBITS_ONE,
        .parity     = UART_PARITY_NO,
        .flowctrl   = 0
    };

    uart_init(0, &uart_conf);

    DEV_INIT(2, &devtty_cwrite, &devtty_cread, 0, 0, 0, 0);
}

int devtty_cwrite(uint32_t ch, osDev_t dev)
{
    int port = (int)DEV_MINOR(dev);

    uart_putc(port, (uint8_t)ch);

    return DEV_CWR_OK;
}

int devtty_cread(uint32_t * ch, osDev_t dev)
{
    return DEV_CRD_UNDERFLOW;
}

/**
  * @}
  */

/**
  * @}
  */
