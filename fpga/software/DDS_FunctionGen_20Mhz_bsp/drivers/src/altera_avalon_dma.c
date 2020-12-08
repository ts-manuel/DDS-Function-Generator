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

#include "altera_avalon_dma.h"
#include "sys/alt_irq.h"

#include "altera_avalon_dma_regs.h"

#include <errno.h>

/*
 * alt_avalon_dma_launch_bidir() is called to launch a new transaction when
 * both the receive and transmit channels are using incrementing addresses,
 * i.e. both channels are accesing memory rather than devices.
 */

void alt_avalon_dma_launch_bidir (alt_avalon_dma_priv* priv)
{
  alt_avalon_dma_txslot* tx_slot;
  alt_avalon_dma_rxslot* rx_slot;

  if ((priv->tx_start != priv->tx_end) && (priv->rx_start != priv->rx_end))
  {
    priv->active = 1;

    tx_slot = &priv->tx_buf[priv->tx_start];
    rx_slot = &priv->rx_buf[priv->rx_start];

    IOWR_ALTERA_AVALON_DMA_RADDRESS (priv->base, (alt_u32) tx_slot->from);
    IOWR_ALTERA_AVALON_DMA_WADDRESS (priv->base, (alt_u32) rx_slot->data);

    IOWR_ALTERA_AVALON_DMA_LENGTH (priv->base,
       (tx_slot->len > rx_slot->len) ? rx_slot->len : tx_slot->len);
  }
  else
  {
    priv->active = 0;
  }
}

/*
 * alt_avalon_dma_launch_txonly() is called to launch a new transaction when
 * only the transmit channels is using incrementing addresses,
 * i.e. the receive channel is accessing a single memory location (which is
 * probably a device register).
 */

static void alt_avalon_dma_launch_txonly (alt_avalon_dma_priv* priv)
{
  alt_avalon_dma_txslot* tx_slot;

  if (priv->tx_start != priv->tx_end)
  {
    priv->active = 1;
    tx_slot      = &priv->tx_buf[priv->tx_start];

    IOWR_ALTERA_AVALON_DMA_RADDRESS (priv->base, (alt_u32) tx_slot->from);
    IOWR_ALTERA_AVALON_DMA_LENGTH (priv->base, tx_slot->len);
  }
  else
  {
    priv->active = 0;
  }
}

/*
 * alt_avalon_dma_launch_rxonly() is called to launch a new transaction when
 * only the receive channel is using incrementing addresses,
 * i.e. the transmit channel is accessing a single memory location (which is
 * probably a device register).
 */

static void alt_avalon_dma_launch_rxonly (alt_avalon_dma_priv* priv)
{
  alt_avalon_dma_rxslot* rx_slot;

  if (priv->rx_start != priv->rx_end)
  {
    priv->active = 1;
    rx_slot      = &priv->rx_buf[priv->rx_start];

    IOWR_ALTERA_AVALON_DMA_WADDRESS (priv->base, (alt_u32) rx_slot->data);
    IOWR_ALTERA_AVALON_DMA_LENGTH (priv->base, rx_slot->len);
  }
  else
  {
    priv->active = 0;
  }
}

/*
 * alt_avalon_dma_ioctl is used to process ioctl request for a given DMA
 * device instance. See alt_dma_dev.h for the meaning of the supported
 * ioctl requests.
 */

