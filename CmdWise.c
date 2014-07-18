#include"CmdWise.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

cw_root_t RCMDs;


int cw_init()
{

    RCMDs.cur_cmd=NULL;
    RCMDs.last_cmd=NULL;
    RCMDs.heads.cmd_heads=NULL;
    RCMDs.heads.n_cmds=0;

    return 1;
}

void cw_fini()
{
    //nothing

}


static void cw_set_current_cmd(cw_cmd_t*cmd)
{
    RCMDs.cur_cmd=cmd;
}


static cw_cmd_t* cw_get_current_cmd()
{
    return (cw_cmd_t*)RCMDs.cur_cmd;
}



static void cw_set_llast_cmd(cw_cmd_t*cmd)
{
    RCMDs.last_cmd=cmd;
}

static cw_cmd_t* cw_get_llast_cmd()
{
    return (cw_cmd_t*)RCMDs.last_cmd;
}



static cw_cmd_t*new_cmd(const char*cmdname,const char*cmddesc,cmd_cb cmdcallback,cw_arg_t atype)
{

    cw_cmd_t*pcmd=(cw_cmd_t*)malloc(sizeof(cw_cmd_t));
    if(!pcmd)
        return NULL;

    bzero(pcmd,sizeof(cw_cmd_t));

    if(cmdname)
        pcmd->cmd_name=strdup(cmdname);
    if(cmddesc)
        pcmd->cmd_desc=strdup(cmddesc);

    pcmd->cmd_argtype=atype;
    pcmd->cmd_callback=cmdcallback;

    pcmd->next=pcmd->prev=NULL;

    return pcmd;

}

/**
 *  Delete `cmd' and its' all successors(subcmds).
 * */

static void del_cmd(cw_cmd_t*cmd)
{

    if(!cmd)
        return;
//
    if(cmd->cmd_name)
        free(cmd->cmd_name);
    if(cmd->cmd_desc)
        free(cmd->cmd_desc);
//
    if(cmd->subcmds.cmd_heads){
        cw_cmd_t*todel=NULL;
        cw_cmd_t*curcmd=cmd->subcmds.cmd_heads;
        while(curcmd){
            todel=curcmd;
            curcmd=curcmd->next;
            del_cmd(todel);
        }
    }
    free(cmd);

}

static cw_cmd_t*find_cmd_from_cmdhead(cw_cmdhead_t*head,const char*cname,cw_cmd_pair_t*opair/*out*/)
{
    
    cw_cmd_t*cur_cmd=head->cmd_heads;
    cw_cmd_t*last_cmd=NULL;
    if(!cur_cmd || !cname){
        if(opair){
            opair->cur_cmd=NULL;
            opair->last_cmd=NULL;
        }
        return NULL;
    }
    while(cur_cmd->next){
        if(!strcmp(cur_cmd->cmd_name,cname)){
            break;
        }
        last_cmd=cur_cmd;
        cur_cmd=cur_cmd->next;
    }
    if(strcmp(cur_cmd->cmd_name,cname)){
    //there is no cmd named `cname'
        last_cmd=cur_cmd;
        cur_cmd=NULL;
    }
    if(opair){
        opair->cur_cmd=cur_cmd;
        opair->last_cmd=last_cmd;
    }
    return cur_cmd;

}


static cw_cmd_t*find_cmd_from_cmdhead_(cw_cmdhead_t*head,const char*cname)
{
    
    cw_cmd_t*cur_cmd=head->cmd_heads;

    if(!cname || !cur_cmd){
        return NULL;
    }
    while(cur_cmd){
        
        if(!strcmp(cur_cmd->cmd_name,cname)){
            break;
        }

        cur_cmd=cur_cmd->next;
    }
    return cur_cmd;

}



