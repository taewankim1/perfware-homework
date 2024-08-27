/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */

#include "sim86.h"
#include "sim86_memory.h"
// #include "sim86_text.h"
#include "sim86_decode.h"
#include <iostream>

// #include "sim86_memory.cpp"
// #include "sim86_text.cpp"
// #include "sim86_decode.cpp"

static void DisAsm8086(memory *Memory, u32 DisAsmByteCount, segmented_access DisAsmStart)
{
    segmented_access At = DisAsmStart;
    
    disasm_context Context = DefaultDisAsmContext();
    
    u32 Count = DisAsmByteCount;
    printf(";Count %d\n",Count);
    while(Count)
    {
        instruction Instruction = DecodeInstruction(&Context, Memory, &At);
        printf("Instruction size %d\n",Instruction.Size);
        if(Instruction.Op)
        {
            if(Count >= Instruction.Size)
            {
                Count -= Instruction.Size;
            }
            else
            {
                fprintf(stderr, "ERROR: Instruction extends outside disassembly region\n");
                break;
            }
            
            UpdateContext(&Context, Instruction);
            // if(IsPrintable(Instruction))
            // {
            //     PrintInstruction(Instruction, stdout);
            //     printf("\n");
            // }
        }
        else
        {
            fprintf(stderr, "ERROR: Unrecognized binary in instruction stream.\n");
            break;
        }
    }
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

int main(int ArgCount, char **Args)
{
    memory *Memory = (memory *)malloc(sizeof(memory));
    printf("Hi\n");
    if(ArgCount > 1)
    {
        for(int ArgIndex = 1; ArgIndex < ArgCount; ++ArgIndex)
        {
            char *FileName = Args[ArgIndex];
            u32 BytesRead = LoadMemoryFromFile(FileName, Memory, 0);
            printf(";BytesRead %d\n", BytesRead);
            for(int i=0;i<BytesRead;++i){
                print_binary(Memory->Bytes[i]);
            }
            
            printf("; %s disassembly:\n", FileName);
            printf("bits 16\n");
            DisAsm8086(Memory, BytesRead, {});
        }
    }
    else
    {
        fprintf(stderr, "USAGE: %s [8086 machine code file] ...\n", Args[0]);
    }
    
    return 0;
}