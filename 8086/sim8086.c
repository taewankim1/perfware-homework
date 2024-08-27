#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "sim8086.h"

#define NUM_REGISTERS 8
bool execution = false;
uint16_t registers[NUM_REGISTERS];
const char* reg_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
const char* reg_word[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };

void print_registers(void) {
    printf("\n");
    printf("Final registers:\n");
    printf("        %s: 0x%04X (%d)\n", reg_word[0], registers[0], registers[0]);
    printf("        %s: 0x%04X (%d)\n", reg_word[3], registers[3], registers[3]);
    printf("        %s: 0x%04X (%d)\n", reg_word[1], registers[1], registers[1]);
    printf("        %s: 0x%04X (%d)\n", reg_word[2], registers[2], registers[2]);
    for (int i=4;i<NUM_REGISTERS;i++){
        printf("        %s: 0x%04X (%d)\n", reg_word[i], registers[i], registers[i]);
    }
}

void exec_imm_tofrom_reg(uint8_t REG, uint16_t val){
    printf("; %s:0x%04X->0x%04X\n",reg_word[REG],registers[REG],val);
    registers[REG] = val;
}

void exec_reg_to_reg(uint8_t dest, uint8_t src){
    printf("; %s:0x%04X->0x%04X\n",reg_word[dest],registers[dest],registers[src]);
    registers[dest] = registers[src];
}


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

const char* get_REG(uint8_t W, uint8_t REG){
    if (W == 0){
        return reg_byte[REG];
    }
    else{
        return reg_word[REG];
    }
}

const char* get_effective_address(uint8_t R_M){
    const char* address[] = { "bx + si", "bx + di", "bp + si", "bp + di",
        "si", "di", "bp", "bx" };
    return address[R_M];
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

char get_sign_displacement(int test){
    if (test) return '+';
    else return '-';
}

const int16_t get_displacement(uint8_t* data, uint8_t MOD, int* iptr){
    int16_t displacement = 0;
    if (MOD == 1){
        displacement += (int8_t) data[(*iptr)++];
    }
    else if (MOD == 2){
        displacement += data[(*iptr)++];
        displacement += (data[(*iptr)++] << 8);
    }
    return displacement;
}

void print_reg_to_reg(char* operation,uint8_t D,uint8_t W,uint8_t R_M,uint8_t REG){
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
    if (execution) exec_reg_to_reg(dest,src);
    else printf("\n");
}
void print_reg_memory_to_from_reg(char* operation,int16_t displacement,uint8_t D,uint8_t W,uint8_t R_M,uint8_t REG){
    if (D == 1){
        printf("%s %s, [%s %c %d]\n",
            operation,
            get_REG(W,REG),
            get_effective_address(R_M),
            get_sign_displacement(displacement >= 0),
            abs(displacement)
            );
    }
    else if(D == 0){
        printf("%s [%s %c %d], %s\n",
            operation,
            get_effective_address(R_M),
            get_sign_displacement(displacement >= 0),
            abs(displacement),
            get_REG(W,REG));
    }
    else{
        fprintf(stderr, "D = %d not implemented!\n",D);
        exit(EXIT_FAILURE);
    }
}

void disasm_regmem_tofrom_either(uint8_t Left,char* operation,uint8_t* data,int* iptr){
    uint8_t Right = data[(*iptr)++];
    uint8_t W = Left & 1;
    uint8_t D = (Left >> 1) & 1;
    uint8_t R_M = Right & 7;
    uint8_t REG = (Right >> 3) & 7;
    uint8_t MOD = (Right >> 6) & 3;
    if (MOD == 3){
        print_reg_to_reg(operation,D,W,R_M,REG);
    }
    else if (MOD == 0 || MOD == 1 || MOD == 2){
        int16_t displacement = get_displacement(data, MOD, iptr);
        print_reg_memory_to_from_reg(operation,displacement,D,W,R_M,REG);
    }
    else{
        fprintf(stderr, "MOD = %d not implemented!\n",MOD);
        exit(EXIT_FAILURE);
    }
}
void disasm_imm_tofrom_reg(uint8_t Left,char* operation,uint8_t* data,int* iptr){
    uint8_t W = (Left >> 3) & 1;
    uint8_t REG = Left & 7;
    uint16_t val = data[(*iptr)++];
    if (W  == 1){
        val = val + (data[(*iptr)++] << 8);
    }
    printf("%s %s, %d",
        operation,
        get_REG(W,REG), val);
    if (execution) exec_imm_tofrom_reg(REG, val);
    else printf("\n");
}
void disasm_imm_tofrom_regmem(uint8_t Left,char* operation,uint8_t* data,int* iptr){
    uint8_t W = Left & 1;
    uint8_t Right = data[(*iptr)++];
    uint8_t MOD = (Right >> 6) & 3;
    uint8_t R_M = Right & 7;
    int16_t displacement = get_displacement(data, MOD, iptr);
    uint16_t val = data[(*iptr)++];
    char explicit_size[5];
    if (W == 0){
        strcpy(explicit_size,"byte");
    }
    else if (W == 1){
        val = val + (data[(*iptr)++] << 8);
        strcpy(explicit_size,"word");
    }
    printf("%s [%s %c %d], %s %d \n",
        operation,
        get_effective_address(R_M),
        get_sign_displacement(displacement >= 0),
        abs(displacement),
        explicit_size,
        val
        );
}

void disasm_arithmetic_imm_tofrom_regmem(uint8_t Left,uint8_t* data,int* iptr){
    uint8_t SW = Left & 3;
    uint8_t W = Left & 1;
    uint8_t Right = data[(*iptr)++];
    uint8_t OPCODE = (Right >> 3) & 7;
    char operation[4];
    get_arithmetic(OPCODE, operation);
    uint8_t MOD = (Right >> 6) & 3;
    uint8_t R_M = Right & 7;
    if (MOD == 3){
        uint16_t val = data[(*iptr)++];
        if (SW  == 1){
            val = val + (data[(*iptr)++] << 8);
        }
        printf("%s %s, %d; MOD: %d\n",
            operation,
            get_REG(W,R_M), val, MOD);
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
            int16_t displacement = get_displacement(data, 2, iptr);
            uint16_t val = data[(*iptr)++];
            if (SW == 1){
                val = val + (data[(*iptr)++] << 8);
            }
            printf("%s %s [%c %d], %d; R_M: %d, MOD: %d \n",
                operation,
                explicit_size,
                // get_effective_address(R_M),
                get_sign_displacement(displacement >= 0),
                abs(displacement),
                val,
                R_M,
                MOD
                );
        }
        else{
            int16_t displacement = get_displacement(data, MOD, iptr);
            uint16_t val = data[(*iptr)++];
            if (SW == 1){
                val = val + (data[(*iptr)++] << 8);
            }
            printf("%s %s [%s %c %d], %d; R_M: %d, MOD: %d \n",
                operation,
                explicit_size,
                get_effective_address(R_M),
                get_sign_displacement(displacement >= 0),
                abs(displacement),
                val,
                R_M,
                MOD
                );
        }
    }
}

