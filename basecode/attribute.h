 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: attribute.h,v $
 * Revision:  $Revision: 1.2 $
 * Date:      $Date: 2003/10/08 04:47:36 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#ifndef _E_attribute_h
#define _E_attribute_h
#include <iostream>
#include <vector>
#include "attrib.h"

// storage class for attribute values

class Attribute {
    int id;
    double value;
public:
    Attribute(): id(NO_NB),value(0.0) { }
    Attribute(int idval): id(idval),value(0) { }
    Attribute(int idval, double avalue): id(idval),value(avalue) { }
    void setType(int idval) { id = idval; }
    void setValue(double avalue) { value = avalue; }
    int getType() { return(id); }
    double getValue() { return(value); }
    void remove() { id = NO_NB; value = 0.0; }
    const Attribute & operator=(const Attribute &t) { id=t.id; value=t.value;
                                                      return t; }
    bool isIntAttribute();
    bool isRealAttribute();
    friend std::ostream & operator<< (std::ostream &, const Attribute &);
};

#endif
