//
//  main.c
//  MaxASM
//
//  Created by Max Pastushkov on 11/27/17.
//  Copyright © 2017 Max Pastushkov. All rights reserved.
//

#include "main.h"

char *file_src;
char *file_dst;
cmd commands[MAXCMD];
lbl labels[MAXCMD];
char cur[100];
FILE *fp;
FILE *fd;
int cur_cmd;
int cur_lbl;
int cur_line;

/* Options */
int compile_only = 0; // -c compile and store results in the .mx
int execute_only = 0; // -x execute the file
int debug = 0; // -d display debug info

void trim(char * s) {
    char * p = s;
    long l = strlen(p);
    
    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;
    
    memmove(s, p, l + 1);
}

int parse_opts(char *s) {
    if (s[0] != '-')
        return -1;
    if (strchr(s, 'c'))
        compile_only = 1;
    if (strchr(s, 'x'))
        execute_only = 1;
    if (strchr(s, 'd'))
        debug = 1;
    
    if (compile_only && execute_only) {
        printf("Conflicting options: -x and -c. Switching to compile-only mode\n");
        execute_only = 0;
    }
    
    return 0;
}

int write_file() {
    
    char *p;
    
    long l = strlen(file_src);
    file_dst = malloc(l+1);
    strcpy(file_dst, file_src);
    p = strchr(file_dst, '.');
    if (p) {
        p[1] = 'm';
        p[2] = 'x';
        p[3] = 0;
    }
   
    if (debug)
        printf("Writing output file: %s\n", file_dst);
    
    fd = fopen(file_dst, "w+");
    if (!fd) {
        perror("Output file error");
        return -1;
    }
    
    fwrite(commands, sizeof(cmd), cur_cmd, fd);
    fclose(fd);


    return 0;
}

int read_file() {
    
    cmd *c = commands;
    int i = 0;
    
    printf("Reading %s...\n", file_src);
    
    fp = fopen(file_src, "r");
    if (!fp) {
        perror("Cannot open file!");
        return -1;
    }
    
    while (!feof(fp)) {
        long b = fread(c, sizeof(cmd), 1, fp);
/*        if (b != sizeof(cmd)) {
            perror("Invalid file!");
            fclose(fp);
            return -1;
        }
*/        
        c++;
        i++;
        
        if (i > MAXCMD) {
            perror("File too long!");
            fclose(fp);
            return -1;
        }
    }
    
    fclose(fp);
    return 0;
}

int compile_file() {
    
    printf("Compiling %s...\n", file_src);
    
    fp = fopen(file_src, "r");
    if (!fp) {
        perror("Cannot open file!");
        return -1;
    }
    
    /* First pass */
    printf("First pass:\n");
    cur_cmd = cur_lbl = 0;
    cur_line = 1;
    
    while (!feof(fp)) {
        fscanf(fp, "%[^\n]\n", cur);
        trim(cur);
        parse_lbl(cur);
        cur_line++;
    }
    
    /* Second pass */
    printf("Second pass:\n");
    fseek(fp, 0, SEEK_SET);
    cur_cmd = cur_lbl = 0;
    cur_line = 1;
    
    while (!feof(fp)) {
        fscanf(fp, "%[^\n]\n", cur);
        trim(cur);
        parse_cmd(cur);
        cur_line++;
    }
    
    fclose(fp);

    return 0;
}

int main(int argc, char * argv[]) {
    
    if (argc < 2) {
        printf("Usage: MaxASM file.ma\n");
        return -1;
    }
    
    if (argc > 2) {
        parse_opts(argv[2]);
    }
    
    file_src = argv[1];
    
    if (compile_only) {
        compile_file();
        write_file();
    } else if (execute_only) {
        read_file();
        proc();
    } else {
        compile_file();
        proc();
    }
    
    return 0;
}

int parse_lbl(char *s) {
    if (s[0] == ';')
        return 0;
    char *p = strchr(s, ':');
    if (p) {
        /* Label here */
        *p = 0;
        labels[cur_lbl].idx = cur_cmd;
        strncpy(labels[cur_lbl].label, s, sizeof(labels[cur_lbl].label));
        printf("Label: %i, Name: %s\n", labels[cur_lbl].idx, labels[cur_lbl].label);
        cur_lbl++;

    } else {
        /* Command here */
        cur_cmd++;
    }
    return 0;
}

