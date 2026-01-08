#!/bin/bash

# run_validate.sh - Run validation analysis on all neutrino flavors
#
# Usage:
#   ./run_validate.sh                    # Run all flavors in production mode + merge
#   ./run_validate.sh debug [FLAVOR] [FILE]  # Run in debug mode (default: flavor 1, file 0)
#   ./run_validate.sh flavor N          # Run specific flavor N (1=nue, 2=numu, 3=anue, 4=anumu)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

if ! command -v root &> /dev/null; then
    echo -e "${RED}ERROR: ROOT not found. Please set up your ROOT environment.${NC}"
    exit 1
fi

if [ -z "$RATROOT" ]; then
    echo -e "${RED}ERROR: RATROOT not set. Please set up your RAT environment.${NC}"
    exit 1
fi

mkdir -p Plots
mkdir -p logs

MODE="production"
FLAVOR_START=1
FLAVOR_END=4
DO_MERGE=false
DEBUG_FLAVOR=1
DEBUG_FILE=0

declare -a FLAVOR_NAMES=("" "nue" "numu" "anue" "anumu")

if [ "$1" == "debug" ]; then
    MODE="debug"
    if [ -n "$2" ]; then
        DEBUG_FLAVOR=$2
    fi
    if [ -n "$3" ]; then
        DEBUG_FILE=$3
    fi
    FLAVOR_START=$DEBUG_FLAVOR
    FLAVOR_END=$DEBUG_FLAVOR
    echo -e "${YELLOW}Running in DEBUG mode (flavor $DEBUG_FLAVOR [${FLAVOR_NAMES[$DEBUG_FLAVOR]}], file $DEBUG_FILE)${NC}"
elif [ "$1" == "flavor" ] && [ -n "$2" ]; then
    FLAVOR_START=$2
    FLAVOR_END=$2
    echo -e "${YELLOW}Running flavor $2 [${FLAVOR_NAMES[$2]}] only${NC}"
elif [ -z "$1" ]; then
    DO_MERGE=true
    echo -e "${YELLOW}Running all flavors in production mode (will merge at end)${NC}"
fi

if [ ! -f "validate.so" ] || [ "validate.C" -nt "validate.so" ]; then
    echo -e "${YELLOW}Compiling validate.C...${NC}"
    make clean
    make
    if [ $? -ne 0 ]; then
        echo -e "${RED}Compilation failed!${NC}"
        exit 1
    fi
    echo -e "${GREEN}Compilation successful!${NC}"
fi

echo -e "${GREEN}Starting validation analysis...${NC}"
START_TIME=$(date +%s)

declare -a OUTPUT_FILES

for k in $(seq $FLAVOR_START $FLAVOR_END); do
    TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
    FLAVOR_NAME=${FLAVOR_NAMES[$k]}
    echo ""
    echo -e "${GREEN}[$TIMESTAMP] Processing flavor $k [$FLAVOR_NAME]...${NC}"

    LOG_FILE="logs/validate_flavor_${k}_${FLAVOR_NAME}_$(date +%Y%m%d_%H%M%S).log"

    if [ "$MODE" == "debug" ]; then
        echo "  Running: root -l -q -e 'validate($k,true,true,$DEBUG_FILE)'"
        echo "   - Flavor: $k [$FLAVOR_NAME], File: $DEBUG_FILE"
        echo "   - Note: Using .rootlogon.C to preload libraries"
        root -l -q -e "validate($k,true,true,$DEBUG_FILE)" 2>&1 | tee "$LOG_FILE"
    else
        echo "  Running: root -b -l -q -e 'validate($k)'"
        echo "   - Note: Using .rootlogon.C to preload libraries"
        root -b -l -q -e "validate($k,false,false,0)" > "$LOG_FILE" 2>&1
    fi

    if [ ${PIPESTATUS[0]} -eq 0 ]; then
        echo -e "${GREEN}  ✓ Flavor $k [$FLAVOR_NAME] completed successfully${NC}"

        NEWEST_FILE=$(ls -t validate_${FLAVOR_NAME}_*.root 2>/dev/null | head -1)
        if [ -n "$NEWEST_FILE" ]; then
            OUTPUT_FILES+=("$NEWEST_FILE")
            echo -e "${BLUE}    Found output: $NEWEST_FILE${NC}"
        fi
    else
        echo -e "${RED}  ✗ Flavor $k [$FLAVOR_NAME] failed! Check $LOG_FILE${NC}"
    fi
done

END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))
MINUTES=$((ELAPSED / 60))
SECONDS=$((ELAPSED % 60))

echo ""
echo -e "${GREEN}================================================${NC}"
echo -e "${GREEN}Validation analysis complete!${NC}"
echo -e "${GREEN}Total time: ${MINUTES}m ${SECONDS}s${NC}"
echo -e "${GREEN}Logs saved to: logs/${NC}"
echo -e "${GREEN}Histograms saved to: validate_*.root${NC}"
if [ "$MODE" == "debug" ]; then
    echo -e "${GREEN}Event displays saved to: Plots/${NC}"
fi
echo -e "${GREEN}================================================${NC}"

if [ "$DO_MERGE" = true ]; then
    echo ""
    echo -e "${BLUE}================================================${NC}"
    echo -e "${BLUE}Merging output files...${NC}"
    echo -e "${BLUE}================================================${NC}"

    MERGE_START=$(date +%s)

    MERGE_TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    MERGED_OUTPUT="validate_merged_all_${MERGE_TIMESTAMP}.root"

    if [ ${#OUTPUT_FILES[@]} -eq 0 ]; then
        echo -e "${YELLOW}No output files found to merge${NC}"
        echo -e "${YELLOW}Looking for files matching: validate_*.root${NC}"
        ls -la validate_*.root 2>/dev/null || echo -e "${YELLOW}No validate_*.root files found${NC}"
    else
        echo -e "${BLUE}Merging ${#OUTPUT_FILES[@]} files into ${MERGED_OUTPUT}...${NC}"

        hadd -f "$MERGED_OUTPUT" "${OUTPUT_FILES[@]}" > "logs/merge_${MERGE_TIMESTAMP}.log" 2>&1

        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ Merge successful!${NC}"
            echo -e "${GREEN}  Merged output: ${MERGED_OUTPUT}${NC}"

            SIZE=$(du -h "$MERGED_OUTPUT" | cut -f1)
            echo -e "${GREEN}  File size: ${SIZE}${NC}"

            echo ""
            echo -e "${BLUE}Creating combined histograms (summing all flavors)...${NC}"

            root -b -l -q "merge_datasets.C(\"$MERGED_OUTPUT\")" > "logs/combine_${MERGE_TIMESTAMP}.log" 2>&1

            if [ $? -eq 0 ]; then
                echo -e "${GREEN}✓ Combined histograms created successfully${NC}"
            else
                echo -e "${YELLOW}⚠ Warning: Combined histogram creation had issues. Check logs/combine_${MERGE_TIMESTAMP}.log${NC}"
            fi

        else
            echo -e "${RED}✗ Merge failed! Check logs/merge_${MERGE_TIMESTAMP}.log${NC}"
        fi
    fi

    MERGE_END=$(date +%s)
    MERGE_ELAPSED=$((MERGE_END - MERGE_START))

    echo ""
    echo -e "${BLUE}================================================${NC}"
    echo -e "${BLUE}Merge complete! (${MERGE_ELAPSED}s)${NC}"
    echo -e "${BLUE}================================================${NC}"
fi

echo ""
echo -e "${GREEN}All done!${NC}"