# SPDX-License-Identifier: GPL-2.0
#
# Cosim Makefiles
#
# Copyright (C) 2023, Charleye<wangkart@aliyun.com>
#
# Borrowed systemctlm-cosim-demo Makefile
#

INSTALL_DIR ?= $(CURDIR)/install
DESTDIR ?= $(CURDIR)
SC_DIR ?= $(CURDIR)/libsystemctlm-soc
RP_DIR ?= $(SC_DIR)/libremote-port
RTL_DIR ?= $(CURDIR)/verilog-uart/rtl

SYSTEMC_DIR ?= /usr/local
SYSTEMC_INCLUDE ?= $(SYSTEMC_DIR)/include/
SYSTEMC_LIBDIR ?= $(SYSTEMC_DIR)/lib-linux64

CXXFLAGS += -std=c++17 -Wall -Wextra -Wno-array-bounds
CXXFLAGS += -fPIC -Wno-maybe-uninitialized
CXXFLAGS += -I$(CURDIR)
CXXFLAGS += -I$(SYSTEMC_INCLUDE)
CXXFLAGS += -DHAVE_VERILOG_VERILATOR
CXXFLAGS += -I$(SC_DIR)
CXXFLAGS += -I$(SC_DIR)/sigi
CXXFLAGS += -I$(SC_DIR)/tests/test-modules
CXXFLAGS += -I$(SC_DIR)/libremote-port

SRCS += $(CURDIR)/sigi-demo.cc
SRCS += $(CURDIR)/xilinx-axidma.cc
SRCS += $(CURDIR)/trace.cc

SC_SRCS += $(SC_DIR)/sigi/hobot-sigi.cc
SC_SRCS += $(SC_DIR)/tests/test-modules/memory.cc
SC_SRCS += $(RP_DIR)/remote-port-tlm.cc
SC_SRCS += $(RP_DIR)/remote-port-tlm-wires.cc
SC_SRCS += $(RP_DIR)/remote-port-tlm-memory-master.cc
SC_SRCS += $(RP_DIR)/remote-port-tlm-memory-slave.cc
SC_SRCS += $(RP_DIR)/remote-port-tlm-pci-ep.cc
SC_SRCS += $(RP_DIR)/remote-port-tlm-ats.cc
SC_SRCS += $(RP_DIR)/remote-port-sk.c
SC_SRCS += $(RP_DIR)/remote-port-proto.c
SC_SRCS += $(RP_DIR)/safeio.c

VL_FILES += apb_timer.v
VL_ENV += SYSTEMC_INCLUDE=$(SYSTEMC_INCLUDE) SYSTEMC_LIBDIR=$(SYSTEMC_LIBDIR)

# Gives some compatibility with vcs
VL_FLAGS += --exe --pins-bv 2 -Wno-fatal
VL_FLAGS += --output-split-cfuncs 500
VL_FLAGS += -I$(RTL_DIR)
VL_FLAGS += $(VL_FILES)
VL_FLAGS += --sc uart --top-module uart

TARGETS = $(DESTDIR)/sigi_demo

all: $(TARGETS)

$(TARGETS): $(SC_SRCS) $(SRCS) $(CURDIR)/sigi-demo.cc
	$(VL_ENV) verilator --build $(VL_FLAGS) $^ -CFLAGS '$(CXXFLAGS)' --Mdir $(DESTDIR)/obj_dir -o $@

clean:
	$(RM) -r $(DESTDIR)/obj_dir
	$(RM) $(TARGETS)