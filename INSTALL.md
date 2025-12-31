# Installation and Setup Guide

## Quick Start

```bash
# 1. Access SL7 container

# 2. Set up environment
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup cmake v3_22_0
setup root v6_28_10b -q e20:p3915:prof
setup geant4 v4_11_0_p02a -q e20:prof:qt
setup xerces_c v3_2_3b -q e20
setup clhep v2_4_5_1a -q e20:prof
export CLHEP_ROOT=/cvmfs/larsoft.opensciencegrid.org/products/clhep/v2_4_5_1a/Linux64bit+3.10-2.17-e20-prof
export CPLUS_INCLUDE_PATH=$CLHEP_ROOT/include:$CPLUS_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=$XERCESCROOT/include:$CPLUS_INCLUDE_PATH
export THEIA_DATA_PATH=/your/path/to/theia/data         # user-defined
export THEIA_PRODUCTION_TAG=20251030_prep               # user-defined
export RATPAC_TWO_PATH=/your/path/to/ratpac-two_v2.2.1  # user-defined
source $RATPAC_TWO_PATH/ratpac.sh


# Git clone repository, then make the run script executable
chmod +x run_validate.sh

# 4. Build
make

# 5. Run
./run_validate.sh
```

## Detailed Setup

### 1. File Organization

Ensure your directory structure looks like this:

```
your_workspace/
├── validate.C
├── Makefile
├── run_validate.sh
├── README.md
├── INSTALL.md
├── include/
│   ├── Config.h
│   ├── Statistics.h
│   ├── FileManager.h
│   ├── HistogramManager.h
│   ├── EventDisplay.h
│   └── EventProcessor.h
└── src/
    ├── Config.C
    ├── Statistics.C
    ├── FileManager.C
    ├── HistogramManager.C
    ├── EventDisplay.C
    └── EventProcessor.C
```

### 2. Environment Setup

See Quick Start's Step 2

### 3. Verify Environment

```bash
# Check ROOT
root-config --version
root-config --cflags
root-config --libs

# Check RAT
echo $RATROOT
ls $RATROOT/lib/libRATEvent.*
ls $RATROOT/include/RAT/DS/
```

### 4. First Build

```bash
# Clean build
make clean
make

# You should see:
# Compiling src/Config.C...
# Compiling src/Statistics.C...
# Compiling src/FileManager.C...
# Compiling src/HistogramManager.C...
# Compiling src/EventDisplay.C...
# Compiling src/EventProcessor.C...
# Creating shared library...
# Done!'
```

### 5. Test Run

```bash
# Quick test with one file in debug mode
root -l
root [0] validate(5, true, true, 55)   # Dataset 5, debug, event display, file 55
```

# If successful, you'll see:
# - Hostname detection
# - Path setup
# - File loading
# - Event processing
# - Statistics summary
```

## Common Issues

### Issue: "ERROR: input tree 'T' not found"

**Cause**: Input file format is wrong or file is corrupted

**Solution**:
```bash
# Check the file
root -l /path/to/input/file.root
# In ROOT:
.ls
# You should see a TTree named "T"
```

### Issue: "undefined reference to RAT::DS::MC"

**Cause**: RAT libraries not found

**Solution**:
```bash
# Verify RATROOT is set
echo $RATROOT

# Check if library exists
ls -l $RATROOT/lib/libRATEvent.*

# If missing, rebuild RAT or check path
```

### Issue: "validate.so: cannot open shared object file"

**Cause**: Library dependencies not found

**Solution**:
```bash
# Check library dependencies
ldd validate.so

# Add missing library paths to LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$RATROOT/lib:$LD_LIBRARY_PATH
```

### Issue: Compilation warnings about C++11

**Cause**: ROOT not built with C++11 support

**Solution**:
```bash
# Check ROOT C++ standard
root-config --cflags | grep std

# If no C++11, may need to rebuild ROOT or use older syntax
# Or force C++11 in Makefile:
CXXFLAGS = -Wall -O2 -std=c++11 $(ROOTCFLAGS) $(RATFLAGS)
```

### Issue: "Machine unknown" error

**Cause**: Hostname not recognized in FileManager

**Solution**:
Edit `src/FileManager.C` and add your hostname:

```cpp
void FileManager::FindPathsForHostname(const std::string& hostname,
                                       const std::string& chunk) {
    // ... existing code ...

    // Add your machine:
    else if (hostname == "your_hostname") {
        std::cout << " : Matched your_hostname" << std::endl;
        output_file_path = "/your/output/path/" + dataset + "/Theia_25kt_genie_";
        input_file_path = "/your/input/path/" + dataset + "/genie_root_file_";
    }

    // ... rest of code ...
}
```

## Performance Tuning

### For Maximum Speed

1. Use production mode (no debug):
   ```bash
   ./run_validate.sh
   ```

2. Process on local disk (not network):
   ```bash
   # Copy files to local disk first
   cp /pnfs/path/*.root /local/scratch/
   # Edit paths in FileManager.C
   # Rebuild and run
   ```

3. Parallel processing:
   ```bash
   # Run multiple datasets simultaneously
   root -b -l -q 'validate.C+(1,false,false,0)' &
   root -b -l -q 'validate.C+(2,false,false,0)' &
   root -b -l -q 'validate.C+(3,false,false,0)' &
   wait
   ```

### For Debug/Development

1. Process single files:
   ```bash
   ./run_validate.sh debug 7 123             # Dataset 7, file 123
   ```
   or
   ```bash
   root -l -q 'validate.C+(7,true,true,123)' # Dataset 7, file 123
   ```

2. Enable event display :
   ```bash
   root -l -q 'validate.C+(4,true,true,12)'
   ```

3. Check specific events:
   Add breakpoint in `EventProcessor::ProcessEvent()`

## Updating the Code

### After changing headers (.h files):

```bash
make clean
make
```

### After changing source (.C files):

```bash
make  # Automatic recompilation
```

### After changing validate.C:

```bash
# Just run - ROOT will recompile automatically
./run_validate.sh
```

## Getting Help

1. Check the README.md for usage examples
2. Look at Statistics output for file/event issues
3. Check logs in `logs/` directory
4. Run in debug mode to see detailed event info
5. Contact: mattia.fani@cern.ch
