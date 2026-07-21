#!/usr/bin/env bash
# =============================================================
# regression.sh — Full regression for RISC-V single-cycle CPU
# Usage: bash scripts/regression.sh
# Run from the project root directory.
# =============================================================

set -uo pipefail

GREEN='\033[32m'
RED='\033[31m'
CYAN='\033[36m'
RESET='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0
FAILED_TESTS=()

# All tests: program name == test name == tb subdirectory name
TESTS=(
    arithmetic
    beq
    bne
    blt
    bge
    bltu
    bgeu
    jump
    lw
    sw
    alu_rtype
    alu_itype
    lui_auipc
)

echo -e "${CYAN}"
echo "=============================================="
echo "  RISC-V CPU Full Regression"
echo "  $(date)"
echo "=============================================="
echo -e "${RESET}"

for TEST in "${TESTS[@]}"; do
    echo -e "${CYAN}--- Running: ${TEST} ---${RESET}"

    # Force program.hex regeneration for this specific test.
    # We only remove the shared hex file (not the whole build tree) so that:
    #   1. The correct program is always loaded — never a stale binary.
    #   2. The ELF / obj_dir caches are preserved for faster rebuilds.
    rm -f software/program.hex

    if make PROGRAM="${TEST}" TEST="${TEST}" run 2>&1; then
        echo -e "${GREEN}[PASS] ${TEST}${RESET}\n"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo -e "${RED}[FAIL] ${TEST}${RESET}\n"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        FAILED_TESTS+=("${TEST}")
    fi
done

echo -e "${CYAN}"
echo "=============================================="
echo "  Regression Summary"
echo "=============================================="
echo -e "${RESET}"
echo -e "${GREEN}  Passed : ${PASS_COUNT}${RESET}"

if [ "${FAIL_COUNT}" -gt 0 ]; then
    echo -e "${RED}  Failed : ${FAIL_COUNT}${RESET}"
    echo -e "${RED}  Failing tests:${RESET}"
    for t in "${FAILED_TESTS[@]}"; do
        echo -e "${RED}    - ${t}${RESET}"
    done
    exit 1
else
    echo "  Failed : 0"
    echo -e "${GREEN}  All tests passed!${RESET}"
    exit 0
fi
