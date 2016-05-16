STACK_DIR   := stack
MOD_SRCS    := CallStackHandler.cpp CatchHandler.cpp ExceptionHandler.cpp StackHandler.cpp ThrowHandler.cpp

LIB_SRCS    += $(addprefix $(STACK_DIR)/, $(MOD_SRCS))
