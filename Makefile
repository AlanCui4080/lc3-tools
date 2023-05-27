export CXX		= g++
export CXXDEP	= g++ -MM *.cpp
export LD		= ld
export AR		= ar
export RANLIB		= ranlib
export RM		= rm
export CLANG_FORMAT	= clang-format
export FIND		= find
export XARGS		= xargs

export OBJS		:= emulator.o

export CXXFLAGS	+= -std=c++23 -g -O2 -fsanitize=undefined -fsanitize=address
export LDFLAGS		=

all: dep.mk $(OBJS)

dep dep.mk: $(shell ls *.cpp)
	@echo [CXXDEP] $@
	@$(CXXDEP) $(CXXFLAGS) > dep.mk

%.o: %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) $< -c

%.so: $(OBJS)
	@echo [LD] $@
	@$(LD) $(LDFLAGS) $(OBJS) -shared -o $@

%.a: $(OBJS)
	@echo [AR] $@
	@$(AR) rc $@ $(OBJS)
	@echo [RANLIB] $@
	@$(RANLIB) $@

clean:
	@echo [RM] $(LIBS) $(OBJS) dep.mk
	-@$(RM) -f $(LIBS) $(OBJS) dep.mk

include dep.mk