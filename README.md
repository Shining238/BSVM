# BSVM

Pour compiler : ./build.sh

Pour assembler un .asm en .bsin : ./build/bsm file.asm out.bsin

Pour executer un .bsin sur la VM : ./build/bsvm file.bsin


Pour commenter l'assembleur : "//"

---- Documentation de l'assembleur ----

Syscalls :

    ------------------------Files------------------------
    id 0: write(size_t size, uint64_t addr, int fd)
    id 2: read(size_t size, uint64_t addr, int fd)
    id 3: open(char *filename)
    id 4: close(int fd)

    -------------------------I/O-------------------------
    id 5: print_int(int64_t value)
    id 6: print_str(char *addr)
    id 7: read_int(void)
    id 8: read_str(size_t size, uint64_t addr) //memory pointed to by addr must be allocated

    id 9: exit(uint8_t exit_code)

    ------------------------Memory------------------------
    id 10: malloc(size_t size)
    id 11: free(uint64_t addr)
    id 12: memset(size_t size, uint64_t addr, uin8_t value)
    id 13: memcpy(size_t size, uint64_t src, uint64_t dest)

    ------------------------Dumps------------------------
    id 14: dump_regs(void)
    id 15: dump_stack(void)
    id 16: dump_memory(size_t size, uint64_t addr)


Directives assembleur :

    .entry addr (set program entry point to addr)

    .string "string" (writes string to current address in data segment)

    .word x (write x (8 bytes) to current address in data segment)

    .byte x (write x (1 byte) to curretn address in data segment)

    .addr addr (writes addr to current address in data segment)

    .text (declares a text section)

    .data (declare a data section)


Instructions :

    ADD reg imm/reg/dir/ind/idx  -- ADD R0, x : R0 = R0 + x (imm)

    SUB reg imm/reg/dir/ind/idx  -- SUB R0, R1 : R0 = R0 - R1 (reg)
    
    MUL reg imm/reg/dir/ind/idx  -- MUL R0, [1000] : R0 = R0 * MEM[1000] (dir)
    
    DIV reg imm/reg/dir/ind/idx  -- DIV R0, [R1] : R0 = R0 * MEM[R1] (ind)
    
    CMP reg imm/reg/dir/ind/idx  -- CMP R0, [R1+12] : set SR flags for R0 - MEM[R1+12] (idx)
    
    NEG reg                      -- NEG R0 : R0 = -R0

    AND reg imm/reg/dir/ind/idx  -- AND R0, R1 : R0 = R0 & R1

    OR reg imm/reg/dir/ind/idx   -- OR R0, R1 : R0 = R0 | R1

    XOR reg imm/reg/dir/ind/idx  -- XOR R0, R1 : R0 = R0 ^ R1

    NOT reg                      -- NOT R0 : R0 = ~R0

    SHL reg imm/reg/dir/ind/idx  -- SHL R0 x : R0 = R0 << x

    SHR reg imm/reg/dir/ind/idx  -- SHR R0 x : R0 = R0 >> x

    ST reg dir/ind/idx           -- ST R0, [R1] : MEM[R1] = R0

    LD reg imm/reg/dir/ind/idx   -- LD R0, R1 : R0 = R1

    SWP reg imm/reg/dir/ind/idx  -- SWP R0, [12] : R0 = MEM[12] et MEM[12] = R0

    PUSH imm/reg                 -- PUSH x : MEM[SP] = x et SP = SP - 8

    POP reg                      -- POP R0 : R0 = MEM[SP] et SP = SP + 8

    FAS reg dir/ind/idx          -- FAS R0, [12] : R0 = MEM[12] et MEM[12] = 1

    CALL imm/reg/dir/ind/idx     -- CALL x : PUSH PC et PC = x

    RTN                          -- RTN : POP PC

    NOP                          -- NOP : no operation

    RST                          -- RST : resets the vm

    HALT                         -- HALT : stop the vm

    TRAP                         -- TRAP : syscall (see below for examples)

    JMP imm/reg/dir/ind/idx      -- JMP x : PC = x
    
    ---- After a CMP instruction ----

    JEQ imm/reg/dir/ind/idx      -- JEQ x : PC = x if equal

    JNE imm/reg/dir/ind/idx      -- JNE x : PC = x if not equal

    JLE imm/reg/dir/ind/idx      -- JLE x : PC = x if lesser or equal

    JLT imm/reg/dir/ind/idx      -- JLT x : PC = x if lesser than

    JGE imm/reg/dir/ind/idx      -- JGE x : PC = x if greater or equal

    JGT imm/reg/dir/ind/idx      -- JGT x : PC = x if greater than


------------------EXAMPLES------------------

.entry start

.text // text section

start:
    LD R0, 12
    LD R1, 5
    ADD R0, R1 // R0 = 12 + 5
    CALL label // -> R1 = 412
    MUL R0, R1 // R0 = 412 * 17

    PUSH R0
    LD R0, 5 // syscall for print_int
    TRAP

    PUSH newline
    LD R0, 6 //syscall for print_str
    TRAP

    PUSH 12 // size
    PUSH string // char *addr
    PUSH 1  // fd of stdout
    LD R0, 0 // 0 : syscall id for write
    TRAP    // call

    CMP R0, -1  // test the return value
    JEQ failure_exit
    JNE success_exit

label:
    LD R1, [value] // R1 = 412
    RTN

failure_exit:
    LD R0, 14 // syscall id for dump_regs
    TRAP
    PUSH 1 // exit code
    LD R0, 9  //syscall id for exit
    TRAP
    HALT

success_exit:
    PUSH 0
    LD R0, 9
    TRAP
    HALT

.data // data section

value:
    .word 412

string:
    .string "hello world\n"

newline:
    .string "\n"