//---------------------------------------------------------------------------
//
// Name:        lz77App.h
// Author:      Derric
// Created:     1/27/2009 10:07:25 PM
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __LZ77DLGApp_h__
#define __LZ77DLGApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif


#include <fstream>
using namespace std;


class lz77DlgApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
		static void run(std::string inputfile,std::string outputfile,
            int in_offset,int out_offset,int size,int option );
		
};

#endif