void disasm_arithmetic_imm_to_acc(uint8_t Left,uint8_t* data,int* iptr){
    uint8_t OPCODE = (Left >> 3) & 7;
    char operation[4];
    get_arithmetic(OPCODE, operation);
    uint8_t W = Left & 1;
    int16_t val = data[(*iptr)++];
    char ax_al[3];
    if (W == 0){
        strcpy(ax_al,"al");
        val = (int8_t) val;
    }
    else if (W == 1){
        val = val + (data[(*iptr)++] << 8);
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

void disasm_jump(uint8_t Left, uint8_t* data, int* iptr){
    // char destination[12];
    char jump_type[5];
    get_jump_type(Left,jump_type);
    printf("%s %d\n",
        jump_type,(int8_t) data[(*iptr)++]
        );
}

void disasm_loop(uint8_t Left, uint8_t* data, int* iptr){
    char loop_type[7];
    if ((Left & 15) == 0b0010) strcpy(loop_type,"loop");
    else if ((Left & 15) == 0b0001) strcpy(loop_type,"loopz");
    else if ((Left & 15) == 0b0000) strcpy(loop_type,"loopnz");
    else if ((Left & 15) == 0b0011) strcpy(loop_type,"jcxz");
    printf("%s %d\n",
        loop_type,(int8_t) data[(*iptr)++]
        );
}

void disasm(uint8_t* data, size_t size){
    // size_t len_instruction = size / 2;
    uint8_t D;
    uint8_t W;
    uint8_t MOD;
    uint8_t REG;
    uint8_t R_M;
    const char* source;
    const char* destination;
    int i = 0;
    while (i < size){
        uint8_t Left = data[i++];
        if (Left >> 2 == 0b100010){
            char operation[] = "mov";
            disasm_regmem_tofrom_either(Left,operation,data,&i);
        }
        else if((Left >> 4) == 0b1011){
            char operation[] = "mov";
            disasm_imm_tofrom_reg(Left,operation,data,&i);
        }
        else if(Left >> 1 == 0b1100011){
            char operation[] = "mov";
            disasm_imm_tofrom_regmem(Left,operation,data,&i);
        }
        else if(Left >> 6 == 0b00 && ((Left >> 2) & 1) == 0){
            uint8_t OPCODE = (Left >> 3) & 7;
            char operation[4];
            get_arithmetic(OPCODE, operation);
            disasm_regmem_tofrom_either(Left,operation,data,&i);
        }
        else if(Left >> 6 == 0b00 && ((Left >> 2) & 1) == 1){
            disasm_arithmetic_imm_to_acc(Left,data,&i);
        }
        else if(Left >> 2 == 0b100000){
            disasm_arithmetic_imm_tofrom_regmem(Left,data,&i);
        }
        else if(Left >> 4 == 0b0111){
            disasm_jump(Left,data,&i);
        }
        else if(Left >> 4 == 0b1110){
            disasm_loop(Left,data,&i);
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
    if (argc == 2){
        int i = 0;
        size_t size;
        uint8_t* data = read_file(argv[1], &size);
        for(i = 0;i<size;i++){
            print_binary(data[i]);
        }
        printf("bits 16\n");
        disasm(data, size);
        free(data);
        return EXIT_SUCCESS;
    }
    if (argc == 3 && !strcmp(argv[1],"-exec")){
        execution = true;
        printf("--- test: %s\n",argv[2]);
        int i = 0;
        size_t size;
        uint8_t* data = read_file(argv[2], &size);
        for(i = 0;i<size;i++){
            print_binary(data[i]);
        }
        printf("bits 16\n");
        disasm(data, size);
        free(data);
        print_registers();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}