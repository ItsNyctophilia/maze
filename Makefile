.DEFAULT_GOAL := maze
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Wwrite-strings -Waggregate-return -Wfloat-equal

maze:

.PHONY: debug
debug: CFLAGS += -g
debug: maze

.PHONY: profile
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: maze

# If this doesn't run, check the executable bit on test.bash
# TODO: Implement check script
#.PHONY: check
#check: maze
#check:
#./test/test.bash


.PHONY: clean
clean:
	$(RM) *.o maze

