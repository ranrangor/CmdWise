#include<stdio.h>
#include"CmdWise.h"


extern cw_root_t RCMDs;

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



    cw_register_cmd(help,ARG_N,"help command",help_cb);
    cw_register_cmd(help_all,ARG_0,"help all command",help_all_cb);
    cw_register_cmd(help_you,ARG_0,"help you command",NULL);

    cw_register_cmd(list_all,ARG_0,"list all command",list_all_cb);
    cw_register_cmd(list_show,ARG_N,"list show command",list_show_cb);
    cw_register_cmd(list_show_all,ARG_0,"list show all command",list_show_all_cb);

    cw_register_cmd(list,ARG_0,"list  command",list_cb);
    cw_register_cmd(list_all_but,ARG_1,"list all but command",list_all_but_cb);


    cw_register_cmd(play_list,ARG_1,"play list command",NULL);
    cw_register_cmd(play_mode,ARG_1,"play mode command",NULL);
    cw_register_cmd(play_all,ARG_0,"play all command",NULL);



    cw_print_cmds_tree();

    printf("-----\n\n");
//    cw_unregister_cmd(&RCMDs.heads,list_all);
    cw_unregister_cmd(play_all);

    cw_print_cmds_tree();


    printf("==================\n\n");



    char**argvs;

    char linebuf[1024];

    while(fgets(linebuf,1024,stdin)){
        

        argvs=cw_parse_line(linebuf);


        cw_invoke_cmd(argvs);

        cw_print_argvs(argvs);

        cw_free_argvs(argvs);

    }



    return 0;

}