int parse_cmd(char *s) {
    char *t1, *t2, *t3;
    if (s[0] == ';')
        return 0;
    t1 = strtok(s, " ");
    t2 = strtok(NULL, " ");
    t3 = strtok(NULL, " ");
    //printf("1: %s, 2: %s, 3: %s\n", t1, t2, t3);
    if (t1 == NULL)
        return -1;
    else if (!strcmp(t1, "mov"))
        return parse2(MOV, t2, t3);
    else if (!strcmp(t1, "add"))
        return parse2(ADD, t2, t3);
    else if (!strcmp(t1, "sub"))
        return parse2(SUB, t2, t3);
    else if (!strcmp(t1, "cmp"))
        return parse2(CMP, t2, t3);
    else if (!strcmp(t1, "jmp"))
        return parse1(JMP, t2);
    else if (!strcmp(t1, "bne"))
        return parse1(BNE, t2);
    else if (!strcmp(t1, "beq"))
        return parse1(BEQ, t2);
    else if (!strcmp(t1, "bge"))
        return parse1(BGE, t2);
    else if (!strcmp(t1, "blt"))
        return parse1(BLT, t2);
    
    return 0;
}

int parse0(int c, char *t1) {
    return 0;
}

int parse1(int c, char *t2) {

    int i, idx = -1;
    for (i=0; i<MAXCMD; i++) {
        if (!strcmp(t2, labels[i].label)) {
            idx = labels[i].idx;
            break;
        }
    }
    if (idx == -1) {
        comp_error("Invalid label");
        return -1;
    }
    
    commands[cur_cmd].cmd = c;
    commands[cur_cmd].op1 = idx;
    commands[cur_cmd].type1 = TYPE_LBL;
    
    printf("Command: %i, cmd: %i, op1: %i, type1: %i, op2: %i, type2: %i\n", cur_cmd,
           commands[cur_cmd].cmd, commands[cur_cmd].op1, commands[cur_cmd].type1,
           commands[cur_cmd].op2, commands[cur_cmd].type2);
    
    cur_cmd++;
 
    return 0;
}

int parse2(int c, char *t2, char *t3) {

    int reg_src = -1;
    int reg_dst = -1;
    int num_src = 0;
 
    if (t2 == NULL || t3 == NULL) {
        return comp_error("Invalid operands");
    }
        
    /* Processing destination */
    if (t2[0] == 'r') {
        if (strlen(t2) < 2)
            return comp_error("Incorrect destination register");
        t2++;
        reg_dst = atoi(t2);
        if (reg_dst > 8)
            return comp_error("Invalid destination register");
    } else
        /* Destination can only be a register */
        return comp_error("Destination must be register");
    
    /* Processing source */
    if (t3[0] == 'r') {
        if (strlen(t3) < 2)
            return comp_error("Incorrect source register");
        reg_src = atoi(++t3);
        if (reg_src > 8)
            return comp_error("Invalid source register");
    } else {
        num_src = atoi(t3);
    }
    
    /* Creating command */
    commands[cur_cmd].cmd = c;
    commands[cur_cmd].op1 = reg_dst;
    commands[cur_cmd].type1 = TYPE_REG;
    commands[cur_cmd].op2 = (reg_src == -1) ? num_src : reg_src;
    commands[cur_cmd].type2 = (reg_src == -1) ? TYPE_NUM : TYPE_REG;

    printf("Command: %i, cmd: %i, op1: %i, type1: %i, op2: %i, type2: %i\n", cur_cmd,
           commands[cur_cmd].cmd, commands[cur_cmd].op1, commands[cur_cmd].type1,
           commands[cur_cmd].op2, commands[cur_cmd].type2);

    cur_cmd++;
    return 0;
}

int comp_error(char *s) {
    printf("Error: %s (Line %i)\n", s, cur_line);
    return -1;
}
