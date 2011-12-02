//---------------------------------------------------------------------------
//
// Name:        lz77App.cpp
// Author:      Derric
// Created:     1/27/2009 10:07:25 PM
// Description: lz77 compressor- Main use is to compress and decompress Nintendo *.cms and *.tpl files
//
//---------------------------------------------------------------------------


#include "lz77Frame.h"


class lz77App: public wxApp{
	virtual bool OnInit();
};

IMPLEMENT_APP(lz77App)
bool lz77App::OnInit(){
	lz77Frame* frame = new lz77Frame(NULL);
	//frame->Center();
	frame->Show(true);		
	return true;
}
 
