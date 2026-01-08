CXX = g++
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs) -lPhysics

ifeq ($(RATROOT),)
    $(error RATROOT environment variable is not set. Please run: source setup.sh)
endif

$(info Using RATROOT: $(RATROOT))

RATFLAGS = -I$(RATROOT)/include -I$(RATROOT)/include/stlplus
RATLIBS = -L$(RATROOT)/lib -lRATEvent

CXXFLAGS = -Wall -O2 -std=c++11 $(ROOTCFLAGS) $(RATFLAGS)
LDFLAGS = $(ROOTLIBS) $(RATLIBS)

SRCDIR = src
INCDIR = include
OBJDIR = obj

$(shell mkdir -p $(OBJDIR))

SOURCES = $(SRCDIR)/Config.C \
          $(SRCDIR)/Statistics.C \
          $(SRCDIR)/FileManager.C \
          $(SRCDIR)/HistogramManager.C \
          $(SRCDIR)/EventDisplay.C \
          $(SRCDIR)/EventProcessor.C

OBJECTS = $(SOURCES:$(SRCDIR)/%.C=$(OBJDIR)/%.o)

HEADERS = $(INCDIR)/Config.h \
          $(INCDIR)/Statistics.h \
          $(INCDIR)/FileManager.h \
          $(INCDIR)/HistogramManager.h \
          $(INCDIR)/EventDisplay.h \
          $(INCDIR)/EventProcessor.h

# Main target
all: validate.so

# Create shared library
validate.so: $(OBJECTS) validate.C
	@echo ""
	@echo "=========================================="
	@echo "Creating shared library..."
	@echo "=========================================="
	$(CXX) -shared -fPIC $(CXXFLAGS) $(OBJECTS) validate.C -o $@ $(LDFLAGS)
	@echo ""
	@echo "=========================================="
	@echo "✓ Build successful!"
	@echo "=========================================="
	@echo ""
	@echo "Quick start:"
	@echo "  ./run_validate.sh             - Production mode: Process all 4 flavors, create merged files"
	@echo "  ./run_validate.sh debug       - Debug mode: Flavor 1 (nue), file 0 (default)"
	@echo "  ./run_validate.sh debug 1     - Debug mode: Flavor 1 (nue), file 0"
	@echo "  ./run_validate.sh debug 2 50  - Debug mode: Flavor 2 (numu), file 50"
	@echo ""
	@echo "Output files:"
	@echo "  validate_{flavor}_*.root       # Individual flavor outputs"
	@echo "  validate_merged_all_*.root     # All flavors merged"
	@echo "  Plots/event_*.pdf              # Event displays (debug mode)"
	@echo "  logs/*.log                     # Log files"
	@echo ""

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.C $(INCDIR)/%.h
	@echo "Compiling $<..."
	$(CXX) -fPIC $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJDIR)/*.o validate.so validate_d.so validate_C.d
	rm -rf $(OBJDIR)

# Debug target to show detected paths
debug-paths:
	@echo "RATROOT: $(RATROOT)"
	@echo "RATFLAGS: $(RATFLAGS)"
	@echo "RATLIBS: $(RATLIBS)"
	@echo "ROOTCFLAGS: $(ROOTCFLAGS)"
	@echo "ROOTLIBS: $(ROOTLIBS)"

# Help
help:
	@echo "Available targets:"
	@echo "  all (default) - Build the shared library"
	@echo "  clean         - Remove build files"
	@echo "  debug-paths   - Show detected paths"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make"
	@echo "  ./run_validate.sh             - Production mode: Process all 4 flavors, create merged files"
	@echo "  ./run_validate.sh debug       - Debug mode: Flavor 1 (nue), file 0 (default)"
	@echo "  ./run_validate.sh debug 1     - Debug mode: Flavor 1 (nue), file 0"
	@echo "  ./run_validate.sh debug 2 50  - Debug mode: Flavor 2 (numu), file 50"

.PHONY: all clean help debug-paths