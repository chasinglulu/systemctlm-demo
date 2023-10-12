# SPDX-License-Identifier: GPL-2.0
#
# Cosim Makefiles
#
# Copyright (C) 2023, Charleye<wangkart@aliyun.com>
#
# Borrowed systemctlm-cosim-demo Makefile
#

-include .config.mk

INSTALL ?= install

SYSTEMC ?= /usr/local
SYSTEMC_INCLUDE ?= $(SYSTEMC)/include/
SYSTEMC_LIBDIR ?= $(SYSTEMC)/lib-linux64

HAVE_VERILOG ?=n
HAVE_VERILOG_VERILATOR ?=y
HAVE_VERILOG_VCS ?=n

CFLAGS += -Wall -O2 -g
CXXFLAGS += -Wall -O2 -g

ifneq "$(SYSTEMC_INCLUDE)" ""
CPPFLAGS += -I $(SYSTEMC_INCLUDE)
endif

CPPFLAGS += -I .
LDFLAGS  += -L $(SYSTEMC_LIBDIR) -Wl,-rpath $(SYSTEMC_LIBDIR)
#LDLIBS += -pthread -Wl,-Bstatic -lsystemc -Wl,-Bdynamic
LDLIBS   += -pthread -lsystemc

SIGI_TOP_C = sigi-demo.cc
SIGI_TOP_O = $(SIGI_TOP_C:.cc=.o)

SIGI_OBJS += $(SIGI_TOP_O)

# SC_OBJS += trace.o
# SC_OBJS += debugdev.o
# SC_OBJS += demo-dma.o
# SC_OBJS += sigi-axidma.o

LIBSOC_PATH=libsystemctlm-soc
CPPFLAGS += -I $(LIBSOC_PATH)

LIBSOC_SIGI_PATH=$(LIBSOC_PATH)/sigi
SC_OBJS += $(LIBSOC_SIGI_PATH)/hobot-sigi.o
CPPFLAGS += -I $(LIBSOC_SIGI_PATH)

CPPFLAGS += -I $(LIBSOC_PATH)/tests/test-modules/
SC_OBJS += $(LIBSOC_PATH)/tests/test-modules/memory.o

LIBRP_PATH=$(LIBSOC_PATH)/libremote-port
C_OBJS += $(LIBRP_PATH)/safeio.o
C_OBJS += $(LIBRP_PATH)/remote-port-proto.o
C_OBJS += $(LIBRP_PATH)/remote-port-sk.o
SC_OBJS += $(LIBRP_PATH)/remote-port-tlm.o
SC_OBJS += $(LIBRP_PATH)/remote-port-tlm-memory-master.o
SC_OBJS += $(LIBRP_PATH)/remote-port-tlm-memory-slave.o
SC_OBJS += $(LIBRP_PATH)/remote-port-tlm-wires.o
SC_OBJS += $(LIBRP_PATH)/remote-port-tlm-ats.o
SC_OBJS += $(LIBRP_PATH)/remote-port-tlm-pci-ep.o
CPPFLAGS += -I $(LIBRP_PATH)

VENV=SYSTEMC_INCLUDE=$(SYSTEMC_INCLUDE) SYSTEMC_LIBDIR=$(SYSTEMC_LIBDIR)
VOBJ_DIR=obj_dir
VFILES=apb_timer.v

ifeq "$(HAVE_VERILOG_VERILATOR)" "y"
#VERILATOR_ROOT?=/usr/share/verilator
VERILATOR_ROOT?=/usr/local/share/verilator
VERILATOR=verilator

VM_SC?=1
VM_TRACE?=0
VM_COVERAGE?=0
#V_LDLIBS += $(VOBJ_DIR)/Vapb_timer__ALL.a
# V_LDLIBS += $(VOBJ_DIR)/Vaxilite_dev__ALL.a
# V_LDLIBS += $(VOBJ_DIR)/Vaxifull_dev__ALL.a
LDLIBS += $(V_LDLIBS)
VERILATED_O=verilated.o
VERILATED_O+=verilated_threads.o

# Gives some compatibility with vcs
VFLAGS += --pins-bv 2 -Wno-fatal
VFLAGS += --output-split-cfuncs 500

VFLAGS+=--sc --Mdir $(VOBJ_DIR)
VFLAGS += -CFLAGS "-DHAVE_VERILOG" -CFLAGS "-DHAVE_VERILOG_VERILATOR"
CPPFLAGS += -DHAVE_VERILOG
CPPFLAGS += -DHAVE_VERILOG_VERILATOR
CPPFLAGS += -I $(VOBJ_DIR)

ifeq "$(VM_TRACE)" "1"
VFLAGS += --trace
SC_OBJS += verilated_vcd_c.o
SC_OBJS += verilated_vcd_sc.o
CPPFLAGS += -DVM_TRACE=1
endif
endif

OBJS = $(C_OBJS) $(SC_OBJS)
SIGI_OBJS += $(OBJS)

TARGET_SIGI_DEMO = sigi_demo
TARGETS = $(TARGET_SIGI_DEMO)


ifeq "$(HAVE_VERILOG_VERILATOR)" "y"
#
# verilog-uart
#
VUART_DIR = verilog-uart/rtl
include verilog-uart.mk
V_LDLIBS +=$(VOBJ_DIR)/Vuart__ALL.a
endif

all: $(TARGETS)

-include $(SIGI_OBJS:.o=.d)
CFLAGS += -MMD
CXXFLAGS += -MMD

$(SIGI_TOP_O): $(V_LDLIBS)
$(addprefix $(VOBJ_DIR)/,$(VERILATED_O)): $(V_LDLIBS)

$(VOBJ_DIR)/Vuart__ALL.a: $(VUART_CORE)
	$(VENV) $(VERILATOR) $(VFLAGS) $^
	$(MAKE) -C $(VOBJ_DIR) CXXFLAGS="$(CXXFLAGS)" -f Vuart.mk
	$(MAKE) -C $(VOBJ_DIR) CXXFLAGS="$(CXXFLAGS)" -f Vuart.mk $(VERILATED_O)

$(TARGET_SIGI_DEMO): $(SIGI_OBJS) $(VTOP_LIB) $(addprefix $(VOBJ_DIR)/,$(VERILATED_O))
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(VOBJ_DIR)/V%__ALL.a: %.v
	$(VENV) $(VERILATOR) $(VFLAGS) $<
	$(MAKE) -C $(VOBJ_DIR) CXXFLAGS="$(CXXFLAGS)" -f V$(<:.v=.mk)
#	$(MAKE) -C $(VOBJ_DIR) CXXFLAGS="$(CXXFLAGS)" -f V$(<:.v=.mk) $(VERILATED_O)


clean:
	$(RM) $(OBJS) $(OBJS:.o=.d) $(TARGETS)
	$(RM) $(SIGI_OBJS) $(SIGI_OBJS:.o=.d)
	$(RM) -r $(VOBJ_DIR)
