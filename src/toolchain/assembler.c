#include "common.h"
#include "assembler.h"
#include "instructions.h"
#include "instructions_codec.h"
#include "parser.h"
#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define CHK_ASM_STATUS(prog, status, line) do {\
    if (status != ASM_OK){\
        printAsmStatus(prog, status, line);\
        exit(EXIT_FAILURE);\
    }\
} while (0)

void printAsmStatus(char *prog, AsmError status, size_t line){
    printf("%s:%lu ", prog, line);
    switch (status){
        case ASM_OK: printf("asm ok\n"); break;
        case ASM_ENTRY_REDEFINED: printf("entry point redefined\n"); break;
        case ASM_LABEL_REDEFINED: printf("label already exists\n"); break;
        case ASM_LABEL_UNDEFINED: printf("label does not exist\n"); break;
        case ASM_SECTION_UNDEFINED: printf("section is undefined (directive .text or .data is missing)\n"); break;
        case ASM_TOO_FEW_ARGUMENTS: printf("arguments are missing\n"); break;
        case ASM_TOO_MANY_ARGUMENTS: printf("expected less arguments\n"); break;
        case ASM_WRONG_ARGUMENT_TYPE: printf("wrong argument type\n"); break;
        case ASM_INVALID_ADRESSING_MODE: printf("operation does not support this adressing mode\n"); break;
        case ASM_NOT_A_DATA_SECTION: printf("should be in a data section\n"); break;
        case ASM_NOT_A_TEXT_SECTION: printf("should be in a text section\n"); break;
        default: break;
    }
}


int isInSymbolTable(struct Symbol_table *symbol_table, const char *s){

    for (size_t i = 0; i < symbol_table->next; i++){
        if (!strcmp(s, symbol_table->table[i].string)){
            return 1;
        }
    }
    return 0;
}