static void prepend_cmd_into_cmdhead(cw_cmdhead_t*head,cw_cmd_t*cmd)
{


    if(head->cmd_heads==NULL){
        head->cmd_heads=cmd;
        cmd->next=NULL;
        cmd->prev=NULL;
    }else{
        cmd->prev=NULL;
        cmd->next=head->cmd_heads;
        head->cmd_heads->prev=cmd;
        head->cmd_heads=cmd;

    }
    head->n_cmds++;


}

static int remove_cmd_from_cmdhead(cw_cmdhead_t*head,cw_cmd_t*cmd)
{
    if(!head || !cmd)
        return 0;

//    cw_cmd_pair_t cmd_pair;
    cw_cmd_t*cur=find_cmd_from_cmdhead_(head,cmd->cmd_name);//,&cmd_pair);
    
    if(!cur){
        return 0;
    }

    if(cur->prev && cur->next){
        cur->prev->next=cur->next;
        cur->next->prev=cur->prev;
    }else if(cur->prev){//(cur->next == NULL)
        cur->prev->next=cur->next;//NULL 

    }else if(cur->next){//(cur->prev == NULL) modify head
        cur->next->prev=cur->prev;//NULL 
        head->cmd_heads=cur->next;
    }else{//cur->next == cur->prev == NULL
        fprintf(stderr,"Can not get here!!!\n");

    }
/*
    if(cmd_pair.cur_cmd){
        if(cmd_pair.last_cmd){
            cmd_pair.last_cmd->next=cmd_pair.cur_cmd->next; 
            return 1;
        }else{
            //last_cmd==NULL;head node
            head->cmd_heads=cmd_pair.cur_cmd->next;
            return 2;
        }

    }
    return 0;
*/
    head->n_cmds--;
    return 1;

}

static int n_argvs(char**argvs)
{
    int i=0;
    while(*argvs++)
        i++;

    return i;
}

void _cw_register_cmd(cw_cmdhead_t*cmdhead,char**argvs,cw_arg_t argtype,const char*cmd_desc,cmd_cb callback)
{
   const char* cur_cmd_name=argvs[0];
   int cur_cmd_len=n_argvs(argvs);
  
   if(cur_cmd_len<1){
        fprintf(stderr,"Unrecognitived CMDs\n");
   }

   cw_cmd_t*cur_cmd=find_cmd_from_cmdhead_(cmdhead,cur_cmd_name);

   if(!cur_cmd){
           cw_cmd_t*ncmd;
       
       if(cur_cmd_len==1)
            ncmd=new_cmd(cur_cmd_name,cmd_desc,callback,argtype);
       else
            ncmd=new_cmd(cur_cmd_name,NULL,NULL,ARG_0);

       prepend_cmd_into_cmdhead(cmdhead,ncmd);
       cur_cmd=ncmd;

   }

   if(cur_cmd_len>1)
       _cw_register_cmd(&cur_cmd->subcmds,&argvs[1],argtype,cmd_desc,callback);



}


void cw_register_cmd(char**argvs,cw_arg_t argtype,const char*cmd_desc,cmd_cb callback)
{
    _cw_register_cmd( &RCMDs.heads,argvs,argtype,cmd_desc,callback);

}



int _cw_unregister_cmd(cw_cmdhead_t*cmdhead,char**argvs)
{
    const char* cur_cmd_name=argvs[0];
    int cur_cmd_len=n_argvs(argvs);
   
   if(cur_cmd_len<1){
        fprintf(stderr,"Unrecognitived CMDs\n");
        return 0;
   }

    cw_cmd_t*cur_cmd=find_cmd_from_cmdhead_(cmdhead,cur_cmd_name);
    
    if(!cur_cmd)
        return 0;

    if(cur_cmd_len==1){
        remove_cmd_from_cmdhead(cmdhead,cur_cmd);
        del_cmd(cur_cmd);
        return 1;
    }else
        return _cw_unregister_cmd(&cur_cmd->subcmds,&argvs[1]);

}

int cw_unregister_cmd(char**argvs)
{
    return _cw_unregister_cmd( &RCMDs.heads,argvs);

}




