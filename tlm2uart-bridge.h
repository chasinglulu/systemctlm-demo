/* SPDX-License-Identifier: GPL-2.0
 *
 * TLM Generic-Payload Streams to UART RX bridge.
 *
 * Copyright (C) 2023, Charleye<wangkart@aliyun.com>
 * All rights reserved.
 *
 */

#ifndef __TLM2UART_BRIDGE_H__
#define __TLM2UART_BRIDGE_H__

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>

class tlm2uart_bridge
: public sc_core::sc_module
{
public:
	tlm_utils::simple_target_socket<tlm2uart_bridge> tgt_socket;

	sc_in<bool> clk;
	sc_out<bool> rxd;
	sc_in<bool> rx_busy;

	tlm2uart_bridge(sc_core::sc_module_name name);
};

tlm2uart_bridge::tlm2uart_bridge(sc_core::sc_module_name name)
	: sc_module(name),
	tgt_socket("tgt_socket"),
	clk("clk"),
	rxd("rxd"),
	rx_busy("rx_busy")
{

}

#endif