// Written in 2003
// Author: Dr. J.K. Becker
// Copyright: Dr. J.K. Becker (becker@jkbecker.de)
#ifndef _E_preferences_h
  #define _E_preferences_h

  #include "wx/wx.h"
  #include "wx/colordlg.h"
  #include "wx/font.h"
  #include "wx/spinctrl.h"
  #include "wx/scrolwin.h"
  #include "wx/image.h"
  #include "wx/notebook.h"
  #include <math.h>
  //#include "wx/glcanvas.h"
  //#include <GL/gl.h>
  //#include <GL/glu.h>
  #include <stdlib.h>
  #include "wx/grid.h"

//////////Elle-Zeug
  #include "nodes.h"
  #include "interface.h"
  #include "init.h"
  #include "file.h"
  #include "unodes.h"
  #include "general.h"

enum
{
    ID_PCancel, ID_POk, ID_PApply, ID_PDColor, ID_PTColor, ID_PLColor, ID_Pbnodesize, ID_Plinesize, ID_Pbnodeshow,
    ID_Pbitmapsize, ID_Pflynnshownumbers, ID_Pcolorsingleflynn, ID_Pnodeshownumbers, ID_Unodes3d, ID_PResetSingleFlynnColor,
    ID_Prangeflagunodes, ID_punoderangeflagmaxval, ID_punoderangeflagminval, ID_Pnotrangeflagunodes,
    ID_punodenotrangeflagmaxval, ID_punodenotrangeflagminval, ID_PSave,
ID_PLoad, ID_Punodetriangles,ID_FClampColor,ID_FCCMin,ID_FCCMax,ID_BClampColor,ID_BCCMin,ID_BCCMax,ID_NClampColor,ID_NCCMin,ID_NCCMax,ID_UClampColor,ID_UCCMin,ID_UCCMax,
    ID_punodelistbox,ID_pbnodelistbox,ID_pflynnlistbox,ID_plinelistbox,
    ID_makeunodes,ID_Punodesize,ID_Prainchange,ID_DIALOG,ID_TEXTCTRL,ID_PANEL,ID_DEFAULT,
    ID_CSAVE ,ID_CLOAD,ID_sflynn,ID_sbnode,ID_sunode,ID_CTEXT,
};

#define CLAMP1 0.0039215686

class Canvas;



class Preferences : public wxDialog
{
    DECLARE_CLASS( Preferences )

public :
    Preferences( wxWindow * parent );
    wxBitmap bnodebit, linebit, tnodebit;
    wxPen pbnodepen, plinepen, ptnodepen;
    wxBrush pbnodebrush, ptnodebrush;
    wxString attribute;
    int nodesize, useunodea, attrib,battrib, usebnodea,unodesize,rainchange;
    wxListBox * cflynn, * cunode, * cbnode, * cline;
    wxStaticText * bitmaplabel;
    bool plineshow, pflynnshownumbers, prangeflagunodes, pnotrangeflagunodes;
    bool pnodeshownumbers, unodes3d;
private:
    wxToolBar * toolbar;
    wxNotebook * book;
    wxBitmap * toolbarbitmap[2];
    int GetAttributeInt( wxString welche,int type );
    wxTextCtrl * uvalmin,*uvalmax,*Fccmin,*Fccmax,*Bccmin,*Bccmax,*Nccmin,*Nccmax,*Uccmin,*Uccmax;
    double urangevalmax, urangevalmin, unotrangevalmax, unotrangevalmin;;
    wxString GetAttributeName( int welche,int type );
    void GetUserColor(wxColour *col);
    void OnClose( wxCloseEvent & event );
    void OnListBox(wxCommandEvent & event);
    void OnLoadPrefs( wxCommandEvent & event );
    void OnSavePrefs( wxCommandEvent & event );
    void OnDNodeColor( wxCommandEvent & event );
    void OnTNodeColor( wxCommandEvent & event );
    void OnApply( wxCommandEvent & event );
    void OnCancel( wxCommandEvent & event );
    void OnOk( wxCommandEvent & event );
    void OnLineColor( wxCommandEvent & event );
    void OnBNodeSize( wxSpinEvent & event );
    void OnLineSize( wxSpinEvent & event );
    void OnFlynnShowNumbers( wxCommandEvent & event );
    void OnNodeShowNumbers( wxCommandEvent & event );
    void OnColorSingleFlynn( wxCommandEvent & event );
    void OnResetSingleFlynn( wxCommandEvent & event );
    void OnUnodes3d(wxCommandEvent & event);
    void OnRangeFlagUnodes( wxCommandEvent & event );
    void OnUnodesRangeFlagTxtMax( wxCommandEvent & event );
    void OnUnodesRangeFlagTxtMin( wxCommandEvent & event );
    void OnNotRangeFlagUnodes( wxCommandEvent & event );
    void OnUnodesNotRangeFlagTxtMax( wxCommandEvent & event );
    void OnUnodesNotRangeFlagTxtMin( wxCommandEvent & event );
    void OnUNodesTriangulate(wxCommandEvent &event);
    void OnUNodeSize( wxSpinEvent & event );
    void OnRainChange( wxSpinEvent & event );
    void OnPaint( wxPaintEvent & event );
    wxPanel * CreateUNodePage( void );
    wxPanel * CreateBNodePage( void );
    wxPanel * CreateLinePage( void );
    wxPanel * CreateFlynnPage( void );
    wxPanel * CreateCmapPage( void );
    wxPanel* ColormapCreateControls();
    void ColormapOnChangeColorPercent( wxCommandEvent & event );
    void ColormapOnDefault( wxCommandEvent & event );
    void ColormapUpdateColorDisp();
    void ColormapUploadCmap();
    void ColormapOnLoad( wxCommandEvent & event );
    void ColormapOnSave( wxCommandEvent & event );
    void ColormapResetToDef();
    void ColormapOnRangeFlynn(wxCommandEvent & event);
    void ColormapOnRangeBNode(wxCommandEvent & event);
    void ColormapOnRangeUNode(wxCommandEvent & event);
    void ColormapOnChangeColorValue(wxCommandEvent &event);
    void OnFCCMin( wxCommandEvent & event );
    void OnFCCMax( wxCommandEvent & event );
    void OnBCCMin( wxCommandEvent & event );
    void OnBCCMax( wxCommandEvent & event );
    void OnNCCMin( wxCommandEvent & event );
    void OnNCCMax( wxCommandEvent & event );
    void OnUCCMin( wxCommandEvent & event );
    void OnUCCMax( wxCommandEvent & event );
    void OnFClampColor( wxCommandEvent & event );
    void OnBClampColor( wxCommandEvent & event );
    void OnNClampColor( wxCommandEvent & event );
    void OnUClampColor( wxCommandEvent & event );
    wxPanel *glcan;
    wxBitmap cbit;
    bool first, triangulate,fclampcolor,bclampcolor,nclampcolor,uclampcolor;
    wxStaticText * fmin,*fmax;
    wxChoice * fselect,*bselect,*uselect;
    wxStaticText * min,*max;
    double fccmin,fccmax,bccmin,bccmax,nccmin,nccmax,uccmin,uccmax;
    DECLARE_EVENT_TABLE()
};


#endif //_E_preferences_h
