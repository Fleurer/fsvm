#include <stdlib.h>
#include <stdio.h>

#include "opcode.h"
#include "fvm.h"

// main stack {{0_0}}
#define base ((fvm_current())->stack)
#define sp ((fvm_current())->sp)
// just for debug
const char fvm_op_names   [100][100] = { FVM_OP_NAMES };
const char fvm_tag_names  [100][100] = { FVM_TAG_NAMES };

Obj fcall(int argc, Func* func) {
    Proto   *proto = func->proto;
    // make a closure
    Env     *env = fnew_env(func->env);
    if (env==NULL) {
        fvm_panic("VMError: bad Env:<0x%lx>.\n", (Addr)env);
    }

    if (argc < proto->c_params) {
        fvm_panic("ApplyError: Func:<0x%lx> params do not match. (%d of %d)", (Addr)func, argc, proto->c_params);
    }

    int i=0;
    for(i=argc-1;i>=0;i--){
        fbind_var(env, Vstr(proto->consts[i]), fpop());
    }
    
    fvm_run(proto, env);
    // do some clean here
    // env can be cleaned now . TODO: if refcount were introduced, map (-1) vars.ref
    
    return *sp;
}

#define _next_op (*(pc++))
#define _next_opr ((int)(*(pc++)))

#define _dbg_show_op (op > OP_FOO)?\
    printf("op:%s,%d\n", fvm_op_names[(int)op], *(pc)): \
    printf("op:%s\n", fvm_op_names[(int)op]);

int fvm_run(Proto *proto, Env *env) {
    if (FDEBUG) {_dbg_show_proto(proto);};

    Op op, *pc;
    pc = proto->opcodes;
    while ((op = _next_op) != OP_RET ) {
        _dbg_show_op;
        switch (op){
        case NOP: break;
                                /* stack */
        case OP_LOAD_NIL: {
                                fpush(Fnil);
        } break;     

        case OP_LOAD_NUM: {
                                int n = _next_opr;
                                Obj r = fnum(n);
                                fpush(r);
        } break;

        case OP_LOAD_CONST: {
                                int n = _next_opr;            
                                Obj r = fget_const(proto, n);
                                fpush(r);
        } break;

        case OP_LOAD_TMP: {
                                int n = _next_opr;
                                Obj r = fget_tmp(env, n);
                                fpush(r);
        } break;

        case OP_LOAD_NAME: {
                                char *name = Vstr(fpop());
                                Obj r = fget_var(env, name);
                                if (fis_nil(r)){
                                    fvm_panic("NameError: <%s> is not availible\n", name);
                                }
                                fpush(r);
        } break;

        case OP_STORE_NAME: {
                                char *name = Vstr(fpop());         
                                Obj v = fpop();
                                Obj r = fset_var(env, name, v);
                                if (fis_nil(r)){
                                    fvm_panic("NameError: <%s> is not availible\n", name);
                                }
        } break;
        
        case OP_POP: {
                                fpop();            
        } break;
        
        case OP_STORE_TMP: {
                                int n = _next_opr;                   
                                Obj obj = fpop();
                                fset_tmp(env, n, obj);
        } break;

                                /* arithmetics */
        case OP_ADD: {
                                Obj a, b, r;
                                b = fpop();
                                a = fpop();
                                r = fnum( f2_cint(a) + f2_cint(b) );
                                fpush(r);
        } break;

        case OP_SUB: {
                                Obj a, b, r;
                                b = fpop();
                                a = fpop();
                                r = fnum( f2_cint(a) - f2_cint(b) );
                                fpush(r);      
        } break;
                                /* logic */
        case OP_EQ: {
                                Obj a, b;    
                                b = fpop();
                                a = fpop();
                                if ( feq( a, b ) ){
                                    fpush(b);
                                }
                                else {
                                    fpush(Fnil);
                                }
        } break;

        case OP_GT: {
                                Obj a, b;    
                                b = fpop();
                                a = fpop();
                                if ( fgt( a, b ) ){
                                    fpush(b);
                                }
                                else {
                                    fpush(Fnil);
                                }  
        } break;

        case OP_LT: {
                                Obj a, b;    
                                b = fpop();
                                a = fpop();
                                if ( flt( a, b ) ){
                                    fpush(b);
                                }
                                else {
                                    fpush(Fnil);
                                }  
        } break;

        case OP_AND: {
                                Obj a, b;
                                b = fpop();
                                a = fpop();
                                if (fnot_nil(a) && fnot_nil(b)) {
                                    fpush(b);
                                } else {
                                    fpush(Fnil);
                                }
        } break;    

        case OP_OR: {
                                Obj a, b;     
                                b = fpop();
                                a = fpop();
                                if (fnot_nil(a)) {
                                    fpush(a);
                                }
                                else if (fnot_nil(b)) {
                                    fpush(b);
                                }
                                else {
                                    fpush(Fnil); 
                                }
        } break;

        case OP_NOT:{
                                Obj a, r;    
                                a = fpop();
                                if (fis_nil(a)) {
                                    r = fnum(1);
                                }
                                else {
                                    r = Fnil;
                                }
                                fpush(r);
        } break;

        case OP_JMP: {
                                int offset = _next_opr;     
                                if (offset < 0){
                                    offset -= 2;
                                }
                                pc += offset;
        } break;
        
        case OP_BRANCH: {
                                Obj a = fpop();
                                int offset = _next_opr;     
                                if (fnot_nil(a)){
                                    // same as jmp
                                    if (offset < 0){
                                        offset -= 2;
                                    }
                                    pc += offset;
                                }
        } break;
                                /* operations */
        case OP_MKFUNC: {
                                int pid = _next_opr;
                                Proto *proto = env->vm->protos[pid];
                                Func *func =fnew_func(proto, env);
                                Obj ofunc = ffunc(func);          
                                fpush(ofunc);
        } break;

        case OP_CALL: {
                                int n = _next_opr;
                                Obj obj = *sp--;
                                if (T(obj) != T_FUNC) {
                                    fvm_panic("TypeError: <Obj:%lx> is not a function.\n", V(obj));
                                }
                                fcall(n, Vfunc(obj));
        } break;

        case OP_PRINT: {
                                Obj a = fpop();
                                char *str = f2_cstr(a);
                                printf("%s\n", str);
        } break;

        case OP_PRINT_STACK: {
                                 Obj* stack = base;
                                 int i=0, offset=sp-stack+1;
                                 printf("stack depth: %d\n", offset);
                                 for (i=offset-1; i>=0; i--){
                                     printf("%3d | %8s | %s\n", i, fvm_tag_names[stack[i].tag], f2_cstr(stack[i]));
                                 }             
        } break;

        default:
                                fvm_panic("VMError: unknown opcode: %d\n", *pc);
        }
    }
    return 0;
}


// stack helpers 
Obj fpush(Obj obj) {
    *(++sp) = obj;
    return obj;
}

Obj fpop() {
    if (sp-1 < base) {
        fvm_panic("Stack Error: pop below the stack base\n");
    }
    return *(sp--);
}
