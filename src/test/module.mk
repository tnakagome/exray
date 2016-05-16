TEST_DIR     := test
TEST_SRCS    := throwtest.cpp
TESTLIB_SRCS := testlib.cpp

TEST_SRCS    := $(addprefix $(TEST_DIR)/, $(TEST_SRCS))
TESTLIB_SRCS := $(addprefix $(TEST_DIR)/, $(TESTLIB_SRCS))
