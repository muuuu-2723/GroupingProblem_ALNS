GCC    = g++
CFLAGS = -O3 -std=c++17 -finput-charset=CP932 -fexec-charset=CP932
SRCDIR = src
BINDIR = bin
INCLUDE= -I./$(SRCDIR)/include
SUBDIRS = Debug Destroy/RandomDestroy Destroy/RandomGroupDestroy Destroy/MinimumDestroy Destroy/MinimumGroupDestroy Group Input Item MyRandom Search/DecreaseGroup Search/GroupPenaltyGreedy Search/ItemPenaltyGreedy Search/NeighborhoodGraph Search/RelationGreedy Search/ShiftNeighborhood Search/SwapNeighborhood Search/ValueAverageGreedy Search/ValueDiversityGreedy Search/ValueSumGreedy Search/WeightPenaltyGreedy Solution Weight
OBJDIR = obj
SRCS  = $(SRCDIR)/Main.cpp
RM = cmd.exe /C del
#				$(wildcard dirA/*.cpp)\
				$(wildcard dirB/*.cpp)\
				$(wildcard dirC/*.cpp)\
				$(wildcard dirC/dirD/*.cpp)
SRCS += $(foreach SUBDIR,$(SUBDIRS),$(wildcard $(SRCDIR)/$(SUBDIR)/*.cpp))
#$(warning SRCS = $(SRCS))

OBJS   = $(subst $(SRCDIR),$(OBJDIR),$(SRCS:.cpp=.o))
#$(warning OBJS = $(OBJS))
CLNOBJS = $(subst /,\,$(OBJS))

DEPS   = $(CLNOBJS:.o=.d)
TILS   = $(SRCS:.cpp=.cpp~)
TARGET = $(BINDIR)/run.exe
CLNTARGET = $(subst /,\,$(TARGET))

OBJDIRS = $(addprefix $(OBJDIR)\, $(subst /,\,$(SUBDIRS)))

default:
	@call bat\make_build.bat "$(OBJDIRS)" $(BINDIR)
	@make all --no-print-directory

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(GCC) $(CFLAGS) -o $@ -c -MMD -MP $< $(INCLUDE)

$(TARGET): $(OBJS)
	$(GCC) $(CFLAGS) -o $@ $+

all : $(OBJS) $(TARGET)

clean:
	$(RM) $(CLNOBJS) $(DEPS) $(CLNTARGET)

-include $(DEPS)