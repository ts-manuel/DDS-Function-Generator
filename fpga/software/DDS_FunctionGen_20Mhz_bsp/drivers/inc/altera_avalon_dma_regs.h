/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003 Altera Corporation, San Jose, California, USA.           *
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

#ifndef __ALTERA_AVALON_DMA_REGS_H__
#define __ALTERA_AVALON_DMA_REGS_H__

#include <io.h>

#define IOADDR_ALTERA_AVALON_DMA_STATUS(base)       __IO_CALC_ADDRESS_NATIVE(base, 0)
#define IORD_ALTERA_AVALON_DMA_STATUS(base)         IORD(base, 0)
#define IOWR_ALTERA_AVALON_DMA_STATUS(base, data)   IOWR(base, 0, data)

#define ALTERA_AVALON_DMA_STATUS_DONE_MSK           (0x1)
#define ALTERA_AVALON_DMA_STATUS_DONE_OFST          (0)
#define ALTERA_AVALON_DMA_STATUS_BUSY_MSK           (0x2)
#define ALTERA_AVALON_DMA_STATUS_BUSY_OFST          (1)
#define ALTERA_AVALON_DMA_STATUS_REOP_MSK           (0x4)
#define ALTERA_AVALON_DMA_STATUS_REOP_OFST          (2)
#define ALTERA_AVALON_DMA_STATUS_WEOP_MSK           (0x8)
#define ALTERA_AVALON_DMA_STATUS_WEOP_OFST          (3)
#define ALTERA_AVALON_DMA_STATUS_LEN_MSK            (0x10)
#define ALTERA_AVALON_DMA_STATUS_LEN_OFST           (4)

#define IOADDR_ALTERA_AVALON_DMA_RADDRESS(base)     __IO_CALC_ADDRESS_NATIVE(base, 1)
#define IORD_ALTERA_AVALON_DMA_RADDRESS(base)       IORD(base, 1)
#define IOWR_ALTERA_AVALON_DMA_RADDRESS(base, data) IOWR(base, 1, data)

#define IOADDR_ALTERA_AVALON_DMA_WADDRESS(base)     __IO_CALC_ADDRESS_NATIVE(base, 2)
#define IORD_ALTERA_AVALON_DMA_WADDRESS(base)       IORD(base, 2)
#define IOWR_ALTERA_AVALON_DMA_WADDRESS(base, data) IOWR(base, 2, data)

#define IOADDR_ALTERA_AVALON_DMA_LENGTH(base)       __IO_CALC_ADDRESS_NATIVE(base, 3)
#define IORD_ALTERA_AVALON_DMA_LENGTH(base)         IORD(base, 3)
#define IOWR_ALTERA_AVALON_DMA_LENGTH(base, data)   IOWR(base, 3, data)

#define IOADDR_ALTERA_AVALON_DMA_CONTROL(base)      __IO_CALC_ADDRESS_NATIVE(base, 6)
#define IORD_ALTERA_AVALON_DMA_CONTROL(base)        IORD(base, 6)
#define IOWR_ALTERA_AVALON_DMA_CONTROL(base, data)  IOWR(base, 6, data)

#define ALTERA_AVALON_DMA_CONTROL_BYTE_MSK             (0x1)
#define ALTERA_AVALON_DMA_CONTROL_BYTE_OFST            (0)
#define ALTERA_AVALON_DMA_CONTROL_HW_MSK               (0x2)
#define ALTERA_AVALON_DMA_CONTROL_HE_OFST              (1)
#define ALTERA_AVALON_DMA_CONTROL_WORD_MSK             (0x4)
#define ALTERA_AVALON_DMA_CONTROL_WORD_OFST            (2)
#define ALTERA_AVALON_DMA_CONTROL_GO_MSK               (0x8)
#define ALTERA_AVALON_DMA_CONTROL_GO_OFST              (3)
#define ALTERA_AVALON_DMA_CONTROL_I_EN_MSK             (0x10)
#define ALTERA_AVALON_DMA_CONTROL_I_EN_OFST            (4)
#define ALTERA_AVALON_DMA_CONTROL_REEN_MSK             (0x20)
#define ALTERA_AVALON_DMA_CONTROL_REEN_OFST            (5)
#define ALTERA_AVALON_DMA_CONTROL_WEEN_MSK             (0x40)
#define ALTERA_AVALON_DMA_CONTROL_WEEN_OFST            (6)
#define ALTERA_AVALON_DMA_CONTROL_LEEN_MSK             (0x80)
#define ALTERA_AVALON_DMA_CONTROL_LEEN_OFST            (7)
#define ALTERA_AVALON_DMA_CONTROL_RCON_MSK             (0x100)
#define ALTERA_AVALON_DMA_CONTROL_RCON_OFST            (8)
#define ALTERA_AVALON_DMA_CONTROL_WCON_MSK             (0x200)
#define ALTERA_AVALON_DMA_CONTROL_WCON_OFST            (9)
#define ALTERA_AVALON_DMA_CONTROL_DWORD_MSK            (0x400)
#define ALTERA_AVALON_DMA_CONTROL_DWORD_OFST           (10)
#define ALTERA_AVALON_DMA_CONTROL_QWORD_MSK            (0x800)
#define ALTERA_AVALON_DMA_CONTROL_QWORD_OFST           (11)
#define ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_MSK    (0x1000)
#define ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_OFST   (12)

#endif /* __ALTERA_AVALON_DMA_REGS_H__ */
