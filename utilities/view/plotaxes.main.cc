
/*
 *  main.cc
 */

#include <stdio.h>
#include <stdlib.h>

#include "runopts.h"
#include "init.h"
#include "error.h"
#include "setup.h"
#include "parseopts.h"

main(int argc, char **argv)
{
    int err=0;
    extern int InitThisProcess(void);
 
    /*
     * initialise
     */
    ElleInit();
    
    if (argc>1) {
        if (err=ParseOptions(argc,argv))
            OnError("",err);
    }

    /*
     * set up the X window
     */
    if (ElleDisplay()) SetupApp(argc,argv);

    /*
     * set the function to the one in your process file
     */
    ElleSetInitFunction(InitThisProcess);

    /*
     * run your initialisation function and start the application
     */
    StartApp();

     return(0);
} 
