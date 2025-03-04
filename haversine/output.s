	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 14, 0	sdk_version 14, 4
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	mov	w8, #0
	str	w8, [sp, #20]                   ; 4-byte Folded Spill
	stur	wzr, [x29, #-4]
	bl	__ZL14GetOSTimerFreqv
	stur	x0, [x29, #-16]
	ldur	x8, [x29, #-16]
	mov	x9, sp
	str	x8, [x9]
	adrp	x0, l_.str@PAGE
	add	x0, x0, l_.str@PAGEOFF
	bl	_printf
	bl	__ZL11ReadOSTimerv
	mov	x8, x0
	ldr	w0, [sp, #20]                   ; 4-byte Folded Reload
	str	x8, [sp, #24]
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZL14GetOSTimerFreqv
__ZL14GetOSTimerFreqv:                  ; @_ZL14GetOSTimerFreqv
	.cfi_startproc
; %bb.0:
	mov	x0, #16960
	movk	x0, #15, lsl #16
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZL11ReadOSTimerv
__ZL11ReadOSTimerv:                     ; @_ZL11ReadOSTimerv
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	add	x0, sp, #16
	mov	x1, #0
	bl	_gettimeofday
	bl	__ZL14GetOSTimerFreqv
	ldr	x8, [sp, #16]
	mul	x8, x0, x8
	ldrsw	x9, [sp, #24]
	add	x8, x8, x9
	str	x8, [sp, #8]
	ldr	x0, [sp, #8]
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #48
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"    OS Freq: %llu\n"

.subsections_via_symbols
