#!/bin/bash

GREEN='\033[32m'
RED='\033[31m'
NC='\033[0m'

# Test 1: Program prints error message in response to
# invocation error

PROGRAM=./maze

# Define the sample files to use as input
FILES=""

# Define the options to pass to the program
OPTIONS=""

#Define the expected output
EXPECTED_OUTPUT="Usage: ./maze mazefile"

# Run the program with the sample files and options
$PROGRAM $OPTIONS ${FILES[@]} 2> output.txt

# Expected: Program exits with code 1 for INVOCATION_ERROR
# and outputs usage statement
if [ $? -eq 1 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "1.  Invocation error test              : ${GREEN}PASS${NC}"
else
    echo -e "1.  Invocation error test              : ${RED}FAIL${NC}"
fi

# Test 2: program does not crash with empty file

FILES="./samp/empty.txt"
OPTIONS=""
EXPECTED_OUTPUT="Error: empty file"
$PROGRAM $OPTIONS ${FILES[@]} 2> output.txt

# Expected: Program prints error message and exits with code 4 for INVALID_MAP
if [ $? -eq 4 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "2.  Empty file test                    : ${GREEN}PASS${NC}"
else
    echo -e "2.  Empty file test                    : ${RED}FAIL${NC}"
fi

# Test 3: program solves basic maze

FILES="./samp/basic_maze.txt"
OPTIONS=""
EXPECTED_OUTPUT="########
##...#>#
##.#.#.#
#@.#...#
########"
$PROGRAM $OPTIONS ${FILES[@]} > output.txt

# Expected: Program solves maze and exits with code 0 for SUCCESS
if [ $? -eq 0 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "3.  Basic maze test                    : ${GREEN}PASS${NC}"
else
    echo -e "3.  Basic maze test                    : ${RED}FAIL${NC}"
fi

# Test 4: program handles invalid symbols in input file

FILES="./samp/invalid_symbol.txt"
OPTIONS=""
EXPECTED_OUTPUT="Error: invalid symbol(s) in maze"
$PROGRAM $OPTIONS ${FILES[@]} 2> output.txt

# Expected: Program prints error message and exits with code 4 for INVALID_MAP
if [ $? -eq 4 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "4.  Invalid symbol test                : ${GREEN}PASS${NC}"
else
    echo -e "4.  Invalid symbol test                : ${RED}FAIL${NC}"
fi

# Test 5: program handles impossible maze

FILES="./samp/no_solution.txt"
OPTIONS=""
EXPECTED_OUTPUT="#########
#     ##
#    #  # 
#>   # @#
########"
$PROGRAM $OPTIONS ${FILES[@]} > output.txt

# Expected: Program solves maze and exits with code 0 for SUCCESS
if [ $? -eq 0 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "5.  Impossible maze test               : ${GREEN}PASS${NC}"
else
    echo -e "5.  Impossible maze test               : ${RED}FAIL${NC}"
fi

# Test 6: program handles unbounded maze

FILES="./samp/out_of_bound.txt"
OPTIONS=""
EXPECTED_OUTPUT="Error: unbounded maze"
$PROGRAM $OPTIONS ${FILES[@]} 2> output.txt

# Expected: Program prints error message exits with code 4 for INVALID_MAP
if [ $? -eq 4 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "6.  Unbounded maze test                : ${GREEN}PASS${NC}"
else
    echo -e "6.  Unbounded maze test                : ${RED}FAIL${NC}"
fi

# Test 7: program handles larger, more complex mazes

FILES="./samp/map00.txt"
OPTIONS=""
EXPECTED_OUTPUT="###################### 
#  > ####   ##@.....#  
# .. ##     #######.###
##.#### #   ### .... ##
 #.#### #######..### ##
 #.####......... ### ##
 #......######       ##
#####################  "
$PROGRAM $OPTIONS ${FILES[@]} > output.txt

# Expected: Program solves maze and exits with code 0 for SUCCESS
if [ $? -eq 0 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "7.  Sample maze 00 test                : ${GREEN}PASS${NC}"
else
    echo -e "7.  Sample maze 00 test                : ${RED}FAIL${NC}"
fi

# Test 8: program handles larger, more complex mazes

FILES="./samp/map01.txt"
OPTIONS=""
EXPECTED_OUTPUT="#######################
#  > ####   ##@.....###
# .. ##     #######.###
##.#### #   ### .... ##
##.#### #######..### ##
##.####......... ### ##
##......######       ##
#######################"
$PROGRAM $OPTIONS ${FILES[@]} > output.txt

# Expected: Program solves maze and exits with code 0 for SUCCESS
if [ $? -eq 0 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "8.  Sample maze 01 test                : ${GREEN}PASS${NC}"
else
    echo -e "8.  Sample maze 01 test                : ${RED}FAIL${NC}"
fi

# Test 9: program handles larger, more complex mazes

FILES="./samp/map02.txt"
OPTIONS=""
EXPECTED_OUTPUT="################################################################################
#   ############################################################       #########
# > #######      ##########       #########   ##################       #########
# . #######           #####      .....#####   ###                              #
##.########      #### #####     ..###.#####   ###    ###########       ####### #
##.########      ####       ..... ###.#####   ###    ######################### #
##.########      ########## .     ###.#####   ###    ######################### #
##.######################## .     ###.####### ###    #######  .............### #
##.#########################.########.####### ##############  .     ######.....#
##.#########################.####.....####### ################.###############.#
##..........################.####.########### ################.###############.#
###########.##########    ##.####.########### ######          .#    @ ########.#
###########.##########    ##.####.########### ###### #########.#    . ########.#
###########.##########    ##.####.         ## ######      ####.#    . ########.#
###########.##########      .####.##       ## ######      ####.#    . ########.#
###########.################.####.##       ## ######      ####.#    . ########.#
########   ...##############.####.##       ## ################.#    . ########.#
########     ................####.##       ## ################.#    . ########.#
####   #      ###################.##########...................#   .. ########.#
####   ### #######           ####.##########.###################   .  ########.#
###### ### ####### #########     .##########.##      ###########  ..  ########.#
######     ####### ##############..........#.##      #############.###########.#
#################      ###################...        #############.###########.#
#################      ###########################################.............#
################################################################################
"

$PROGRAM $OPTIONS ${FILES[@]} > output.txt

# Expected: Program solves maze and exits with code 0 for SUCCESS
if [ $? -eq 0 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "9.  Sample maze 02 test                : ${GREEN}PASS${NC}"
else
    echo -e "9.  Sample maze 02 test                : ${RED}FAIL${NC}"
fi

# Test 10: program handles larger, more complex mazes

FILES="./samp/map03.txt"
OPTIONS=""
EXPECTED_OUTPUT="############################################################################# 
#   #########################################################       ######### 
# >.........     ##########       ######   ##################       ######### 
#   #######...........#####      ......... ###   ...........................##
###########      ####.#####     ..### ## ..###   .###########       #######..#
###########      ####............ ### ##  ........##########################.#
###########      #######################   ###    ##########################.#
########################### ......### ########    #######  .............###..#
##.........................#.####.### #### ##############...     ######.....# 
##.#######################...####.........................# ############### # 
##..        ################################################################# 
## ........ #                                                                 
##        ..#                                                                 
##         .################################################################# 
###########.##########    ## ####       ## ######          ...     ######## # 
###########.##########       #### ##    ## ######      ####.#...   ######## # 
###########.################ #### ##    ## ######      ####.#  .   ######## # 
########   .  ############## #### ##    ## ################.#  .   ######## # 
########   .                 #### ##    ## ################.#  .   ######## # 
####   #  ..  ################### #######...................#  .   ######## # 
####   ###.#######...........#### #######.###################  .   ######## # 
###### ###.#######.#########......#######.##      ###########  .   ######## # 
######    .#######.##############.......#.##      #############.########### # 
##########.........    ################...        #############....@        # 
############################################################################# 
"

$PROGRAM $OPTIONS ${FILES[@]} > output.txt

# Expected: Program solves maze and exits with code 0 for SUCCESS
if [ $? -eq 0 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "10. Sample maze 03 test                : ${GREEN}PASS${NC}"
else
    echo -e "10. Sample maze 03 test                : ${RED}FAIL${NC}"
fi

# Cleanup temp files
rm output.txt

