# Theia Analysis - Setup Instructions

## Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/mattiafani/theia-analysis.git
cd theia-analysis

# 2. Setup environment (sets RATROOT and THEIA_DATA_PATH)
source setup.sh

# 3. Compile
make clean && make

# 4. Test with one file
./run_validate.sh debug 1 0

# 5. Run full analysis
./run_validate.sh
```

## Environment Variables Required

### RATROOT
Points to your RAT-PAC installation directory (the one containing `include/`, `lib/`, etc.)

**Auto-detection:** The `setup.sh` script will try to find `ratpac-two/install` in the parent directory.

**Manual setup:**
```bash
export RATROOT=/path/to/ratpac-two/install
export LD_LIBRARY_PATH=$RATROOT/lib:$LD_LIBRARY_PATH
```

### THEIA_DATA_PATH
Points to your data directory containing `inputs/` and `outputs/` subdirectories.

**Expected structure:**
```
$THEIA_DATA_PATH/
├── inputs/
│   ├── nue_100000.rootracker.root
│   ├── numu_100000.rootracker.root
│   ├── anue_100000.rootracker.root
│   └── anumu_100000.rootracker.root
└── outputs/
    ├── nue/
    │   └── theia_nue_*.root
    ├── numu/
    │   └── theia_numu_*.root
    ├── anue/
    │   └── theia_anue_*.root
    └── anumu/
        └── theia_anumu_*.root
```

**Manual setup:**
```bash
export THEIA_DATA_PATH=/path/to/your/data
```

## Using Apptainer/Singularity Containers

If you're using ROOT and RAT-PAC through a container (like on UMN-MSI):

```bash
# Enter the container
apptainer shell /path/to/ratpac2.sif

# Inside the container, setup environment
cd /path/to/theia-analysis
source setup.sh

# Compile and run
make clean && make
./run_validate.sh debug 1 0
```

**Or run everything through the container without entering it:**
```bash
apptainer exec /path/to/ratpac2.sif bash -c "cd /path/to/theia-analysis && source setup.sh && make clean && make"
apptainer exec /path/to/ratpac2.sif bash -c "cd /path/to/theia-analysis && source setup.sh && ./run_validate.sh debug 1 0"
```

## Customizing setup.sh

Edit the `setup.sh` file to change default paths:

```bash
# Line ~20: Default RAT-PAC location
DEFAULT_RATROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../ratpac-two/install" 2>/dev/null && pwd)"

# Line ~24: Default data location  
DEFAULT_THEIA_DATA_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/data"
```

Common setups:

### On UMN-MSI cluster
```bash
# In setup.sh, modify:
DEFAULT_RATROOT="/path/to/your/Theia/ratpac-two/install"
DEFAULT_THEIA_DATA_PATH="/path/to/your/Theia/data"
```

### On dunegpvm
```bash
# In setup.sh, modify:
DEFAULT_RATROOT="/path/to/your/rat/install"
DEFAULT_THEIA_DATA_PATH="/path/to/your/data"
```

## Verifying Setup

After sourcing `setup.sh`, you should see:
```
========================================
Setting up Theia Analysis Environment
========================================
✓ Set RATROOT=/path/to/ratpac-two/install
✓ Added RAT libraries to LD_LIBRARY_PATH
✓ Set THEIA_DATA_PATH=/path/to/data
✓ ROOT found: version X.XX.XX
========================================
Environment setup complete!
========================================
```

Check paths are correct:
```bash
echo $RATROOT
ls $RATROOT/include/RAT/DS/MC.hh  # Should exist

echo $THEIA_DATA_PATH  
ls $THEIA_DATA_PATH/inputs/       # Should list input files
```

## Troubleshooting

### "RATROOT not set and default path not found"
Either:
1. Edit `setup.sh` to set correct `DEFAULT_RATROOT`, or
2. Manually set before sourcing: `export RATROOT=/your/path/to/rat/install`

### "ROOT not found"
Make sure ROOT is in your PATH:
```bash
# If using container
apptainer exec /path/to/container.sif root --version

# If using modules (on cluster)
module load root

# If installed locally
source /path/to/root/bin/thisroot.sh
```

### "Invalid RATROOT - include/RAT not found"
Your RATROOT is pointing to wrong directory. It should point to the `install` directory that contains:
```
install/
├── include/
│   └── RAT/
│       └── DS/
│           └── MC.hh
└── lib/
    └── libRATEvent.so
```

### Compilation errors
```bash
# Clean rebuild
make clean
make debug-paths  # Shows what paths are being used
make
```

## For Collaborators

1. Clone the repository
2. Edit `setup.sh` with your local paths (or just set environment variables)
3. Source the setup script: `source setup.sh`
4. Compile: `make`
5. Test: `./run_validate.sh debug 1 0`

**Do not commit your local paths to git!** Each user should customize `setup.sh` locally or use environment variables.

## Git Best Practices

Add your customized `setup.sh` to `.gitignore` if you've modified it locally:
```bash
# Create a local setup script
cp setup.sh setup_local.sh
# Edit setup_local.sh with your paths
# Use: source setup_local.sh

# Add to .gitignore
echo "setup_local.sh" >> .gitignore
```

This way you can have personal settings while keeping the repository clean.