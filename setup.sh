#!/bin/bash
# setup.sh - Setup environment for Theia analysis
#
# Usage: source setup.sh
#
# This script sets up the necessary environment variables for compilation and running

# Detect if we're being sourced (not executed)
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    echo "ERROR: This script must be sourced, not executed!"
    echo "Usage: source setup.sh"
    exit 1
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Setting up Theia Analysis Environment${NC}"
echo -e "${BLUE}========================================${NC}"

# ============================================================================
# USER CONFIGURATION - Modify these paths for your setup
# ============================================================================

# Path to RAT-PAC installation (contains include/, lib/, etc.)
# Default: assumes ratpac-two/install is in the parent directory
DEFAULT_RATROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../ratpac-two/install" 2>/dev/null && pwd)"
echo ${BASH_SOURCE[0]}
# Path to your Theia data directory (contains inputs/ and outputs/ subdirectories)
# Modify this to point to your data location
DEFAULT_THEIA_DATA_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/data"

export RATROOT="../ratpac-two/install"
export THEIA_DATA_PATH="../data"

# ============================================================================
# Environment Setup
# ============================================================================

# Set RATROOT if not already set
if [ -z "$RATROOT" ]; then
    if [ -d "$DEFAULT_RATROOT" ]; then
        export RATROOT="$DEFAULT_RATROOT"
        echo -e "${GREEN}✓ Set RATROOT=$RATROOT${NC}"
    else
        echo -e "${RED}ERROR: RATROOT not set and default path not found!${NC}"
        echo -e "${YELLOW}Please set RATROOT manually:${NC}"
        echo -e "${YELLOW}  export RATROOT=/path/to/ratpac/install${NC}"
        return 1
    fi
else
    echo -e "${GREEN}✓ Using existing RATROOT=$RATROOT${NC}"
fi

# Verify RATROOT is valid
if [ ! -d "$RATROOT/include/RAT" ]; then
    echo -e "${RED}ERROR: Invalid RATROOT - $RATROOT/include/RAT not found!${NC}"
    echo -e "${YELLOW}Please check your RATROOT setting.${NC}"
    return 1
fi

# Add RAT libraries to library path
if [ -d "$RATROOT/lib" ]; then
    export LD_LIBRARY_PATH="$RATROOT/lib:$LD_LIBRARY_PATH"
    echo -e "${GREEN}✓ Added RAT libraries to LD_LIBRARY_PATH${NC}"
fi

# Set THEIA_DATA_PATH if not already set
if [ -z "$THEIA_DATA_PATH" ]; then
    if [ -d "$DEFAULT_THEIA_DATA_PATH" ]; then
        export THEIA_DATA_PATH="$DEFAULT_THEIA_DATA_PATH"
        echo -e "${GREEN}✓ Set THEIA_DATA_PATH=$THEIA_DATA_PATH${NC}"
    else
        echo -e "${YELLOW}⚠ THEIA_DATA_PATH not set and default path not found!${NC}"
        echo -e "${YELLOW}  Will need to set before running analysis:${NC}"
        echo -e "${YELLOW}  export THEIA_DATA_PATH=/path/to/data${NC}"
    fi
else
    echo -e "${GREEN}✓ Using existing THEIA_DATA_PATH=$THEIA_DATA_PATH${NC}"
fi

# Verify data path structure (if set)
if [ -n "$THEIA_DATA_PATH" ] && [ -d "$THEIA_DATA_PATH" ]; then
    if [ ! -d "$THEIA_DATA_PATH/inputs" ] || [ ! -d "$THEIA_DATA_PATH/outputs" ]; then
        echo -e "${YELLOW}⚠ Warning: Expected data structure not found in $THEIA_DATA_PATH${NC}"
        echo -e "${YELLOW}  Expected: $THEIA_DATA_PATH/inputs/ and $THEIA_DATA_PATH/outputs/${NC}"
    fi
fi

# Check ROOT
if command -v root &> /dev/null; then
    ROOT_VERSION=$(root-config --version 2>/dev/null)
    echo -e "${GREEN}✓ ROOT found: version $ROOT_VERSION${NC}"
else
    echo -e "${RED}ERROR: ROOT not found!${NC}"
    echo -e "${YELLOW}Please setup ROOT before proceeding.${NC}"
    return 1
fi

echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Environment setup complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "${BLUE}Next steps:${NC}"
echo -e "  1. Compile: ${YELLOW}make clean && make${NC}"
echo -e "  2. Test:    ${YELLOW}./run_validate.sh debug 1 0${NC}"
echo -e "  3. Run:     ${YELLOW}./run_validate.sh${NC}"
echo ""
echo -e "${BLUE}Configuration:${NC}"
echo -e "  RATROOT:           $RATROOT"
echo -e "  THEIA_DATA_PATH:   $THEIA_DATA_PATH"
echo ""