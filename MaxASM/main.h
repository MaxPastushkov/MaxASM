//
//  main.h
//  MaxASM
//
//  Created by Max Pastushkov on 11/27/17.
//  Copyright © 2017 Max Pastushkov. All rights reserved.
//

#ifndef main_h
#define main_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "proc.h"

#define MAXCMD 10

// Command set

#define MOV 1
#define ADD 2
#define SUB 3
#define JMP 4
#define CMP 5
#define BNE 6
#define BEQ 7
#define BLT 8
#define BGE 9

// Operand types

#define TYPE_REG 1
#define TYPE_NUM 2
#define TYPE_LBL 3

typedef struct {
    int cmd;
    int op1, op2;
    int type1, type2;
} cmd;

typedef struct {
    int idx;
    char label[10];
} lbl;

int parse_lbl(char *s);
int parse_cmd(char *s);
int parse2(int c, char *t2, char *t3);
int parse1(int c, char *t2);
int parse0(int c, char *t1);
int comp_error(char *s);

extern cmd commands[];

#endif /* main_h */
