.PHONY: all

#------------------------------------------------------------------------------
# Build configurations
#------------------------------------------------------------------------------

# Generate code for 32-bit or 64-bit environment.
# The value is either 32 or 64
BUILD_MODE       := 64

# Adding Support for C++11 exception mechanisms; comment out if unnecessary.
CPP_MODE         := C++11

#------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------
SRCDIR           := $(CURDIR)/src
OBJDIR           := $(CURDIR)/obj

LIB_SRCS         :=
SUBDIRS          :=
CFLAGS           :=
CPPFLAGS         := -pthread -fPIC -m$(BUILD_MODE) -ggdb -rdynamic
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
ifeq ($(CPP_MODE),C++11)
CPPFLAGS        += -std=c++11 -DCPPMODE=CPP11
endif

LDFLAGS         += -lc -lstdc++ -ldl -lrt

EXRAY_LIB       := libexray.so

TEST_LIB        := libtest.so

TEST_BIN        := throwtest preloadtest

ifeq ($(CPP_MODE),C++11)
TEST_BIN        += cxx11threadtest memorytest rethrowtest
else
endif

all: $(EXRAY_LIB) $(TEST_LIB) $(TEST_BIN)

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

#------------------------------------------------------------------------------
# Libraries
#------------------------------------------------------------------------------
$(EXRAY_LIB): $(LIB_OBJS)
	$(CXX) -shared $(LIB_OBJS) -o $(EXRAY_LIB) $(LDFLAGS)

$(TEST_LIB): $(TESTLIB_OBJS) $(EXRAY_LIB)
	$(CXX) -shared $(TESTLIB_OBJS) -o $(TEST_LIB) $(LDFLAGS)

#------------------------------------------------------------------------------
# Test binaries
#------------------------------------------------------------------------------
throwtest: $(TEST_LIB) $(TEST_OBJS) $(EXRAY_LIB)
	$(CXX) $(CPPFLAGS) $(TEST_SRCS) -o $@ -L. -lexray $(LDFLAGS) -ltest

preloadtest: $(TEST_LIB) $(TEST_OBJS)
	$(CXX) -DPRELOAD $(CPPFLAGS) $(TEST_SRCS) -o $@ $(LDFLAGS) -L. -ltest

$(LIB_OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	for dir in $(SUBDIRS); do mkdir -p $(OBJDIR)/$$dir; done

#------------------------------------------------------------------------------
# C++11 compatibility
#------------------------------------------------------------------------------
ifeq ($(CPP_MODE),C++11)

cxx11threadtest: $(SRCDIR)/test/cxx11threadtest.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ -L/usr/lib64 -lc -lstdc++

rethrowtest: $(SRCDIR)/test/rethrowtest.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ -L/usr/lib64 -lc -lstdc++

memorytest: $(SRCDIR)/test/memorytest.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ -L/usr/lib64 -lc -lstdc++

endif

#------------------------------------------------------------------------------
clean:
	rm -rf $(OBJDIR) $(TEST_BIN) $(TEST_LIB) $(PRELOAD_LIB) $(EXRAY_LIB)
