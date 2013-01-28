// Written in 2006
// Author: Dr. J.K. Becker
// Copyright: Dr. J.K. Becker (becker@jkbecker.de)

#include "canvas.h"
#include "dsettings.h"
#include "display.h"
#include "psprint.h"
#include "showelle.h"

extern DSettings * GetDisplayOptions();


/*!  Implements the class Canvas (wxWindows). See www.wxwindows.org */
IMPLEMENT_CLASS( Canvas, wxScrolledWindow )
/*! Event table for the class Canvas */
BEGIN_EVENT_TABLE( Canvas, wxScrolledWindow )
EVT_PAINT( Canvas::OnPaint )
EVT_SIZE( Canvas::OnSize )
EVT_MOUSE_EVENTS( Canvas::OnMouse )
EVT_CLOSE( Canvas::EndSession )
EVT_CHAR( Canvas::OnKey )
END_EVENT_TABLE()



Canvas::Canvas( wxWindow * parent, int vsize, int hsize ) :  wxScrolledWindow( parent,CANVASWIN,wxDefaultPosition,wxSize(hsize, vsize ),wxHSCROLL|wxVSCROLL)
{
    zoomfactor = 1;
    lineshow = true;
    bnodeshow = true;
    flynnshow = false;
    flynnshownumbers = false;
    nodeshownumbers = false;
    enablezoom = false;
    getinfobnode = false;
    getinfounode = false;
    markpoly = false;
    mzoom = false;
    pcount = 0;
    leftclick = true;
    viewstartx=viewstarty=0;
    //initial colors of lines and bnodes, these can be changed if the users wants to
    DSettings * dset = GetDisplayOptions();
    int r, g, b;
    dset->GetDNodeColor( & r, & g, & b );
    bnodepen.SetColour( wxColour( r, g, b ) );
    bnodepen.SetStyle( wxSOLID );
    bnodepen.SetWidth( 1 );
    bnodebrush.SetColour( wxColour( r, g, b ) );
    bnodebrush.SetStyle( wxSOLID );
    bnodesize = dset->GetNodeSize();
    dset->GetLineColor( & r, & g, & b );
    linepen.SetColour( wxColour( r, g, b ) );
    linepen.SetStyle( wxSOLID );
    linepen.SetWidth( dset->GetLineSize() );
    useunodeattrib = 0;
    unodeshow = false;
    SetBackgroundColour(wxColour(100,100,100));
    isinit=false;
    printps=false;
}

/*!  * Function reading key events. */
void Canvas::OnKey( wxKeyEvent & event )
{}

/*! Check the unit cell and scale image accordingly
	*/
void Canvas::CheckUnitCell()
{
    if(isinit)
    {
        wxImage im;
        wxSize size;
        CellData unitcell;
        ElleCellBBox(&unitcell);
        //size=GetVirtualSize();
	    size.SetWidth(image->GetWidth());
	    size.SetHeight(image->GetHeight());
	    im=image->ConvertToImage();
        if(unitcell.xlength!=1 || unitcell.ylength!=1) {
        image=new wxBitmap(im.Rescale((int)(size.GetWidth()*unitcell.xlength),(int)(size.GetHeight()*unitcell.ylength)));
        }
	else 
		if(size.GetWidth()>=size.GetHeight())
			image=new wxBitmap(im.Rescale(size.GetWidth(),size.GetWidth()));
		else
			image=new wxBitmap(im.Rescale(size.GetHeight(),size.GetHeight()));
        SetScrollbarsTicks();
        Scroll(viewstartx,viewstarty);
    }
}

void Canvas::ResetScreen()
{
    EllePlotRegions(0);
}

void Canvas::DrawToPS(bool t)
{
    printps=t;
}
void Canvas::Init( void )
{
    int vsize,hsize;
    GetClientSize(&hsize,&vsize);
    SetVirtualSize(wxSize(hsize,vsize));
    image=new wxBitmap(hsize,vsize);
    wxMemoryDC dc;
    dc.SelectObject(*image);
    dc.SetBrush(wxBrush(wxColour(0,0,0),wxSOLID));
    dc.DrawRectangle(0,0,hsize,vsize);
    dc.SelectObject(wxNullBitmap);
    isinit=true;
    //SetScrollbarsTicks();
}

