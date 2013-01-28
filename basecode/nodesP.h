 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: nodesP.h,v $
 * Revision:  $Revision: 1.1.1.1 $
 * Date:      $Date: 2002/09/18 03:16:14 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#ifndef _E_nodesP_h
#define _E_nodesP_h

#define  INITIAL_ATTRIBS  12

typedef struct {
    int nbnodes[3];
    int grains[3];
    double bndtype[3];
    int type;
    int state;
    double x;
    double y;
    double prev_x;
    double prev_y;
    double *attributes;
} NodeAttrib;

typedef struct {
    NodeAttrib *elems;
    int *randomorder;
    int maxnodes;
    int maxattributes; /* currently INITIAL_ATTRIBS */
    int activeattributes[INITIAL_ATTRIBS];
    int CONCactive;
} NodeArray;

extern NodeAttrib *ElleNode(int node);
#endif