static int alt_avalon_dma_ioctl (alt_avalon_dma_priv* priv, int req, void* arg)
{
  int     status = 0;
  alt_u32 mode;

  /*
   * Ioctl calls should not be made while the device is active (i.e. while there
   * are DMA transfers outstanding), since this can lead to unpredictable
   * behaviour.
   *
   * The test below attempts to trap this error. It is not foolproof,
   * since it cannot catch concurrent calls to alt_alavalon_dma_prepare()
   * or alt_avalon_dma_send(), but it should at least catch the most
   * common class of problems.
   */

  if ((priv->tx_start != priv->tx_end) ||
      (priv->rx_start != priv->rx_end) ||
      IORD_ALTERA_AVALON_DMA_LENGTH (priv->base))
  {
    return -EIO;
  }

  /* Now process the ioctl. */

  switch (req)
  {
  case ALT_DMA_TX_STREAM_ON:
    if (!(priv->flags & ALT_AVALON_DMA_RX_STREAM))
    {
      IOWR_ALTERA_AVALON_DMA_RADDRESS (priv->base, (alt_u32) arg);
      priv->flags |= ALT_AVALON_DMA_TX_STREAM;
      priv->launch = alt_avalon_dma_launch_rxonly;
    }
    else
    {
      status = -EIO;
    }
    break;
  case ALT_DMA_TX_STREAM_OFF:
    priv->flags &= ~ALT_AVALON_DMA_TX_STREAM;
    priv->launch = alt_avalon_dma_launch_bidir;
    break;
  case ALT_DMA_RX_STREAM_ON:
    if (!(priv->flags & ALT_AVALON_DMA_TX_STREAM))
    {
      IOWR_ALTERA_AVALON_DMA_WADDRESS (priv->base, (alt_u32) arg);
      priv->flags |= ALT_AVALON_DMA_RX_STREAM;
      priv->launch = alt_avalon_dma_launch_txonly;
    }
    else
    {
      status = -EIO;
    }
    break;
  case ALT_DMA_RX_STREAM_OFF:
    priv->flags &= ~ALT_AVALON_DMA_RX_STREAM;
    priv->launch = alt_avalon_dma_launch_bidir;
    break;
  case ALT_DMA_SET_MODE_8:
    priv->flags = (priv->flags & ~ALT_AVALON_DMA_MODE_MSK) |
      ALT_AVALON_DMA_MODE_8;
    break;
  case ALT_DMA_SET_MODE_16:
    priv->flags = (priv->flags & ~ALT_AVALON_DMA_MODE_MSK) |
      ALT_AVALON_DMA_MODE_16;
    break;
  case ALT_DMA_SET_MODE_32:
    priv->flags = (priv->flags & ~ALT_AVALON_DMA_MODE_MSK) |
      ALT_AVALON_DMA_MODE_32;
    break;
  case ALT_DMA_SET_MODE_64:
    priv->flags = (priv->flags & ~ALT_AVALON_DMA_MODE_MSK) |
      ALT_AVALON_DMA_MODE_64;
    break;
  case ALT_DMA_SET_MODE_128:
    priv->flags = (priv->flags & ~ALT_AVALON_DMA_MODE_MSK) |
      ALT_AVALON_DMA_MODE_128;
    break;
  default:
    status = -ENOTTY;
  }

  if (!status)
  {
     switch (priv->flags & ALT_AVALON_DMA_MODE_MSK)
     {
     case ALT_AVALON_DMA_MODE_8:
       mode = ALTERA_AVALON_DMA_CONTROL_BYTE_MSK;
       break;
     case ALT_AVALON_DMA_MODE_16:
       mode = ALTERA_AVALON_DMA_CONTROL_HW_MSK;
       break;
     case ALT_AVALON_DMA_MODE_32:
       mode = ALTERA_AVALON_DMA_CONTROL_WORD_MSK;
       break;
     case ALT_AVALON_DMA_MODE_64:
       mode = ALTERA_AVALON_DMA_CONTROL_DWORD_MSK;
       break;
     default:
       mode = ALTERA_AVALON_DMA_CONTROL_QWORD_MSK;
     }

    if (priv->flags & ALT_AVALON_DMA_TX_STREAM)
    {
      IOWR_ALTERA_AVALON_DMA_CONTROL (priv->base,
        mode                               |
        ALTERA_AVALON_DMA_CONTROL_GO_MSK   |
        ALTERA_AVALON_DMA_CONTROL_I_EN_MSK |
        ALTERA_AVALON_DMA_CONTROL_REEN_MSK |
        ALTERA_AVALON_DMA_CONTROL_WEEN_MSK |
        ALTERA_AVALON_DMA_CONTROL_LEEN_MSK |
        ALTERA_AVALON_DMA_CONTROL_RCON_MSK);
    }
    else if (priv->flags & ALT_AVALON_DMA_RX_STREAM)
    {
      IOWR_ALTERA_AVALON_DMA_CONTROL (priv->base,
        mode                               |
        ALTERA_AVALON_DMA_CONTROL_GO_MSK   |
        ALTERA_AVALON_DMA_CONTROL_I_EN_MSK |
        ALTERA_AVALON_DMA_CONTROL_REEN_MSK |
        ALTERA_AVALON_DMA_CONTROL_WEEN_MSK |
        ALTERA_AVALON_DMA_CONTROL_LEEN_MSK |
        ALTERA_AVALON_DMA_CONTROL_WCON_MSK);
    }
    else
    {
      IOWR_ALTERA_AVALON_DMA_CONTROL (priv->base,
        mode                                    |
        ALTERA_AVALON_DMA_CONTROL_GO_MSK        |
        ALTERA_AVALON_DMA_CONTROL_I_EN_MSK      |
        ALTERA_AVALON_DMA_CONTROL_REEN_MSK      |
        ALTERA_AVALON_DMA_CONTROL_WEEN_MSK      |
        ALTERA_AVALON_DMA_CONTROL_LEEN_MSK);
    }
  }

  return status;
}

