/ FUZIX for PDP-8/HD6120
/ Low-level core routines: Reset, Boot, Trap Handling, Context Switch

/ External symbols (from C or other assembly)
/ EXTERN C_TRAP_DISPATCHER  / C function to handle trap details
/ EXTERN C_INTERRUPT_DISPATCHER / C function for interrupt details
/ EXTERN CURRENT_PCB_PTR    / Pointer to current Process Control Block
/ EXTERN NEXT_PCB_PTR       / Pointer to next PCB for context switch

/ Fixed save area for registers during interrupt in Bank 7
/ This needs to be carefully defined, e.g. in current task's kernel stack or PCB.
INT_SAVE_AC,    0   / Saved Accumulator
INT_SAVE_L,     0   / Saved Link bit
INT_SAVE_PC,    0   / Saved Program Counter (return address)
INT_SAVE_IF,    0   / Saved Instruction Field (Bank)
INT_SAVE_DF,    0   / Saved Data Field (Bank)


/ Reset and Boot Sequence
/ Typically starts at a fixed address when machine powers on or resets.
/ For FUZIX, this might be a secondary bootloader loaded by a simpler monitor.
/ The roadmap says: "copy monitor loader from CF into bank 7, zap PC=6200"
/ This implies this code here IS that loader, or is called by it.
*BOOT_ENTRY, NOP
    / 1. Basic hardware init (if any needed beyond monitor's setup)
    / 2. Setup initial Data Field (DF) and Instruction Field (IF) to Bank 7
    CIF 70          / Change instruction field to 7 (kernel bank)
    CDF 70          / Change data field to 7 (kernel bank)

    / 3. (Optional) Clear kernel BSS area if not done by C startup.

    / 4. Setup initial software stack pointer for kernel if needed.
    /    DCA KERNEL_STACK_POINTER

    / 5. Jump to kernel C main() or equivalent startup.
    JMP C_KERNEL_MAIN   / Jump to the main C kernel entry point (e.g. at 06200)


/ Common Interrupt Handler
INT_HANDLER, NOP
    / 1. Save CPU context (AC, L, PC, IF, DF)
    /    IMPORTANT: Must save IF/DF *before* changing them to kernel bank if not already there.
    /    This sequence is tricky due to limited registers.
    /    Assume we are already in the process's IF/DF or a known state.
    /    The HD6120 interrupt sequence might automatically switch to bank 0 or a specific bank.
    /    This needs to be clarified based on HD6120 datasheet section on interrupts.

    / Simplistic save sequence (actual PDP-8 code is more complex):
    / (Store AC, L, PC to fixed area in Bank 7 or process's kernel stack)
    TAD (INT_SAVE_AC) / Example, not real code
    DCA INT_SAVE_AC   / Store AC

    / Read current IF/DF (e.g. using RIF, RDF instructions if they exist and are safe here)
    / Store them

    / 2. Switch to Kernel Bank (Bank 7 for IF and DF)
    CIF 70
    CDF 70

    / 3. Call high-level C interrupt dispatcher
    JMS C_INTERRUPT_DISPATCHER

    / 4. Restore CPU context (IF, DF, PC, L, AC) from save area
    /    Order is reverse of save. DF/IF must be restored last before JMP I.
    /    Ensure atomicity if interrupts can nest (usually disabled here).

    / 5. Return from interrupt (e.g., JMP I @SAVED_PC_LOCATION)
    JMP I (INT_SAVE_PC) / Indirect jump to the saved PC


/ Common Trap Handler (for System Calls)
TRAP_HANDLER, NOP
    / Entered from SYSCALL_ENTRY_POINT or similar.
    / Assumes syscall number is in AC, or other agreed-upon location.
    / Assumes already in Kernel Bank (Bank 7 IF/DF) due to syscall stub.

    / 1. Save CPU context (similar to INT_HANDLER, but might be slightly different)
    /    e.g. save user's AC, L, PC. User's IF/DF should be part of PCB.
    /    Store AC (contains syscall number) into PCB or temp location.

    / 2. Call high-level C trap/syscall dispatcher
    /    Pass syscall number and pointer to saved registers/PCB.
    JMS C_TRAP_DISPATCHER

    / 3. Restore CPU context for the user process (from PCB)
    /    This includes user's AC, L, PC, IF, DF.

    / 4. Return to user mode (JMP I @USER_SAVED_PC, ensuring IF/DF are set for user)
    JMP I (USER_SAVED_PC_IN_PCB)


/ Context Switch Routine
/ SAVE_CONTEXT:
/   Called when current task is to be suspended.
/   Saves essential registers (AC, L, PC, IF, DF, Stack Pointer) into CURRENT_PCB_PTR.
SAVE_CONTEXT, NOP
    / Get CURRENT_PCB_PTR
    / Store AC into PCB.AC_OFFSET(CURRENT_PCB_PTR)
    / Store L into PCB.L_OFFSET(CURRENT_PCB_PTR)
    / Store PC (return address from this JMS) into PCB.PC_OFFSET(CURRENT_PCB_PTR)
    / Store current IF, DF (bank registers) into PCB
    / Store software stack pointer into PCB
    JMP I @. / Return

/ RESTORE_CONTEXT:
/   Called when new task (NEXT_PCB_PTR) is to be resumed.
/   Loads registers from NEXT_PCB_PTR.
RESTORE_CONTEXT, NOP
    / Get NEXT_PCB_PTR
    / Load IF, DF from PCB into hardware bank registers
    / Load software stack pointer from PCB
    / Load L from PCB
    / Load AC from PCB
    / JMP I PCB.PC_OFFSET(NEXT_PCB_PTR) / Jump to new task's saved PC

    END
