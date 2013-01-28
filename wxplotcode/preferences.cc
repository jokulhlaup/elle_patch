// Written in 2003
// Author: Dr. J.K. Becker
// Copyright: Dr. J.K. Becker (becker@jkbecker.de)

#include "preferences.h"
#include "dsettings.h"
#include "display.h"
#include "showelle.h"

extern DSettings * GetDisplayOptions();

IMPLEMENT_CLASS( Preferences, wxDialog )

BEGIN_EVENT_TABLE( Preferences, wxDialog )
EVT_BUTTON( ID_PApply, Preferences::OnApply )
EVT_BUTTON( ID_PCancel, Preferences::OnCancel )
EVT_BUTTON( ID_POk, Preferences::OnOk )
EVT_BUTTON( ID_PLoad, Preferences::OnLoadPrefs )
EVT_BUTTON( ID_PSave, Preferences::OnSavePrefs )
EVT_BUTTON( ID_PDColor, Preferences::OnDNodeColor )
EVT_BUTTON( ID_PLColor, Preferences::OnLineColor )
EVT_BUTTON( ID_PTColor, Preferences::OnTNodeColor )
EVT_SPINCTRL( ID_Pbnodesize, Preferences::OnBNodeSize )
EVT_SPINCTRL( ID_Plinesize, Preferences::OnLineSize )
EVT_CHECKBOX( ID_Pflynnshownumbers, Preferences::OnFlynnShowNumbers )
EVT_SPINCTRL( ID_Punodesize, Preferences::OnUNodeSize )
EVT_SPINCTRL( ID_Prainchange, Preferences::OnRainChange )
EVT_CHECKBOX( ID_Pnodeshownumbers, Preferences::OnNodeShowNumbers )
EVT_CHECKBOX( ID_Prangeflagunodes, Preferences::OnRangeFlagUnodes )
EVT_CLOSE( Preferences::OnClose )
// LE EVT_PAINT( Preferences::OnPaint )
EVT_CHECKBOX( ID_Pnotrangeflagunodes, Preferences::OnNotRangeFlagUnodes )
EVT_BUTTON( ID_Pcolorsingleflynn, Preferences::OnColorSingleFlynn )
EVT_BUTTON( ID_PResetSingleFlynnColor, Preferences::OnResetSingleFlynn )
EVT_BUTTON( ID_Unodes3d, Preferences::OnUnodes3d )
EVT_CHECKBOX(ID_Punodetriangles, Preferences::OnUNodesTriangulate )
EVT_TEXT( ID_punoderangeflagmaxval, Preferences::OnUnodesRangeFlagTxtMax )
EVT_TEXT( ID_punoderangeflagminval, Preferences::OnUnodesRangeFlagTxtMin )
EVT_TEXT( ID_punodenotrangeflagmaxval, Preferences::OnUnodesNotRangeFlagTxtMax )
EVT_LISTBOX( ID_pbnodelistbox, Preferences::OnListBox )
EVT_LISTBOX( ID_pflynnlistbox, Preferences::OnListBox )
EVT_LISTBOX( ID_punodelistbox, Preferences::OnListBox )
EVT_LISTBOX( ID_plinelistbox, Preferences::OnListBox )
EVT_TEXT( ID_punodenotrangeflagminval, Preferences::OnUnodesNotRangeFlagTxtMin )
EVT_TEXT_ENTER( ID_TEXTCTRL, Preferences::ColormapOnChangeColorPercent )
EVT_TEXT_ENTER( ID_CTEXT, Preferences::ColormapOnChangeColorValue )
EVT_BUTTON( ID_DEFAULT, Preferences::ColormapOnDefault )
EVT_BUTTON( ID_CLOAD, Preferences::ColormapOnLoad )
EVT_BUTTON( ID_CSAVE, Preferences::ColormapOnSave )
EVT_CHOICE( ID_sflynn, Preferences::ColormapOnRangeFlynn )
EVT_CHOICE( ID_sbnode, Preferences::ColormapOnRangeBNode )
EVT_CHOICE( ID_sunode, Preferences::ColormapOnRangeUNode )
EVT_CHECKBOX( ID_FClampColor, Preferences::OnFClampColor )
EVT_CHECKBOX( ID_BClampColor, Preferences::OnBClampColor )
EVT_CHECKBOX( ID_NClampColor, Preferences::OnNClampColor )
EVT_CHECKBOX( ID_UClampColor, Preferences::OnUClampColor )
EVT_TEXT( ID_FCCMin, Preferences::OnFCCMin )
EVT_TEXT( ID_FCCMax, Preferences::OnFCCMax)
EVT_TEXT( ID_BCCMin, Preferences::OnBCCMin )
EVT_TEXT( ID_BCCMax, Preferences::OnBCCMax)
EVT_TEXT( ID_NCCMin, Preferences::OnNCCMin )
EVT_TEXT( ID_NCCMax, Preferences::OnNCCMax)
EVT_TEXT( ID_UCCMin, Preferences::OnUCCMin )
EVT_TEXT( ID_UCCMax, Preferences::OnUCCMax)
END_EVENT_TABLE()


