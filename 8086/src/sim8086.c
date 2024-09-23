#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "sim8086.h"

static uint8_t* read_file(const char* path, size_t* out_size){
    FILE* f = fopen(path, "rb"); // "rb" <- reading it in binary mode
    assert(f != NULL);

    assert(fseek(f,0,SEEK_END) == 0);
    size_t size = ftell(f);

    assert(fseek(f,0,SEEK_SET) == 0);
    uint8_t* data = malloc(size);
    assert(fread(data, size, 1, f) == 1);
    if (out_size) *out_size = size;
    fclose(f);
    return data;
}

void dump_memory(){
    FILE *f = fopen("output.data","wb");
    assert(f!= NULL);
    if (fwrite(memory, sizeof(uint8_t), MEMORY_SIZE, f) != MEMORY_SIZE)
    {
        fprintf(stderr, "ERROR writing to file");
    }
    fclose(f);
}

void print_binary(unsigned int num) {
    // Get the number of bits in an unsigned int
    int bits_to_print = 8;
    int i;
    // Iterate through each bit, starting from the most significant bit
    printf(";");
    for (i = bits_to_print - 1; i >= 0; i--) {
        // Print the bit (0 or 1)
        printf("%u", (num >> i) & 1);
    }
    printf("\n");
}


void print_flag(uint8_t test){
   if ( ((test >> 1) & 1) == 1) printf("S");
   if ( (test & 1) == 1) printf("Z");
}

void print_ip(int ip_old){
    printf(" ip: 0x%01x->0x%01x",ip_old,ip);
}

void print_registers(void) {
    printf("\n");
    printf("Final registers:\n");
    int list_reg[8] = {0,3,1,2,4,5,6,7};
    for (int i=0;i<NUM_REGISTERS;i++){
        int idx = list_reg[i];
        if (registers[idx] != 0){
            printf("        %s: 0x%04X (%d)\n", reg_word[idx], registers[idx], registers[idx]);
        }
    }
    printf("        %s: 0x%04X (%d)\n", "ip", ip, ip);
    printf("   flags: ");
    print_flag(flags);
    printf("\n");
}

void mov_imm_tofrom_reg(uint8_t REG, uint16_t val){
    printf("; %s:0x%04X->0x%04X",reg_word[REG],registers[REG],val);
    registers[REG] = val;
}

void mov_imm_to_regmem(uint8_t R_M, uint16_t displacement, uint8_t val){
    uint32_t reg = get_effective_address_value(R_M);
    printf(";");
    memory[reg + displacement] = val;
    printf("displacement %d",displacement);
}

void mov_imm_to_direct_mem(uint16_t address, uint16_t val){
    memory[address] = val;
}

void mov_reg_to_reg(uint8_t dest, uint8_t src){
    printf("; %s:0x%04X->0x%04X",reg_word[dest],registers[dest],registers[src]);
    registers[dest] = registers[src];
}

void mov_reg_tofrom_direct(uint8_t D, uint8_t REG,int16_t displacement){
    if (D == 1){
        printf("; %s:0x%04X->0x%04X",reg_word[REG],registers[REG],memory[displacement]);
        registers[REG] = memory[displacement];
    }
    else if (D == 0){
        printf(";");
        uint16_t reg = registers[REG];
        memory[displacement] = reg;
    }
}

void mov_reg_tofrom_mem(uint8_t D,int16_t displacement,uint8_t REG,uint8_t R_M){
    uint32_t m_address = get_effective_address_value(R_M) + displacement;
    if (D == 1){
        printf("; %s:0x%04X->0x%04X",reg_word[REG],registers[REG],memory[m_address]);
        registers[REG] = memory[m_address];
    }
    else if (D == 0){
        printf(";");
        uint16_t reg = registers[REG];
        memory[m_address] = reg;
    }
}

