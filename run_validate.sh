#!/bin/bash

# run_validate.sh - Run validation analysis on all datasets
#
# Usage:
#   ./run_validate.sh                    # Run all datasets in production mode + merge
#   ./run_validate.sh debug [DS] [FILE]  # Run in debug mode (default: dataset 4, file 4)
#   ./run_validate.sh dataset N          # Run specific dataset N

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
DATASET_START=1
DATASET_END=8
DO_MERGE=false
DEBUG_DATASET=4
DEBUG_FILE=4

if [ "$1" == "debug" ]; then
    MODE="debug"
    if [ -n "$2" ]; then
        DEBUG_DATASET=$2
    fi
    if [ -n "$3" ]; then
        DEBUG_FILE=$3
    fi
    DATASET_START=$DEBUG_DATASET
    DATASET_END=$DEBUG_DATASET
    echo -e "${YELLOW}Running in DEBUG mode (dataset $DEBUG_DATASET, file $DEBUG_FILE)${NC}"
elif [ "$1" == "dataset" ] && [ -n "$2" ]; then
    DATASET_START=$2
    DATASET_END=$2
    echo -e "${YELLOW}Running dataset $2 only${NC}"
elif [ -z "$1" ]; then
    DO_MERGE=true
    echo -e "${YELLOW}Running all datasets in production mode (will merge at end)${NC}"
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

for k in $(seq $DATASET_START $DATASET_END); do
    TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
    echo ""
    echo -e "${GREEN}[$TIMESTAMP] Processing dataset $k...${NC}"

    LOG_FILE="logs/validate_dataset_${k}_$(date +%Y%m%d_%H%M%S).log"

    if [ "$MODE" == "debug" ]; then
        echo "  Running: root -l -q -e 'validate($k,true,true,$DEBUG_FILE)'"
        echo "   - Dataset: $k, File: $DEBUG_FILE"
        echo "   - Note: Using .rootlogon.C to preload libraries"
        root -l -q -e "validate($k,true,true,$DEBUG_FILE)" 2>&1 | tee "$LOG_FILE"
    else
        echo "  Running: root -b -l -q -e 'validate($k)'"
        echo "   - Note: Using .rootlogon.C to preload libraries"
        root -b -l -q -e "validate($k,false,false,0)" > "$LOG_FILE" 2>&1
    fi

    if [ ${PIPESTATUS[0]} -eq 0 ]; then
        echo -e "${GREEN}  ✓ Dataset $k completed successfully${NC}"

        NEWEST_FILE=$(ls -t validate_0${k}_*.root 2>/dev/null | head -1)
        if [ -n "$NEWEST_FILE" ]; then
            OUTPUT_FILES+=("$NEWEST_FILE")
            echo -e "${BLUE}    Found output: $NEWEST_FILE${NC}"
        fi
    else
        echo -e "${RED}  ✗ Dataset $k failed! Check $LOG_FILE${NC}"
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
        echo -e "${YELLOW}Looking for files matching: validate_0*_*.root${NC}"
        ls -la validate_0*.root 2>/dev/null || echo -e "${YELLOW}No validate_0*.root files found${NC}"
    else
        echo -e "${BLUE}Merging ${#OUTPUT_FILES[@]} files into ${MERGED_OUTPUT}...${NC}"

        hadd -f "$MERGED_OUTPUT" "${OUTPUT_FILES[@]}" > "logs/merge_${MERGE_TIMESTAMP}.log" 2>&1

        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ Merge successful!${NC}"
            echo -e "${GREEN}  Merged output: ${MERGED_OUTPUT}${NC}"

            SIZE=$(du -h "$MERGED_OUTPUT" | cut -f1)
            echo -e "${GREEN}  File size: ${SIZE}${NC}"

            echo ""
            echo -e "${BLUE}Creating combined histograms (summing all datasets)...${NC}"

            root -b -l -q "merge_datasets.C(\"$MERGED_OUTPUT\")" > "logs/combine_${MERGE_TIMESTAMP}.log" 2>&1

            if [ $? -eq 0 ]; then
                echo -e "${GREEN}✓ Combined histograms created successfully${NC}"
            else
                echo -e "${YELLOW}⚠ Warning: Combined histogram creation had issues. Check logs/combine_${MERGE_TIMESTAMP}.log${NC}"
            fi

            # echo ""
            # echo -e "${BLUE}Creating per-dataset merged files...${NC}"
            # for ds in $(seq 1 8); do
            #     PATTERN="validate_0${ds}_*.root"
            #     DS_OUTPUT="validate_merged_dataset_0${ds}_${MERGE_TIMESTAMP}.root"

            #     FILE_COUNT=$(ls $PATTERN 2>/dev/null | wc -l)

            #     if [ $FILE_COUNT -gt 0 ]; then
            #         echo -e "${BLUE}  Merging dataset ${ds} (${FILE_COUNT} files)...${NC}"
            #         hadd -f "$DS_OUTPUT" $PATTERN > "logs/merge_ds${ds}_${MERGE_TIMESTAMP}.log" 2>&1

            #         if [ $? -eq 0 ]; then
            #             DS_SIZE=$(du -h "$DS_OUTPUT" | cut -f1)
            #             echo -e "${GREEN}    ✓ ${DS_OUTPUT} (${DS_SIZE})${NC}"
            #         fi
            #     fi
            # done

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
