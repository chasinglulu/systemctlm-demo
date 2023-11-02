/* SPDX-License-Identifier: GPL-2.0
 *
 * Hobot SystemC/TLM-2.0 Sigi Wrapper.
 *
 * Copyright (C) 2023, Charleye<wangkart@aliyun.com>
 * All rights reserved.
 *
 * Borrowed zynqmp_demo code
 *
 */

// #include <inttypes.h>
#include <stdio.h>
// #include <signal.h>
// #include <unistd.h>

#include <systemc>
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "sigi/hobot-sigi.h"
#include "xilinx-axidma.h"
#include "soc/interconnect/iconnect.h"
#include "tests/test-modules/memory.h"

// #include "tlm-bridges/tlm2apb-bridge.h"
#include "tlm-bridges/tlm2axis-bridge.h"
#include "tlm-bridges/axis2tlm-bridge.h"

// #include "tlm-uart-phy.h"

#ifdef HAVE_VERILOG_VERILATOR
#include "Vuart.h"
#include "verilated.h"
#endif

#define NR_MASTERS	3
#define NR_DEVICES	3

SC_MODULE(Top)
{
	SC_HAS_PROCESS(Top);
	iconnect<NR_MASTERS, NR_DEVICES> *bus;
	hobot_sigi soc;

	axidma_mm2s dma_mm2s_A;
	axidma_s2mm dma_s2mm_C;

	tlm2axis_bridge<8> tlm2axis;
	axis2tlm_bridge<8> axis2tlm;

	sc_signal<bool> rst, rst_n;
	sc_clock *clk;

	Vuart *uart;
	// tlm_uart_phy phy;

	sc_signal<sc_bv<8> > rx_axis_tdata;
	sc_signal<sc_bv<1> > rx_axis_tstrb;
	sc_signal<bool> rx_axis_tvaild;
	sc_signal<bool> rx_axis_tready;
	sc_signal<bool> rx_axis_tlast;
	sc_signal<bool> rx_axis_tuser;
	sc_signal<bool> rxd;
	sc_signal<bool> rx_busy;
	sc_signal<bool> rx_overrun_error;
	sc_signal<bool> rx_frame_error;

	sc_signal<sc_bv<8> > tx_axis_tdata;
	sc_signal<sc_bv<1> > tx_axis_tstrb;
	sc_signal<bool> tx_axis_tvaild;
	sc_signal<bool> tx_axis_tready;
	sc_signal<bool> tx_axis_tlast;
	sc_signal<bool> tx_axis_tuser;
	sc_signal<bool> txd;
	sc_signal<bool> tx_busy;

	sc_signal<bool> wire;

	sc_signal<sc_bv<16> > prescale;

	void gen_rst_n(void)
	{
		rst_n.write(!rst.read());
	}

	Top(sc_module_name name, const char *sk_desc, sc_time quantum) :
		soc("sigi", sk_desc, remoteport_tlm_sync_untimed_ptr),
		dma_mm2s_A("dma_mm2s_A"),
		dma_s2mm_C("dma_s2mm_C"),
		tlm2axis("tlm2axis"),
		axis2tlm("axis2tlm"),
		rst("rst"),
		rst_n("rst_n"),
		// phy("phy"),
		rx_axis_tdata("rx_axis_tdata"),
		rx_axis_tstrb("rx_axis_tstrb"),
		rx_axis_tvaild("rx_axis_tvaild"),
		rx_axis_tready("rx_axis_tready"),
		rx_axis_tlast("rx_axis_tlast"),
		rx_axis_tuser("rx_axis_tuser"),
		rxd("rxd"),
		rx_busy("rx_busy"),
		rx_overrun_error("rx_overrun_error"),
		rx_frame_error("rx_frame_error"),
		tx_axis_tdata("tx_axis_tdata"),
		tx_axis_tstrb("tx_axis_tstrb"),
		tx_axis_tvaild("tx_axis_tvaild"),
		tx_axis_tready("tx_axis_tready"),
		tx_axis_tlast("tx_axis_tlast"),
		tx_axis_tuser("tx_axis_tuser"),
		txd("txd"),
		tx_busy("tx_busy"),
		wire("wire"),
		prescale("prescale")
	{
		SC_METHOD(gen_rst_n);
		sensitive << rst;

		m_qk.set_global_quantum(quantum);

		soc.rst(rst);

		bus = new iconnect<NR_MASTERS, NR_DEVICES> ("bus");

		bus->memmap(0x60004000, 0x100 - 1, ADDRMODE_RELATIVE, -1, dma_mm2s_A.tgt_socket);
		bus->memmap(0x60005000, 0x100 - 1, ADDRMODE_RELATIVE, -1, dma_s2mm_C.tgt_socket);
		bus->memmap(0x0UL, 0xffffffff - 1, ADDRMODE_RELATIVE, -1, *(soc.s_axi_msp[0]));

		cout << "Hello World" << endl;

		// Entry Point
		soc.s_axi_mmp[0]->bind(*(bus->t_sk[0]));

		// useless connection
		dma_mm2s_A.init_socket.bind(*(bus->t_sk[1]));

		dma_s2mm_C.init_socket.bind(*(bus->t_sk[2]));

		dma_mm2s_A.stream_socket.bind(tlm2axis.tgt_socket);
		axis2tlm.socket.bind(dma_s2mm_C.stream_socket);

		dma_mm2s_A.irq(soc.d2h_irq[0]);
		dma_s2mm_C.irq(soc.d2h_irq[1]);

		clk = new sc_clock("clk", sc_time(10, SC_US));
		uart = new Vuart("uart");

		tlm2axis.clk(*clk);
		tlm2axis.resetn(rst_n);
		tlm2axis.tvalid(tx_axis_tvaild);
		tlm2axis.tdata(tx_axis_tdata);
		tlm2axis.tstrb(tx_axis_tstrb);
		tlm2axis.tready(tx_axis_tready);
		tlm2axis.tlast(tx_axis_tlast);
		tlm2axis.tuser(tx_axis_tuser);

		axis2tlm.clk(*clk);
		axis2tlm.resetn(rst_n);
		axis2tlm.tdata(rx_axis_tdata);
		axis2tlm.tstrb(rx_axis_tstrb);
		axis2tlm.tvalid(rx_axis_tvaild);
		axis2tlm.tready(rx_axis_tready);
		axis2tlm.tlast(rx_axis_tlast);
		axis2tlm.tuser(rx_axis_tuser);

		uart->clk(*clk);
		uart->rst(rst_n);
		uart->prescale(prescale);
		uart->s_axis_tdata(tx_axis_tdata);
		uart->s_axis_tvalid(tx_axis_tvaild);
		uart->s_axis_tready(tx_axis_tready);
		uart->m_axis_tdata(rx_axis_tdata);
		uart->m_axis_tvalid(rx_axis_tvaild);
		uart->m_axis_tready(rx_axis_tready);
		uart->tx_busy(tx_busy);
		uart->rx_busy(rx_busy);
		uart->rx_frame_error(rx_frame_error);
		uart->rx_overrun_error(rx_overrun_error);

		// loopback mode
		uart->txd(wire);
		uart->rxd(wire);

		// uart->txd(txd);
		// uart->rxd(rxd);

		// phy.rx.clk(*clk);
		// phy.rx.rxd(rxd);
		// phy.rx.rx_busy(rx_busy);

		// phy.tx.clk(*clk);
		// phy.tx.txd(txd);
		// phy.tx.tx_busy(tx_busy);

		soc.tie_off();
	}

private:
	tlm_utils::tlm_quantumkeeper m_qk;
};

void usage(void)
{
	cout << "tlm socket-path sync-quantum-ns" << endl;
}

int sc_main(int argc, char* argv[])
{
	Top *top;
	uint64_t sync_quantum;
	sc_trace_file *trace_fp = NULL;

#if HAVE_VERILOG_VERILATOR
	Verilated::commandArgs(argc, argv);
#endif

	if (argc < 3) {
		sync_quantum = 10000;
	} else {
		sync_quantum = strtoull(argv[2], NULL, 10);
	}

	sc_set_time_resolution(1, SC_PS);

	top = new Top("top", argv[1], sc_time((double) sync_quantum, SC_NS));

	if (argc < 3) {
		sc_start(1, SC_PS);
		usage();
		sc_stop();
		exit(EXIT_FAILURE);
	}

	trace_fp = sc_create_vcd_trace_file("trace");
	//trace(trace_fp, *top, top->name());
	/* Pull the reset signal.  */
	top->rst.write(true);
	sc_start(1, SC_US);
	top->rst.write(false);

	sc_start();
	if (trace_fp) {
		sc_close_vcd_trace_file(trace_fp);
	}
	return 0;

	return 0;
}
