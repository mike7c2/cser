CC             = gcc
OD             = objdump
CFLAGS         = -I. -g -O2 -std=c99 -DCSER_DBG_HASH_MODE
DEPS           = cser_dbg.h
OBJ            = cser_dbg.o cser_dbg_test.o
BUILD_LOG      = build.log
.DEFAULT_GOAL  := cser_dbg_test


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 2>&1 | tee -a $(BUILD_LOG)

.PHONY: clear_log
clear_log:
	rm $(BUILD_LOG) -f

cser_dbg_test: $(clear_log) $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -Wall -pedantic -DCSER_DBG_HASH_MODE -Wextra -Wl,-Map,cser.map 2>&1 | tee -a $(BUILD_LOG)

clean: $(clear_log)
	rm *.o
	rm cser_dbg_test -f
