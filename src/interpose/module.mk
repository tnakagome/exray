INTERPOSE_DIR := interpose
MOD_SRCS      := Exception.cpp CFunction.cpp # PThread.cpp

LIB_SRCS      += $(addprefix $(INTERPOSE_DIR)/, $(MOD_SRCS))
