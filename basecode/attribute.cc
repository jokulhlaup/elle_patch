 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: attribute.cc,v $
 * Revision:  $Revision: 1.2 $
 * Date:      $Date: 2003/12/19 01:15:24 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#include <iostream>
#include <algorithm>
#include "attribute.h"
#include "file.h"

/*****************************************************

static const char rcsid[] =
       "$Id: attribute.cc,v 1.2 2003/12/19 01:15:24 levans Exp $";

******************************************************/

const int NUM_IFA=8;
static int IntFlynnAttrib[]={ COLOUR,EXPAND,MINERAL,GRAIN,SPLIT,
                              F_ATTRIB_I, F_ATTRIB_J, F_ATTRIB_K };
const int NUM_RFA=11;
static int RealFlynnAttrib[]={ ENERGY,VISCOSITY,S_EXPONENT,
						E3_ALPHA,E3_BETA,E3_GAMMA,
                     AGE,CYCLE,DISLOCDEN,F_ATTRIB_A,F_ATTRIB_B,F_ATTRIB_C };

bool Attribute::isIntAttribute()
{
    return(std::find(IntFlynnAttrib,IntFlynnAttrib+NUM_IFA,getType()) !=
                  IntFlynnAttrib+NUM_IFA);
}

bool Attribute::isRealAttribute()
{
    return(std::find(RealFlynnAttrib,RealFlynnAttrib+NUM_RFA,getType()) !=
                  RealFlynnAttrib+NUM_RFA);
}

//Write object to an ostream reference
std::ostream & operator<< (std::ostream &os, const Attribute &t)
{
		std::cout << t.id << ", "<< t.value << '\n';
    return os;
}

