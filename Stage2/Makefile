
CC = gcc
CFLAGS = -Wall -pthread -pedantic
DEPS = util.h
OBJ = utils.o
TARGETS = U2 Q2

all: U2 Q2

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

Q2: $(OBJ)
	@echo "Compiling $@ ..."
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo "Successfully compiled!"
	@echo "Run with ./$@"

U2: $(OBJ)
	@echo "Compiling $@ ..."
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo "Successfully compiled!"
	@echo "Run with ./$@"

test: all
	@echo "Starting Tests ..."
	@touch result.log
	@./test -q 2 -u 3 -f fifo.server -n 5 | tee -a result.log
	@echo "----------------------------"
	@./test -q 5 -u 5 -f fifo.server -n 1 | tee -a result.log
	@echo "----------------------------"
	@./test -q 2 -u 5 -f fifo.server -n 2 | tee -a result.log
	@echo "----------------------------"
	@./test -q 10 -u 10 -f fifo.server -n 20 | tee -a result.log
	@echo "----------------------------"
	@./test -q 5 -u 10 -f fifo.server -n 50 | tee -a result.log
	@echo "----------------------------"
	@./test -q 10 -u 5 -f fifo.server -n 0 | tee -a result.log
	@echo "----------------------------"
	@./test -q 15 -u 10 -f fifo.server -n 1000 | tee -a result.log
	@echo "----------------------------"
	@./test -q 30 -u 35 -f fifo.server -n 2 | tee -a result.log
	@echo "----------------------------"
	@./test -q 70 -u 80 -f fifo.server -n 10 | tee -a result.log
	@echo "----------------------------"
	@./test -q 2 -u 3 -f fifo.server -n 5 -l 3 | tee -a result.log
	@echo "----------------------------"
	@./test -q 5 -u 5 -f fifo.server -n 1 -l 20 | tee -a result.log
	@echo "----------------------------"
	@./test -q 2 -u 5 -f fifo.server -n 2 -l 5 | tee -a result.log
	@echo "----------------------------"
	@./test -q 10 -u 10 -f fifo.server -n 20 -l 10 | tee -a result.log
	@echo "----------------------------"
	@./test -q 10 -u 5 -f fifo.server -n 0 -l 2| tee -a result.log
	@echo "----------------------------"
	@./test -q 15 -u 10 -f fifo.server -n 1000 -l 20 | tee -a result.log
	@echo "----------------------------"
	@./test -q 30 -u 35 -f fifo.server -n 2 -l 4 | tee -a result.log
	@echo "----------------------------"
	@./test -q 70 -u 80 -f fifo.server -n 10 -l 100 | tee -a result.log
	@echo "----------------------------"


	@echo "\033[1;32mTests PASSED:" `grep "PASSED" result.log | wc -l`;
	@echo "\033[1;31mTests FAILED:" `grep "FAILED" result.log | wc -l`;

	@rm result.log
	@make -s clean

clean:
	@rm *.o $(TARGETS)