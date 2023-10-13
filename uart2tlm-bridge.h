/* SPDX-License-Identifier: GPL-2.0
 *
 * UART TX to TLM Generic-Payload Streams bridge.
 *
 * Copyright (C) 2023, Charleye<wangkart@aliyun.com>
 * All rights reserved.
 *
 */

#ifndef __UART2TLM_BRIDGE_H__
#define __UART2TLM_BRIDGE_H__

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>

class uart2tlm_bridge
: public sc_core::sc_module
{
public:
	tlm_utils::simple_initiator_socket<uart2tlm_bridge> init_socket;

	sc_in<bool> clk;
	sc_in<bool> txd;
	sc_in<bool> tx_busy;

	uart2tlm_bridge(sc_core::sc_module_name name);
};

uart2tlm_bridge::uart2tlm_bridge(sc_core::sc_module_name name)
	: sc_module(name),
	init_socket("init_socket"),
	clk("clk"),
	txd("txd"),
	tx_busy("tx_busy")
{

}

#endif