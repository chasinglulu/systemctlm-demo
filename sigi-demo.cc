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

#include "soc/interconnect/iconnect.h"
#include "tests/test-modules/memory.h"

#include "checkers/pc-axilite.h"
#include "tlm-bridges/tlm2axilite-bridge.h"
#include "tlm-bridges/tlm2axi-bridge.h"
#include "tlm-bridges/tlm2apb-bridge.h"


int sc_main(int argc, char* argv[])
{

	// sc_set_time_resolution(1, SC_PS);

	// if (argc < 3) {
	// 	sc_start(1, SC_PS);
	// 	sc_stop();
	// 	exit(EXIT_FAILURE);
	// }

	printf("exit\n");
	sc_start(1, SC_US);

	return 0;
}
