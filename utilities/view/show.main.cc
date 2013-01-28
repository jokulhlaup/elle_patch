
/*
 *  main.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "runopts.h"
#include "init.h"
#include "error.h"
#include "parseopts.h"
#include "setup.h"

main(int argc, char **argv)
{
    int err=0;
    ElleRunFunc init;
    extern int InitShow(void);

    ElleInit();

    ElleSetInitFunction(InitShow);

    if (err=ParseOptions(argc,argv))
        OnError("",err);

    if (ElleDisplay()) SetupApp(argc,argv);

    StartApp();
} 
