/ FUZIX for PDP-8/HD6120
/ Interrupt and Trap Vectors
/
/ This file would contain the initial jump points for interrupts
/ and software traps (system calls).
/ The PDP-8 has a single interrupt vector point, typically 0000 or 0001.
/ Let's assume 0001 for interrupts. System calls might use a specific
/ instruction like BPT (breakpoint trap) or a specific JMS.

*0001       / Interrupt vector location
    JMP INT_HANDLER / Jump to the common interrupt handler routine

/ Syscall entry point.
/ User programs will JMS to this location (or a per-bank stub pointing here)
/ after setting up syscall arguments.
/ This must be in Bank 7 (Kernel Bank) or accessible via a thunk.
*SYSCALL_ENTRY_POINT,  NOP / Placeholder address, e.g. 6000 in Bank 7 as per map
    JMP TRAP_HANDLER / Jump to the common trap/syscall handler

/ Bank Switch Thunks (Conceptual)
/ These would be small routines to change the active memory bank (field).
/ For HD6120, this involves writing to Instruction Field (IF) and Data Field (DF) registers.
/ Example: SET_DATA_FIELD_3
/     CDF 30 / Change Data Field to 3 (assuming 0-7 for banks)
/     JMP I @. / Return

/ More thunks as needed:
/ SET_INST_FIELD_7, SET_DATA_FIELD_USER, etc.

    END
