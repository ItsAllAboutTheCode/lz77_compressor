//---------------------------------------------------------------------------
//
// Name:        lz77Dlg.cpp
// Author:      Derric
// Created:     1/27/2009 10:07:25 PM
// Description: lz77Dlg class implementation
//
//---------------------------------------------------------------------------

#include "lz77Dlg.h"
#include "lz77App.h"


//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// lz77Dlg
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(lz77Dlg,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(lz77Dlg::OnClose)
	EVT_ACTIVATE(lz77Dlg::lz77DlgActivate)
	
	EVT_TEXT(ID_OUTPUT_OFF,lz77Dlg::OutputOffUpdated)
	
	EVT_TEXT(ID_SIZE,lz77Dlg::SizeUpdated)
	EVT_BUTTON(ID_BROWSEBUTTON2,lz77Dlg::BrowseButton2Click)
	EVT_BUTTON(ID_BROWSEBUTTON1,lz77Dlg::BrowseButton1Click)
	
	EVT_TEXT(ID_INPUT_OFF,lz77Dlg::InputStartOffUpdated1)
	EVT_BUTTON(ID_CLOSEBUTTON,lz77Dlg::WxButton2Click)
	EVT_BUTTON(ID_OKBUTTON,lz77Dlg::WxButton1Click)
	EVT_TEXT_ENTER(ID_OUTPUTFILE,lz77Dlg::WxButton1Click)
	EVT_TEXT_ENTER(ID_INPUTFILE,lz77Dlg::WxButton1Click)
	EVT_TEXT_ENTER(ID_INPUT_OFF,lz77Dlg::WxButton1Click)
	EVT_TEXT_ENTER(ID_SIZE,lz77Dlg::WxButton1Click)
	EVT_TEXT_ENTER(ID_OUTPUT_OFF,lz77Dlg::WxButton1Click)

END_EVENT_TABLE()
////Event Table End

lz77Dlg::lz77Dlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
	size_to_compress=0;
	in_offset=0;
	out_offset=0;
	
}



lz77Dlg::~lz77Dlg()
{
} 


void lz77Dlg::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetTitle(wxT("FE lz77 compressor"));
	SetIcon(wxNullIcon);
	SetSize(8,8,444,336);
	Center();
	

	WxOpenFileDialog1 =  new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.cms and tpl files(*.cms;*.tpl)|*.cms;*.tpl|All Files|*.*"), wxOPEN);

	WxOpenFileDialog2 =  new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.tpl and cms files(*.cms;*.tpl)|*.cms;*.tpl|All Files|*.*"), wxSAVE);

	WxStaticText5 = new wxStaticText(this, ID_WXSTATICTEXT5, wxT("Size from Offset"), wxPoint(303, 81), wxDefaultSize, 0, wxT("WxStaticText5"));

	WxStaticText4 = new wxStaticText(this, ID_WXSTATICTEXT4, wxT("Offset(HEX)"), wxPoint(313, 187), wxDefaultSize, 0, wxT("WxStaticText4"));

	wxArrayString arrayStringFor_WxRadioBox1;
	arrayStringFor_WxRadioBox1.Add(wxT("Compress"));
	arrayStringFor_WxRadioBox1.Add(wxT("Un-Compress"));
	
	WxRadioBox1 = new wxRadioBox(this, ID_WXRADIOBOX1, wxT("Options"), wxPoint(12, 11), wxSize(133, 72), arrayStringFor_WxRadioBox1, 1, wxRA_SPECIFY_COLS, wxDefaultValidator, wxT("WxRadioBox1"));
	WxRadioBox1->SetSelection(0);

	OutputOff = new wxTextCtrl(this, ID_OUTPUT_OFF, wxT(""), wxPoint(305, 208), wxSize(78, 19), wxTE_PROCESS_ENTER, wxDefaultValidator, wxT("OutputOff"));

	Size = new wxTextCtrl(this, ID_SIZE, wxT(""), wxPoint(304, 106), wxSize(80, 19), wxTE_PROCESS_ENTER, wxDefaultValidator, wxT("Size"));

	WxStaticBox1 = new wxStaticBox(this, ID_WXSTATICBOX1, wxT("Output file info"), wxPoint(288, 160), wxSize(112, 110));

	BrowseButton2 = new wxButton(this, ID_BROWSEBUTTON2, wxT("Browse"), wxPoint(144, 164), wxSize(75, 25), 0, wxDefaultValidator, wxT("BrowseButton2"));

	BrowseButton1 = new wxButton(this, ID_BROWSEBUTTON1, wxT("Browse"), wxPoint(142, 113), wxSize(75, 25), 0, wxDefaultValidator, wxT("BrowseButton1"));

	InputStartOff = new wxTextCtrl(this, ID_INPUT_OFF, wxT(""), wxPoint(304, 56), wxSize(80, 19), wxTE_PROCESS_ENTER, wxDefaultValidator, wxT("InputStartOff"));

	WxStaticText3 = new wxStaticText(this, ID_WXSTATICTEXT3, wxT("Input file"), wxPoint(16, 104), wxDefaultSize, 0, wxT("WxStaticText3"));

	WxStaticText2 = new wxStaticText(this, ID_WXSTATICTEXT2, wxT("Output file"), wxPoint(16, 152), wxDefaultSize, 0, wxT("WxStaticText2"));

	CloseButton = new wxButton(this, ID_CLOSEBUTTON, wxT("Close"), wxPoint(206, 210), wxSize(60, 35), 0, wxDefaultValidator, wxT("CloseButton"));

	Outputfile = new wxTextCtrl(this, ID_OUTPUTFILE, wxT(""), wxPoint(16, 168), wxSize(121, 19), wxTE_PROCESS_ENTER, wxDefaultValidator, wxT("Outputfile"));

	Inputfile = new wxTextCtrl(this, ID_INPUTFILE, wxT(""), wxPoint(16, 120), wxSize(121, 19), wxTE_PROCESS_ENTER, wxDefaultValidator, wxT("Inputfile"));

	OKButton = new wxButton(this, ID_OKBUTTON, wxT("OK"), wxPoint(135, 210), wxSize(49, 35), 0, wxDefaultValidator, wxT("OKButton"));

	WxStaticText1 = new wxStaticText(this, ID_WXSTATICTEXT1, wxT("Offset(HEX)"), wxPoint(313, 35), wxDefaultSize, 0, wxT("WxStaticText1"));

	Input_file_info = new wxStaticBox(this, ID_INPUT_FILE_INFO, wxT("Input file info"), wxPoint(288, 16), wxSize(113, 121));

	////GUI Items Creation End
	
	
	Inputfile->MoveAfterInTabOrder(WxRadioBox1);
	BrowseButton1->MoveAfterInTabOrder(Inputfile);
	Outputfile->MoveAfterInTabOrder(BrowseButton1);
	BrowseButton2->MoveAfterInTabOrder(Outputfile);
	InputStartOff->MoveAfterInTabOrder(BrowseButton2);
	Size->MoveAfterInTabOrder(InputStartOff);
	OutputOff->MoveAfterInTabOrder(Size);
	OKButton->MoveAfterInTabOrder(OutputOff);
	CloseButton->MoveAfterInTabOrder(OKButton);
}

