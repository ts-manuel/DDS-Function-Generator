#ifndef __ALTERA_AVALON_DMA_H__
#define __ALTERA_AVALON_DMA_H__

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2009 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#include "sys/alt_dma_dev.h"
#include "sys/alt_warning.h"
#include "sys/alt_dev.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * This is the device driver for the altera Avalon DMA device. 
 *
 * The HAL DMA model describes a DMA trasaction as being performed by two 
 * drivers: a transmit and a receive channel. In the case of this device these
 * two drivers map onto a single instance of the physical DMA device. This
 * means that for every device instance, a pair of device drivers will be 
 * created.
 */

#ifndef ALT_AVALON_DMA_NSLOTS
#define ALT_AVALON_DMA_NSLOTS (4)
#endif

#define ALT_AVALON_DMA_NSLOTS_MSK (ALT_AVALON_DMA_NSLOTS - 1)

/*
 * Flags used to describe the current operating mode of the drivers.
 */

#define ALT_AVALON_DMA_MODE_MSK  (0xf)
#define ALT_AVALON_DMA_MODE_8    (0x0)
#define ALT_AVALON_DMA_MODE_16   (0x1)
#define ALT_AVALON_DMA_MODE_32   (0x3)
#define ALT_AVALON_DMA_MODE_64   (0x7)
#define ALT_AVALON_DMA_MODE_128  (0xf)
#define ALT_AVALON_DMA_TX_STREAM (0x20)
#define ALT_AVALON_DMA_RX_STREAM (0x40)

/*
 * An array of "alt_avalon_dma_txslot" instances are used to maintain a list of
 * the pending transmit requests.
 */

typedef struct
{
  const void*      from;
  alt_u32          len;
  alt_txchan_done* done;
  void*            handle;
} alt_avalon_dma_txslot;

/*
 * An array of "alt_avalon_dma_rxslot" instances are used to maintain a list of
 * the pending receive requests.
 */

typedef struct
{
  void*            data;
  alt_u32          len;
  alt_rxchan_done* done;
  void*            handle;
} alt_avalon_dma_rxslot;

/*
 * Device specific data. A single instance of this structure is created for
 * each physical device.
 */

typedef struct alt_avalon_dma_priv_s alt_avalon_dma_priv;

struct alt_avalon_dma_priv_s {
  void*                  base;
  volatile alt_u32       tx_start;
  alt_u32                tx_end;
  volatile alt_u32       rx_start;
  alt_u32                rx_end;
  alt_u32                flags;
  alt_u32                max_length;
  void                   (*launch) (alt_avalon_dma_priv* priv);
  int                    active;
  alt_avalon_dma_txslot  tx_buf[ALT_AVALON_DMA_NSLOTS];
  alt_avalon_dma_rxslot  rx_buf[ALT_AVALON_DMA_NSLOTS];
};

/*
 *
 */

typedef struct
{
  alt_dma_txchan_dev     dev;
  alt_avalon_dma_priv*   priv;
} alt_avalon_dma_txchan;

/*
 *
 */

typedef struct
{
  alt_dma_rxchan_dev     dev;
  alt_avalon_dma_priv*   priv;
} alt_avalon_dma_rxchan;

/*
 *
 */

extern int alt_avalon_dma_prepare (alt_dma_rxchan   dma, 
           void*            data,
           alt_u32          len,
           alt_rxchan_done* done,  
           void*            handle);

extern int alt_avalon_dma_space (alt_dma_txchan dma);
  
extern int alt_avalon_dma_send (alt_dma_txchan dma, 
        const void* from, 
        alt_u32 len,
        alt_txchan_done* done, 
        void* handle);

extern void alt_avalon_dma_init (alt_avalon_dma_txchan* tx,
         alt_avalon_dma_rxchan* rx,
         void*                  base, 
         alt_u32                ic_id,
         alt_u32                irq);

extern int alt_avalon_dma_tx_ioctl (alt_dma_txchan dma, 
            int req, 
            void* arg);

extern int alt_avalon_dma_rx_ioctl (alt_dma_rxchan dma, 
            int req, 
            void* arg);

extern void alt_avalon_dma_launch_bidir (alt_avalon_dma_priv* priv);

/*
 * The macro ALTERA_AVALON_DMA_INSTANCE is used by alt_sys_init.c to reserve
 * per device memory.
 */

#define ALTERA_AVALON_DMA_INSTANCE(name, dev)    \
  static alt_avalon_dma_priv dev##_priv =        \
  {                                              \
    (void*) name##_BASE,                         \
    0,                                           \
    0,                                           \
    0,                                           \
    0,                                           \
    ALT_AVALON_DMA_MODE_32,                      \
    (1 << name##_LENGTHWIDTH) - 1,               \
    alt_avalon_dma_launch_bidir,                 \
    0                                            \
  };                                             \
                                                 \
  static alt_avalon_dma_txchan dev##_tx =        \
  {                                              \
    {                                            \
      ALT_LLIST_ENTRY,                           \
      name##_NAME,                               \
      alt_avalon_dma_space,                      \
      alt_avalon_dma_send,                       \
      alt_avalon_dma_tx_ioctl                    \
    },                                           \
    &dev##_priv                                  \
  };                                             \
  static alt_avalon_dma_rxchan dev##_rx =        \
  {                                              \
    {                                            \
      ALT_LLIST_ENTRY,                           \
      name##_NAME,                               \
      ALT_AVALON_DMA_NSLOTS - 1,                 \
      alt_avalon_dma_prepare,                    \
      alt_avalon_dma_rx_ioctl                    \
    },                                           \
    &dev##_priv                                  \
  }

/*
 * The macro ALTERA_AVALON_DMA_INIT is called by the auto-generated function
 * alt_sys_init() to initialise a given device instance.
 */

#define ALTERA_AVALON_DMA_INIT(name, dev)                                  \
  if (name##_IRQ == ALT_IRQ_NOT_CONNECTED)                                 \
  {                                                                        \
    ALT_LINK_ERROR ("Error: Interrupt not connected for " #dev ". "        \
                    "The ALTERA Avalon DMA driver requires that an "       \
                    "interrupt is connected. Please select an IRQ for "    \
                    "this device in SOPC builder.");                       \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    alt_avalon_dma_init (&dev##_tx, &dev##_rx, (void*) name##_BASE,        \
                         name##_IRQ_INTERRUPT_CONTROLLER_ID, name##_IRQ);  \
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ALTERA_AVALON_DMA_H__ */
