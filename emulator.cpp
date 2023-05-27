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
#include <bitset>
#define __lc3_packed __attribute__((packed))
#define __lc3_x86_64 __X86_64__
namespace lc3
{
    namespace opcode
    {
        struct ADD
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int sr:3;
            unsigned int is_imm:1;
            union
            {
                struct
                {
                    unsigned int zero:2;
                    unsigned int sr2:3;
                };
                unsigned int imm:5;
            };
        }__lc3_packed;
        struct AND
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int sr:3;
            unsigned int is_imm:1;
            union
            {
                struct
                {
                    unsigned int zero:2;
                    unsigned int sr2:3;
                };
                unsigned int imm:5;
            };
        }__lc3_packed;
        struct BR
        {
            unsigned int opcode:4;
            unsigned int n:1;
            unsigned int z:1;
            unsigned int p:1;
            unsigned int offset:9;
        }__lc3_packed;
        struct JMP
        {
            unsigned int opcode:4;
            unsigned int zero3:3;
            unsigned int baser:3;
            unsigned int zero6:6;
        }__lc3_packed;
        struct JSR
        {
            unsigned int opcode:4;
            unsigned int is_imm:1;
            union
            {
                unsigned int offset:11;
                struct
                {
                    unsigned int zero2:2;
                    unsigned int baser:3;
                    unsigned int zero6:6;
                };
            };
        }__lc3_packed;
        struct LD
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int offset:9;
        };
        struct LDI
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int offset:9;
        };
        struct LDR
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int baser:3;
            unsigned int offset:6;
        };
        struct LEA
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int offset:9;
        };
        struct NOT
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int sr:3;
            unsigned int one6:6;
        };
        struct RET
        {
            unsigned int opcode:4;
            unsigned int zero3:3;
            unsigned int one3:3;
            unsigned int zero6:6;
        };
        struct RTI
        {
            unsigned int opcode:4;
            unsigned int zero12:12;
        };
        struct ST
        {
            unsigned int opcode:4;
            unsigned int sr:3;
            unsigned int offset:9;
        };
        struct STI
        {
            unsigned int opcode:4;
            unsigned int sr:3;
            unsigned int offset:9;
        };
        struct STR
        {
            unsigned int opcode:4;
            unsigned int sr:3;
            unsigned int baser:3;
            unsigned int offset:6;
        };
        struct TRAP
        {
            unsigned int opcode:4;
            unsigned int zero4:4;
            unsigned int trapvect:8;
        };
        struct INVAILD
        {
            unsigned int invaild:16;
        };
    }
}