static int get_remainder_nargvs(char**argvs)
{
    int i=0;
    while(*argvs++){
        i++;
    }
    return i;
}


/*
 *Filter `--' string,and treat subsequent strings as arguments.
 */
static int get_valid_argvs_pos(char**argvs)
{

    int pos=0;
    while(*argvs){

        if(!strcmp(*argvs,"--")){
            pos++;
        }else{
            break;
        }
        argvs++;
    }
    return pos;

}

//FIXME
int _cw_invoke_cmd(cw_cmdhead_t*cmdhead,char**argvs)
{
//    static cmd_cb pcallback; 
    cmd_cb pcallback;
    int apos;
    int largs;
    cw_cmd_t*last_cmd,*curp_cmd;

    cw_cmd_t*curcmd=find_cmd_from_cmdhead_(cmdhead,*argvs);

    if(curcmd){
        if(!cw_get_current_cmd()){//each line's first cmd(chosen tobe the Main cmd)
            cw_set_current_cmd(curcmd);
        }
        cw_set_llast_cmd(curcmd);

        return _cw_invoke_cmd(&curcmd->subcmds,&argvs[1]);

    }else{ //can not find a cmd named `*argvs'
        // so this is a argument.
        if(cw_get_llast_cmd()){//last available cmd callback;
            //To filter separator `--'
            apos=get_valid_argvs_pos(argvs);
            largs=get_remainder_nargvs(argvs+apos);
            last_cmd=cw_get_llast_cmd();
            curp_cmd=cw_get_current_cmd();

            pcallback=last_cmd->cmd_callback;
            if(pcallback)
                pcallback(argvs+apos);
            else{
                fprintf(stderr,"No Corresponding Callback set for CMD[%s:%s]\n",
                        curp_cmd->cmd_name,last_cmd->cmd_name);
//                return 0;
            }
            cw_arg_t tp=last_cmd->cmd_argtype;
            //reset llast cmd ref.
            cw_set_llast_cmd(NULL);
//            pcallback=NULL;
//           printf("argtype::%d\n",last_cmd->cmd_argtype) ;
//            switch(last_cmd->cmd_argtype){
            switch(tp){
                
                case ARG_0:
                    if(largs>0){
                        return _cw_invoke_cmd(&curp_cmd->subcmds,argvs+apos+0);
                    }
                    break;
                case ARG_1:
                    if(largs>1){
                        return _cw_invoke_cmd(&curp_cmd->subcmds,argvs+apos+1);
                    }
                    break;
                case ARG_2:
                    if(largs>2){
                        return _cw_invoke_cmd(&curp_cmd->subcmds,argvs+apos+2);
                    }
                    break;
                case ARG_N:

                    break;
                default:
                    break;
            }

        }else{
            fprintf(stderr,"WARNING:: Unknown Command\n");
            return 0;
        }

    }
return 0;
}


int cw_invoke_cmd(char**argvs)
{
    return _cw_invoke_cmd(&RCMDs.heads,argvs);
    
}


/*FIXME to process `quotes' */
char**cw_parse_line(char*line)
{

    if(!line)
        return NULL;

    char**oargvs=(char**)malloc(sizeof(char*));
//    oargvs[0]=NULL;

    const char*delimit=" \t\n";
    int i;
    char*str,*savstr;
    char*tok;
    str=line;

    for(i=0;(tok=strtok_r(str,delimit,&savstr));i++,str=NULL){
        
        oargvs=realloc(oargvs,sizeof(char*)*(2+i));

        oargvs[i]=strdup(tok);
//        oargvs[i+1]=NULL;
    }
    oargvs[i]=NULL;

    return oargvs;

}

void cw_free_argvs(char**argvs)
{
    if(!argvs)
        return;

    char**iargv=argvs;
    while(*iargv){
        free(*iargv);
        iargv++;
    }

    free(argvs);

}


void cw_print_argvs(char **argvs)
{
    int i=1;
    while(*argvs){

        printf("=(%d)=>|%s|\n",i,*argvs);
        argvs++;
        i++;
    }

}



