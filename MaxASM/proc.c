//
//  proc.c
//  MaxASM
//
//  Created by Max Pastushkov on 12/5/17.
//  Copyright © 2017 Max Pastushkov. All rights reserved.
//

#include "proc.h"

/* Registers */
int R[8];
int PC;

/* Flags */
int C, Z;

int reg_init() {
    int i;
    for (i=0; i<8; i++)
        R[i] = 0;
    PC = C = Z = 0;
    
    return 0;
}

int proc_mov(cmd c) {
    
    if (c.type2 == TYPE_REG) {
        /* Moving register into register */
        R[c.op1] = R[c.op2];
        printf("mov r%i, r%i; %i (PC: %i)\n", c.op1, c.op2, R[c.op1], PC);
    } else if (c.type2 == TYPE_NUM) {
        /* Moving number into register */
        R[c.op1] = c.op2;
        printf("mov r%i, %i\n", c.op1, c.op2);
    }
    
    return 0;
}

int proc_add(cmd c) {
    
    if (c.type2 == TYPE_REG) {
        /* Adding register to register */
        R[c.op1] = R[c.op1] + R[c.op2];
        printf("add r%i, r%i; %i\n", c.op1, c.op2, R[c.op1]);
    } else if (c.type2 == TYPE_NUM) {
        /* Adding number to register */
        R[c.op1] = R[c.op1] + c.op2;
        printf("add r%i, %i; %i\n", c.op1, c.op2, R[c.op1]);
    }
    
    return 0;
}

int proc_sub(cmd c) {
    
    if (c.type2 == TYPE_REG) {
        /* Subbing register from register */
        R[c.op1] = R[c.op1] - R[c.op2];
        printf("sub r%i, r%i; %i\n", c.op1, c.op2, R[c.op1]);
    } else if (c.type2 == TYPE_NUM) {
        /* Subbing number from register */
        R[c.op1] = R[c.op1] - c.op2;
        printf("sub r%i, %i; %i\n", c.op1, c.op2, R[c.op1]);
    }
    
    return 0;
}

int proc_cmp(cmd c) {
    
    if (c.type2 == TYPE_REG) {
        /* Comparing register and register */
        Z = R[c.op1] == R[c.op2] ? 1 : 0;
        C = R[c.op1] >= R[c.op2] ? 0 : 1;
        printf("cmp r%i, r%i; Z: %i, C: %i\n", c.op1, c.op2, Z, C);
    } else if (c.type2 == TYPE_NUM) {
        /* Comparing number and register */
        Z = R[c.op1] == c.op2 ? 1 : 0;
        C = R[c.op1] >= c.op2 ? 0 : 1;
        printf("cmp r%i, %i; Z: %i, C: %i\n", c.op1, c.op2, Z, C);
    }
    
    return 0;
}

int proc_bne(cmd c) {
    
    if (!Z) {
        /* If Z flag is reset */
        PC = c.op1;
        printf("BNE %i\n", c.op1);
        proc();
    }
    
    return 0;
}

int proc_beq(cmd c) {
    
    if (Z) {
        /* If Z flag is set */
        PC = c.op1;
        printf("BEQ %i\n", c.op1);
        proc();
    }
    
    return 0;
}

int proc_blt(cmd c) {
    
    if (C) {
        /* If C flag is set */
        PC = c.op1;
        printf("BLT %i\n", c.op1);
        proc();
    }
    
    return 0;
}

int proc_bge(cmd c) {
    
    if (!C) {
        /* If C flag is reset */
        PC = c.op1;
        printf("BGE %i\n", c.op1);
        proc();
    }
    
    return 0;
}

int proc_jmp(cmd c) {
    
        PC = c.op1;
        printf("JMP %i\n", c.op1);
        proc();
    
    return 0;
}

int proc_cmd() {
    
    cmd c = commands[PC];
    
    switch (c.cmd) {
        case MOV:
            proc_mov(c);
            break;
        case ADD:
            proc_add(c);
            break;
        case SUB:
            proc_sub(c);
            break;
        case CMP:
            proc_cmp(c);
            break;
        case BNE:
            proc_bne(c);
            break;
        case BEQ:
            proc_beq(c);
            break;
        case BLT:
            proc_blt(c);
            break;
        case BGE:
            proc_bge(c);
            break;
        case JMP:
            proc_jmp(c);
            break;
    }
    
    return 0;
}

/* Processing */
int proc() {
    
    for ( ; PC<MAXCMD; PC++) {
        if (commands[PC].cmd == 0)
            break;
        proc_cmd();
    }
    
    return 0;
}