void check_flag(uint16_t result){
    uint8_t old_flags = flags;
    flags = 0;
    // sign flag
    if ( (result >> 15) == 1){
        flags = flags | 2;
    } 
    // zero flag
    if (result == 0){
        flags = flags | 1;
    } 
    if ( (old_flags ^ flags) != 0){
        printf(" flags:");
        print_flag(old_flags);
        printf("->");
        print_flag(flags);
    }
}

void add_reg_to_reg(uint8_t dest, uint8_t src){
    uint16_t result = registers[dest] + registers[src];
    printf("; %s:0x%04X->0x%04X",reg_word[dest],registers[dest],result);
    registers[dest] = result;
    check_flag(result);
}

void add_imm_tofrom_reg(uint8_t REG, uint16_t val){
    uint16_t result = registers[REG] + val;
    printf("; %s:0x%04X->0x%04X",reg_word[REG],registers[REG],result);
    registers[REG] = result;
    check_flag(result);
}

void add_reg_tofrom_mem(uint8_t D,int16_t displacement,uint8_t REG,uint8_t R_M){
    uint32_t m_address = get_effective_address_value(R_M) + displacement;
    if (D == 1){
        uint16_t result = registers[REG] + memory[m_address];
        printf("; %s:0x%04X->0x%04X",reg_word[REG],registers[REG],result);
        registers[REG] = result;
        check_flag(result);
    }
    else if (D == 0){
        printf(";");
        uint16_t result = registers[REG] + memory[m_address];
        memory[m_address] = result;
    }
}

void sub_reg_to_reg(uint8_t dest, uint8_t src){
    uint16_t result = registers[dest] - registers[src];
    printf("; %s:0x%04X->0x%04X",reg_word[dest],registers[dest],result);
    registers[dest] = result;
    check_flag(result);
}

void sub_imm_tofrom_reg(uint8_t REG, uint16_t val){
    uint16_t result = registers[REG] - val;
    printf("; %s:0x%04X->0x%04X",reg_word[REG],registers[REG],result);
    registers[REG] = result;
    check_flag(result);
}

void cmp_reg_to_reg(uint8_t dest, uint8_t src){
    uint16_t result = registers[dest] - registers[src];
    printf(";");
    check_flag(result);
}

void cmp_imm_tofrom_reg(uint8_t REG, uint16_t val){
    uint16_t result = registers[REG] - val;
    printf(";");
    check_flag(result);
}

void clock_mov_reg_to_reg(){
    clock_count += 2;
    printf("; Clocks: +2 = %u | ",clock_count);
}

void clock_mov_imm_tofrom_reg(){
    clock_count += 4;
    printf("; Clocks: +4 = %u | ",clock_count);
}

void clock_mov_reg_tofrom_direct(int displacement){
    int total_clock = 8;
    int ea = 6;
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,8,ea);
    }
    else{
        total_clock += 4;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,8,ea,4);
    }
}

void clock_mov_reg_from_memory(uint8_t R_M, int displacement){
    int total_clock = 8;
    int ea;
    // total_clock += ea;

    if (displacement == 0){
        if (R_M == 0 || R_M == 3) ea = 7;
        else if (R_M == 1 || R_M == 2) ea = 8;
        else ea = 5;
    }
    else{
        if (R_M == 0 || R_M == 3) ea = 11;
        else if (R_M == 1 || R_M == 2) ea = 12;
        else ea = 9;
    }
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,8,ea);
    }
    else{
        total_clock += 4;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,8,ea,4);
    }
}

void clock_mov_reg_to_memory(uint8_t R_M, int displacement){
    int base = 9;
    int total_clock = base;
    int ea;
    // total_clock += ea;

    if (displacement == 0){
        if (R_M == 0 || R_M == 3) ea = 7;
        else if (R_M == 1 || R_M == 2) ea = 8;
        else ea = 5;
    }
    else{
        if (R_M == 0 || R_M == 3) ea = 11;
        else if (R_M == 1 || R_M == 2) ea = 12;
        else ea = 9;
    }
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,base,ea);
    }
    else{
        total_clock += 4;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,base,ea,4);
    }
}