/*
 * Perform an ioctl request for a transmit channel.
 */

int alt_avalon_dma_tx_ioctl (alt_dma_txchan dma,
           int req,
           void* arg)
{
  return alt_avalon_dma_ioctl (((alt_avalon_dma_txchan*) dma)->priv,
             req,
             arg);
}

/*
 * Perform an ioctl request for a receive channel.
 */

int alt_avalon_dma_rx_ioctl (alt_dma_rxchan dma,
           int req,
           void* arg)
{
  return alt_avalon_dma_ioctl (((alt_avalon_dma_rxchan*) dma)->priv,
             req,
             arg);
}

/*
 * Register a DMA receive requests.
 */

int alt_avalon_dma_prepare (alt_dma_rxchan   dma,
                            void*            data,
                            alt_u32          len,
                            alt_rxchan_done* done,
                            void*            handle)
{
  alt_avalon_dma_rxslot* slot;
  alt_u32                start;
  alt_u32                end;
  alt_u32                next;
  alt_u32                align_mask;
  alt_avalon_dma_priv*   priv;

  priv = ((alt_avalon_dma_rxchan*) dma)->priv;

  /*
   * Ensure that the data is correctly aligned, and that it's not too
   * big for the device.
   */

  align_mask = priv->flags & ALT_AVALON_DMA_MODE_MSK;

  if ((((alt_u32) data) & align_mask) || (len & align_mask) ||
     (len > priv->max_length))
  {
    return -EINVAL;
  }

  start = priv->rx_start;
  end   = priv->rx_end;
  slot  = &priv->rx_buf[end];

  next  = (end + 1) & ALT_AVALON_DMA_NSLOTS_MSK;

  if (next == start)
  {
    return -ENOSPC;
  }

  slot->data   = data;
  slot->len    = len;
  slot->done   = done;
  slot->handle = handle;

  priv->rx_end = next;

  if (!priv->active)
  {
    priv->launch (priv);
  }

  return 0;
}

/*
 * alt_avalon_dma_space() returns the number of tranmit requests that can be
 * posted to the specified DMA transmit channel.
 *
 * A negative value indicates that the value could not be determined.
 */

int alt_avalon_dma_space (alt_dma_txchan dma)
{
  alt_avalon_dma_priv* priv;

  alt_u32 start;
  alt_u32 end;

  priv = ((alt_avalon_dma_txchan*) dma)->priv;

  start = priv->tx_start;
  end   = priv->tx_end;

  return (start > end) ? start - end - 1 :
    ALT_AVALON_DMA_NSLOTS + start - end - 1;
}

/*
 * alt_avalon_dma_send() posts a transmit request to a DMA transmit channel.
 * The input arguments are:
 *
 * dma: the channel to use.
 * from: a pointer to the start of the data to send.
 * length: the length of the data to send in bytes.
 * done: callback function that will be called once the data has been sent.
 * handle: opaque value passed to "done".
 *
 * The return value will be negative if the request cannot be posted, and
 * zero otherwise.
 */

int alt_avalon_dma_send (alt_dma_txchan   dma,
                         const void*      from,
                         alt_u32          len,
                         alt_txchan_done* done,
                         void*            handle)
{
  alt_avalon_dma_txslot* slot;
  alt_u32                start;
  alt_u32                end;
  alt_u32                align_mask;
  alt_u32                next;
  alt_avalon_dma_priv*   priv;

  priv = ((alt_avalon_dma_txchan*) dma)->priv;

  /*
   * Ensure that the data is correctly aligned, and not too large
   * for the device
   */

  align_mask = priv->flags & ALT_AVALON_DMA_MODE_MSK;

  if ((((alt_u32) from) & align_mask) ||
        (len & align_mask)            ||
        (len > priv->max_length))
  {
    return -EINVAL;
  }

  start = priv->tx_start;
  end   = priv->tx_end;
  slot  = &priv->tx_buf[end];
  next  = (end + 1) & ALT_AVALON_DMA_NSLOTS_MSK;

  if (next == start)
  {
    return -ENOSPC;
  }

  /* Fill in the descriptor */

  slot->from   = from;
  slot->len    = len;
  slot->done   = done;
  slot->handle = handle;

  priv->tx_end = next;

  if (!priv->active)
  {
    priv->launch (priv);
  }

  return 0;
}

