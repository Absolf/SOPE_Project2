
CC = gcc
CFLAGS = -Wall -pthread
DEPS = utils.h
OBJ = utils.o
TARGETS = U1 Q1

all: U1 Q1

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

U1: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@
	
Q1: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

test: all
	@echo "Starting Tests ..."
	@touch result.log
	@./test -q 2 -u 3 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 5 -u 5 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 2 -u 5 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 10 -u 10 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 5 -u 10 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 10 -u 5 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 15 -u 10 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 30 -u 35 -f fifo.server | tee -a result.log
	@echo "----------------------------"
	@./test -q 70 -u 80 -f fifo.server | tee -a result.log
	@echo "----------------------------"


clean:
	@rm *.o $(TARGETS)