void lz77Dlg::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

/*
* WxRadioBox1Click
*/
void lz77Dlg::WxRadioBox1Click(wxCommandEvent& event)
{
	// insert your code here

}

/*
* lz77DlgActivate
*/
void lz77Dlg::lz77DlgActivate(wxActivateEvent& event)
{
	// insert your code here
}

/*
* WxButton1Click-OK Button
*/
void lz77Dlg::WxButton1Click(wxCommandEvent& event)
{
	// insert your code here
	
	lz77DlgApp::run(std::string(Inputfile->GetValue()),std::string(Outputfile->GetValue()),in_offset
		,out_offset,size_to_compress,WxRadioBox1->GetSelection());
}

/*
* WxButton2Click-Close Button
*/
void lz77Dlg::WxButton2Click(wxCommandEvent& event)
{


	// insert your code here
	Close();
}

/*
* BrowseButton1Click
*/
void lz77Dlg::BrowseButton1Click(wxCommandEvent& event)
{
	// Gets input filename from user

	WxOpenFileDialog1->ShowModal();
	if	(WxOpenFileDialog1->GetFilename().length()<1)
		return;

	Inputfile->SetValue(WxOpenFileDialog1->GetPath());
}

/*
* BrowseButton2Click
*/
void lz77Dlg::BrowseButton2Click(wxCommandEvent& event)
{
	// insert your code here
	WxOpenFileDialog2->ShowModal();
	if	(WxOpenFileDialog2->GetFilename().length()<1)
		return;

	Outputfile->SetValue(WxOpenFileDialog2->GetPath());
}



/*
* InputStartOffUpdated1
*/
void lz77Dlg::InputStartOffUpdated1(wxCommandEvent& event)
{
	// insert your code here
	
	
	wxRegEx match("(^(0x)?[0-9]|[a-f]|[A-F]+$)");
	bool results=false;
	results=match.Matches(InputStartOff->GetValue());
	if (results)
	{
		
		InputStartOff->GetValue().ToULong(&in_offset,16);
	}
	else
		in_offset=0;
}

/*
* OutputOffUpdated
*/
void lz77Dlg::OutputOffUpdated(wxCommandEvent& event)
{
	// insert your code here
	wxRegEx match("(^(0x)?[0-9]|[a-f]|[A-F]+$)");
	bool results=false;
	results=match.Matches(OutputOff->GetValue());
	if (results)
	{
		
		OutputOff->GetValue().ToULong(&out_offset,16);
	}
	else
		out_offset=0;

}

/*
* SizeUpdated
*/
void lz77Dlg::SizeUpdated(wxCommandEvent& event)
{
	// insert your code here
	wxRegEx match("(^[0-9]+$)");
	bool results=false;
	results=match.Matches(Size->GetValue());
	if (results)
	{
		
		Size->GetValue().ToULong(&in_offset,16);
	}
	else
		size_to_compress=0;


}
