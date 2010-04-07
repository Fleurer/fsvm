/*
 * opcode.h
 * Fleurer Lee
 *
 * */

#ifndef opcode_h
#define opcode_h

// opcodes
enum{
    OP_RET = 0, 
    NOP, //_0
    OP_POP, //_0
    OP_BIND_NAME, //_0
    OP_LOAD_NAME, //_0
    OP_STORE_NAME, //_0
    /* arithmetics */
    OP_ADD,  //_0
    OP_SUB, //_0
    OP_MUL, //_0
    OP_DIV, //_0
    OP_MOD, //_0
    /* logic */
    OP_EQ, //_0
    OP_GT, //_0
    OP_LT, //_0
    //
    OP_AND, //_0
    OP_OR,  //_0
    OP_NOT, //_0
    /* blah for debug? */
    OP_PRINT, //_0
    OP_PRINT_STACK, //_0
    OP_LOAD_NIL, //_0
    OP_1, 
    //
    // with opr 
    //
    /* stack */
    OP_LOAD_NUM,        //_4
    OP_LOAD_CONST,      //_1
    OP_LOAD_LOCAL,      //_1
    OP_STORE_LOCAL,     //_1
    OP_LOAD_OUTER,      //_1
    OP_STORE_OUTER,     //_1
    //
    OP_MKFUNC,          //_1
    OP_CALL,            //_1
    /* branch */
    OP_JMP,             //_4
    OP_BRANCH,          //_4
    // blah~
    OP_LAST
};


#define FVM_OP_NAMES \
    "ret", \
    "nop", \
    "pop", \
    "bind_var", \
    "load_var", \
    "store_var", \
    "add", \
    "sub", \
    "mul", \
    "div", \
    "mod", \
    "eq", \
    "gt", \
    "lt", \
    "and", \
    "or", \
    "not", \
    "print", \
    "print_stack", \
    "_1", \
    "load_nil", \
    "load_num", \
    "load_const", \
    "load_local", \
    "store_local", \
    "load_outer", \
    "store_outer", \
    "mk_func", \
    "call", \
    "jmp",\
    "branch", \
    "last"


#endif
