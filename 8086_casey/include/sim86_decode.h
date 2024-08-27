/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */
#ifndef SIM86_DECODE_H
#define SIM86_DECODE_H

#include "sim86.h"
#include "sim86_decode.h"

enum instruction_bits_usage : u8
{
    Bits_Literal,
    Bits_MOD,
    Bits_REG,
    Bits_RM,
    Bits_SR,
    Bits_Disp,
    Bits_Data,

    Bits_HasDisp,
    Bits_DispAlwaysW,
    Bits_HasData,
    Bits_WMakesDataW,
    Bits_RMRegAlwaysW,
    Bits_RelJMPDisp,
    Bits_D,
    Bits_S,
    Bits_W,
    Bits_V,
    Bits_Z,
    
    Bits_Count,
};

struct instruction_bits
{
    instruction_bits_usage Usage;
    u8 BitCount;
    u8 Shift;
    u8 Value;
};

struct instruction_format
{
    operation_type Op;
    instruction_bits Bits[16];
};

struct disasm_context
{
    register_index DefaultSegment;
    u32 AdditionalFlags;
};

disasm_context DefaultDisAsmContext(void);
instruction DecodeInstruction(disasm_context *Context, memory *Memory, segmented_access *At);
void UpdateContext(disasm_context *Context, instruction Instruction);

#endif SIM86_DECODE_H