static void print_indent(int level)
{
    while(level--){
        printf("  ");
    }

}

void _cw_print_cmds_tree(cw_cmdhead_t*cmdhead)
{
    static int level=0;

    cw_cmd_t*curcmd=cmdhead->cmd_heads;
    if(cmdhead){
        int i=1;
        while(curcmd){
            print_indent(level);
            printf("(%d)::[%s]:CB{%p}:%d::{%s}\n",i++,curcmd->cmd_name,curcmd->cmd_callback,curcmd->cmd_argtype,curcmd->cmd_desc);

            level++;
            _cw_print_cmds_tree(&curcmd->subcmds);
            level--;
//            print_cmd(curcmd);
            curcmd=curcmd->next;
        }
    }
}

void cw_print_cmds_tree()
{
    _cw_print_cmds_tree(&RCMDs.heads);
}


/*


static int help_cb(void*argv)
{
    printf("help callback\n");
}

static int help_all_cb(void*argv)
{
    printf("help_all callback\n");
}

static int list_all_but_cb(void*argv)
{
    printf("list all but callback\n");
    printf("<%s>\n",(char*)argv);
}

static int list_all_cb(void*argv)
{
    printf("list all  callback\n");
}

static int list_cb(void*argv)
{
    printf("list  callback\n");
}

static int list_show_cb(void*argv)
{
    printf("list show callback\n");
}






static int list_show_all_cb(void*argv)
{
    printf("list show all  callback\n");
}







int main(int argc,char**argv)
{
    


    char*help[]={"help",NULL};
    char*help_all[]={"help","all",NULL};
    char*help_you[]={"help","you",NULL};
    char*list_all[]={"list","all",NULL};
    char*list_all_but[]={"list","all","but",NULL};
    char*list[]={"list",NULL};
    char*list_show[]={"list","show",NULL};
    char*list_show_all[]={"list","show","all",NULL};
    char*play_list[]={"play","list",NULL};
    char*play_mode[]={"play","mode",NULL};
    char*play_all[]={"play","all",NULL};


    cw_init();



    _cw_register_cmd(&RCMDs.heads,help,ARG_N,"help command",help_cb);
    _cw_register_cmd(&RCMDs.heads,help_all,ARG_0,"help all command",help_all_cb);
    _cw_register_cmd(&RCMDs.heads,help_you,ARG_0,"help you command",NULL);

    _cw_register_cmd(&RCMDs.heads,list_all,ARG_0,"list all command",list_all_cb);
    _cw_register_cmd(&RCMDs.heads,list_show,ARG_N,"list show command",list_show_cb);
    _cw_register_cmd(&RCMDs.heads,list_show_all,ARG_0,"list show all command",list_show_all_cb);

    _cw_register_cmd(&RCMDs.heads,list,ARG_0,"list  command",list_cb);
    _cw_register_cmd(&RCMDs.heads,list_all_but,ARG_1,"list all but command",list_all_but_cb);


    _cw_register_cmd(&RCMDs.heads,play_list,ARG_1,"play list command",NULL);
    _cw_register_cmd(&RCMDs.heads,play_mode,ARG_1,"play mode command",NULL);
    _cw_register_cmd(&RCMDs.heads,play_all,ARG_0,"play all command",NULL);



    cw_print_cmds_tree(&RCMDs.heads);

    printf("-----\n\n");
//    cw_unregister_cmd(&RCMDs.heads,list_all);
    _cw_unregister_cmd(&RCMDs.heads,play_all);

    cw_print_cmds_tree(&RCMDs.heads);


    printf("==================\n\n");



    char**argvs;

    char linebuf[1024];

    while(fgets(linebuf,1024,stdin)){
        

        argvs=cw_parse_line(linebuf);


        cw_invoke_cmd(&RCMDs.heads,argvs);

        cw_print_argvs(argvs);

        cw_free_argvs(argvs);

    }



    return 0;

}


*/
