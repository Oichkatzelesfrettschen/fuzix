#ifndef _PDP8_HEAD_H
#define _PDP8_HEAD_H

/*
 * FUZIX for PDP-8/HD6120
 *
 * PDP-8 Specifics:
 * - 12-bit words
 * - 4K word banks (fields)
 * - Characters (8-bit) packed two per word.
 *   Left Byte (LB): bits 6-11 (mask 07700)
 *   Right Byte (RB): bits 0-5 (mask 00077)
 *   (Actual bit assignment for LB/RB might vary based on convention, TBD)
 */

/* Basic types - assuming CC8 or similar K&R compiler for PDP-8 */
/* 'int' is likely 12-bit by default in such a compiler */
/* 'char' is problematic. We'll operate on packed chars. */
typedef unsigned int uint16_t; /* For holding a 12-bit word */
typedef signed int int16_t;   /* For holding a 12-bit signed word */
typedef unsigned char uint8_t; /* For an 8-bit byte */
typedef signed char int8_t;   /* For an 8-bit byte */

/*
 * Placeholder: Actual bit positions for Left Byte (LB) and Right Byte (RB)
 * within a 12-bit word need to be definitively chosen.
 * Example:
 * LB: bits 6-11 (mask 07700, shift 6)
 * RB: bits 0-5  (mask 00077, shift 0)
 *
 * Or, if bytes are ordered MSB first in word:
 * LB (MSB): bits 4-11 of a 12-bit value if we consider it padded to 16-bits for extraction
 * RB (LSB): bits 0-3 of a 12-bit value
 *
 * For now, let's assume high bits for one char, low bits for another.
 * Standard is often high byte = first byte in memory for packed structures.
 */
#define MASK_LB 07700 /* Octal for bits 6-11 */
#define SHIFT_LB 6
#define MASK_RB 00077 /* Octal for bits 0-5 */
#define SHIFT_RB 0

/* Macros for byte packing/unpacking from a 12-bit word */
/* GET_LB: Gets the "left" (say, high) byte from a word */
#define GET_LB(word) ((uint8_t)(((word) & MASK_LB) >> SHIFT_LB))
/* GET_RB: Gets the "right" (say, low) byte from a word */
#define GET_RB(word) ((uint8_t)(((word) & MASK_RB) >> SHIFT_RB))

/* PUT_LB: Puts a byte into the "left" (high) part of a word */
/* Assumes 'word_var' is an lvalue. Preserves the right byte. */
#define PUT_LB(word_var, byte_val)     ((word_var) = ((word_var) & MASK_RB) | (((uint16_t)(byte_val) << SHIFT_LB) & MASK_LB))
/* PUT_RB: Puts a byte into the "right" (low) part of a word */
/* Assumes 'word_var' is an lvalue. Preserves the left byte. */
#define PUT_RB(word_var, byte_val)     ((word_var) = ((word_var) & MASK_LB) | (((uint16_t)(byte_val) << SHIFT_RB) & MASK_RB))


/* Memory Mapping (Conceptual) */
#define KERNEL_BANK 7
#define USER_BANK_START 0
#define USER_BANK_END 6

/* Address within a bank (12-bit) */
#define PAGE_SIZE       010000  /* 4K words */
#define ADDR_MASK       007777  /* Mask for 12-bit address within a bank */

/* Zero Page (addresses 0-0177 in any bank) */
#define ZERO_PAGE_TOP   000177

/* Kernel specific layout in Bank 7 (octal addresses) */
#define KERN_INTERRUPT_TABLE_BASE 06000
#define KERN_BANK_SWITCH_THUNKS   06100 /* Approx. after vectors */
#define KERN_TEXT_BASE            06200 /* Kernel code */
#define KERN_DATA_BASE            07000 /* Kernel .data/.bss */
#define KERN_ZP_WORK_AREA         07600 /* Kernel zero-page (in bank 7) */

/* User process image layout (within a 4K bank, conceptual sizes) */
#define USER_TEXT_SIZE  04000 /* 2K words for text */
#define USER_DATA_SIZE  02000 /* 1K words for data */
#define USER_STACK_SIZE 01000 /* 500 words for stack (approx 0760 octal) */
#define USER_HEAP_SIZE  01000 /* 500 words for heap (approx 0760 octal) */


/*
 * PDP8_ADDR(offset, bank)
 * Macro to form an address or manage bank switching.
 * Its actual implementation is highly dependent on how bank registers
 * are exposed or managed (e.g., via specific IOT instructions or memory-mapped regs
 * accessible from C, or if it needs to call an assembly thunk).
 * For now, a placeholder.
 */
// extern void set_active_bank(uint8_t bank_num);
// extern uint16_t read_word_from_bank(uint16_t offset, uint8_t bank_num);
// extern void write_word_to_bank(uint16_t offset, uint8_t bank_num, uint16_t value);


/* Placeholder for system call numbers */
#define SYS_exit    0
#define SYS_fork    1
#define SYS_read    2
#define SYS_write   3
#define SYS_open    4
#define SYS_close   5
#define SYS_brk     6
// ... more syscalls

#endif /* _PDP8_HEAD_H */
