IDIR := inclues
SDIR := src
ODIR := .obj
BDIR := build

CC := gcc -std=gnu11
CFLAGS := -g3
CFLAGS += -I$(IDIR) -MMD

_SRC := $(shell find $(SDIR) -maxdepth 2 -type f -regex ".*\.c")
ALL_OBJ := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(_SRC))

MAINS := $(ODIR)/aggregator/aggregator.o $(ODIR)/worker/worker.o

OBJ := $(filter-out $(MAINS), $(ALL_OBJ))

DEPS := $(patsubst $(SDIR)/%.c, $(ODIR)/%.d, $(_SRC))

$(ODIR):
	@mkdir $(BDIR)
	@mkdir $(ODIR)
	@mkdir $(ODIR)/aggregator
	@mkdir $(ODIR)/common
	@mkdir $(ODIR)/worker

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(BDIR)/diseaseAggregator $(BDIR)/worker

$(BDIR)/diseaseAggregator: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/aggregator/aggregator.o -o $@

$(BDIR)/worker: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/worker/worker.o -o $@

.PHONY: clean

clean:
	rm -rf diseaseAggregator worker $(ODIR) $(BDIR)

$(VERBOSE).SILENT:

-include $(DEPS)
