# SPDX-License-Identifier: GPL-2.0
#
# Cosim Makefiles
#
# Copyright (C) 2023, Charleye<wangkart@aliyun.com>
#
# Makefile fragment listing all verilog-uart verilog files.
#

VUART_CORE += $(VUART_DIR)/uart.v
VUART_CORE += $(VUART_DIR)/uart_rx.v
VUART_CORE += $(VUART_DIR)/uart_tx.v