void clock_add_reg_to_reg(){
    clock_count += 3;
    printf("; Clocks: +3 = %u | ",clock_count);
}

void clock_add_imm_tofrom_reg(){
    clock_count += 4;
    printf("; Clocks: +4 = %u | ",clock_count);
}

void clock_add_reg_from_memory(uint8_t R_M, int displacement){
    int base = 9;
    int total_clock = base;
    int ea;
    // total_clock += ea;

    if (displacement == 0){
        if (R_M == 0 || R_M == 3) ea = 7;
        else if (R_M == 1 || R_M == 2) ea = 8;
        else ea = 5;
    }
    else{
        if (R_M == 0 || R_M == 3) ea = 11;
        else if (R_M == 1 || R_M == 2) ea = 12;
        else ea = 9;
    }
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,base,ea);
    }
    else{
        total_clock += 4;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,base,ea,4);
    }
}

void clock_add_reg_to_memory(uint8_t R_M, int displacement){
    int base = 16;
    int total_clock = base;
    int ea;
    // total_clock += ea;

    if (displacement == 0){
        if (R_M == 0 || R_M == 3) ea = 7;
        else if (R_M == 1 || R_M == 2) ea = 8;
        else ea = 5;
    }
    else{
        if (R_M == 0 || R_M == 3) ea = 11;
        else if (R_M == 1 || R_M == 2) ea = 12;
        else ea = 9;
    }
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,base,ea);
    }
    else{
        total_clock += 8;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,base,ea,8);
    }
}

void clock_add_reg_tofrom_direct(int displacement){
    int base = 9;
    int total_clock = base;
    int ea = 6;
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,base,ea);
    }
    else{
        total_clock += 4;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,base,ea,4);
    }
}

void clock_add_imm_to_memory(uint8_t R_M, int displacement){
    int base = 17;
    int total_clock = base;
    int ea;
    // total_clock += ea;

    if (displacement == 0){
        if (R_M == 0 || R_M == 3) ea = 7;
        else if (R_M == 1 || R_M == 2) ea = 8;
        else ea = 5;
    }
    else{
        if (R_M == 0 || R_M == 3) ea = 11;
        else if (R_M == 1 || R_M == 2) ea = 12;
        else ea = 9;
    }
    total_clock += ea;
    if (displacement % 2 == 0){
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea) | ",total_clock,clock_count,base,ea);
    }
    else{
        total_clock += 8;
        clock_count += total_clock;
        printf("; Clocks: %+d = %u (%d + %dea + %dp) | ",total_clock,clock_count,base,ea,8);
    }
}




void get_arithmetic(uint8_t MOD, char* ans){
    if (MOD == 0){
        strcpy(ans,"add");
    }
    else if (MOD == 5){
        strcpy(ans,"sub");
    }
    else if (MOD == 7){
        strcpy(ans,"cmp");
    }
}

const int16_t get_displacement(uint8_t* data, uint8_t MOD){
    int16_t displacement = 0;
    if (MOD == 1){
        displacement += (int8_t) data[ip++];
    }
    else if (MOD == 2){
        displacement += data[ip++];
        displacement += (data[ip++] << 8);
    }
    return displacement;
}