void addToSymbolTable(struct Symbol_table *symbol_table, struct Symbol *s){
    struct Symbol *tmp;

    if (symbol_table->next == symbol_table->size){
        tmp = realloc(symbol_table->table, 2 * symbol_table->size);
        if (!tmp){
            free(symbol_table);
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        symbol_table->table = tmp;
        symbol_table->size *= 2;
    }

    symbol_table->table[symbol_table->next].addr = s->addr;
    symbol_table->table[symbol_table->next].string = s->string;
    symbol_table->next++;
}

int getSymbolAddr(struct Symbol_table *symbol_table, char *s, uint64_t *addr){
    for (size_t i = 0; i < symbol_table->next; i++){
        if (!strcmp(s, symbol_table->table[i].string)){
            *addr = symbol_table->table[i].addr;
            return 1;
        }
    }
    return 0;
}


MODE getModeFromOperands(const Operand *a, const Operand *b, int args_count, AsmError *status){
    if (args_count == 0){
        if ((a->type != OPERAND_NONE) || (b->type != OPERAND_NONE)){
            *status = ASM_TOO_MANY_ARGUMENTS;
            return MODE_NONE;
        }
        return MODE_NONE;
    }
    else if (args_count == 1){
        if (b->type != OPERAND_NONE){
            *status = ASM_TOO_MANY_ARGUMENTS;
            return MODE_NONE;
        }
        if (a->type == OPERAND_REG){
            *status = ASM_OK;
            return MODE_REGU;
        }
        b = a;
    }
    if ((args_count >= 1) && (a->type == OPERAND_NONE)){
        *status = ASM_TOO_FEW_ARGUMENTS;
        return MODE_NONE;
    }
    if (args_count == 2){
        if (b->type == OPERAND_NONE){
            *status = ASM_TOO_FEW_ARGUMENTS;
            return MODE_NONE;
        }
        if (a->type != OPERAND_REG){
            *status = ASM_WRONG_ARGUMENT_TYPE;
            return MODE_NONE;
        }
    }
    *status = ASM_OK;
    switch (b->type){
        case OPERAND_IDENT:
        case OPERAND_VALUE: return MODE_IMM;
        case OPERAND_REG: return MODE_REG;
        case OPERAND_MEM_IDENT:
        case OPERAND_MEM_IDX_IDENT:
        case OPERAND_MEM_VALUE: return MODE_DIR;
        case OPERAND_MEM_REG: return MODE_IND;
        case OPERAND_MEM_IDX: return MODE_IDX;
        default: *status = ASM_WRONG_ARGUMENT_TYPE; return MODE_NONE;
    }
    
    *status = ASM_WRONG_ARGUMENT_TYPE;
    return MODE_NONE;
}



AsmError assemblerFirstPass(char *prog, struct Assembler *ass, const struct IR_Node *IRList, size_t size){

    ass->symtable.size = 16;
    ass->symtable.next = 0;
    ass->symtable.table = malloc(ass->symtable.size * sizeof(struct Symbol));

    size_t text_addr = 0;
    size_t data_addr = DATA_BASE;
    Section section = SECTION_UNDEF;

    int entry_ok = 0;

    struct Symbol s = {0};

    int args_count = 0;

    AsmError status;

    MODE mode;

    for (size_t i = 0; i < size; i++){
        ass->error_line = IRList[i].line;
        if (IRList[i].type == IR_LABEL){
            if (!isInSymbolTable(&ass->symtable, IRList[i].label.name.ident.str)){
                if (section == SECTION_UNDEF){
                    return ASM_SECTION_UNDEFINED;
                }
                s.string = IRList[i].label.name.ident.str;
                s.addr = (section == SECTION_DATA) ? data_addr : text_addr;
                addToSymbolTable(&ass->symtable, &s);
            }
            else {
                return ASM_LABEL_REDEFINED;
            }
        }
        else if (IRList[i].type == IR_DIR){
            switch (IRList[i].directive.type){
                case DIR_BYTE: if (IRList[i].directive.value.type != OPERAND_VALUE) return ASM_WRONG_ARGUMENT_TYPE;
                        if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                        data_addr += 1;
                        break;
                case DIR_WORD:
                case DIR_ADDR:
                        if ((IRList[i].directive.value.type != OPERAND_VALUE) && (IRList[i].directive.value.type != OPERAND_IDENT)){
                            return ASM_WRONG_ARGUMENT_TYPE;
                        }
                        if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                        data_addr += 8;
                        break;
                case DIR_STRING: if (IRList[i].directive.string.type != OPERAND_STRING) return ASM_WRONG_ARGUMENT_TYPE;
                        data_addr += IRList[i].directive.string.ident.length;
                        if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                        break;
                case DIR_ENTRY: if (IRList[i].directive.string.type != OPERAND_IDENT) return ASM_WRONG_ARGUMENT_TYPE;
                        if (entry_ok) return ASM_ENTRY_REDEFINED;
                        entry_ok = 1;
                        break;
                case DIR_DATA: if (IRList[i].directive.string.type != OPERAND_NONE) return ASM_TOO_MANY_ARGUMENTS;
                        section = SECTION_DATA;
                        break;
                case DIR_TEXT: if (IRList[i].directive.string.type != OPERAND_NONE) return ASM_TOO_MANY_ARGUMENTS;
                        section = SECTION_TEXT;
                        break;
                default: fprintf(stderr, "BULLSHIT\n"); exit(EXIT_FAILURE); break;
            }
        }

        else if (IRList[i].type == IR_INSTR){
            if (section != SECTION_TEXT) return ASM_NOT_A_TEXT_SECTION;
            args_count = getArgsCount(IRList[i].instr.op);
            mode = getModeFromOperands(&IRList[i].instr.a, &IRList[i].instr.b, args_count, &status);
            CHK_ASM_STATUS(prog, status, IRList[i].line);
            if (get_handler(IRList[i].instr.op, mode) == NULL){
                return ASM_INVALID_ADRESSING_MODE;
            }
            text_addr += mode_size[mode];
        }
    }
    ass->bin.headers.code_size = text_addr;
    ass->bin.headers.data_size = data_addr - DATA_BASE;
    return ASM_OK;
}


AsmError assemblerSecondPass(char *prog, struct Assembler *ass, const struct IR_Node *IRList, size_t size){

    uint64_t addr = 0;
    int args_count = 0;
    uint8_t *base_reg;
    Operand const *ope;

    AsmError status;

    size_t text_addr = 0, data_addr = 0;

    Section section = SECTION_UNDEF;

    ass->bin.headers.magic_number = MAGIC_NUMBER;
    ass->bin.bytecode = malloc(ass->bin.headers.code_size);
    ass->bin.data = malloc(ass->bin.headers.data_size);

    struct Instruction instr = {0};

    for (size_t i = 0; i < size; i++){
        memset(&instr, 0, sizeof(instr));
        ass->error_line = IRList[i].line;
        
        if (IRList[i].type == IR_LABEL) continue;
        
        if (IRList[i].type == IR_DIR){
            switch (IRList[i].directive.type){
                case DIR_BYTE:  if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                    ass->bin.data[data_addr] = (uint8_t) IRList[i].directive.value.value;
                    data_addr++;
                    break;
                case DIR_ADDR: if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                if (IRList[i].directive.string.type == OPERAND_IDENT){
                    if (getSymbolAddr(&ass->symtable, IRList[i].directive.string.ident.str, &addr)){
                        for (size_t j = 0; j < 8; j++){
                            ass->bin.data[data_addr+j] = (uint8_t) (addr >> (8 * j));
                        }
                        data_addr += 8;
                        break;
                    }
                    else return ASM_LABEL_UNDEFINED;
                }
                case DIR_WORD: if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                    for (size_t j = 0; j < 8; j++){
                        ass->bin.data[data_addr+j] = (uint8_t) (IRList[i].directive.value.value >> (8 * j));
                    }
                    data_addr += 8;
                    break;
                case DIR_DATA: section = SECTION_DATA; break;
                case DIR_TEXT: section = SECTION_TEXT; break;
                case DIR_ENTRY: if (getSymbolAddr(&ass->symtable, IRList[i].directive.string.ident.str, &addr)){
                    ass->bin.headers.entry_point = addr;
                    break;
                    }
                    else return ASM_LABEL_UNDEFINED;
                case DIR_STRING: if (section != SECTION_DATA) return ASM_NOT_A_DATA_SECTION;
                    memcpy(ass->bin.data + data_addr, IRList[i].directive.string.ident.str, IRList[i].directive.string.ident.length);
                    data_addr += IRList[i].directive.string.ident.length;
                    break;
                default: fprintf(stderr, "MEGA BULLSHIT\n"); exit(EXIT_FAILURE);
            }
            continue;
        }

        if (IRList[i].type == IR_INSTR){
            if (section != SECTION_TEXT) return ASM_NOT_A_TEXT_SECTION;

            instr.opcode = IRList[i].instr.op;
            args_count = getArgsCount(IRList[i].instr.op);
            instr.mode = getModeFromOperands(&IRList[i].instr.a, &IRList[i].instr.b, args_count, &status);
            CHK_ASM_STATUS(prog, status, IRList[i].line);

            base_reg = (args_count <= 1) ? &instr.a : &instr.b;
            ope = (args_count <= 1) ? &IRList[i].instr.a : &IRList[i].instr.b;

            if (args_count == 2){
                instr.a = IRList[i].instr.a.reg;
            }
            switch (ope->type){
                case OPERAND_NONE: break;
                case OPERAND_VALUE:
                case OPERAND_MEM_VALUE: instr.args.imm = ope->value; break;
                case OPERAND_REG: *base_reg = ope->reg; break;
                case OPERAND_MEM_REG: *base_reg = ope->mem.base_reg; break;
                case OPERAND_MEM_IDX:
                    *base_reg = ope->mem.base_reg;
                    instr.args.offset = ope->mem.offset;
                    break;
                case OPERAND_IDENT:
                case OPERAND_MEM_IDENT: if (getSymbolAddr(&ass->symtable, ope->ident.str, &addr)){
                    instr.args.imm = (int64_t) addr;
                    break;
                }
                else return ASM_LABEL_UNDEFINED;
                case OPERAND_MEM_IDX_IDENT:
                    if (getSymbolAddr(&ass->symtable, ope->mem.ident.str, &addr)){
                        instr.args.imm = ((int64_t) addr + ope->mem.offset);
                        break;
                    }
                    return ASM_LABEL_UNDEFINED;
                default: fprintf(stderr, "ULTRA MEGA BULLSHIT\n"); exit(EXIT_FAILURE);
            }

            instructionEncode(&instr, ass->bin.bytecode + text_addr, ass->bin.headers.code_size - text_addr);
            text_addr += mode_size[instr.mode];
        }
    }

    return ASM_OK;
}