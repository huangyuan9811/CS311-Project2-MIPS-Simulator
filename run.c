/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
	if (pc >= MEM_TEXT_START + text_size)
		return NULL;
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
	instruction* instr = get_inst_info(CURRENT_STATE.PC);

	int debug_set = FALSE;

	if (instr == NULL) {
		RUN_BIT = FALSE;
		return;
	}

	if (debug_set) {
		printf("opcode: 0x%x\n", OPCODE(instr));
		
		if (OPCODE(instr) == 0x00)
			printf("funct: 0x%x\n", FUNC(instr));
	}

	if (OPCODE(instr) == 0x00) { // R-type
		unsigned char rs = RS(instr);
		unsigned char rt = RT(instr);
		unsigned char rd = RD(instr);
		unsigned char shamt = SHAMT(instr);
		
		CURRENT_STATE.PC += 4;

		switch (FUNC(instr)) {
			case 0x21: // addu
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
			break;
			case 0x24: // and
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
			break;
			case 0x08: // jr
			CURRENT_STATE.PC = CURRENT_STATE.REGS[rs];
			break;
			case 0x27: // nor
			CURRENT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
			break;
			case 0x25: // or
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
			break;
			case 0x2B: // sltu
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt];
			break;
			case 0x00: // sll
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
			break;
			case 0x02: // srl
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
			break;
			case 0x23: // subu
			CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
			break;
			default:
			// printf("\n\n\nInvalid R-type instruction with funct=%x\n\n\n", FUNC(instr));
			break;
		}
	} else if (OPCODE(instr) == 0x02 || OPCODE(instr) == 0x03) { // J-type
		uint32_t target = TARGET(instr);

		if (debug_set) {
			printf("target: 0x%x\n", target);
		}

		if (OPCODE(instr) == 0x02) { // j
			CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target << 2);
		} else { // jal
			CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
			CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target << 2);
		}
	} else { // I-type
		unsigned char rs = RS(instr);
		unsigned char rt = RT(instr);
		int32_t imm = (int32_t)IMM(instr);

		if (debug_set) {
			printf("imm: %d\n", imm);
			printf("rs: %d\n", rs);
			printf("rt: %d\n", rt);
		}

		CURRENT_STATE.PC += 4;	

		switch (OPCODE(instr)) {
			case 0x09: // addiu
			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
			break;
			case 0x0C: // andi
			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
			break;
			case 0x04: // beq
			CURRENT_STATE.PC += (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt] ?(imm << 2) :0);
			break;
			case 0x05: // bne
			// debug
			if (debug_set) {
				printf("bne rs != rt\n");
				printf("(0x%x) != (0x%x)\n", CURRENT_STATE.REGS[rs], CURRENT_STATE.REGS[rt]);
				printf("Old PC: 0x%x\n", CURRENT_STATE.PC);
			}

			CURRENT_STATE.PC += (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt] ?(imm << 2) :0);
			
			if (debug_set) {
				printf("New PC: 0x%x\n", CURRENT_STATE.PC);
			}
			break;
			case 0x0F: // lui
			CURRENT_STATE.REGS[rt] = imm << 16;
			break;
			case 0x23: // lw
			CURRENT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + imm);
			break;
			case 0x0D: // ori
			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
			break;
			case 0x0B: // sltiu
			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < imm;
			break;
			case 0x2B: // sw
			mem_write_32(CURRENT_STATE.REGS[rs] + imm, CURRENT_STATE.REGS[rt]);
			break;
			default:
			// printf("\n\n\nInvalid I-type instruction with opcode=%x\n\n\n", OPCODE(instr));
			break;
		}
	}
}