void print_reg_to_reg(char* operation,uint8_t D,uint8_t W,uint8_t R_M,uint8_t REG,int ip_old){
    const char* source;
    const char* destination;
    uint8_t src = REG;
    uint8_t dest = R_M;
    if (D != 0){
        uint8_t tmp = src;
        src = dest;
        dest = tmp;
    }

    if (D == 0){
        destination = get_REG(W,R_M);
        source = get_REG(W,REG);
    }
    else{
        source = get_REG(W,R_M);
        destination = get_REG(W,REG);
    }
    printf("%s %s, %s",
    operation,destination, source);
    if (showclocks){
        if (!strcmp(operation,"mov")) clock_mov_reg_to_reg();
        if (!strcmp(operation,"add")) clock_add_reg_to_reg();
    }
    if (execution){
        if (!strcmp(operation,"mov")) mov_reg_to_reg(dest,src);
        else if (!strcmp(operation,"add")) add_reg_to_reg(dest,src);
        else if (!strcmp(operation,"sub")) sub_reg_to_reg(dest,src);
        else if (!strcmp(operation,"cmp")) cmp_reg_to_reg(dest,src);
        print_ip(ip_old);
    }     
    printf("\n");
}

void print_reg_memory_to_from_reg(char* operation,
    int16_t displacement,uint8_t D,uint8_t W,uint8_t R_M,uint8_t REG,int ip_old){
    if (D == 1){
        printf("%s %s, [%s%+d]",
            operation,
            get_REG(W,REG),
            get_effective_address(R_M),
            displacement
            );
        if (showclocks){
            if (!strcmp(operation,"mov")) clock_mov_reg_from_memory(R_M,displacement);
            if (!strcmp(operation,"add")) clock_add_reg_from_memory(R_M,displacement);
        }
    }
    else if(D == 0){
        printf("%s [%s%+d], %s",
            operation,
            get_effective_address(R_M),
            displacement,
            get_REG(W,REG));
        if (showclocks){
            if (!strcmp(operation,"mov")) clock_mov_reg_to_memory(R_M,displacement);
            if (!strcmp(operation,"add")) clock_add_reg_to_memory(R_M,displacement);
        }
    }
    else{
        fprintf(stderr, "D = %d not implemented!\n",D);
        exit(EXIT_FAILURE);
    }
    if (execution){
        if (!strcmp(operation,"mov")) mov_reg_tofrom_mem(D,displacement,REG,R_M);
        else if (!strcmp(operation,"add")) add_reg_tofrom_mem(D,displacement,REG,R_M);
        else if (!strcmp(operation,"sub")) printf("not implemented yet");
        else if (!strcmp(operation,"cmp")) printf("not implemented yet");
        print_ip(ip_old);
    }     
    printf("\n");
}

void print_reg_tofrom_direct(char* operation,int16_t displacement,uint8_t D,uint8_t W,uint8_t R_M,uint8_t REG,int ip_old){
    const char* reg_name = get_REG(W,REG);
    if (D == 1){
        printf("%s %s, [%+d]",
            operation,
            reg_name,
            displacement
            );
        if (showclocks){
            if (!strcmp(operation,"mov")) clock_mov_reg_tofrom_direct(displacement);
            if (!strcmp(operation,"add")) clock_add_reg_tofrom_direct(displacement);
        }
    }
    else if(D == 0){
        printf("%s [%+d], %s",
            operation,
            displacement,
            reg_name);
    }
    else{
        fprintf(stderr, "D = %d not implemented!\n",D);
        exit(EXIT_FAILURE);
    }
    if (execution){
        if (!strcmp(operation,"mov")) mov_reg_tofrom_direct(D,REG,displacement);
        // else if (!strcmp(operation,"add")) add_reg_to_reg(dest,src);
        // else if (!strcmp(operation,"sub")) sub_reg_to_reg(dest,src);
        // else if (!strcmp(operation,"cmp")) cmp_reg_to_reg(dest,src);
        print_ip(ip_old);
    }     
    printf("\n");
}