// Initialize everyting. Set all default values for colors and diameters and so on and show the
//preferences window. Oh, the buttons and stuff are defined here too!
Preferences::Preferences( wxWindow * parent ) :wxDialog( parent, PREFSWIN, "Preferences", wxPoint( 100, 100 ), wxSize( 500, 480 ),wxCAPTION | wxSYSTEM_MENU )
{
    wxMemoryDC bnodedc, linedc, tnodedc;
    DSettings * doptions = GetDisplayOptions();
    int r, g, b, tr, tg, tb;
    Settings * user_options = GetUserOptions();
    if(user_options->draw_nodes)
        triangulate=true;
    else
        triangulate=false;
    bnodebit.Create( 50, 50 );
    linebit.Create( 50, 50 );
    tnodebit.Create( 50, 50 );
    bnodedc.SelectObject( bnodebit );
    linedc.SelectObject( linebit );
    tnodedc.SelectObject( tnodebit );
    //get BNode stored values
    doptions->GetDNodeColor( & r, & g, & b );
    pbnodepen.SetColour( wxColour( r, g, b ) );
    pbnodepen.SetStyle( wxSOLID );
    pbnodepen.SetWidth( 1 );
    pbnodebrush.SetColour( wxColour( r, g, b ) );
    pbnodebrush.SetStyle( wxSOLID );
    nodesize = doptions->GetNodeSize();
    unodesize=doptions->GetUNodeSize();
    bnodedc.SetPen( pbnodepen );
    bnodedc.SetBrush( pbnodebrush );
    bnodedc.SetBackground( wxBrush( wxColour( 0, 0, 0 ), wxSOLID ) );

    bnodedc.Clear();

    bnodedc.BeginDrawing();
    bnodedc.DrawCircle( 25, 25, nodesize );
    bnodedc.EndDrawing();
    //getTNode values
    doptions->GetTNodeColor( & tr, & tg, & tb );
    ptnodepen.SetColour( wxColour( tr, tg, tb ) );
    ptnodebrush.SetColour( wxColour( tr, tg, tb ) );
    tnodedc.SetPen( ptnodepen );
    tnodedc.SetBrush( ptnodebrush );
    tnodedc.SetBackground( wxBrush( wxColour( 0, 0, 0 ), wxSOLID ) );
    tnodedc.Clear();
    tnodedc.BeginDrawing();
    tnodedc.DrawCircle( 25, 25, nodesize );
    tnodedc.EndDrawing();

    //Get stored values for the lines
    linedc.SetBackground( wxBrush( wxColour( 0, 0, 0 ), wxSOLID ) );
    linedc.Clear();
    doptions->GetLineColor( & r, & g, & b );
    plinepen.SetColour( wxColour( r, g, b ) );
    plinepen.SetStyle( wxSOLID );
    plinepen.SetWidth( doptions->GetLineSize() );
    linedc.SetPen( plinepen );
    linedc.BeginDrawing();
    linedc.DrawLine( 0, 25, 50, 25 );
    linedc.EndDrawing();

    attrib = doptions->GetShowArgs( FLYNNS );
    useunodea = doptions->GetShowArgs( UNODES );
    usebnodea = doptions->GetShowArgs( BNODES );
    rainchange=doptions->GetRainChange();
    urangevalmax = doptions->GetUnodesRangeFlagMaxValue();
    urangevalmin = doptions->GetUnodesRangeFlagMinValue();
    prangeflagunodes = doptions->GetUnodesRangeFlag();
    unotrangevalmax = doptions->GetUnodesNotRangeFlagMaxValue();
    unotrangevalmin = doptions->GetUnodesNotRangeFlagMinValue();;
    pnotrangeflagunodes = doptions->GetUnodesNotRangeFlag();
    unodes3d = false;
    //LE triangulate=false;

    fclampcolor=doptions->GetClampColor(0,&fccmin,&fccmax);
    bclampcolor=doptions->GetClampColor(1,&bccmin,&bccmax);
    nclampcolor=doptions->GetClampColor(2,&nccmin,&nccmax);
    uclampcolor=doptions->GetClampColor(3,&uccmin,&uccmax);

    // toolbar=CreateToolBar(wxNO_BORDER, -1, "ToolBar");
    cbit.Create(256,50);
    wxMemoryDC dc;
    dc.SelectObject(cbit);
    dc.SetBrush(wxBrush(wxColour(0,0,0),wxSOLID));
    dc.DrawRectangle(0,0,256,50);
    dc.SelectObject(wxNullBitmap);
    
    wxBoxSizer * base = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer * erster = new wxBoxSizer( wxHORIZONTAL );
    base->Add( erster, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    wxButton * ok = new wxButton( this, ID_POk, "OK", wxDefaultPosition, wxDefaultSize );
    wxButton * apply = new wxButton( this, ID_PApply, "Apply", wxDefaultPosition, wxDefaultSize );
    wxButton * cancel = new wxButton( this, ID_PCancel, "Cancel", wxDefaultPosition, wxDefaultSize );
    wxButton * save = new wxButton( this, ID_PSave, "Save", wxDefaultPosition, wxDefaultSize );
    wxButton * load = new wxButton( this, ID_PLoad, "Load", wxDefaultPosition, wxDefaultSize );
    erster->Add( ok, 0, wxALIGN_LEFT | wxALL, 5 );
    erster->Add( apply, 0, wxALIGN_LEFT | wxALL, 5 );
    erster->Add( cancel, 0, wxALIGN_LEFT | wxALL, 5 );
    erster->Add( save, 0, wxALIGN_LEFT | wxALL, 5 );
    erster->Add( load, 0, wxALIGN_LEFT | wxALL, 5 );
    wxBoxSizer * zweiter = new wxBoxSizer( wxHORIZONTAL );
    base->Add( zweiter, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    book = new wxNotebook( this, -1, wxDefaultPosition, wxSize(500,500) );
    wxPanel * p = ( wxPanel * ) NULL;
    p = CreateFlynnPage();
    book->AddPage( p, "Flynns", FALSE );
    p = CreateLinePage();
    book->AddPage( p, "Boundaries", FALSE );
    p = CreateBNodePage();
    book->AddPage( p, "BNodes", FALSE );
    p = CreateUNodePage();
    book->AddPage( p, "UNodes", FALSE );
    p=ColormapCreateControls();
    book->AddPage( p, "Colormap", FALSE );
    zweiter->Add( book, 0, wxALIGN_LEFT | wxALL, 5 );
    this->SetSizer( base );
    this->Fit();
    this->SetAutoLayout( TRUE );
    this->Refresh( true, NULL ); //LE
}

/* LE
void Preferences::OnPaint( wxPaintEvent & event )
{
    wxPaintDC dc( this );
    ColormapUpdateColorDisp();
}
*/

void Preferences::OnListBox( wxCommandEvent & event )
{
    int in;
    wxString attrib;
    double max=0, min=0;

    // LE
    /*
     * This fn is called by all the attribute list boxes
     * if an attribute is chosen, the min max revert
     * to the range in the file
     */
    switch(event.GetId())
    {
    case ID_pbnodelistbox:
        if ( ( in = cbnode->GetSelection() ) != 0 )
        {
            attrib = cbnode->GetString( in );
		in = GetAttributeInt( attrib,BNODES );

            ElleFindNodeAttributeRange( in, & min, & max );
            attrib = "";
            attrib.Printf( "%lf", max );
            Nccmax->SetValue( attrib );
            attrib = "";
            attrib.Printf( "%lf", min );
            Nccmin->SetValue( attrib );
        }
        break;
    case ID_pflynnlistbox:
        if ( ( in = cflynn->GetSelection() ) != 0 )
        {
            attrib = cflynn->GetString( in );
		in = GetAttributeInt( attrib,FLYNNS );

            ElleFindFlynnAttributeRange( in, & min, & max );
            attrib = "";
            attrib.Printf( "%lf", max );
            Fccmax->SetValue( attrib );
            attrib = "";
            attrib.Printf( "%lf", min );
            Fccmin->SetValue( attrib );
        }
        break;
    case ID_punodelistbox:
        if ( ( in = cunode->GetSelection() ) != 0 )
        {
            attrib = cunode->GetString( in );
        in = GetAttributeInt( attrib,UNODES );

            ElleFindUnodeAttributeRange( in, & min, & max );
            attrib = "";
            attrib.Printf( "%lf", max );
            uvalmax->SetValue( attrib );
            Uccmax->SetValue( attrib );
            attrib = "";
            attrib.Printf( "%lf", min );
            uvalmin->SetValue( attrib );
            Uccmin->SetValue( attrib );
        }
        break;
    case ID_plinelistbox:
        if ( ( in = cline->GetSelection() ) != 0 )
        {
            attrib = cline->GetString( in );
		in = GetAttributeInt( attrib,BOUNDARIES );

            ElleFindBndAttributeRange( in, & min, & max );
            attrib = "";
            attrib.Printf( "%lf", max );
            Bccmax->SetValue( attrib );
            attrib = "";
            attrib.Printf( "%lf", min );
            Bccmin->SetValue( attrib );
        }
        break;
    default: break;
    }
}

void Preferences::OnLoadPrefs( wxCommandEvent & event )
{
    DSettings * doptions = GetDisplayOptions();
    doptions->LoadDSettings();
    if(doptions->LoadDSettings())
    {
        ColormapUpdateColorDisp();
        EllePlotRegions( 0 );
        wxMessageBox( "Preferences loaded successfully!", "This is an important message!", wxOK | wxICON_INFORMATION );
        Show(false);
    }
}

void Preferences::OnSavePrefs( wxCommandEvent & event )
{
    DSettings * doptions = GetDisplayOptions();
    doptions->SaveDSettings();
}

void Preferences::OnClose(wxCloseEvent &event)
{
    Destroy();
}

wxPanel * Preferences::CreateUNodePage( void )
{
    wxPanel * pppanel = new wxPanel( book );
    wxString mm;
    int x, count, * welche=0;
    wxStaticText * stext = new wxStaticText( pppanel, -1, "UNode size" );
    mm.Printf( "%d", unodesize );
    wxSpinCtrl * unodespin = new wxSpinCtrl( pppanel, ID_Punodesize, mm, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 2 );
    cunode = new wxListBox( pppanel, ID_punodelistbox, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    ///////////////////////////////////////////////////////
    //append attributes to list selector for unodes
    ElleUnodeAttributeList( & welche, & count );
    cunode->Append( "NONE" );
    cunode->Append( "LOCATION" );
    for ( x = 0; x < count; x++ )
    {
		cunode->Append( GetAttributeName( welche[x],UNODES ) );
    }
	wxString name = GetAttributeName( useunodea,UNODES );
    int number = cunode->FindString( name );
    cunode->SetSelection( number, true );
    if (welche) free( welche );
    ///////////////////////////////////////////////////////////
    wxCheckBox * utriangle = new wxCheckBox( pppanel, ID_Punodetriangles, "Triangulate unodes", wxDefaultPosition, wxDefaultSize );
    utriangle->SetValue( triangulate );
    //wxButton * single = new wxButton( pppanel, ID_Unodes3d, "Show UNode-surface", wxDefaultPosition, wxDefaultSize );
    wxBoxSizer * fuenfter = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * unflag = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * left = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * left1 = new wxBoxSizer( wxVERTICAL );
    //show only unodes with between these values
    wxCheckBox * flagunodes =new wxCheckBox( pppanel, ID_Prangeflagunodes, "Only draw unodes with values between", wxDefaultPosition, wxDefaultSize );
    flagunodes->SetValue( prangeflagunodes );
    mm = "";
    mm.Printf( "%lf", urangevalmin );
    uvalmin = new wxTextCtrl( pppanel, ID_punoderangeflagminval, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    uvalmin->SetValue( mm );
    mm = "";
    mm.Printf( "%lf", urangevalmax );
    uvalmax = new wxTextCtrl( pppanel, ID_punoderangeflagmaxval, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    uvalmax->SetValue( mm );
    wxStaticText * tx = new wxStaticText( pppanel, -1, "and" );

    unflag->Add( uvalmin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    unflag->Add( tx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    unflag->Add( uvalmax, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    //do NOT show unodes with between these values
    wxBoxSizer * notunflag = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox * notflagunodes =new wxCheckBox( pppanel, ID_Pnotrangeflagunodes, "Do NOT draw unodes with values between",wxDefaultPosition, wxDefaultSize );
    notflagunodes->SetValue( pnotrangeflagunodes );
    mm = "";
    mm.Printf( "%lf", unotrangevalmin );
    wxTextCtrl * notuvalmin =new wxTextCtrl( pppanel, ID_punodenotrangeflagminval, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    notuvalmin->SetValue( mm );
    mm = "";
    mm.Printf( "%lf", unotrangevalmax );
    wxTextCtrl * notuvalmax =new wxTextCtrl( pppanel, ID_punodenotrangeflagmaxval, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    notuvalmax->SetValue( mm );
    wxStaticText * nottx = new wxStaticText( pppanel, -1, "and" );

    notunflag->Add( notuvalmin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    notunflag->Add( nottx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    notunflag->Add( notuvalmax, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    left1->Add( stext, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    left1->Add( unodespin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    left->Add( left1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    left->Add( cunode, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( left, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( utriangle, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    //fuenfter->Add( single, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( flagunodes, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( unflag, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( notflagunodes, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( notunflag, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxBoxSizer * v2 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox * ccolor =new wxCheckBox( pppanel, ID_UClampColor, "Clamp color between", wxDefaultPosition, wxDefaultSize );
    ccolor->SetValue( uclampcolor );
    Uccmin = new wxTextCtrl( pppanel, ID_UCCMin, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",uccmin);
    Uccmin->SetValue( mm );
    Uccmax = new wxTextCtrl( pppanel, ID_UCCMax, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",uccmax);
    Uccmax->SetValue( mm );
    wxStaticText * tx1 = new wxStaticText( pppanel, -1, "and" );

    v2->Add( ccolor, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Uccmin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( tx1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Uccmax, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fuenfter->Add( v2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    pppanel->SetSizer( fuenfter );
    return pppanel;
}

void Preferences::OnUNodesTriangulate(wxCommandEvent &event)
{
    triangulate = event.IsChecked();
}

void Preferences::OnUnodesRangeFlagTxtMax( wxCommandEvent & event )
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & urangevalmax );
}

void Preferences::OnUnodesRangeFlagTxtMin( wxCommandEvent & event )
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & urangevalmin );
}

void Preferences::OnUnodesNotRangeFlagTxtMax( wxCommandEvent & event )
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & unotrangevalmax );
}

void Preferences::OnUnodesNotRangeFlagTxtMin( wxCommandEvent & event )
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & unotrangevalmin );
}

void Preferences::OnUnodes3d(wxCommandEvent & event)
{
    unodes3d = true;
    DSettings * doptions = GetDisplayOptions();
    useunodea = GetAttributeInt( cunode->GetStringSelection(),UNODES );
}

wxPanel * Preferences::CreateFlynnPage( void )
{

    wxPanel * fpanel = new wxPanel( book );
    wxBoxSizer * vierter = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * v1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * v2 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox * pfshownumbers = new wxCheckBox( fpanel, ID_Pflynnshownumbers, "Show Numbers", wxDefaultPosition, wxDefaultSize );
    pfshownumbers->SetValue( false );
    pflynnshownumbers = false;
    wxButton * single = new wxButton( fpanel, ID_Pcolorsingleflynn, "Color Single\nFlynn", wxDefaultPosition, wxDefaultSize );
    wxButton * rsingle = new
                         wxButton( fpanel, ID_PResetSingleFlynnColor, "Reset Single\nFlynn", wxDefaultPosition, wxDefaultSize );
    cflynn = new wxListBox( fpanel, ID_pflynnlistbox, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    //////////////////////////////////////////////////////////
    int x, count, * welche=0;
    ElleFlynnDfltAttributeList( & welche, & count );
    cflynn->Append( "NONE" );
    for ( x = 0; x < count; x++ )
    {
		cflynn->Append( GetAttributeName( welche[x],FLYNNS ) );
    }
	wxString name = GetAttributeName( attrib,FLYNNS );
    int number = cflynn->FindString( name );
    cflynn->SetSelection( number, true );
    if (welche) free( welche );
    ////////////////////////////////////////////////////////////
    wxString mm;
    wxCheckBox * ccolor =new wxCheckBox( fpanel, ID_FClampColor, "Clamp color between", wxDefaultPosition, wxDefaultSize );
    ccolor->SetValue( fclampcolor );
    Fccmin = new wxTextCtrl( fpanel, ID_FCCMin, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",fccmin);
    Fccmin->SetValue( mm );
    Fccmax = new wxTextCtrl( fpanel, ID_FCCMax, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",fccmax);
    Fccmax->SetValue( mm );
    wxStaticText * tx = new wxStaticText( fpanel, -1, "and" );

    v2->Add( ccolor, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Fccmin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( tx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Fccmax, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    vierter->Add( pfshownumbers, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    vierter->Add( cflynn, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v1->Add( single, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v1->Add( rsingle, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    vierter->Add( v1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    vierter->Add( v2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    fpanel->SetSizer( vierter );
    return fpanel;
}

wxPanel * Preferences::CreateLinePage( void )
{
    DSettings * doptions = GetDisplayOptions();
    wxPanel * lpanel = new wxPanel( book );

    wxBoxSizer * dritter = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * v2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * v1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * d1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * d2 = new wxBoxSizer( wxHORIZONTAL );
    dritter->Add( d2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    d2->Add( d1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    wxButton * linecolor = new wxButton( lpanel, ID_PLColor, "Line-Color", wxDefaultPosition, wxDefaultSize );
    wxStaticBitmap * linebut = new wxStaticBitmap( lpanel, -1, linebit, wxDefaultPosition, wxDefaultSize );
    wxString ls;
    ls.Printf( "%d", doptions->GetLineSize() );
    wxStaticText * linetx = new wxStaticText( lpanel, -1, "Line width" );
    wxSpinCtrl * linespin = new wxSpinCtrl( lpanel, ID_Plinesize, ls, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 2 );
    d1->Add( linecolor, 0, wxALIGN_LEFT | wxALL, 5 );
    d2->Add( linebut, 0, wxALIGN_LEFT | wxALL, 5 );
    d1->Add( linetx, 0, wxALIGN_LEFT | wxALL, 5 );
    d1->Add( linespin, 0, wxALIGN_LEFT | wxALL, 5 );

    cline = new wxListBox( lpanel, ID_plinelistbox, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    v1->Add( cline, 0, wxALIGN_LEFT | wxALL, 5 );
    cline->Append( "NONE" );
    cline->Append( "STANDARD" );
    cline->Append( "MISORIENTATION" );
    cline->Append( "RAINBOW" );
	wxString name = GetAttributeName( doptions->GetShowArgs( BOUNDARIES),BOUNDARIES );
    int number = cline->FindString( name );
    cline->SetSelection( number, true );
    if ( doptions->GetShowArgs( BOUNDARIES ) == NONE )
        plineshow = false;
    else
        plineshow = true;
    linebut->SetBitmap( linebit );
    wxString mm;
    wxStaticText * raintx = new wxStaticText( lpanel, -1, "Change line color every xth stage" );
    mm.Printf( "%d", rainchange );
    wxSpinCtrl *rchange = new wxSpinCtrl( lpanel, ID_Prainchange, mm, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 10 );
    v1->Add( raintx, 0, wxALIGN_LEFT | wxALL, 5 );
    v1->Add( rchange, 0, wxALIGN_LEFT | wxALL, 5 );

    wxCheckBox * ccolor =new wxCheckBox( lpanel, ID_BClampColor, "Clamp color between", wxDefaultPosition, wxDefaultSize );
    ccolor->SetValue( bclampcolor );
    Bccmin = new wxTextCtrl( lpanel, ID_BCCMin, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",bccmin);
    Bccmin->SetValue( mm );
    Bccmax = new wxTextCtrl( lpanel, ID_BCCMax, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",bccmax);
    Bccmax->SetValue( mm );
    wxStaticText * tx = new wxStaticText( lpanel, -1, "and" );

    v2->Add( ccolor, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Bccmin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( tx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Bccmax, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    dritter->Add( v1, 0, wxALIGN_LEFT | wxALL, 5 );
    dritter->Add( v2, 0, wxALIGN_LEFT | wxALL, 5 );

    lpanel->SetSizer( dritter );

    return lpanel;
}

wxPanel * Preferences::CreateBNodePage( void )
{
    wxString mm;
    wxPanel * bpanel = new wxPanel( book );

    wxBoxSizer * zweiter = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * z2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * z1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * v3 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer * v1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * v2 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBitmap * bnodebut = new wxStaticBitmap( bpanel, -1, bnodebit, wxPoint( 100, 60 ), wxSize( 50, 50 ) );
    z2->Add( bnodebut, 0, wxALIGN_LEFT | wxALL, 5 );
    wxStaticBitmap * tnodebut = new wxStaticBitmap( bpanel, -1, tnodebit, wxPoint( 100, 160 ), wxSize( 50, 50 ) );
    z2->Add( tnodebut, 0, wxALIGN_LEFT | wxALL, 5 );
    wxButton * dcolor = new wxButton( bpanel, ID_PDColor, "D-Node-Color", wxDefaultPosition, wxDefaultSize );
    z1->Add( dcolor, 0, wxALIGN_LEFT | wxALL, 5 );
    wxButton * tcolor = new wxButton( bpanel, ID_PTColor, "T-Node-Color", wxDefaultPosition, wxDefaultSize );
    z1->Add( tcolor, 0, wxALIGN_LEFT | wxALL, 5 );
    mm.Printf( "%d", nodesize );
    wxSpinCtrl * bnodespin = new wxSpinCtrl( bpanel, ID_Pbnodesize, mm, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 2 );
    z1->Add( bnodespin, 0, wxALIGN_LEFT | wxALL, 5 );
    wxCheckBox * bnumshow = new wxCheckBox( bpanel, ID_Pnodeshownumbers, "Show Nodenumbers", wxDefaultPosition, wxDefaultSize );
    z1->Add( bnumshow, 0, wxALIGN_LEFT | wxALL, 5 );
    bnodebut->SetBitmap( bnodebit );
    tnodebut->SetBitmap( tnodebit );
    pnodeshownumbers = false;
    bnumshow->SetValue( false );
    cbnode = new wxListBox( bpanel, ID_pbnodelistbox, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    v3->Add( cbnode, 0, wxALIGN_LEFT | wxALL, 5 );
    ///////////////////////////////////////////////////////
    int x, count, * welche=0;
    //append attributes to list selector for unodes
    ElleNodeAttributeList( & welche, & count );
    cbnode->Append( "NONE" );
    cbnode->Append( "NEIGHBOURS" );
    cbnode->Append( "TRIPLE" );
    for ( x = 0; x < count; x++ )
    {
		cbnode->Append( GetAttributeName( welche[x],BNODES ) );
	}
	wxString name = GetAttributeName( usebnodea,BNODES );
    int number = cbnode->FindString( name );
    cbnode->SetSelection( number, true );
    if (welche) free( welche );
    ///////////////////////////////////////////////////////////
    wxCheckBox * ccolor =new wxCheckBox( bpanel, ID_NClampColor, "Clamp color between", wxDefaultPosition, wxDefaultSize );
    ccolor->SetValue( nclampcolor );
    Nccmin = new wxTextCtrl( bpanel, ID_NCCMin, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",nccmin);
    Nccmin->SetValue( mm );
    Nccmax = new wxTextCtrl( bpanel, ID_NCCMax, mm, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    mm.Printf("%lf",nccmax);
    Nccmax->SetValue( mm );
    wxStaticText * tx = new wxStaticText( bpanel, -1, "and" );

    v2->Add( ccolor, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Nccmin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( tx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v2->Add( Nccmax, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    v1->Add( z1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    v1->Add( z2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    zweiter->Add( v1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    zweiter->Add( v3, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );
    zweiter->Add( v2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    bpanel->SetSizer( zweiter );

    return bpanel;
}

void Preferences::OnColorSingleFlynn( wxCommandEvent & event )
{
    wxTextEntryDialog * count = new wxTextEntryDialog( this, "Please enter number of Flynn", "HEY THERE!" );
    wxColour col;
    count->ShowModal();
    GetUserColor(&col);
    if(col.Ok())
    {
        ElleFlynnSetColor( atoi( count->GetValue() ), true, col.Red(), col.Green(), col.Blue() );
        EllePlotRegions( 0 );
    }
}

void Preferences::OnResetSingleFlynn( wxCommandEvent & event )
{
    wxTextEntryDialog * count = new wxTextEntryDialog( this, "Please enter number of Flynn", "HEY THERE!" );
    count->ShowModal();
    ElleFlynnSetColor( atoi( count->GetValue() ), false, 0, 0, 0 );
    EllePlotRegions( 0 );
}

void Preferences::OnRangeFlagUnodes( wxCommandEvent & event )
{
    //The user wants to see the flagged unodes or not
    prangeflagunodes = event.IsChecked();
}

void Preferences::OnNotRangeFlagUnodes( wxCommandEvent & event )
{
    //The user wants to see the flagged unodes or not
    pnotrangeflagunodes = event.IsChecked();
}

void Preferences::OnFlynnShowNumbers( wxCommandEvent & event )
{
    //The user wants to see the numbers, or not
    pflynnshownumbers = event.IsChecked();
}

void Preferences::OnNodeShowNumbers( wxCommandEvent & event )
{
    //The user wants to see the numbers, or not
    pnodeshownumbers = event.IsChecked();
}

void Preferences::OnCancel( wxCommandEvent & event )
{
    GetParent()->Update();
    Show(false);
    Close(true);
}

void Preferences::OnLineSize( wxSpinEvent & event )
{
    //The user changed the thickness of the lines
    wxMemoryDC linedc;
    linedc.SelectObject( linebit );
    linedc.SetBackground( wxBrush( wxColour( 0, 0, 0 ), wxSOLID ) );
    linedc.Clear();
    plinepen.SetWidth( event.GetPosition() );
    linedc.SetPen( plinepen );
    linedc.BeginDrawing();
    linedc.DrawLine( 0, 25, 50, 25 );
    linedc.EndDrawing();
    Refresh( true, NULL );
}

void Preferences::OnLineColor( wxCommandEvent & event )
{
    wxColour col;
    //the color of the lines should be changed, let's open a color diolog
    GetUserColor(&col);
    if(col.Ok())
    {
        wxMemoryDC dc;
        plinepen.SetColour( col );
        dc.SelectObject( linebit );
        dc.BeginDrawing();
        dc.SetPen( plinepen );
        dc.DrawLine( 0, 25, 50, 25 );
        dc.EndDrawing();
        Refresh( true, NULL );
    }
}

void Preferences::OnUNodeSize( wxSpinEvent & event )
{
    unodesize = event.GetPosition();
}

void Preferences::OnRainChange( wxSpinEvent & event )
{
    rainchange = event.GetPosition();
}

void Preferences::OnBNodeSize( wxSpinEvent & event )
{
    //The user has changed the diameter of the nodes
    wxMemoryDC bnodedc, tnodedc;

    bnodedc.SelectObject( bnodebit );
    bnodedc.SetBackground( wxBrush( wxColour( 0, 0, 0 ), wxSOLID ) );
    bnodedc.Clear();
    bnodedc.SetPen( pbnodepen );
    bnodedc.SetBrush( pbnodebrush );
    bnodedc.BeginDrawing();
    nodesize = event.GetPosition();
    bnodedc.DrawCircle( 25, 25, nodesize );
    bnodedc.EndDrawing();

    tnodedc.SelectObject( tnodebit );
    tnodedc.SetBackground( wxBrush( wxColour( 0, 0, 0 ), wxSOLID ) );
    tnodedc.Clear();
    tnodedc.SetPen( ptnodepen );
    tnodedc.SetBrush( ptnodebrush );
    tnodedc.BeginDrawing();
    tnodedc.DrawCircle( 25, 25, nodesize );
    tnodedc.EndDrawing();
    Refresh( true, NULL );
}

void Preferences::OnApply( wxCommandEvent & event )
{
    DSettings * doptions = GetDisplayOptions();
    //get the value of the attribs
    wxColor col;
    int size;
    size = plinepen.GetWidth();
    col = plinepen.GetColour();
    doptions->SetLineColor( col.Red(), col.Green(), col.Blue() );
    doptions->SetLineSize( size );
    doptions->SetNodeSize( nodesize );
    doptions->SetUNodeSize( unodesize );
    col = pbnodepen.GetColour();
    doptions->SetDNodeColor( col.Red(), col.Green(), col.Blue() );
    col = ptnodepen.GetColour();
    doptions->SetTNodeColor( col.Red(), col.Green(), col.Blue() );
    doptions->SetUnodesRangeFlagMaxValue( urangevalmax );
    doptions->SetUnodesRangeFlagMinValue( urangevalmin );
    doptions->SetUnodesRangeFlag( prangeflagunodes );
    doptions->SetUnodesNotRangeFlagMaxValue( unotrangevalmax );
    doptions->SetUnodesNotRangeFlagMinValue( unotrangevalmin );
    doptions->SetUnodesNotRangeFlag( pnotrangeflagunodes );
	useunodea = GetAttributeInt( cunode->GetStringSelection(),UNODES );
	usebnodea = GetAttributeInt( cbnode->GetStringSelection(),BNODES );
	attrib = GetAttributeInt( cflynn->GetStringSelection(),FLYNNS );

    Settings * user_options = GetUserOptions();
    if(triangulate)
        user_options->draw_nodes=1;
    else
        user_options->draw_nodes=0;

    if ( attrib != NONE )
        doptions->SetShowArgs( FLYNNS, true, attrib );
    else
        doptions->SetShowArgs( FLYNNS, false, attrib );

	battrib = GetAttributeInt( cline->GetStringSelection(),BOUNDARIES );
    if ( battrib != NONE )
    {
        doptions->SetShowArgs( BOUNDARIES, true, battrib );
        if(battrib==RAINBOW)
            doptions->SetRainStages(ElleCount());
    }
    else
        doptions->SetShowArgs( BOUNDARIES, false, attrib );
    if ( usebnodea != NONE )
        doptions->SetShowArgs( BNODES, true, usebnodea );
    else
        doptions->SetShowArgs( BNODES, false, usebnodea );

    if ( useunodea != NONE )
    {
        if ( useunodea == LOCATION )
            useunodea = U_LOCATION;
        doptions->SetShowArgs( UNODES, true, useunodea );
    }
    else
        doptions->SetShowArgs( UNODES, false, useunodea );
    //( ( Canvas * ) GetParent() )->ClearScreen();
    if ( unodes3d == true )
    {
        unodes3d = false;
        ( ( Canvas * ) GetParent() )->ResetScreen();
    }
    doptions->ShowFlynnNumbers( pflynnshownumbers );
    doptions->ShowNodeNumbers( pnodeshownumbers );
    doptions->SetRainChange(rainchange);

    if(fclampcolor && attrib!=NONE)
    {
        doptions->SetClampColor(0,true,fccmin,fccmax);
        user_options->SetFlynnAttribOption(attrib,fccmin,fccmax);
    }
    else
        doptions->SetClampColor(0,false,fccmin,fccmax);

    if(bclampcolor && battrib!=NONE && battrib!=RAINBOW)
    {
        doptions->SetClampColor(1,true,bccmin,bccmax);
        user_options->SetBndAttribOption(battrib,bccmin,bccmax);
    }
    else
        doptions->SetClampColor(1,false,bccmin,bccmax);

    if(nclampcolor &&usebnodea!=NONE)
    {
        doptions->SetClampColor(2,true,nccmin,nccmax);
        user_options->SetNodeAttribOption(usebnodea,nccmin,nccmax);
    }
    else
        doptions->SetClampColor(2,false,nccmin,nccmax);

    if(uclampcolor && useunodea!=NONE && useunodea!=U_LOCATION && useunodea!=LOCATION)
    {
        doptions->SetClampColor(3,true,uccmin,uccmax);
        user_options->SetUnodeAttribOption(useunodea,uccmin,uccmax);
    }
    else
        doptions->SetClampColor(3,false,uccmin,uccmax);

    EllePlotRegions( 0 );
    //GetParent()->Refresh( true, NULL );
}

void Preferences::OnOk( wxCommandEvent & event )
{
    DSettings * doptions = GetDisplayOptions();
    wxColor col;
    int size;
    size = plinepen.GetWidth();
    col = plinepen.GetColour();
    doptions->SetLineColor( col.Red(), col.Green(), col.Blue() );
    doptions->SetLineSize( size );
    doptions->SetNodeSize( nodesize );
    doptions->SetUNodeSize( unodesize );
    col = pbnodepen.GetColour();
    doptions->SetDNodeColor( col.Red(), col.Green(), col.Blue() );
    col = ptnodepen.GetColour();
    doptions->SetTNodeColor( col.Red(), col.Green(), col.Blue() );

    doptions->SetUnodesRangeFlagMinValue( urangevalmin );
    doptions->SetUnodesRangeFlagMaxValue( urangevalmax );
    doptions->SetUnodesRangeFlag( prangeflagunodes );

    doptions->SetUnodesNotRangeFlagMaxValue( unotrangevalmax );
    doptions->SetUnodesNotRangeFlagMinValue( unotrangevalmin );
    doptions->SetUnodesNotRangeFlag( pnotrangeflagunodes );

    doptions->SetRainChange(rainchange);
    Settings * user_options = GetUserOptions();
    if(triangulate)
        user_options->draw_nodes=1;
    else
        user_options->draw_nodes=0;
	attrib = GetAttributeInt( cflynn->GetStringSelection(),FLYNNS );
	useunodea = GetAttributeInt( cunode->GetStringSelection(),UNODES );
	usebnodea = GetAttributeInt( cbnode->GetStringSelection(),BNODES );
    if ( attrib != NONE )
        doptions->SetShowArgs( FLYNNS, true, attrib );
    else
        doptions->SetShowArgs( FLYNNS, false, attrib );

	battrib = GetAttributeInt( cline->GetStringSelection(),BOUNDARIES );
    if ( battrib != NONE )
    {
        doptions->SetShowArgs( BOUNDARIES, true, battrib );
        if(battrib==RAINBOW)
            doptions->SetRainStages(ElleCount());
    }
    else
        doptions->SetShowArgs( BOUNDARIES, false, battrib );

    if ( usebnodea != NONE )
        doptions->SetShowArgs( BNODES, true, usebnodea );
    else
        doptions->SetShowArgs( BNODES, false, usebnodea );

    if ( useunodea != NONE )
    {
        if ( useunodea == LOCATION )
            useunodea = U_LOCATION;

        doptions->SetShowArgs( UNODES, true, useunodea );
    }
    else
        doptions->SetShowArgs( UNODES, false, useunodea );
    //( ( Canvas * ) GetParent() )->ClearScreen();
    if ( unodes3d == true )
    {
        unodes3d = false;
        ( ( Canvas * ) GetParent() )->ResetScreen();
    }
    doptions->ShowFlynnNumbers( pflynnshownumbers );
    doptions->ShowNodeNumbers( pnodeshownumbers );

    if(fclampcolor && attrib!=NONE)
    {
        doptions->SetClampColor(0,true,fccmin,fccmax);
        user_options->SetFlynnAttribOption(attrib,fccmin,fccmax);
    }
    else
        doptions->SetClampColor(0,false,fccmin,fccmax);

    if(bclampcolor && battrib!=NONE && battrib!=RAINBOW)
    {
        doptions->SetClampColor(1,true,bccmin,bccmax);
        user_options->SetBndAttribOption(battrib,bccmin,bccmax);
    }
    else
        doptions->SetClampColor(1,false,bccmin,bccmax);

    if(nclampcolor &&usebnodea!=NONE)
    {
        doptions->SetClampColor(2,true,nccmin,nccmax);
        user_options->SetNodeAttribOption(usebnodea,nccmin,nccmax);
    }
    else
        doptions->SetClampColor(2,false,nccmin,nccmax);

    if(uclampcolor && useunodea!=NONE && useunodea!=U_LOCATION && useunodea!=LOCATION)
    {
        doptions->SetClampColor(3,true,uccmin,uccmax);
        user_options->SetUnodeAttribOption(useunodea,uccmin,uccmax);
    }
    else
        doptions->SetClampColor(3,false,uccmin,uccmax);

    //( ( ShowelleFrame * ) GetParent())->Refresh( true, NULL );
    EllePlotRegions( 0 );
    Show(false);
    Close(true);
}

void Preferences::OnDNodeColor( wxCommandEvent & event )
{
    wxColourDialog dia( this );
    wxColour col;
    GetUserColor(&col);
    if(col.Ok())
    {
        wxMemoryDC bnodedc;
        pbnodepen.SetColour( col );
        pbnodebrush.SetColour( col );
        bnodedc.SelectObject( bnodebit );
        bnodedc.BeginDrawing();
        bnodedc.SetPen( pbnodepen );
        bnodedc.SetBrush( pbnodebrush );
        bnodedc.DrawCircle( 25, 25, nodesize );
        bnodedc.EndDrawing();
        Refresh( true, NULL );
    }
}

void Preferences::OnTNodeColor( wxCommandEvent & event )
{
    wxColourDialog dia( this );
    wxColour col;
    GetUserColor(&col);
    if(col.Ok())
    {
        wxMemoryDC tnodedc;
        ptnodepen.SetColour( col );
        ptnodebrush.SetColour( col );
        tnodedc.SelectObject( tnodebit );
        tnodedc.BeginDrawing();
        tnodedc.SetPen( ptnodepen );
        tnodedc.SetBrush( ptnodebrush );
        tnodedc.DrawCircle( 25, 25, nodesize );
        tnodedc.EndDrawing();
        Refresh( true, NULL );
    }
}

wxString Preferences::GetAttributeName( int welche, int type )
{
    char *clabel = new char[MAX_OPTION_NAME];
    bool valid = false;
    clabel[0] = '\0';
    switch(type) {
    case FLYNNS: if (id_match(region_terms,welche,clabel))
                   valid = true;
                 break;
    case BOUNDARIES: if (id_match(boundary_terms,welche,clabel))
                   valid = true;
                 break;
    case BNODES: if (id_match(node_terms,welche,clabel))
                   valid = true;
                 break;
    case UNODES: if (id_match(unode_terms,welche,clabel))
                   valid = true;
                 break;
    default:     break;
    }
    wxString label = clabel;
    delete clabel;
    return(label);
}

int Preferences::GetAttributeInt( wxString welche,int type )
{
	int id = -1;
    switch(type) {
    case FLYNNS:
	id = name_match( ( ( char * ) welche.c_str() ), region_terms );
		break;
    case BOUNDARIES:
	id = name_match( ( ( char * ) welche.c_str() ), boundary_terms );
		break;
    case BNODES:
	id = name_match( ( ( char * ) welche.c_str() ), node_terms );
		break;
    case UNODES:
	id = name_match( ( ( char * ) welche.c_str() ), unode_terms );
		break;
    default:     break;
    }
#if XY
	id = name_match( ( ( char * ) welche.c_str() ), FileKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), BoundaryKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), MineralKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), VelocityKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), StressKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), StrainKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), FiniteStrainKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), FlynnAgeKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), FlynnStrainKeys );
	if ( id == -1 )
		id = name_match( ( ( char * ) welche.c_str() ), Leftovers );
#endif
	return ( id );
}


void Preferences::ColormapOnRangeFlynn( wxCommandEvent & event )
{
    int attr;
    double max, min;
    wxString attrib;
    bselect->SetSelection( 0 );
    uselect->SetSelection( 0 );
    if ( ( attr = fselect->GetSelection() ) != 0 )
    {
        attrib = fselect->GetString( attr );
		attr = GetAttributeInt( attrib,FLYNNS );
        ElleFindFlynnAttributeRange( attr, & min, & max );
        attrib = "";
        attrib.Printf( "%lf", max );
        fmax->SetLabel( attrib );
        attrib = "";
        attrib.Printf( "%lf", min );
        fmin->SetLabel( attrib );
    }
    else
    {
        fmin->SetLabel( "             " );
        fmax->SetLabel( "          " );
    }
}

void Preferences::ColormapOnRangeBNode( wxCommandEvent & event )
{
    int attr;
    double max, min;
    wxString attrib;

    fselect->SetSelection( 0 );
    uselect->SetSelection( 0 );
    if ( ( attr = bselect->GetSelection() ) != 0 )
    {
        attrib = bselect->GetString( attr );
		attr = GetAttributeInt( attrib,BOUNDARIES );

        ElleFindBndAttributeRange( attr, & min, & max );
        attrib = "";
        attrib.Printf( "%lf", max );
        fmax->SetLabel( attrib );
        attrib = "";
        attrib.Printf( "%lf", min );
        fmin->SetLabel( attrib );
    }
    else
    {
        fmin->SetLabel( "             " );
        fmax->SetLabel( "          " );
    }

}

void Preferences::ColormapOnRangeUNode( wxCommandEvent & event )
{
    int attr;
    double max, min;
    wxString attrib;

    bselect->SetSelection( 0 );
    fselect->SetSelection( 0 );
    if ( ( attr = uselect->GetSelection() ) != 0 )
    {
        attrib = uselect->GetString( attr );
		attr = GetAttributeInt( attrib,UNODES );

        ElleFindUnodeAttributeRange( attr, & min, & max );
        attrib = "";
        attrib.Printf( "%lf", max );
        fmax->SetLabel( attrib );
        attrib = "";
        attrib.Printf( "%lf", min );
        fmin->SetLabel( attrib );
    }
    else
    {
        fmin->SetLabel( "             " );
        fmax->SetLabel( "          " );
    }

}

void Preferences::ColormapOnLoad( wxCommandEvent & event )
{
    string filename;
    DSettings * doptions = GetDisplayOptions();
    filename = wxFileSelector( "Choose Colormap-File", "", "", "", "Colormap-Files  (*.cmap)|*.cmap|All Files (*)|*",wxOPEN );
    doptions->CmapLoad( filename );
    ColormapUpdateColorDisp();
    // LE Refresh( true, NULL );
}

void Preferences::ColormapOnSave( wxCommandEvent & event )
{
    string filename;
    DSettings * doptions = GetDisplayOptions();
    filename = wxFileSelector( "Choose filename to save Colormap", "", "", "",
                               "Colormap-Files (*.cmap)|*.cmap|All Files (*)|*",wxSAVE );
    doptions->CmapSave( filename );
    Refresh( true, NULL );
}

void Preferences::ColormapUploadCmap()
{
    /*DSettings * doptions = GetDisplayOptions();
    int n, j, r, g, b, set;
    unsigned char * AUX;
    glcan->SetCurrent();
    glReadBuffer( GL_BACK );
    AUX = (unsigned char *)malloc( 3 * 256 * sizeof( unsigned char ) );
    glReadPixels( 0, 25, 255, 1, GL_RGB, GL_UNSIGNED_BYTE, AUX );
    glFinish();
    glcan->SwapBuffers();
    for ( n = 0, j = 0; j < 256; n = n + 3, j++ )
    {
    	doptions->CmapGetColor( j, & r, & g, & b, & set );
    	if ( set == 0 )
    			doptions->CmapChangeColor( j, AUX[n], AUX[n + 1], AUX[n + 2], 0 );
    }
    delete AUX ;
    */
}

void Preferences::ColormapResetToDef()
{
    DSettings * doptions = GetDisplayOptions();
    doptions->CmapResetToDefault();
    ColormapUpdateColorDisp();
    first = true;
    Refresh(true,NULL);
}

wxPanel* Preferences::ColormapCreateControls(void)
{
    wxPanel * cpanel = new wxPanel( book);
    wxBoxSizer * item2 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer * item3 = new wxBoxSizer( wxVERTICAL );
    item2->Add( item3, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxBoxSizer * colsiz = new wxBoxSizer( wxHORIZONTAL );
    item3->Add( colsiz, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxStaticText * t1 = new wxStaticText( cpanel, -1, "Change colour at" );
    colsiz->Add( t1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxTextCtrl * item4 = new wxTextCtrl( cpanel, ID_TEXTCTRL, _( "" ), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    colsiz->Add( item4, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxStaticText * t2 = new wxStaticText( cpanel, -1, "%" );
    colsiz->Add( t2, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    /*glcan = new wxPanel(cpanel,-1,wxDefaultPosition,wxSize(256,50));
    glcan->Show(true);
    glcan->Refresh(true,NULL);
    item3->Add( glcan, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );*/
    wxStaticBitmap * cbar = new wxStaticBitmap( cpanel, -1, cbit, wxDefaultPosition, wxDefaultSize );

    item3->Add( cbar, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxBoxSizer * item6 = new wxBoxSizer( wxHORIZONTAL );
    item2->Add( item6, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxButton * item8 = new wxButton( cpanel, ID_DEFAULT, _( "Default" ), wxDefaultPosition, wxDefaultSize, 0 );

    item6->Add( item8, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    wxBoxSizer * last = new wxBoxSizer( wxVERTICAL );
    item2->Add( last, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxButton * save = new wxButton( cpanel, ID_CSAVE, _( "Save cmap" ), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( save, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    wxButton * load = new wxButton( cpanel, ID_CLOAD, _( "Load cmap" ), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( load, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );


    wxBoxSizer * last1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer * last2 = new wxBoxSizer( wxHORIZONTAL );

    int x, count, * welche;
    wxString choices1[] =
        {
            _T( "Flynns" )
        };
    wxString choices2[] =
        {
            _T( "BNodes" )
        };
    wxString choices3[] =
        {
            _T( "UNodes" )
        };
    fselect = new wxChoice( cpanel, ID_sflynn, wxDefaultPosition, wxDefaultSize, 1, choices1 );
    bselect = new wxChoice( cpanel, ID_sbnode, wxDefaultPosition, wxDefaultSize, 1, choices2 );
    uselect = new wxChoice( cpanel, ID_sunode, wxDefaultPosition, wxDefaultSize, 1, choices3 );
    ElleFlynnDfltAttributeList( & welche, & count );
    for ( x = 0; x < count; x++ )
    {
		fselect->Append( GetAttributeName( welche[x],FLYNNS ) );
	}
    if (welche) { free(welche); welche=0; }

    ElleNodeAttributeList( & welche, & count );
    for ( x = 0; x < count; x++ )
    {
		bselect->Append( GetAttributeName( welche[x],BNODES ) );
    }
    if (welche) { free(welche); welche=0; }

    ElleUnodeAttributeList( & welche, & count );
    for ( x = 0; x < count; x++ )
    {
		uselect->Append( GetAttributeName( welche[x],UNODES ) );
    }
    if (welche) { free(welche); welche=0; }

    min = new wxStaticText( cpanel, -1, "Min.:" );
    max = new wxStaticText( cpanel, -1, "Max.:" );
    fmin = new wxStaticText( cpanel, -1, "                      " );
    fmax = new wxStaticText( cpanel, -1, "            " );

    last1->Add( fselect, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    last1->Add( bselect, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    last1->Add( uselect, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    last2->Add( min, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    last2->Add( fmin, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    last2->Add( max, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    last2->Add( fmax, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    last->Add( last1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    last->Add( last2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    wxBoxSizer * vlast = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText * c1 = new wxStaticText( cpanel, -1, "Change colour at value:" );
    wxTextCtrl * c2 = new wxTextCtrl( cpanel, ID_CTEXT, _( "" ), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );

    vlast->Add( c1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    vlast->Add( c2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    item2->Add( vlast, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxString mm;
    cpanel->SetSizer( item2 );
    //ColormapResetToDef();
    ColormapUpdateColorDisp();
    return cpanel;
}

void Preferences::GetUserColor(wxColour *col)
{
    *col=wxGetColourFromUser();
}

void Preferences::ColormapOnChangeColorValue( wxCommandEvent & event )
{
    int in;
    int r, g, b;
    double tmp, vmin, vmax, dp;
    DSettings * doptions = GetDisplayOptions();
    wxColour col;
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & tmp );
    ( fmin->GetLabel() ).ToDouble( & vmin );
    ( fmax->GetLabel() ).ToDouble( & vmax );
    if ( tmp > vmax || tmp < vmin )
        wxMessageBox( "Value out of range!", "Attention", wxOK, this );
    else
    {
        dp = ( vmax - vmin ) / 100;
        tmp = tmp / dp;
        in = ( int )( ceil( tmp * 2.55 ) ); //ohoh, that calls for trouble!
        if ( tmp < 0 )
            tmp = 0.0;
        if ( tmp > 255 )
            tmp = 255.0;
        GetUserColor(&col);
        if(col.Ok())
        {
            r = col.Red(); g = col.Green(); b = col.Blue();
            doptions->CmapChangeColor( in, r, g, b, 1 );
            ColormapUpdateColorDisp();
            book->Refresh( true, NULL );
            ColormapUploadCmap();
        }
        in = ( ( wxTextCtrl * ) event.GetEventObject() )->GetLineLength( 0 );
        ( ( wxTextCtrl * ) event.GetEventObject() )->SetSelection( 0, in );
    }
}

void Preferences::ColormapOnChangeColorPercent( wxCommandEvent & event )
{
    int in;
    int r, g, b;
    double tmp;
    DSettings * doptions = GetDisplayOptions();
    wxColour col;
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & tmp );
    in = ( int )( ceil( tmp * 2.55 ) ); //ohoh, that calls for trouble!
    if ( tmp < 0 || tmp > 255 )
        wxBell();
    else
    {
        GetUserColor(&col);
        if(col.Ok())
        {
            r = col.Red(); g = col.Green(); b = col.Blue();
            doptions->CmapChangeColor( in, r, g, b, 1 );
            ColormapUpdateColorDisp();
            book->Refresh( true, NULL );
            ColormapUploadCmap();
        }
    }
    in = ( ( wxTextCtrl * ) event.GetEventObject() )->GetLineLength( 0 );
    ( ( wxTextCtrl * ) event.GetEventObject() )->SetSelection( 0, in );
}

void Preferences::ColormapUpdateColorDisp()
{
    int n, set;
    int r, g, b;
    wxMemoryDC dc;
    wxPen pen;
    if(cbit.Ok())
    {
        dc.SelectObject(cbit);
        DSettings * doptions = GetDisplayOptions();
        for ( n = 0; n < 256; n++ )
        {
            doptions->CmapGetColor( n, & r, & g, & b, & set );
            pen.SetColour(wxColour(r,g,b));
            dc.SetPen(pen);
            dc.DrawLine(n,0,n,50 );
        }
	Refresh(true,NULL); // LE
  	book->Refresh(true,NULL);
    }
}

void Preferences::ColormapOnDefault( wxCommandEvent & event )
{
    ColormapResetToDef();
    Refresh( true, NULL );
}

void Preferences::OnFCCMin(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & fccmin );
}
void Preferences::OnFCCMax(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & fccmax );
}
void Preferences::OnBCCMin(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & bccmin );
}
void Preferences::OnBCCMax(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & bccmax );
}
void Preferences::OnNCCMin(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & nccmin );
}
void Preferences::OnNCCMax(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & nccmax );
}
void Preferences::OnUCCMin(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & uccmin );
}
void Preferences::OnUCCMax(wxCommandEvent &event)
{
    ( ( ( wxTextCtrl * ) event.GetEventObject() )->GetValue() ).ToDouble( & uccmax );
}

void Preferences::OnFClampColor( wxCommandEvent & event )
{
    fclampcolor=event.IsChecked();
}

void Preferences::OnBClampColor( wxCommandEvent & event )
{
    bclampcolor=event.IsChecked();
}

void Preferences::OnNClampColor( wxCommandEvent & event )
{
    nclampcolor=event.IsChecked();
}

void Preferences::OnUClampColor( wxCommandEvent & event )
{
    uclampcolor=event.IsChecked();
}