/*! Laeuft und macht das was es soll */
void Canvas::EndSession( wxCloseEvent & event )
{
    this->Destroy();
}

/*!  This function is always called when the display needs to be updated. */
void Canvas::OnPaint( wxPaintEvent & event )
{
    if(isinit){
        wxPaintDC dc( this );
        DoPrepareDC(dc);
        dc.DrawBitmap(*image,0,0);
        GetViewStart(&viewstartx,&viewstarty);
    }
}

/*! Clears the screen */
void Canvas::ClearScreen()
{
    wxSize size;
    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    temp_dc.SetBrush(wxBrush(wxColour(0,0,0),wxSOLID));
    temp_dc.DrawRectangle(0,0,size.GetWidth(),size.GetHeight());
    temp_dc.SelectObject(wxNullBitmap);
    Refresh(false,NULL);
}

void Canvas::GLDrawLines( double coords[] [3], int num, int rr, int gg, int bb )
{
    if(!printps)
        IMDrawLines( coords, num, rr, gg, bb );
    else
        PSDrawLines( coords, num, rr,gg,bb);
}

void Canvas::PSDrawLines( double coords[] [3], int num, int rr, int gg, int bb )
{
    int n, linesize;
    wxPen pen;
    wxPoint points[num];
    wxSize size;
    size.Set(300,300);
    CellData unitcell;
    ElleCellBBox(&unitcell);
    for(int n=0; n<num; n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    pen=psDC->GetPen();
    DSettings * dset = GetDisplayOptions();
    linesize = dset->GetLineSize();
    pen.SetColour( rr , gg, bb);
    pen.SetWidth(linesize );
    psDC->SetPen(pen);
    psDC->BeginDrawing();
    for ( n = 0; n < num-1; n++ )
        psDC->DrawLine(points[n].x,points[n].y,points[n+1].x,points[n+1].y );
    psDC->DrawLine(points[0].x,points[0].y,points[num-1].x,points[num-1].y );
    psDC->EndDrawing();
}


/*! Draws a line (or lines) with the defined color */
void Canvas::IMDrawLines( double coords[] [3], int num, int rr, int gg, int bb )
{
    int n, linesize;
    wxPen pen;
    wxPoint points[num];
    wxSize size;
    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    CellData unitcell;
    ElleCellBBox(&unitcell);
    for(int n=0; n<num; n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    pen=temp_dc.GetPen();
    DSettings * dset = GetDisplayOptions();
    linesize = dset->GetLineSize();
    pen.SetColour( rr , gg, bb);
    pen.SetWidth(linesize );
    temp_dc.SetPen(pen);
    temp_dc.BeginDrawing();
    for ( n = 0; n < num-1; n++ )
        temp_dc.DrawLine(points[n].x,points[n].y,points[n+1].x,points[n+1].y );
    temp_dc.DrawLine(points[0].x,points[0].y,points[num-1].x,points[num-1].y );
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}


void Canvas::GLDrawLinesBound( double coords[] [3], int num, int r, int g, int b )
{
    if(!printps)
        IMDrawLinesBound( coords, num,  r, g, b );
    else
        PSDrawLinesBound( coords, num, r, g, b );
}
/*! draws lines with defined boundary attribute */
void Canvas::IMDrawLinesBound( double coords[] [3], int num, int r, int g, int b )
{
    int n, linesize;
    wxPen pen;
    wxPoint points[num];

    wxSize size;
    CellData unitcell;
    ElleCellBBox(&unitcell);
    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    for(int n=0; n<num; n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    pen=temp_dc.GetPen();
    DSettings * dset = GetDisplayOptions();
    linesize = dset->GetLineSize();
    pen.SetColour( r , g, b);
    pen.SetWidth(linesize );
    temp_dc.SetPen(pen);
    temp_dc.BeginDrawing();
    for ( n = 0; n < num-1; n++ )
        temp_dc.DrawLine(points[n].x,points[n].y,points[n+1].x,points[n+1].y );
    temp_dc.DrawLine(points[0].x,points[0].y,points[num-1].x,points[num-1].y );
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}

void Canvas::PSDrawLinesBound( double coords[] [3], int num, int r, int g, int b )
{
    int n, linesize;
    wxPen pen;
    wxPoint points[num];

    wxSize size;
    CellData unitcell;
    ElleCellBBox(&unitcell);
    size.Set(300,300);
    for(int n=0; n<num; n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    pen=psDC->GetPen();
    DSettings * dset = GetDisplayOptions();
    linesize = dset->GetLineSize();
    pen.SetColour( r , g, b);
    pen.SetWidth(linesize );
    psDC->SetPen(pen);
    psDC->BeginDrawing();
    for ( n = 0; n < num-1; n++ )
        psDC->DrawLine(points[n].x,points[n].y,points[n+1].x,points[n+1].y );
    psDC->DrawLine(points[0].x,points[0].y,points[num-1].x,points[num-1].y );
    psDC->EndDrawing();
}

/*! Draws all bnodes. */
void Canvas::ElleDrawBNodes()
{
    int n, count;
    int r, g, b;

    temp_dc.SelectObject(*image);
    wxPoint points;
    Coords xy;
    DSettings * dset = GetDisplayOptions();
    dset->GetDNodeColor( & r, & g, & b );
    bnodepen.SetColour( wxColour( r, g, b ) );
    bnodepen.SetStyle( wxSOLID );
    bnodesize = dset->GetNodeSize();
    bnodepen.SetWidth( bnodesize );
    bnodebrush.SetColour( wxColour( r, g, b ) );
    bnodebrush.SetStyle( wxSOLID );
    count = ElleMaxNodes();
    temp_dc.BeginDrawing();
    for ( n = 0; n < count; n++ ) {
        if ( ElleNodeIsActive( n ) ) {
            if ( ElleNodeIsDouble( n ) )
                bnodepen.SetColour( r , g , b );
            else if ( ElleNodeIsTriple( n ) )
                bnodepen.SetColour( 255,0,0 );
            temp_dc.SetPen(bnodepen);
            ElleNodePosition( n, & xy );
            points.x=(int)(xy.x*image->GetWidth());
            points.y=(int)(image->GetHeight()-(xy.y*image->GetHeight()));
            temp_dc.DrawPoint( points);
        }
    }
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}

void Canvas::GLDrawSingleBNode( double x, double y, int r, int g, int b )
{
    if(!printps)
        IMDrawSingleBNode( x, y, r, g, b );
    else
        PSDrawSingleBNode( x, y, r, g, b );
}
/*! Draws a single bnode at position xy with color rgb */
void Canvas::IMDrawSingleBNode( double x, double y, int r, int g, int b )
{
    float s;

    wxPoint point;
    wxSize size;
    wxPen pen;
    CellData unitcell;
    ElleCellBBox(&unitcell);
    temp_dc.SelectObject(*image);
    pen=temp_dc.GetPen();
    size=temp_dc.GetSize();
    point.x=(int)((x/unitcell.xlength)*size.GetWidth());
    point.y=(int)(size.GetHeight()-((y/unitcell.ylength)*size.GetHeight()));
    DSettings * dset = GetDisplayOptions();
    pen.SetWidth(1);
    pen.SetColour(wxColour(r,g,b));
    temp_dc.SetPen(pen);
    pen.SetWidth(1);
    temp_dc.SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
    temp_dc.BeginDrawing();
    temp_dc.DrawCircle( point.x,point.y,dset->GetNodeSize() );
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}

void Canvas::PSDrawSingleBNode( double x, double y, int r, int g, int b )
{
    float s;

    wxPoint point;
    wxSize size;
    wxPen pen;
    CellData unitcell;
    ElleCellBBox(&unitcell);
    pen=psDC->GetPen();
    size.Set(300,300);
    point.x=(int)((x/unitcell.xlength)*size.GetWidth());
    point.y=(int)(size.GetHeight()-((y/unitcell.ylength)*size.GetHeight()));
    DSettings * dset = GetDisplayOptions();
    pen.SetWidth(1);
    pen.SetColour(wxColour(r,g,b));
    psDC->SetPen(pen);
    pen.SetWidth(1);
    psDC->SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
    psDC->BeginDrawing();
    psDC->DrawCircle( point.x,point.y,dset->GetNodeSize() );
    psDC->EndDrawing();
}


void Canvas::ElleDrawUNodes( int unattrib )
{
	if(!printps)
		IMDrawUNodes(unattrib);
	else
		PSDrawUNodes(unattrib);
}
/*! Draws all unodes. */
void Canvas::IMDrawUNodes( int unattrib )
{
    int n, count, r, g, b, set, coln,unodesize;
    double min = 100000000, max = -1000000000, value;
    double uccmin = 0, uccmax = 0;
    Coords xy;
    bool drawunode = true, uclamp = false;
    DSettings * dset = GetDisplayOptions();
    unodesize = dset->GetUNodeSize();
    useunodeattrib = unattrib;
    wxSize size;
    wxPoint point;
    CellData unitcell;
    ElleCellBBox(&unitcell);

    wxPen pen;
    pen.SetColour(r,g,b);
    pen.SetStyle(wxSOLID);
    pen.SetWidth(1);
    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    count = ElleMaxUnodes();
    if ( useunodeattrib != U_LOCATION ) {
        if ( ElleUnodesActive() ) {
            //LE this is done when the user selects the attrib
            for ( n = 0; n < count; n++ ) {
                ElleGetUnodeAttribute( n, useunodeattrib, & value );
                if ( value < min )
                    min = value;
                if ( value > max )
                    max = value;
            }
            DSettings * dset = GetDisplayOptions();
			//LE
            if (uclamp = dset->GetClampColor(3,&uccmin,&uccmax)) {
				min = uccmin;
				max = uccmax;
			}

            temp_dc.BeginDrawing();
            for ( n = 0; n < count; n++ ) {
                drawunode = true;
                ElleGetUnodePosition( n, & xy );
                ElleGetUnodeAttribute( n, useunodeattrib, & value );
                if ( dset->GetUnodesRangeFlag() ) {
                    if ( value > dset->GetUnodesRangeFlagMaxValue() || value < dset->GetUnodesRangeFlagMinValue() ) {
                        drawunode = false;
                    }
                }
                if ( dset->GetUnodesNotRangeFlag() ) {
                    if ( value < dset->GetUnodesNotRangeFlagMaxValue() && value > dset->GetUnodesNotRangeFlagMinValue() ) {
                        drawunode = false;
                    }
                }
                if ( drawunode ) {
                    if (value<min) value = min; //LE
                    if (value>max) value = max; //LE

                    value = ( value - min ) / ( ( max - min ) / 100 );
                    coln = ( int )( value * 2.55 );
                    dset->CmapGetColor( coln, & r, & g, & b, & set
                                          );
                    pen.SetColour(r,g,b);
                    temp_dc.SetPen(pen);
                    temp_dc.SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
					point.x=(int)((xy.x/unitcell.xlength)*size.GetWidth());
					point.y=(int)(size.GetHeight()-((xy.y/unitcell.ylength)*size.GetHeight()));
                    temp_dc.DrawCircle( point.x,point.y,unodesize );
                }
            }
            temp_dc.EndDrawing();
            temp_dc.SelectObject(wxNullBitmap);
        }
    } else {
        r=g=0;
        b=255;
        temp_dc.BeginDrawing();
        for ( n = 0; n < count; n++ ) {
            ElleGetUnodePosition( n, & xy );
            pen.SetColour(r,g,b);
            temp_dc.SetPen(pen);
            temp_dc.SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
			point.x=(int)((xy.x/unitcell.xlength)*size.GetWidth());
			point.y=(int)(size.GetHeight()-((xy.y/unitcell.ylength)*size.GetHeight()));
            temp_dc.DrawCircle( point.x,point.y,unodesize );
        }
    }
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}

void Canvas::PSDrawUNodes( int unattrib )
{
    int n, count, r, g, b, set, coln,unodesize;
    double min = 100000000, max = -1000000000, value;
    double uccmin = 0, uccmax = 0;
    Coords xy;
    bool drawunode = true, uclamp = false;
    DSettings * dset = GetDisplayOptions();
    unodesize = dset->GetUNodeSize();
    useunodeattrib = unattrib;
    wxSize size;

    wxPen pen;
    pen.SetColour(r,g,b);
    pen.SetStyle(wxSOLID);
    pen.SetWidth(1);
    size=psDC->GetSize();
    count = ElleMaxUnodes();
    if ( useunodeattrib != U_LOCATION ) {
        if ( ElleUnodesActive() ) {
            for ( n = 0; n < count; n++ ) {
                ElleGetUnodeAttribute( n, useunodeattrib, & value );
                if ( value < min )
                    min = value;
                if ( value > max )
                    max = value;
            }
            DSettings * dset = GetDisplayOptions();
			//LE
            if (uclamp = dset->GetClampColor(3,&uccmin,&uccmax)) {
				min = uccmin;
				max = uccmax;
			}

            psDC->BeginDrawing();
            for ( n = 0; n < count; n++ ) {
                drawunode = true;
                ElleGetUnodePosition( n, & xy );
                ElleGetUnodeAttribute( n, useunodeattrib, & value );
                if ( dset->GetUnodesRangeFlag() ) {
                    if ( value > dset->GetUnodesRangeFlagMaxValue() || value < dset->GetUnodesRangeFlagMinValue() ) {
                        drawunode = false;
                    }
                }
                if ( dset->GetUnodesNotRangeFlag() ) {
                    if ( value < dset->GetUnodesNotRangeFlagMaxValue() && value > dset->GetUnodesNotRangeFlagMinValue() ) {
                        drawunode = false;
                    }
                }
                if ( drawunode ) {
                    if (value<min) value = min; //LE
                    if (value>max) value = max; //LE
                    value = ( value - min ) / ( ( max - min ) / 100 );
                    coln = ( int )( value * 2.55 );
                    dset->CmapGetColor( coln, & r, & g, & b, & set
                                          );
                    pen.SetColour(r,g,b);
                    psDC->SetPen(pen);
                    psDC->SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
                    psDC->DrawCircle( (int)(xy.x * size.GetWidth()),(int)(size.GetHeight()-( xy.y * size.GetHeight())),unodesize );
                }
            }
            psDC->EndDrawing();
        }
    } else {
        r=g=0;
        b=255;
        temp_dc.BeginDrawing();
        for ( n = 0; n < count; n++ ) {
            ElleGetUnodePosition( n, & xy );
            pen.SetColour(r,g,b);
            psDC->SetPen(pen);
            psDC->SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
            psDC->DrawCircle( (int)(xy.x * size.GetWidth()),(int)(size.GetHeight()-( xy.y * size.GetHeight())),unodesize );
        }
    }
    psDC->EndDrawing();
}


void Canvas::ElleShowNodeNumbers()
{
    if(!printps)
        IMShowNodeNumbers();
    else
        PSShowNodeNumbers();
}

/*! Draws every fifth nodenumber of a flynn. *Muss ich nochmal bei, die Schrift ist zu gross, oder nur jeden dritten... */
void Canvas::IMShowNodeNumbers()
{
    int n;
    wxString num;
    Coords xy;
    wxSize size;

    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    temp_dc.BeginDrawing();
    temp_dc.SetTextForeground(wxColour(255,255,255));
    for ( n = 0; n < ElleMaxNodes(); n++ ) {
        if ( ElleNodeIsActive( n ) ) {
            num.Printf( "%d", n );
            ElleNodePosition( n, & xy );
            temp_dc.DrawText(num, (int)(xy.x *size.GetWidth()),(int)(size.GetHeight()-( xy.y *size.GetHeight()) ));
            num = "";
        }
    }
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}

void Canvas::PSShowNodeNumbers()
{
    int n;
    wxString num;
    Coords xy;
    wxSize size;
    size=psDC->GetSize();
    psDC->BeginDrawing();
    psDC->SetTextForeground(wxColour(255,255,255));
    for ( n = 0; n < ElleMaxNodes(); n++ ) {
        if ( ElleNodeIsActive( n ) ) {
            num.Printf( "%d", n );
            ElleNodePosition( n, & xy );
            psDC->DrawText(num, (int)(xy.x *size.GetWidth()),(int)(size.GetHeight()-( xy.y *size.GetHeight()) ));
            num = "";
        }
    }
    psDC->EndDrawing();
}


void Canvas::GLDrawPolygon( double coords[] [3], int num, int r, int g, int b )
{
    if(!printps)
        IMDrawPolygon( coords,  num, r, g, b );
    else
        PSDrawPolygon( coords,  num, r, g, b );
}

/*! Draws the flynn flynn with a specified brush. */
void Canvas::IMDrawPolygon( double coords[] [3], int num, int r, int g, int b )
{
    int n;
    wxPen pen;
    pen.SetColour(r,g,b);
    pen.SetWidth(1);
    pen.SetStyle(wxSOLID);

    wxSize size;
    wxPoint points[num];
    CellData unitcell;
    ElleCellBBox(&unitcell);
    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    for(n=0;n<num;n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    temp_dc.SetPen(pen);
    temp_dc.SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
    temp_dc.BeginDrawing();
    temp_dc.DrawPolygon(num,points);
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}


void Canvas::PSDrawPolygon( double coords[] [3], int num, int r, int g, int b )
{
    int n;
    wxPen pen;
    pen.SetColour(r,g,b);
    pen.SetWidth(1);
    pen.SetStyle(wxSOLID);

    wxSize size;
    wxPoint points[num];
    CellData unitcell;
    ElleCellBBox(&unitcell);
    size.Set(300,300);//psDC->GetSize();
    for(n=0;n<num;n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    psDC->SetPen(pen);
    psDC->SetBrush(wxBrush(wxColour(r,g,b),wxSOLID));
    psDC->BeginDrawing();
    psDC->DrawPolygon(num,points);
    psDC->EndDrawing();
}

/*! Draws a transparent polygon. This is used during marking a set of flynns or nodes in the data windows */
void Canvas::GLDrawTransparentPolygon( double coords[] [3], int num, int r, int g, int b)
{
    int n;
    wxPen pen;
    pen.SetColour(r,g,b);
    pen.SetWidth(1);
    pen.SetStyle(wxSOLID);

    wxSize size;
    wxPoint points[num];
    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    CellData unitcell;
    ElleCellBBox(&unitcell);
    for(n=0;n<num;n++) {
        points[n].x=(int)((coords[n][0]/unitcell.xlength)*size.GetWidth());
        points[n].y=(int)(size.GetHeight()-((coords[n][1]/unitcell.ylength)*size.GetHeight()));
    }
    temp_dc.SetPen(pen);
    temp_dc.SetBrush(wxBrush(wxColour(r,g,b),wxCROSSDIAG_HATCH));
    temp_dc.BeginDrawing();
    temp_dc.DrawPolygon(num,points);
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
    Refresh(false,NULL);
}


void Canvas::ElleShowFlynnNumbers( int index, float * x, float * y, int num )
{
    if(!printps)
        IMShowFlynnNumbers( index,  x,  y, num );
    else
        PSShowFlynnNumbers( index,  x,  y, num );
}

/*! Shows all flynn-numbers. */
void Canvas::IMShowFlynnNumbers( int index, float * x, float * y, int num )
{
    Coords xy;
    wxSize size;

    temp_dc.SelectObject(*image);
    size=temp_dc.GetSize();
    double area;
    wxString number;
    number.Printf( "%d", index );
    temp_dc.SetTextForeground(wxColour(255,255,255));
    polyCentroid( x, y, num, & xy.x, & xy.y, & area );
    temp_dc.BeginDrawing();
    temp_dc.DrawText(number,(int)(xy.x * size.GetWidth()),(int)(size.GetHeight()-(xy.y * size.GetHeight() )));
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
    number = "";
}

void Canvas::PSShowFlynnNumbers( int index, float * x, float * y, int num )
{
    Coords xy;
    wxSize size;
    size.Set(300,300);
    double area;
    wxString number;
    number.Printf( "%d", index );
    psDC->SetTextForeground(wxColour(255,255,255));
    polyCentroid( x, y, num, & xy.x, & xy.y, & area );
    psDC->BeginDrawing();
    psDC->DrawText(number,(int)(xy.x * size.GetWidth()),(int)(size.GetHeight()-(xy.y * size.GetHeight() )));
    psDC->EndDrawing();
    number = "";
}
/*! This really shows all flynn numbers, inside the flynn and all that. */
void Canvas::ElleShowFlynnNumbers()
{
    wxSize size;

    temp_dc.SelectObject(*image);
    //temp_dc.SetAxisOrientation(false,true);
    size=temp_dc.GetSize();
    int z, n, num_nodes, * id, count;
    Coords xy, lastxy;
    double * x, * y, area, cx = 0.0, cy = 0.0;
    wxString number;
    lastxy.x = -10;
    lastxy.y = -10;
    count = 0;
    temp_dc.SetTextForeground(wxColour(255,255,255));
    temp_dc.BeginDrawing();
    for ( n = 0; n < ElleMaxFlynns(); n++ ) {
        if ( ElleFlynnIsActive( n ) ) {
            ElleFlynnNodes( n, & id, & num_nodes );
            x = ( double * ) malloc( sizeof( double ) * num_nodes );
            y = ( double * ) malloc( sizeof( double ) * num_nodes );
            for ( z = 0; z < num_nodes; z++ ) {
                if ( ElleNodeIsActive( id[z] ) ) {
                    ElleNodePosition( id[z], & xy );
                    if ( lastxy.x == -10 ) {
                        x[count] = xy.x;
                        y[count] = xy.y;
                        lastxy.x = xy.x;
                        lastxy.y = xy.y;
                        cx = x[count];
                        cy = y[count];
                        count++;
                    } else {
                        if ( xy.x > lastxy.x ) {
                            if ( xy.x - lastxy.x >= 3 * ElleSwitchdistance() )
                                xy.x -= 1;
                        } else {
                            if ( lastxy.x - xy.x >= 3 * ElleSwitchdistance() )
                                xy.x += 1;
                        }
                        if ( xy.y > lastxy.y ) {
                            if ( xy.y - lastxy.y >= 3 * ElleSwitchdistance() )
                                xy.y -= 1;
                        } else {
                            if ( lastxy.y - xy.y >= 3 * ElleSwitchdistance() )
                                xy.y += 1;
                        }
                        x[count] = xy.x;
                        y[count] = xy.y;
                        lastxy.x = xy.x;
                        lastxy.y = xy.y;
                        cx += x[count];
                        cy += y[count];
                        count++;
                    }
                }
            }
            xy.x = cx / ( count - 1 );
            xy.y = cy / ( count - 1 );
            number.Printf( "%d", n );
            temp_dc.DrawText(number, (int)(xy.x *size.GetWidth()),(int)( size.GetHeight()-(xy.y*size.GetHeight() )));
            number = "";
            if(x)
                free( x );
            if(y)
                free( y );
            if(id)
                free( id );
        }
        lastxy.x = -10;
        lastxy.y = -10;
        count = 0;
        cx = 0;
        cy = 0;
    }
    temp_dc.EndDrawing();
    temp_dc.SelectObject(wxNullBitmap);
}

/*!Overlays a picture over the existing one.
No changes on the existing picture can be made. */
void Canvas::OnOverlay()
{
    EllePlotRegions( 0 );
}

/*!Save the current file as a single picture. *Strictly WYSIWYG! Handy function to use
 * during runs, so each stage is saved as a picture immediately instead of saving 
 * elle-files and converting them later.*/
void Canvas::OnSaveSingle( wxString filename )
{
    wxString suffix;
    if ( !filename.IsEmpty() ) {
        suffix = filename.AfterLast( '.' );
        if(suffix==filename || suffix.IsEmpty()) {
            suffix="png";
            filename.Append(".png");
            wxMessageBox("You did not supply a file-type. Will save the picture as png.", "Info",wxOK);
        }
        if ( suffix == "jpg" )
            image->SaveFile( filename, wxBITMAP_TYPE_JPEG );
        if ( suffix == "png" )
            image->SaveFile( filename, wxBITMAP_TYPE_PNG );
        if ( suffix == "pcx" )
            image->SaveFile( filename, wxBITMAP_TYPE_PCX );
    }
}

/*!Adjust the picture to the new size of the window.
 * If the user changes the size of the window, the opengl-Canvas
 * will adjust to it here. */
void Canvas::OnSize( wxSizeEvent & event )
{
    int width,height;
    GetParent()->GetClientSize(&width,&height );
    SetSize(wxSize(width,height));
    SetScrollbarsTicks();
    Refresh(true,NULL);
}

/*! reacts to mouse events */
void Canvas::OnMouse( wxMouseEvent & event )
{
    int w, h,sx,sy;
    double x = event.GetX(), y = event.GetY();
    int vsx,vsy;
    GetViewStart(&vsx,&vsy);
    double c[4] [3];
    wxString str, val;
    w=image->GetWidth();
    h=image->GetHeight();
    GetScrollPixelsPerUnit(&sx,&sy);
    x=x+vsx*sx;
    y=y+vsy*sy;
    y = (h-y)/h;
    x= (x/w);
    if(x>1)
        x=1;
    else if(x<0)
        x=0;
    if(y>1)
        y=1;
    else if(y<0)
        y=0;

    str = "X:";
    val.Printf( "%lf", x  );
    str.append( val );
    str.append( "-Y:" );
    val.Empty();
    val.Printf( "%lf", y  );
    str.append( val );
    ( ( wxFrame * ) GetParent() )->SetStatusText( str, 2 );
    val.Clear();
    str.Clear();
    if ( event.LeftDown() ) {
        if ( markpoly )
            MarkPolygon( x , y );
    }
    if ( event.ButtonDClick( 1 ) ) {
        if ( markpoly )
            markpoly = false;
        if ( FindWindow( FLYNNTABLE ) != NULL )
            ( ( TableData * ) FindWindow( FLYNNTABLE ) )->DecideMark( psc, pcount );
        if ( FindWindow( BNODETABLE ) != NULL )
            ( ( TableData * ) FindWindow( BNODETABLE ) )->DecideMark( psc, pcount );
        if ( FindWindowById( UNODETABLE ) != NULL )
            ( ( TableData * ) FindWindow( UNODETABLE ) )->DecideMark( psc, pcount );
        pcount = 0;
        EllePlotRegions(0);
    }
    if ( getinfobnode || getinfounode ) {
        if ( event.LeftDown() ) {
            if ( getinfobnode )
                ( ( ShowelleFrame * ) GetParent() )->GetInfoBnode( x , y);
            if ( getinfounode )
                ( ( ShowelleFrame * ) GetParent() )->GetInfoUnode( x , y );
            getinfobnode = false;
            getinfounode = false;
            SetCursor( wxCURSOR_ARROW );
        }
    }
}


/*!  Used to mark polygons from the data window. */
void Canvas::MarkPolygon( double scx, double scy )
{

    double tpsc[1000] [3];
    tpsc[pcount] [0] =psc[pcount] [0] = scx;
    tpsc[pcount] [1] =psc[pcount] [1] = scy;
    tpsc[pcount] [2] = psc[pcount] [2] = 0;
    for ( int n = 0; n <= pcount; n++ )
        GLDrawSingleBNode( tpsc[n] [0], tpsc[n] [1], 255, 255, 0 );
    pcount++;
    if ( pcount >= 2 )
        GLDrawTransparentPolygon( tpsc, pcount, 200, 100, 0 ); //orange
}

/*! Ends the zoom and redraws the Canvas in the regular size. */
void Canvas::UnZoom()
{

    wxImage im;
    wxSize size;
    size=GetClientSize();
    im=image->ConvertToImage();
    im.Rescale(size.GetWidth(),size.GetHeight());
    image=new wxBitmap(im);
    SetVirtualSize(image->GetWidth(),image->GetHeight());
    CheckUnitCell();
    zoomfactor = 1;
    SetScrollbarsTicks();
    EllePlotRegions( 0 );
    leftclick = true;
}

/*! dir = direction 0 - up, 1 - down, 2 - left, 3 - right, 4 = just zoom in, 5 - just zoom out
lots of stuff changed because now the zoom is moved using mouse dragging*/
void Canvas::ChangeView( int dir )
{
    wxSize size;
    size=GetSize();
    switch ( dir ) {
    case 4:
        zoomfactor =0.8;
        break;
    case 5:
        zoomfactor = 1.2;
        break;
    default:
        break;
    }
    wxImage im;
    im=image->ConvertToImage();
    im.Rescale((int)(im.GetWidth()*zoomfactor),(int)(im.GetHeight()*zoomfactor));
    image=new wxBitmap(im);
    SetVirtualSize(image->GetWidth(),image->GetHeight());
    SetScrollbarsTicks();
    EllePlotRegions(0);
}
void Canvas::SetScrollbarsTicks()
{
    int width,height;
    if (isinit) {
        width=image->GetWidth();
        height=image->GetHeight();
        SetScrollbars((width/100)+1,(height/100)+1,100,100);
    }
}

wxBitmap* Canvas::GetImage(){
    return image;
}
