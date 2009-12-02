//---------------------------------------------------------------------------
//
// Name:        lz77Dlg.h
// Author:      Derric
// Created:     1/27/2009 10:07:25 PM
// Description: lz77Dlg class declaration
//
//---------------------------------------------------------------------------

#ifndef __LZ77DLG_h__
#define __LZ77DLG_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dialog.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/filedlg.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
////Header Include End
#include <wx/regex.h>

////Dialog Style Start
#undef lz77Dlg_STYLE
#define lz77Dlg_STYLE wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End



class lz77Dlg : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		//Defines the numerical sizes
    	unsigned long size_to_compress;
        unsigned long in_offset;
        unsigned long out_offset;
        
        
		
	public:
		lz77Dlg(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("lz77 compressor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = lz77Dlg_STYLE);
		virtual ~lz77Dlg();
		void WxRadioBox1Click(wxCommandEvent& event);
		void lz77DlgActivate(wxActivateEvent& event);
		
		void WxButton1Click(wxCommandEvent& event);
		void WxButton2Click(wxCommandEvent& event);
		
		void BrowseButton1Click(wxCommandEvent& event);
		void BrowseButton2Click(wxCommandEvent& event);
		void InputStartOffUpdated1(wxCommandEvent& event);
		void OutputOffUpdated(wxCommandEvent& event);
		void SizeUpdated(wxCommandEvent& event);
	
	
	private:
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxFileDialog *WxOpenFileDialog1;
		wxFileDialog *WxOpenFileDialog2;
		wxStaticText *WxStaticText5;
		wxStaticText *WxStaticText4;
		wxTextCtrl *OutputOff;
		wxTextCtrl *Size;
		wxStaticBox *WxStaticBox1;
		wxButton *BrowseButton2;
		wxButton *BrowseButton1;
		wxTextCtrl *InputStartOff;
		wxStaticText *WxStaticText3;
		wxStaticText *WxStaticText2;
		wxButton *CloseButton;
		wxTextCtrl *Outputfile;
		wxTextCtrl *Inputfile;
		wxButton *OKButton;
		wxStaticText *WxStaticText1;
		wxStaticBox *Input_file_info;
		wxRadioBox *WxRadioBox1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXSTATICTEXT5 = 1024,
			ID_WXSTATICTEXT4 = 1023,
			ID_OUTPUT_OFF = 1020,
			ID_SIZE = 1019,
			ID_WXSTATICBOX1 = 1018,
			ID_BROWSEBUTTON2 = 1017,
			ID_BROWSEBUTTON1 = 1016,
			ID_INPUT_OFF = 1015,
			ID_WXSTATICTEXT3 = 1014,
			ID_WXSTATICTEXT2 = 1013,
			ID_CLOSEBUTTON = 1012,
			ID_OUTPUTFILE = 1010,
			ID_INPUTFILE = 1009,
			ID_OKBUTTON = 1008,
			ID_WXSTATICTEXT1 = 1007,
			ID_INPUT_FILE_INFO = 1006,
			ID_WXRADIOBOX1 = 1003,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		
};

#endif
