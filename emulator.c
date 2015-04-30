#include <stdio.h>
#include "instruction.h"
#define MAX_MEMORY 1000

void printReg(int *reg){
	int ndx = 0;
	while(ndx < 32){
		printf("$%d: %X", ndx, reg[ndx]);
		if(ndx % 2)
			printf("\n");
		else
			printf("\t");
	}
}

void run(int *mem, char sr){
	int *pc = mem, reg[32] = {0}, instrCount = 0, memRefs = 0, cycles = 0;
	Instruction instr;
	if(sr == 'r'){
		for(decode(&instr, pc); execute(&instr, pc, reg, mem, &cycles, &memRefs); decode(&instr, pc))
			instrCount++;
		instrCount++;
		printReg(reg);
	}
	else
		for(sr = getchar(), decode(&instr, pc); sr != 'q' && execute(&instr, pc, reg, mem, &cycles, &memRefs); sr = getchar(), decode(&instr, pc)){
			if(sr == 'r'){
				for(decode(&instr, pc); execute(&instr, pc, reg, mem, &cycles, &memRefs); decode(&instr, pc))
					instrCount++;
				sr = 'q';
			}
			instrCount++;
			printReg(reg);
		}

	printf("\nInstructions Executed: %d\nMemory References: %d\nClock Cycles: %d\n", instrCount, memRefs, cycles);
}

int main(int argc, char **argv){
	if(argc != 3 || *argv[1] != '-' || *(argv[1] + 1) != 's' || *(argv[1] + 1) != 'r')
		printf("Usage: ./spim <-step or -run> <file>");
	else{
		FILE *file = fopen(argv[2], "r");
		int mem[MAX_MEMORY];
		int *tmp = mem;
		if(file){
			while(fscanf(file, "%d", tmp++) != EOF)
				;
			run(mem, *(argv[1] + 1));
		}
	}
	return 0;
}
