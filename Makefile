# Makefile for validate analysis

# Compiler and flags
CXX = g++
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs) -lPhysics
# RATFLAGS = -I$(RATROOT)/include
RATFLAGS = -I$(RATROOT)/include -I$(RATROOT)/include/stlplus
RATLIBS = -L$(RATROOT)/lib -lRATEvent

CXXFLAGS = -Wall -O2 -std=c++11 $(ROOTCFLAGS) $(RATFLAGS)
LDFLAGS = $(ROOTLIBS) $(RATLIBS)

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Create obj directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

# Source files
SOURCES = $(SRCDIR)/Config.C \
          $(SRCDIR)/Statistics.C \
          $(SRCDIR)/FileManager.C \
          $(SRCDIR)/HistogramManager.C \
          $(SRCDIR)/EventDisplay.C \
          $(SRCDIR)/EventProcessor.C

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.C=$(OBJDIR)/%.o)

# Headers
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
	@echo "  ./run_validate.sh             - Production mode: Process all 8 datasets, create merged files"
	@echo "  ./run_validate.sh debug       - Debug mode: Dataset 4, file 4 (default)"
	@echo "  ./run_validate.sh debug 1     - Debug mode: Dataset 1, file 4"
	@echo "  ./run_validate.sh debug 7 123 - Debug mode: Dataset 7, file 123"
	@echo ""
	@echo "Output files:"
	@echo "  validate_0X_*.root             # Individual dataset outputs"
	@echo "  validate_merged_all_*.root     # All datasets merged"
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

# Help
help:
	@echo "Available targets:"
	@echo "  all (default) - Build the shared library"
	@echo "  clean         - Remove build files"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make"
	@echo "  ./run_validate.sh             - Production mode: Process all 8 datasets, create merged files"
	@echo "  ./run_validate.sh debug       - Debug mode: Dataset 4, file 4 (default)"
	@echo "  ./run_validate.sh debug 1     - Debug mode: Dataset 1, file 4"
	@echo "  ./run_validate.sh debug 7 123 - Debug mode: Dataset 7, file 123"
.PHONY: all clean help