/*
 * Process a received interrupt. Interrupts are generated when a transaction
 * completes.
 */
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void alt_avalon_dma_irq (void* context)
#else
static void alt_avalon_dma_irq (void* context, alt_u32 id)
#endif
{
  alt_avalon_dma_priv* priv = (alt_avalon_dma_priv*) context;

  alt_avalon_dma_txslot* tx_slot;
  alt_avalon_dma_rxslot* rx_slot;
  alt_u32                tx_start;
  alt_u32                rx_start;
  int                    stream_tx;
  int                    stream_rx;
  alt_irq_context        cpu_sr;

  stream_tx = priv->flags & ALT_AVALON_DMA_TX_STREAM;
  stream_rx = priv->flags & ALT_AVALON_DMA_RX_STREAM;

  tx_start = priv->tx_start;
  rx_start = priv->rx_start;

  tx_slot = &priv->tx_buf[tx_start];
  rx_slot = &priv->rx_buf[rx_start];

  /* Increment the descriptors */

  if (!stream_tx)
  {
    priv->tx_start = (tx_start + 1) & ALT_AVALON_DMA_NSLOTS_MSK;
  }
  if (!stream_rx)
  {
    priv->rx_start = (rx_start + 1) & ALT_AVALON_DMA_NSLOTS_MSK;
  }

  /* clear the interrupt */

  IOWR_ALTERA_AVALON_DMA_STATUS (priv->base, 0);
  
  /* Dummy read to ensure IRQ is cleared prior to ISR completion*/
  IORD_ALTERA_AVALON_DMA_STATUS (priv->base);

  /* launch the next transaction */

  priv->launch (priv);

  /* 
   * Make the callbacks 
   *
   * Other interrupts are explicitly disabled if callbacks
   * are registered because there is no guarantee that they are 
   * preemption-safe. This allows the DMA driver to support 
   * interrupt preemption.
   */
  if (!stream_tx && tx_slot->done)
  {
    cpu_sr = alt_irq_disable_all();
    tx_slot->done (tx_slot->handle);
    alt_irq_enable_all(cpu_sr);
    
  }
  if (!stream_rx && rx_slot->done)
  {
    cpu_sr = alt_irq_disable_all();
    rx_slot->done (rx_slot->handle, rx_slot->data);
    alt_irq_enable_all(cpu_sr);
  }
}

/*
 * Initialise and register the transmit and receive channels for a given
 * physical DMA device.
 */

void alt_avalon_dma_init (alt_avalon_dma_txchan* tx,
                          alt_avalon_dma_rxchan* rx,
                          void*                  base,
                          alt_u32                ic_id,
                          alt_u32                irq)
{

  /* Halt any current transactions (reset the device) */
  IOWR_ALTERA_AVALON_DMA_CONTROL (base, ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_MSK);
  IOWR_ALTERA_AVALON_DMA_CONTROL (base, ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_MSK);

  /* Set the default mode of the device (32 bit block reads and writes from/to memory). */
  IOWR_ALTERA_AVALON_DMA_CONTROL (base,
                          ALTERA_AVALON_DMA_CONTROL_WORD_MSK      |
                          ALTERA_AVALON_DMA_CONTROL_GO_MSK        |
                          ALTERA_AVALON_DMA_CONTROL_I_EN_MSK      |
                          ALTERA_AVALON_DMA_CONTROL_REEN_MSK      |
                          ALTERA_AVALON_DMA_CONTROL_WEEN_MSK      |
                          ALTERA_AVALON_DMA_CONTROL_LEEN_MSK);

  /* Clear any pending interrupts and the DONE flag */
  IOWR_ALTERA_AVALON_DMA_STATUS (base, 0);

  /*
   * Register the interrupt handler, and make the device available to the
   * system.
   */
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
  if (alt_ic_isr_register(ic_id, irq, alt_avalon_dma_irq, tx->priv, 0x0) >= 0)
#else
  if (alt_irq_register (irq, tx->priv, alt_avalon_dma_irq) >= 0)
#endif  
  {
    alt_dma_txchan_reg (&tx->dev);
    alt_dma_rxchan_reg (&rx->dev);
  }
}