void disasm_regmem_tofrom_either(uint8_t Left,char* operation,uint8_t* data){
    int ip_old = ip - 1;
    uint8_t Right = data[ip++];
    uint8_t W = Left & 1;
    uint8_t D = (Left >> 1) & 1;
    uint8_t R_M = Right & 7;
    uint8_t REG = (Right >> 3) & 7;
    uint8_t MOD = (Right >> 6) & 3;
    if (MOD == 3){
        print_reg_to_reg(operation,D,W,R_M,REG,ip_old);
    }
    else if (MOD == 0 && R_M == 6){ // direct address
        int16_t displacement = get_displacement(data, 2);
        print_reg_tofrom_direct(operation,displacement,D,W,R_M,REG,ip_old);
    }
    else if (MOD == 0 || MOD == 1 || MOD == 2){
        int16_t displacement = get_displacement(data, MOD);
        print_reg_memory_to_from_reg(operation,displacement,D,W,R_M,REG,ip_old);
    }
    else{
        fprintf(stderr, "MOD = %d not implemented!\n",MOD);
        exit(EXIT_FAILURE);
    }
}

void disasm_mov_imm_tofrom_reg(uint8_t Left,char* operation,uint8_t* data){
    int ip_old = ip - 1;
    uint8_t W = (Left >> 3) & 1;
    uint8_t REG = Left & 7;
    uint16_t val = data[ip++];
    if (W  == 1){
        val = val + (data[ip++] << 8);
    }
    printf("%s %s, %d",
        operation,
        get_REG(W,REG), val);
    if (showclocks){
        if (!strcmp(operation,"mov")) clock_mov_imm_tofrom_reg();
    }
    if (execution){
        if (!strcmp(operation,"mov")) mov_imm_tofrom_reg(REG, val);
        print_ip(ip_old);
    }     
    printf("\n");
}
void disasm_mov_imm_tofrom_regmem(uint8_t Left,char* operation,uint8_t* data){
    int ip_old = ip - 1;
    uint8_t W = Left & 1;
    uint8_t Right = data[ip++];
    uint8_t MOD = (Right >> 6) & 3;
    uint8_t R_M = Right & 7;
    int16_t displacement;
    if (MOD == 0 && R_M == 6){ // Direct address
        displacement = get_displacement(data, 2);
    }
    else {
        displacement = get_displacement(data, MOD);
    }
    uint16_t val = data[ip++];
    char explicit_size[5];
    if (W == 0){
        strcpy(explicit_size,"byte");
    }
    else if (W == 1){
        val = val + (data[ip++] << 8);
        strcpy(explicit_size,"word");
    }
    if (MOD == 0 && R_M == 6){
        printf("%s %s [%+d], %d",
            operation,
            explicit_size,
            displacement,
            val
            );
        if (showclocks){
            if (!strcmp(operation,"mov")) clock_mov_reg_tofrom_direct(displacement);
            if (!strcmp(operation,"add")) clock_add_reg_tofrom_direct(displacement);
        }
        if (execution){
            if (!strcmp(operation,"mov")) mov_imm_to_direct_mem(displacement, val);
            print_ip(ip_old);
        }     
    }
    else{
        printf("%s [%s%+d], %s %d",
            operation,
            get_effective_address(R_M),
            displacement,
            explicit_size,
            val
            );
        if (execution){
            if (!strcmp(operation,"mov")) mov_imm_to_regmem(R_M,displacement,val);
            print_ip(ip_old);
        }     
    }
    printf("\n");
}

