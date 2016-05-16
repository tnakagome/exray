.PHONY: all

# Generate code for 32-bit or 64-bit environment.
# either 32 or 64
BUILD_MODE       := 64

SRCDIR           := $(CURDIR)/src
OBJDIR           := $(CURDIR)/obj

LIB_SRCS         :=
SUBDIRS          :=
CFLAGS           :=
#CPPFLAGS         := -pthread -fPIC -ggdb
#LDFLAGS          := 
CPPFLAGS         := -pthread -fPIC -m$(BUILD_MODE) -ggdb
LDFLAGS          := -m$(BUILD_MODE)

include $(SRCDIR)/module.mk

LIB_OBJS        := $(addprefix $(OBJDIR)/, $(LIB_SRCS:.cpp=.o))
LIB_SRCS        := $(addprefix $(SRCDIR)/, $(LIB_SRCS))

DEPS            := $(LIB_OBJS:.o=.d)
TMPS            := $(LIB_OBJS) $(LIB_OBJS:.o=.d)

TESTLIB_OBJS    := $(addprefix $(OBJDIR)/, $(TESTLIB_SRCS:.cpp=.o))
TESTLIB_SRCS    := $(addprefix $(SRCDIR)/, $(TESTLIB_SRCS))

TEST_OBJS       := $(addprefix $(OBJDIR)/, $(TEST_SRCS:.cpp=.o))
TEST_SRCS       := $(addprefix $(SRCDIR)/, $(TEST_SRCS))

CPPFLAGS        += -I./src
LDFLAGS         += -lc -lstdc++ -ldl -lrt

EXRAY_LIB       := libexray.so

TEST_LIB        := libtest.so
TEST_BIN        := throwtest preloadtest

all: $(EXRAY_LIB) $(TEST_LIB) $(TEST_BIN)

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

# libraries
$(EXRAY_LIB): $(LIB_OBJS)
	$(CXX) -shared $(LIB_OBJS) -o $(EXRAY_LIB) $(LDFLAGS)

$(TEST_LIB): $(TESTLIB_OBJS) $(EXRAY_LIB)
	$(CXX) -shared $(TESTLIB_OBJS) -o $(TEST_LIB) $(LDFLAGS)

# test binaries
throwtest: $(TEST_LIB) $(TEST_OBJS) $(EXRAY_LIB)
	$(CXX) $(CPPFLAGS) $(TEST_SRCS) -o $@ -L. -lexray $(LDFLAGS) -ltest

preloadtest: $(TEST_LIB) $(TEST_OBJS)
	$(CXX) -DPRELOAD $(CPPFLAGS) $(TEST_SRCS) -o $@ $(LDFLAGS) -L. -ltest

$(LIB_OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	for dir in $(SUBDIRS); do mkdir -p $(OBJDIR)/$$dir; done

clean:
	rm -rf $(OBJDIR) $(TEST_BIN) $(TEST_LIB) $(PRELOAD_LIB) $(EXRAY_LIB)
