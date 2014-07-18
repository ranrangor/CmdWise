#ifndef _H_CMDWISE_
#define _H_CMDWISE_

//#include"lst.h"



typedef enum{

    ARG_0=0,
    ARG_1=1,
    ARG_2=1<<1,
    ARG_3=1<<2,
    ARG_N=1<<3
    
}cw_arg_t;



typedef int (*cmd_cb)(void*);

typedef struct _cmd_head cw_cmdhead_t;

typedef struct _cmd cw_cmd_t;


struct _cmd_head{

    int n_cmds;
    struct _cmd*cmd_heads;

};

struct _cmd{

    cw_cmdhead_t subcmds;

    char*cmd_name;
    char*cmd_desc;
    cmd_cb cmd_callback;
//    int level;
    cw_arg_t cmd_argtype;

    struct _cmd* next;
    struct _cmd* prev;

};




typedef struct _cmd_root{
    
    cw_cmdhead_t heads;
    cw_cmd_t* cur_cmd; //current line's first(main) cmd 
    cw_cmd_t* last_cmd; //last processed cmd within current line
}cw_root_t;


typedef struct _cmd_pair{
    
    cw_cmd_t*last_cmd;
    cw_cmd_t*cur_cmd;

}cw_cmd_pair_t;


int cw_init();

void cw_fini();


void _cw_register_cmd(cw_cmdhead_t*cmdhead,char**argvs,cw_arg_t argtype,const char*cmd_desc,cmd_cb callbacck);
void cw_register_cmd(char**argvs,cw_arg_t argtype,const char*cmd_desc,cmd_cb callbacck);

int _cw_unregister_cmd(cw_cmdhead_t*cmdhead,char**argvs);
int cw_unregister_cmd(char**argvs);

int _cw_invoke_cmd(cw_cmdhead_t*cmdhead,char**argvs);
int cw_invoke_cmd(char**argvs);


char**cw_parse_line(char*line);
void cw_print_argvs(char **argvs);
void _cw_print_cmds_tree(cw_cmdhead_t*cmdhead);
void cw_print_cmds_tree();
void cw_free_argvs(char**argvs);



#endif
