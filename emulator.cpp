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
#include <thread>
#include <variant>
#include <cstdint>
#include <array>
#include <limits>
#include <semaphore>
#include <algorithm>
#include <mutex>

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
        }__lc3_packed;
        struct LDI
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int offset:9;
        }__lc3_packed;
        struct LDR
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int baser:3;
            unsigned int offset:6;
        }__lc3_packed;
        struct LEA
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int offset:9;
        }__lc3_packed;
        struct NOT
        {
            unsigned int opcode:4;
            unsigned int dr:3;
            unsigned int sr:3;
            unsigned int one6:6;
        }__lc3_packed;
        struct RET
        {
            unsigned int opcode:4;
            unsigned int zero3:3;
            unsigned int one3:3;
            unsigned int zero6:6;
        }__lc3_packed;
        struct RTI
        {
            unsigned int opcode:4;
            unsigned int zero12:12;
        }__lc3_packed;
        struct ST
        {
            unsigned int opcode:4;
            unsigned int sr:3;
            unsigned int offset:9;
        }__lc3_packed;
        struct STI
        {
            unsigned int opcode:4;
            unsigned int sr:3;
            unsigned int offset:9;
        }__lc3_packed;
        struct STR
        {
            unsigned int opcode:4;
            unsigned int sr:3;
            unsigned int baser:3;
            unsigned int offset:6;
        }__lc3_packed;
        struct TRAP
        {
            unsigned int opcode:4;
            unsigned int zero4:4;
            unsigned int trapvect:8;
        }__lc3_packed;
        struct INVAILD
        {
            unsigned int invaild:16;
        }__lc3_packed;
    }
    typedef std::variant<opcode::INVAILD,\
        opcode::ADD, opcode::AND, opcode::BR, opcode::JMP, opcode::JSR,\
        opcode::LD, opcode::LDI, opcode::LDR, opcode::LEA, opcode::LEA,\
        opcode::NOT, opcode::RET, opcode::RTI, opcode::ST, opcode::STI,\
        opcode::STR, opcode::TRAP> instruction;

    constexpr auto instruction_per_line = 8;
    typedef uint16_t word;
    typedef std::array<word,instruction_per_line> prefetch_line;   
    class emulator
    {
        private:
        prefetch_line* memory;
        word* memory_word;

        size_t memory_size;

        std::mutex thread_init_lock;
        std::thread prefetch_thread;
        std::thread decode_thread;
        std::thread execute_thread;

        std::binary_semaphore prefetch_read_sem;
        size_t prefetch_address;
        prefetch_line current;
        
        word pc;
        enum{n,z,p};
        std::array<bool,3> cond;

        private:
        const instruction& get_instruction_type(const word& inst) const
        {
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wreturn-local-addr"
            switch(inst & 0xf000)
            {
                case 0b0000 :
                    return *reinterpret_cast<const opcode::BR*>(inst);
                case 0b0001 :
                    return *reinterpret_cast<const opcode::ADD*>(inst);
                case 0b0010 :
                    return *reinterpret_cast<const opcode::LD*>(inst);
                case 0b0011 :
                    return *reinterpret_cast<const opcode::ST*>(inst);
            }
            #pragma GCC diagnostic pop
        }
        inline void decode_init()
        {
            std::lock_guard<std::mutex> lock(thread_init_lock);
            prefetch_read_sem.acquire();
        }
        void decode()
        {
            decode_init();
            while(true)           
            {
                
            }
        }
        inline void prefetch_init()
        {
            std::lock_guard<std::mutex> lock(thread_init_lock);
            current = memory[0];
        }
        void prefetch()
        {
            prefetch_init();
            while(true)
            {
                prefetch_read_sem.acquire();
                current = memory[prefetch_address];
                prefetch_read_sem.release();
            }
        }
        public:
        emulator(size_t size = std::numeric_limits<word>::max())
            : prefetch_read_sem(0)
        {
            memory_size = size;
            memory_word = new word[memory_size];
            memory = reinterpret_cast<prefetch_line*>(memory_word);

            prefetch_thread = std::thread(prefetch);
        }
        ~emulator()
        {
            delete[] memory_word;
        }
    }; 
}