#include "fvm.h"

char* _dbg_tag_names(int t){
   static const char fvm_tag_names  [100][100] = { FVM_TAG_NAMES }; 
   return fvm_tag_names[t];
}
char* _dbg_obj(Obj obj){
    printf("%6s | %s\n", _dbg_tag_names(T(obj)), f2_cstr(obj));
}
char* _dbg_2str(Obj obj){
    static char str[255];
    sprintf(str, "%6s | %s\n", _dbg_tag_names(T(obj)), f2_cstr(obj));
    return str;
}

int _dbg_show_proto(Proto *proto){
    int i;
    printf("Proto<0x%lx>: \n", (Addr)proto);
    printf("\tparams: %d\n", proto->c_params);
    printf("\tlocals: %d", proto->c_lvars);
    printf("\t");
    for(i=0; i<proto->c_lvars; i++){
        printf("%s, ", proto->lnames[i]); 
    }
    printf("\n");
    printf("\touters: %d", proto->c_ovars);
    printf("\t");
    for(i=0; i<proto->c_ovars; i++){
        printf("%s, ", proto->onames[i]); 
    }
    printf("\n");
    printf("\tconsts: %d\n", proto->c_consts);
    for (i=0;i<proto->c_consts;i++){
        printf("\t\t %d : %s", i, _dbg_2str(proto->consts[i]));
    }
    return 0;
}
int _dbg_show_env(Env *env){
    
}

