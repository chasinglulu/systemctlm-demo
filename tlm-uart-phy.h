/* SPDX-License-Identifier: GPL-2.0
 *
 * Partial model of a TLM UART PHY.
 *
 * Copyright (C) 2023, Charleye<wangkart@aliyun.com>
 * All rights reserved.
 *
*/

#ifndef __TLM_UART_PHY_H__
#define __TLM_UART_PHY_H__

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>

#include "tlm2uart-bridge.h"
#include "uart2tlm-bridge.h"

class tlm_uart_phy
: public sc_core::sc_module
{
public:
	tlm2uart_bridge rx;
	uart2tlm_bridge tx;

	tlm_uart_phy(sc_core::sc_module_name name);
};

tlm_uart_phy::tlm_uart_phy(sc_module_name name)
	: sc_module(name),
	  rx("rx"),
	  tx("tx")
{
}

#endif