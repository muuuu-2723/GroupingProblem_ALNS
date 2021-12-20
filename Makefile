GCC    = g++
CFLAGS = -O3 -std=c++17 -finput-charset=CP932
ifeq ($(OS),Windows_NT)
CFLAGS += -fexec-charset=CP932
endif
SRCDIR = src
BINDIR = bin
INCLUDE= -I./$(SRCDIR)/include
SUBDIRS = Debug Destroy/RandomDestroy Destroy/RandomGroupDestroy Destroy/MinimumDestroy Destroy/MinimumGroupDestroy Destroy/UpperWeightGreedyDestroy Group Input Item MyRandom Search/DecreaseGroup Search/GroupPenaltyGreedy Search/ItemPenaltyGreedy Search/NeighborhoodGraph Search/RelationGreedy Search/ShiftNeighborhood Search/SwapNeighborhood Search/ValueAverageGreedy Search/ValueDiversityGreedy Search/ValueSumGreedy Search/WeightPenaltyGreedy Solution Weight
OBJDIR = obj
SRCS  = $(SRCDIR)/Main.cpp
ifeq ($(OS),Windows_NT)
RM = cmd.exe /C del
else
RM = rm
endif
#				$(wildcard dirA/*.cpp)\
				$(wildcard dirB/*.cpp)\
				$(wildcard dirC/*.cpp)\
				$(wildcard dirC/dirD/*.cpp)
SRCS += $(foreach SUBDIR,$(SUBDIRS),$(wildcard $(SRCDIR)/$(SUBDIR)/*.cpp))
#$(warning SRCS = $(SRCS))

OBJS   = $(subst $(SRCDIR),$(OBJDIR),$(SRCS:.cpp=.o))
#$(warning OBJS = $(OBJS))
ifeq ($(OS),Windows_NT)
CLNOBJS = $(subst /,\,$(OBJS))
else
CLNOBJS = $(OBJS)
endif

DEPS   = $(CLNOBJS:.o=.d)
TILS   = $(SRCS:.cpp=.cpp~)
ifeq ($(OS),Windows_NT)
TARGET = $(BINDIR)/run.exe
CLNTARGET = $(subst /,\,$(TARGET))
else
TARGET = $(BINDIR)/run.out
CLNTARGET = $(TARGET)
endif

ifeq ($(OS),Windows_NT)
OBJDIRS = $(addprefix $(OBJDIR)\, $(subst /,\,$(SUBDIRS)))
else
OBJDIRS = $(addprefix $(OBJDIR)/, $(SUBDIRS))
endif

default:
ifeq ($(OS),Windows_NT)
	@call bat\make_build.bat "$(OBJDIRS)" $(BINDIR)
else
	@sh shell/make_build.sh "$(OBJDIRS)" $(BINDIR)
endif
	@make all --no-print-directory

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(GCC) $(CFLAGS) -o $@ -c -MMD -MP $< $(INCLUDE)

$(TARGET): $(OBJS)
	$(GCC) $(CFLAGS) -o $@ $+

all : $(OBJS) $(TARGET)

clean:
	$(RM) $(CLNOBJS) $(DEPS) $(CLNTARGET)

-include $(DEPS)