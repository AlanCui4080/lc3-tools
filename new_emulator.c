/**
 * Copyright (C) 2023 AlanCui4080
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define packed __attribute__((packed))

packed struct instruction {
    unsigned int opcode : 4;
	packed union 
    {
        packed struct{
            unsigned int dr : 3;
            unsigned int sr : 3;
            unsigned int is_imm : 1;
            packed union {
                packed struct {
                    unsigned int zero : 2;
                    unsigned int sr2 : 3;
                };
                unsigned int imm : 5;
            };
        } ADD;
        packed struct{
            unsigned int dr : 3;
            unsigned int sr : 3;
            unsigned int is_imm : 1;
            packed union {
                packed struct {
                    unsigned int zero : 2;
                    unsigned int sr2 : 3;
                };
                unsigned int imm : 5;
            };
        } AND;
        packed struct{
            unsigned int n : 1;
            unsigned int z : 1;
            unsigned int p : 1;
            int offset : 9;
        } BR;
        packed struct{
            unsigned int zero3 : 3;
            unsigned int baser : 3;
            unsigned int zero6 : 6;
        } JMP;
        packed struct{
            unsigned int is_imm : 1;
            packed union {
                int offset : 11;
                packed struct {
                    unsigned int zero2 : 2;
                    unsigned int baser : 3;
                    unsigned int zero6 : 6;
                };
            };
        } JSR;
        packed struct{
            unsigned int dr : 3;
            int offset : 9;
        } LD;
        packed struct{
            unsigned int dr : 3;
            int offset : 9;
        } LDI;
        packed struct{
            unsigned int dr : 3;
            unsigned int baser : 3;
            int offset : 6;
        } LDR;
        packed struct{
            unsigned int dr : 3;
            int offset : 9;
        } LEA;
        packed struct{
            unsigned int dr : 3;
            unsigned int sr : 3;
            unsigned int one6 : 6;
        } NOT;
        packed struct{
            unsigned int zero3 : 3;
            unsigned int one3 : 3;
            unsigned int zero6 : 6;
        } RET;
        packed struct{
            unsigned int zero12 : 12;
        } RTI;
        packed struct{
            unsigned int sr : 3;
            int offset : 9;
        } ST;
        packed struct{
            unsigned int sr : 3;
            int offset : 9;
        } STI;
        packed struct{
            unsigned int sr : 3;
            unsigned int baser : 3;
            int offset : 6;
        } STR;
        packed struct{
            unsigned int zero4 : 4;
            unsigned int trapvect : 8;
        } TRAP;
        packed struct{
            unsigned int invaild : 16;
        } INVAILD;
    };
};
typedef uint16_t word;
enum opcode
{
    OPCODE_ADD = 0b0010,
    OPCODE_AND = 0b1010,
    OPCODE_BR = 0b0000,
    OPCODE_JMP = 0b1100,
    OPCODE_JSR = 0b0100,
    OPCODE_LD = 0b0010,
    OPCODE_LDI = 0b1010,
    OPCODE_LDR = 0b0110,
    OPCODE_LEA = 0b1110,
    OPCODE_NOT = 0b1001,
    OPCODE_RTI = 0b1000,
    OPCODE_ST = 0b0011,
    OPCODE_STI = 0b1011,
    OPCODE_STR = 0b0111,
    OPCODE_TRAP = 0b1111
};
struct emulator
{
    word reg[8];
    word* memory;

    packed struct
    {
        unsigned int privilege:1;
        unsigned int rev:9;
        unsigned int priority:3;
        unsigned int n:1;
        unsigned int p:1;
        unsigned int z:1;
    }psr;

    word pc;
    word ssp;
    word usp;
};
void do_interrupt(struct emulator* ctx, unsigned int vector)
{
    ctx->usp = ctx->reg[6];
    ctx->reg[6] = ctx->ssp;
    memcpy((word*)&ctx->psr, &ctx->reg[6], sizeof(word));
    ctx->reg[6]--;
    ctx->reg[6] = ctx->pc;


    ctx->psr.privilege = 0;
    //ctx->psr.priority = 4;

    ctx->pc = ctx->memory[vector];
}
void tool_setcc(struct emulator* ctx, int reg)
{
    if((signed short)ctx->reg[reg] < 0) ctx->psr.n = 1;
    if((signed short)ctx->reg[reg] == 0) ctx->psr.z = 1;
    if((signed short)ctx->reg[reg] > 0) ctx->psr.p = 1;
}
void instruction_LD(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[ins->LD.dr] = ctx->memory[ctx->pc + ins->LD.offset];
    tool_setcc(ctx,ins->LD.dr);
}
void instruction_LDR(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[ins->LDR.dr] = ctx->memory[ctx->reg[ins->LDR.baser] + ins->LDR.offset];
    tool_setcc(ctx,ins->LDR.dr);
}
void instruction_LDI(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[ins->LDI.dr] = ctx->memory[ctx->memory[ctx->pc + ins->LDI.offset]];
    tool_setcc(ctx,ins->LDI.dr);
}
void instruction_ST(struct emulator* ctx, struct instruction* ins)
{
    ctx->memory[ctx->pc + ins->ST.offset] = ctx->reg[ins->ST.sr];
}
void instruction_STR(struct emulator* ctx, struct instruction* ins)
{
    ctx->memory[ctx->reg[ins->STR.baser] + ins->STR.offset] = ctx->reg[ins->STR.sr];
}
void instruction_STI(struct emulator* ctx, struct instruction* ins)
{
    ctx->memory[ctx->memory[ctx->pc + ins->STI.offset]] = ctx->reg[ins->STI.sr];
}
void instruction_JSR(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[7] = ctx->pc;
    if(ins->JSR.is_imm)
    {
        ctx->pc += ins->JSR.offset;
    }
    else
    {
        ctx->pc = ins->JSR.baser;
    }
}
void instruction_NOT(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[ins->NOT.dr] = ~ctx->reg[ins->NOT.sr];
    tool_setcc(ctx,ins->NOT.dr);
}
void instruction_AND(struct emulator* ctx, struct instruction* ins)
{
    if(ins->AND.is_imm)
    {
        ctx->reg[ins->AND.dr] = ctx->reg[ins->AND.sr] & ins->AND.imm;
    }
    else
    {
        ctx->reg[ins->AND.dr] = ctx->reg[ins->AND.sr] & ctx->reg[ins->AND.sr2];
    }
    tool_setcc(ctx,ins->AND.dr);
}
void instruction_ADD(struct emulator* ctx, struct instruction* ins)
{
    if(ins->ADD.is_imm)
    {
        ctx->reg[ins->ADD.dr] = ctx->reg[ins->ADD.sr] + ins->ADD.imm;
    }
    else
    {
        ctx->reg[ins->ADD.dr] = ctx->reg[ins->ADD.sr] + ctx->reg[ins->ADD.sr2];
    }
    tool_setcc(ctx,ins->ADD.dr);
}
void instruction_BR(struct emulator* ctx, struct instruction* ins)
{
    if(((ctx->psr.n) && ins->BR.n)
    || ((ctx->psr.p) && ins->BR.p)
    || ((ctx->psr.z) && ins->BR.z))
    {
        ctx->pc += ins->BR.offset;
    }
}
void instruction_JMP(struct emulator* ctx, struct instruction* ins)
{
    ctx->pc = ins->JMP.baser;
}
void instruction_RTI(struct emulator* ctx, struct instruction* ins)
{
    if(ctx->psr.privilege == 1)
        do_interrupt(ctx,0x0100);
    ctx->pc = ctx->memory[ctx->reg[6]];
    ctx->reg[6]++;
    word temp = ctx->memory[ctx->reg[6]];
    ctx->reg[6]++;
    memcpy((word*)&ctx->psr, &temp, sizeof(word));
    ctx->reg[6] = ctx->usp;

    (void)ins;
}
void instruction_LEA(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[ins->LEA.dr] = ctx->pc + ins->LEA.offset;
    tool_setcc(ctx,ins->LEA.dr);
}
void instruction_TRAP(struct emulator* ctx, struct instruction* ins)
{
    ctx->reg[7] = ctx->pc;
    ctx->pc = ctx->memory[ins->TRAP.trapvect];
}
void(*instruction_func[16])(struct emulator* ctx, struct instruction* ins) = \
{
    instruction_BR,
    instruction_ADD,
    instruction_LD,
    instruction_ST,
    instruction_JSR,
    instruction_AND,
    instruction_LDR,
    instruction_STR,
    instruction_RTI,
    instruction_NOT,
    instruction_LDI,
    instruction_STI,
    instruction_JMP,
    NULL,
    instruction_LEA,
    instruction_TRAP
};
struct emulator* emulator_initialize(struct emulator* ctx, size_t memory_size)
{
    for (size_t i = 0; i < 8; i++)
    {
        ctx->reg[i] = 0;
    }

    ctx->memory = calloc(sizeof(word), memory_size);
    if (ctx->memory == NULL)
        return NULL;

    memset((word*)&ctx->psr, 0, sizeof(word));

    ctx->pc = 0;

    return ctx;
}
struct instruction* emulator_fetch(struct emulator* ctx)
{
    ctx->pc++;
    return (struct instruction *)&(ctx->memory[ctx->pc - 1]);
}
void emulator_decode_execute(struct emulator* ctx, struct instruction* ins)
{
    if(instruction_func[ins->opcode] == NULL)
        do_interrupt(ctx,0x0101);
    instruction_func[ins->opcode](ctx,ins);
}
struct emulator* emulator_start(struct emulator* ctx)
{
    while(1)
    {
        emulator_decode_execute(ctx,emulator_fetch(ctx));
    }
}