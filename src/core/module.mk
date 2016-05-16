CORE_DIR    := core
MOD_SRCS    := System.cpp Options.cpp Interpose.cpp Initializer.cpp Logger.cpp

LIB_SRCS    += $(addprefix $(CORE_DIR)/, $(MOD_SRCS))
