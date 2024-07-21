#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

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
    const char* reg_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
	const char* reg_word[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
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

void disasm(uint8_t* data, size_t size){
    // size_t len_instruction = size / 2;
    uint8_t OPCODE;
    uint8_t D;
    uint8_t W;
    uint8_t MOD;
    uint8_t REG;
    uint8_t R_M;
    int i = 0;
    while (i < size){
        uint8_t Left = data[i++];
        OPCODE = (Left >> 4);
        if (OPCODE == 0b1000){
            if (Left >> 2 == 0b100010){
                uint8_t Right = data[i++];
                W = Left & 1;
                D = (Left >> 1) & 1;
                R_M = Right & 7;
                REG = (Right >> 3) & 7;
                MOD = (Right >> 6) & 3;
                if (MOD == 3){
                    if (D == 0){
                        printf("mov %s, %s; OPCODE: %u, D: %u, W: %u, MOD %u, REG %u, R_M %u \n",
                        get_REG(W,R_M), get_REG(W,REG),OPCODE, D, W, MOD, REG, R_M);

                    }
                    else{
                        printf("mov %s, %s; OPCODE: %u, D: %u, W: %u, MOD %u, REG %u, R_M %u \n",
                        get_REG(W,REG), get_REG(W,R_M),OPCODE, D, W, MOD, REG, R_M);
                    }
                }
                else{
                    uint16_t displacement = 0;
                    if (MOD == 1) {
                        displacement += data[i++];
                    }
                    else if (MOD == 2){
                        displacement += data[i++];
                        displacement += (data[i++] << 8);
                    }
                    if (D == 1)
                        if (MOD == 0){
                            printf("mov %s, [%s]; OPCODE: %u, D: %u, W: %u, MOD %u, REG %u, R_M %u \n",
                                get_REG(W,REG), get_effective_address(R_M),OPCODE, D, W, MOD, REG, R_M);
                        }
                        else{
                            printf("mov %s, [%s + %d]; OPCODE: %u, D: %u, W: %u, MOD %u, REG %u, R_M %u \n",
                                get_REG(W,REG), get_effective_address(R_M),displacement,OPCODE, D, W, MOD, REG, R_M);
                        }
                    else if (D == 0){
                        if (MOD == 0){
                            printf("mov [%s], %s; OPCODE: %u, D: %u, W: %u, MOD %u, REG %u, R_M %u \n",
                                get_effective_address(R_M),get_REG(W,REG),OPCODE, D, W, MOD, REG, R_M);
                        }
                        else{
                            printf("mov [%s + %d], %s; OPCODE: %u, D: %u, W: %u, MOD %u, REG %u, R_M %u \n",
                                get_effective_address(R_M),displacement,get_REG(W,REG),OPCODE, D, W, MOD, REG, R_M);
                        }
                    }
                }
            }
        }
        else if(OPCODE == 0b1011){
            W = (Left >> 3) & 1;
            REG = Left & 7;
            uint16_t val = data[i++];
            if (W  == 1){
                val = val + (data[i++] << 8);
            }
            printf("mov %s, %d; OPCODE: %u, W: %u, REG %u \n",
                get_REG(W,REG), val,OPCODE, W, REG);
        }
        else{
            fprintf(stderr, "OPCODE=%d not implemented!\n", OPCODE);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv){
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