void disasm_arithmetic_imm_tofrom_regmem(uint8_t Left,uint8_t* data){
    int ip_old = ip - 1;
    uint8_t SW = Left & 3;
    uint8_t W = Left & 1;
    uint8_t Right = data[ip++];
    uint8_t OPCODE = (Right >> 3) & 7;
    char operation[4];
    get_arithmetic(OPCODE, operation);
    uint8_t MOD = (Right >> 6) & 3;
    uint8_t R_M = Right & 7;
    if (MOD == 3){
        uint16_t val = data[ip++];
        if (SW  == 1){
            val = val + (data[ip++] << 8);
        }
        printf("%s %s, %d",
            operation,
            get_REG(W,R_M), val);
        if (showclocks){
            if (!strcmp(operation,"add")) clock_add_imm_tofrom_reg();
        }
        if (execution){
            if (!strcmp(operation,"add")) add_imm_tofrom_reg(R_M, val);
            if (!strcmp(operation,"sub")) sub_imm_tofrom_reg(R_M, val);
            if (!strcmp(operation,"cmp")) cmp_imm_tofrom_reg(R_M, val);
            print_ip(ip_old);
        }     
        printf("\n");
        }
    else{
        char explicit_size[5];
        if (W == 0){
            strcpy(explicit_size,"byte");
        }
        else{
            strcpy(explicit_size,"word");
        }
        if(MOD == 0 && R_M == 6){
            int16_t displacement = get_displacement(data, 2);
            uint16_t val = data[ip++];
            if (SW == 1){
                val = val + (data[ip++] << 8);
            }
            printf("%s %s [%+d], %d",
                operation,
                explicit_size,
                // get_effective_address(R_M),
                displacement,
                val
                );
        }
        else{
            int16_t displacement = get_displacement(data, MOD);
            uint16_t val = data[ip++];
            if (SW == 1){
                val = val + (data[ip++] << 8);
            }
            printf("%s %s [%s%+d], %d",
                operation,
                explicit_size,
                get_effective_address(R_M),
                displacement,
                val
                );
            if (showclocks){
                if (!strcmp(operation,"add")) clock_add_imm_to_memory(R_M,displacement);
            }
        }
        printf("\n");
    }
}

void disasm_arithmetic_imm_to_acc(uint8_t Left,uint8_t* data){
    uint8_t OPCODE = (Left >> 3) & 7;
    char operation[4];
    get_arithmetic(OPCODE, operation);
    uint8_t W = Left & 1;
    int16_t val = data[ip++];
    char ax_al[3];
    if (W == 0){
        strcpy(ax_al,"al");
        val = (int8_t) val;
    }
    else if (W == 1){
        val = val + (data[ip++] << 8);
        strcpy(ax_al,"ax");
    }
    printf("%s %s, %d \n",
        operation,
        ax_al,
        val
        );
}

void get_jump_type(uint8_t Left, char* jump_type){
    if ((Left & 15) == 0b0100) strcpy(jump_type,"je");
    else if ((Left & 15) == 0b1100) strcpy(jump_type,"jl");
    else if ((Left & 15) == 0b1110) strcpy(jump_type,"jle");
    else if ((Left & 15) == 0b0010) strcpy(jump_type,"jb");
    else if ((Left & 15) == 0b0110) strcpy(jump_type,"jbe");
    else if ((Left & 15) == 0b1010) strcpy(jump_type,"jp");
    else if ((Left & 15) == 0b0000) strcpy(jump_type,"jo");
    else if ((Left & 15) == 0b1000) strcpy(jump_type,"js");
    else if ((Left & 15) == 0b0101) strcpy(jump_type,"jnz");
    else if ((Left & 15) == 0b1101) strcpy(jump_type,"jnl");
    else if ((Left & 15) == 0b1111) strcpy(jump_type,"jg");
    else if ((Left & 15) == 0b0011) strcpy(jump_type,"jnb");
    else if ((Left & 15) == 0b0111) strcpy(jump_type,"ja");
    else if ((Left & 15) == 0b1011) strcpy(jump_type,"jnp");
    else if ((Left & 15) == 0b0001) strcpy(jump_type,"jno");
    else if ((Left & 15) == 0b1001) strcpy(jump_type,"jns");
    else{
        fprintf(stderr, "Left = %d not implemented!\n",Left);
        exit(EXIT_FAILURE);
    }

}

