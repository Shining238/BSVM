start:
    ld R0, 6
    ld R1, 7
    add R0, R1
    st R0, 200
    ld R1, 200
    push R1
    cmp R0, 13
    jgt label1
    nop
    halt

label1:
    ld R0, [R1]
    add R0, 1
    HALT