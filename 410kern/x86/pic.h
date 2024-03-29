/** @file 410kern/inc/x86/pic.h
 *  @brief Describes the Programmable Interrupt Controllers frequently
 *         associated with X86 machines.
 *
 *  @note Define X86_PIC_DEFINITIONS before including this file if you
 *        abolutely must have the full definitions in place.  This file
 *        is more usually included for the API it defines, not its constants.
 */

/* Because this obnoxious ball of wax is just a lot (and boy do we mean
 * a lot) of definitions, it's been lifted from gnumach, under the CMU
 * license.  Details follow.  Let this be a lesson to you about the
 * longevity of boring code!
 *
 * Modified by nwf@cs.jhu.edu for CMU 15-410, S2008.
 * Modified again by nwf@cs.jhu.edu for CMU 14-410, S2010.  The OCW2
 * values contained herein were wrong.  Let _that_ be a lesson to you!
 */
/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
Copyright (c) 1988,1989 Prime Computer, Inc.  Natick, MA 01760
All Rights Reserved.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and
without fee is hereby granted, provided that the above
copyright notice appears in all copies and that both the
copyright notice and this permission notice appear in
supporting documentation, and that the name of Prime
Computer, Inc. not be used in advertising or publicity
pertaining to distribution of the software without
specific, written prior permission.

THIS SOFTWARE IS PROVIDED "AS IS", AND PRIME COMPUTER,
INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN
NO EVENT SHALL PRIME COMPUTER, INC.  BE LIABLE FOR ANY
SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR
OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef X86_PIC_H
#define X86_PIC_H

#ifdef X86_PIC_DEFINITIONS

/*
** The following are definitions used to locate the PICs in the system
*/

#define ADDR_PIC_BASE           0x20
#define OFF_ICW                 0x00
#define OFF_OCW                 0x01
#define SIZE_PIC                0x80

#define MASTER_ICW              (ADDR_PIC_BASE + OFF_ICW)
#define MASTER_OCW              (ADDR_PIC_BASE + OFF_OCW)
#define  SLAVE_ICW              (ADDR_PIC_BASE + OFF_ICW + SIZE_PIC)
#define  SLAVE_OCW              (ADDR_PIC_BASE + OFF_OCW + SIZE_PIC)

/*
** The following banks of definitions ICW1, ICW2, ICW3, and ICW4 are used
** to define the fields of the various ICWs for initialisation of the PICs
*/

/*
**      ICW1
*/

#define ICW_TEMPLATE            0x10

#define LEVL_TRIGGER            0x08
#define EDGE_TRIGGER            0x00
#define ADDR_INTRVL4            0x04
#define ADDR_INTRVL8            0x00
#define SINGLE__MODE            0x02
#define CASCADE_MODE            0x00
#define ICW4__NEEDED            0x01
#define NO_ICW4_NEED            0x00

/*
**      ICW2 is used to hold the interrupt base.
*/

/*
**      ICW3
*/

#define SLAVE_ON_IR0            0x01
#define SLAVE_ON_IR1            0x02
#define SLAVE_ON_IR2            0x04
#define SLAVE_ON_IR3            0x08
#define SLAVE_ON_IR4            0x10
#define SLAVE_ON_IR5            0x20
#define SLAVE_ON_IR6            0x40
#define SLAVE_ON_IR7            0x80

#define I_AM_SLAVE_0            0x00
#define I_AM_SLAVE_1            0x01
#define I_AM_SLAVE_2            0x02
#define I_AM_SLAVE_3            0x03
#define I_AM_SLAVE_4            0x04
#define I_AM_SLAVE_5            0x05
#define I_AM_SLAVE_6            0x06
#define I_AM_SLAVE_7            0x07

/*
**      ICW4
*/

#define SNF_MODE_ENA            0x10
#define SNF_MODE_DIS            0x00
#define BUFSLAV_MODE            0x08
#define BUFMSTR_MODE            0x0C
#define NONBUFD_MODE            0x00
#define AUTO_EOI_MOD            0x02
#define NRML_EOI_MOD            0x00
#define I8086_EMM_MOD           0x01
#define SET_MCS_MODE            0x00

/*
**      OCW1
*/

#define PICM_MASK               0xFF
#define PICS_MASK               0xFF

/*
**      OCW2
*/

#define NON_SPEC_EOI            0x20
#define SPECIFIC_EOI            0x60
#define ROT_NON_SPEC            0xA0
#define SET_ROT_AEOI            0x80
#define RSET_ROTAEOI            0x00
#define ROT_SPEC_EOI            0xE0
#define SET_PRIORITY            0xC0
#define NO_OPERATION            0x40

#define SEND_EOI_IR0            0x00
#define SEND_EOI_IR1            0x01
#define SEND_EOI_IR2            0x02
#define SEND_EOI_IR3            0x03
#define SEND_EOI_IR4            0x04
#define SEND_EOI_IR5            0x05
#define SEND_EOI_IR6            0x06
#define SEND_EOI_IR7            0x07

/*
**      OCW3
*/

#define OCW_TEMPLATE            0x08
#define SPECIAL_MASK            0x40
#define MASK_MDE_SET            0x20
#define MASK_MDE_RST            0x00
#define POLL_COMMAND            0x04
#define NO_POLL_CMND            0x00
#define READ_NEXT_RD            0x02
#define READ_IR_ONRD            0x00
#define READ_IS_ONRD            0x01

/*
**      Standard PIC initialization values for PCs.
*/
#define PICM_ICW1       (ICW_TEMPLATE | EDGE_TRIGGER | ADDR_INTRVL8 \
                         | CASCADE_MODE | ICW4__NEEDED)
#define PICM_ICW3       (SLAVE_ON_IR2)
#define PICM_ICW4       (SNF_MODE_DIS | NONBUFD_MODE | NRML_EOI_MOD \
                         | I8086_EMM_MOD)

#define PICS_ICW1       (ICW_TEMPLATE | EDGE_TRIGGER | ADDR_INTRVL8 \
                         | CASCADE_MODE | ICW4__NEEDED)
#define PICS_ICW3       (I_AM_SLAVE_2)
#define PICS_ICW4       (SNF_MODE_DIS | NONBUFD_MODE | NRML_EOI_MOD \
                         | I8086_EMM_MOD)

#endif /* X86_PIC_DEFINITIONS */

    /** @brief Default location of the master PIC's interrupts in the IDT */
#define X86_PIC_MASTER_IRQ_BASE     0x20
    /** @brief Default location of the slave  PIC's interrupts in the IDT */
#define X86_PIC_SLAVE_IRQ_BASE      0x28

#ifndef ASSEMBLER

void pic_init( unsigned char master_base, unsigned char slave_base );
void pic_acknowledge( unsigned char irq );
void pic_acknowledge_any_master(void);
void pic_acknowledge_any_slave(void);
int pic_irq_active( unsigned char irq );

#endif  /* !ASSEMBLER */

#endif  /* !X86_PIC_H */