void disasm_jump(uint8_t Left, uint8_t* data){
    // char destination[12];
    int ip_old = ip - 1;
    char jump_type[5];
    get_jump_type(Left,jump_type);
    printf("%s &%+d;",
        jump_type,(int8_t) data[ip++] + 2
        );
    if (execution) {
        if (!strcmp("jnz",jump_type)){
            if ( (flags & 1) == 0 ){
                ip = ip + ((int8_t) data[ip-1]);
            }
        }
        else if (!strcmp("je",jump_type)){
            if ( (flags & 1) == 1 ){
                ip = ip + ((int8_t) data[ip-1]);
            }
        }
        else if (!strcmp("jb",jump_type)){
            if ( (flags & 0x10) == 0x10 ){
                ip = ip + ((int8_t) data[ip-1]);
            }
        }
        // else if (!strcmp("jp",jump_type)){
        //     if ( (flags & 0x10) == 0x10 ){
        //         ip = ip + ((int8_t) data[ip-1]);
        //     }
        // }
        print_ip(ip_old);
    }
    printf("\n");
}

void disasm_loop(uint8_t Left, uint8_t* data){
    int ip_old = ip - 1;
    char loop_type[7];
    if ((Left & 15) == 0b0010) strcpy(loop_type,"loop");
    else if ((Left & 15) == 0b0001) strcpy(loop_type,"loopz");
    else if ((Left & 15) == 0b0000) strcpy(loop_type,"loopnz");
    else if ((Left & 15) == 0b0011) strcpy(loop_type,"jcxz");
    printf("%s &%+d;",
        loop_type,(int8_t) data[ip++] + 2
        );
    if (execution){
        if (!strcmp("loopnz",loop_type)){
            if ( (flags & 1) == 0 ){
                ip = ip + ((int8_t) data[ip-1]);
                sub_imm_tofrom_reg(1,1);
            }
        }
        print_ip(ip_old);
    }
    printf("\n");
}

void disasm(uint8_t* data, size_t size){
    ip = 0;
    while (ip < size){
        uint8_t Left = data[ip++];
        if (Left >> 2 == 0b100010){
            char operation[] = "mov";
            disasm_regmem_tofrom_either(Left,operation,data);
        }
        else if((Left >> 4) == 0b1011){
            char operation[] = "mov";
            disasm_mov_imm_tofrom_reg(Left,operation,data);
        }
        else if(Left >> 1 == 0b1100011){
            char operation[] = "mov";
            disasm_mov_imm_tofrom_regmem(Left,operation,data);
        }
        else if(Left >> 6 == 0b00 && ((Left >> 2) & 1) == 0){
            uint8_t OPCODE = (Left >> 3) & 7;
            char operation[4];
            get_arithmetic(OPCODE, operation);
            disasm_regmem_tofrom_either(Left,operation,data);
        }
        else if(Left >> 6 == 0b00 && ((Left >> 2) & 1) == 1){
            disasm_arithmetic_imm_to_acc(Left,data);
        }
        else if(Left >> 2 == 0b100000){
            disasm_arithmetic_imm_tofrom_regmem(Left,data);
        }
        else if(Left >> 4 == 0b0111){
            disasm_jump(Left,data);
        }
        else if(Left >> 4 == 0b1110){
            disasm_loop(Left,data);
        }
        else{
            fprintf(stderr, "Left = %d not implemented!\n",Left);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv){
    if (argc == 1){
        fprintf(stderr, "Put a binary file as an argument\n");
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-exec") == 0) {
            execution = true;
        }
        if (strcmp(argv[i], "-dump") == 0) {
            dump = true;
        }
        if (strcmp(argv[i], "-showclocks") == 0) {
            showclocks = true;
        }
    }
    if (execution){
        printf("--- test: %s\n",argv[argc-1]);
    }
    int i = 0;
    size_t size;
    uint8_t* data = read_file(argv[argc-1], &size);
    for(i = 0;i<size;i++){
        print_binary(data[i]);
    }
    printf("bits 16\n");
    disasm(data, size);
    free(data);
    if (execution){
        print_registers();
    }
    if (dump){
        dump_memory();
    }
    return EXIT_SUCCESS;
}