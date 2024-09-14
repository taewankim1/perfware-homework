#pragma once

#define NUM_REGISTERS 8
bool execution = false;
uint8_t flags = 0; // 0,0,0,0,0,0,sign,zero

uint16_t registers[NUM_REGISTERS];
uint8_t memory[1024*1024];
int ip = 0; // instruction pointer
const char* reg_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
const char* reg_word[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
const char* address[] = { "bx + si", "bx + di", "bp + si", "bp + di",
        "si", "di", "bp", "bx" };

const char* get_REG(uint8_t W, uint8_t REG){
    if (W == 0){
        return reg_byte[REG];
    }
    else{
        return reg_word[REG];
    }
}


const char* get_effective_address(uint8_t R_M){
    return address[R_M];
}

uint32_t get_effective_address_value(uint8_t R_M){
    if (R_M == 0){
        return registers[3] + registers[6];
    }
    else if (R_M == 1){
        return registers[3] + registers[7];
    }
    else if (R_M == 2){
        return registers[5] + registers[6];
    }
    else if (R_M == 3){
        return registers[5] + registers[7];
    }
    else if (R_M == 4){
        return registers[6];
    }
    else if (R_M == 5){
        return registers[7];
    }
    else if (R_M == 6){
        return registers[5];
    }
    else if (R_M == 7){
        return registers[3];
    }
    return 0;
}