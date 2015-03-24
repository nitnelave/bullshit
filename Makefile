TARGET=bullshit
SRCDIR=src/
FILES=utf8.cc
INCLUDE_DIRS=.

include Makefile.rules

SRC=$(addprefix $(SRCDIR), $(FILES))
MAIN_FILES=$(SRC) src/main.cc
BENCH_FILES=$(SRC) bench/bench.cc
MAIN_OBJS=$(MAIN_FILES:.cc=.o)
MAIN_DEPS=$(MAIN_OBJS:.o=.d)
DEBUG_OBJS=$(MAIN_FILES:.cc=.g.o)
DEBUG_DEPS=$(DEBUG_OBJS:.o=.d)
BENCH_OBJS=$(BENCH_FILES:.cc=.o)
BENCH_DEPS=$(BENCH_OBJS:.o=.d)
DEBUG_TARGET=$(addprefix $(TARGET), _debug)
BENCH_TARGET=$(addprefix $(TARGET), _bench)
CXX?=g++
CXXFLAGS+= -Wall -pedantic -std=c++14 -pedantic          \
       -Wfloat-equal -Wundef -Wshadow -Wpointer-arith    \
       -Wcast-qual -Wcast-align                		 \
       -Wmissing-declarations                  	   	 \
       -Wunreachable-code
CPPFLAGS+=-MMD $(INCLUDES)
LDFLAGS=
INCLUDES=$(addprefix -I$(SRCDIR), $(INCLUDE_DIRS))

all: exec

all: CXXFLAGS += -O3

all: CPPFLAGS += -DNDEBUG

bench: $(BENCH_TARGET)

bench: INCLUDES += -I bench -isystem hayai/src

bench: TARGET=$(addprefix _bench, $TARGET)

bench: CXXFLAGS += -O3 -g3

bench: CPPFLAGS += -DNDEBUG

debug: $(DEBUG_TARGET)

debug: CXXFLAGS += -g

debug: TARGET=$(addprefix _debug, $TARGET)

exec: $(TARGET)

check:

doc:
	doxygen doxygen.conf

export:
	git archive master --prefix=$(ARCHIVEPREFIX)/ | bzip2 > $(ARCHIVENAME)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BENCH_TARGET): $(BENCH_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(TARGET): $(MAIN_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

%.g.o: %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<


clean:
	@echo Removing files ...
	@$(RM) $(MAIN_OBJS) $(TARGET) $(MAIN_DEPS)
	@$(RM) $(BENCH_OBJS) $(BENCH_TARGET) $(BENCH_DEPS)
	@$(RM) $(DEBUG_OBJS) $(DEBUG_TARGET) $(DEBUG_DEPS)
	@$(RM) -rf doc

.PHONY: all clean exec check

-include $(MAIN_DEPS)
-include $(BENCH_DEPS)
-include $(DEBUG_DEPS)
