 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: error.c,v $
 * Revision:  $Revision: 1.4 $
 * Date:      $Date: 2006/06/15 08:30:40 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runopts.h"
#include "error.h"
#include "log.h"

/*****************************************************

static const char rcsid[] =
       "$Id: error.c,v 1.4 2006/06/15 08:30:40 levans Exp $";

******************************************************/
#ifdef __cplusplus
extern "C" {
#endif
void append_error_message(char *message,int err_num);
void OptsSyntax(int argc,char **argv);
void ElleRemoveTriAttributes();
/*extern int DisplayElleErrorMsg(char *msg,int err_num);*/
#ifdef __cplusplus
}
#endif

char ErrMessage[FILENAME_MAX];

char logbuf[E_LOGBUFSIZ];

void OnError(char *message,int err_num)
{
    /*if (ElleDisplay())*/
        /*DisplayElleErrorMsg(message,err_num);*/
    /*else {*/
        append_error_message(message,err_num);
        sprintf(logbuf,"%s\n",ErrMessage);
        Log( 0,logbuf );
        if (!ElleDisplay()) {
            CleanUp();
            exit(1);
        }
    /*}*/
}

void append_error_message(char *message,int err_num)
{
    int len = 0;
    strcpy(ErrMessage,"");
    strncpy(ErrMessage,ElleAppName(),FILENAME_MAX-1);
    len = strlen(ErrMessage);
    strncat(ErrMessage,": ",FILENAME_MAX-len-1);
    len = strlen(ErrMessage);
    strncat(ErrMessage,message,FILENAME_MAX-len-1);
    len = strlen(ErrMessage);
    switch (err_num) {
    case MALLOC_ERR:strncat(ErrMessage," - Memory error",FILENAME_MAX-len-1);
                break;
    case READ_ERR:strncat(ErrMessage," - Error reading file",FILENAME_MAX-len-1);
                break;
    case OPEN_ERR:strncat(ErrMessage," - Error opening file",FILENAME_MAX-len-1);
                break;
    case EOF_ERR:strncat(ErrMessage," - End of file",FILENAME_MAX-len-1);
                break;
    case NOFILE_ERR:strncat(ErrMessage," - No elle file open",FILENAME_MAX-len-1);
                break;
    case NODENUM_ERR:strncat(ErrMessage," - inactive node",FILENAME_MAX-len-1);
                break;
    case NONB_ERR:strncat(ErrMessage," - neighbour node not found",FILENAME_MAX-len-1);
                break;
    case GRNNUM_ERR:strncat(ErrMessage," - inactive grain",FILENAME_MAX-len-1);
                break;
    case TYPE_ERR:strncat(ErrMessage," - unknown region type",FILENAME_MAX-len-1);
                break;
    case ID_ERR:strncat(ErrMessage," - could not match id",FILENAME_MAX-len-1);
                break;
    case MAXATTRIB_ERR:strncat(ErrMessage," - attribute limit reached",FILENAME_MAX-len-1);
                break;
    case ATTRIBID_ERR:strncat(ErrMessage," - invalid attribute",FILENAME_MAX-len-1);
                break;
    case RGNWRP_ERR:strncat(ErrMessage," - region wrap",FILENAME_MAX-len-1);
                break;
    case LIMIT_ERR:strncat(ErrMessage," - array limit reached",FILENAME_MAX-len-1);
                break;
    case DATA_ERR:strncat(ErrMessage," - data not found",FILENAME_MAX-len-1);
                break;
    case INDEX_ERR:strncat(ErrMessage," - index already used",FILENAME_MAX-len-1);
                break;
    case MAXINDX_ERR:strncat(ErrMessage," - max array index exceeded",FILENAME_MAX-len-1);
                break;
    case ORD_ERR:strncat(ErrMessage," - order array not found",FILENAME_MAX-len-1);
                break;
    case KEY_ERR:strncat(ErrMessage," - Unknown keyword",FILENAME_MAX-len-1);
                break;
    case OLDKEY_ERR:strncat(ErrMessage," - Keyword no longer accepted",FILENAME_MAX-len-1);
                break;
    case INTERSECT_ERR:strncat(ErrMessage," - Intersection not found",FILENAME_MAX-len-1);
                break;
    case NODECNT_ERR:strncat(ErrMessage," - Node count differs",FILENAME_MAX-len-1);
                break;
    case SEGCNT_ERR:strncat(ErrMessage," - Segment count differs",FILENAME_MAX-len-1);
                break;
    case RANGE_ERR:strncat(ErrMessage," - Range error",FILENAME_MAX-len-1);
                break;
    case INVALIDF_ERR:strncat(ErrMessage," - File does not have necessary attributes for process",FILENAME_MAX-len-1);
                break;
    case SYNTAX_ERR: OptsSyntax(0,0);
                strcat(ErrMessage," - Syntax error");
                break;
    case HELP_ERR: OptsSyntax(0,0);
                strcat(ErrMessage," - unknown option");
                break;
    default:    break;
    }
}

void CleanUp(void)
{
    ElleRunFunc exitfn;

    ElleRemoveArrays();
    ElleRemoveTriAttributes();
    ElleRemoveEnergyLUT();
    /*if ((exitfn=ElleExitFunction())!=0) (*exitfn)();*/
}

void OptsSyntax(int argc,char **argv)
{
    fprintf(stderr, "options:\n" );
    fprintf(stderr, "   -i   name of input file\n" );
    fprintf(stderr, "   -s   number of stages to be run\n" );
    fprintf(stderr, "   -f   how often to save .elle files\n" );
    fprintf(stderr, "   -n   no X11 display\n" );
    fprintf(stderr, "   -h   Print this message\n" );
    fprintf(stderr, "Example:  elle_gg -i growth20.elle -s 50 -f 10 -n\n" );
}
