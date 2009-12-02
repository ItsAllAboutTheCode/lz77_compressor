//---------------------------------------------------------------------------
//
// Name:        lz77App.cpp
// Author:      Derric
// Created:     1/27/2009 10:07:25 PM
// Description: lz77 compressor- Main use is to compress and decompress Nintendo *.cms and *.tpl files
//
//---------------------------------------------------------------------------

#include "Compression.h"
#include "lz77App.h"
#include "lz77Dlg.h"



#define compress 0
#define uncompress 1

IMPLEMENT_APP(lz77DlgApp)

bool lz77DlgApp::OnInit()
{
	lz77Dlg* dialog = new lz77Dlg(NULL);
	SetTopWindow(dialog);
	dialog->Show(true);		
	return true;
}
 
int lz77DlgApp::OnExit()
{
	return 0;
}

/*Precondtions: The functions receive the parameters in the following order
inputfile - name of the file to either compress or uncompress
outputfile - name of the file that will get the newly compressed or uncompressed data
in_offset - offset to start compressing or uncompressing data from inputfile: default=0;
out_offset - offset to place compressed or uncompressed data in outputfile:default=0
size - Number of bytes from offset to compress or uncompress
*/

void lz77DlgApp::run(std::string inputfile,std::string outputfile,int in_offset,int out_offset,int size,int option )
{
	wxString message;
	int filesize;
	ifstream ifile;
	ofstream ofile;
	
	
	ifile.open(inputfile.c_str(),ios_base::binary);
		
	if(!ifile)
	{
		wxMessageBox(wxT("Error opening input file\nfile may be in use by another program or may not exist"),wxT("Results"));
		return;
	}
	
	ifile.seekg(0,ios::end);
	filesize=ifile.tellg();
	ifile.seekg(0,ios::beg);
	
	if(filesize<in_offset)
	{
		wxMessageBox(wxT("Offset is bigger than the size of the file"),wxT("Results"));
		ifile.close();
		return;
	}
	else if(filesize<size)
	{
		wxMessageBox(wxT("Size to compress is bigger than the total size of the file"),wxT("Results"));
		ifile.close();
		return;
	}
	else if(filesize<in_offset+size)
	{
		wxMessageBox(wxT("Offset + size to compress is bigger than the total size of the file"),wxT("Results"));
		ifile.close();
		return;
	}
	
	if(size<=0 || size>(filesize-in_offset))
	{
		size=filesize-in_offset;
	}

	
	ofile.open(outputfile.c_str(),ios_base::binary);
	
	
	if(!ofile)
	{
		wxMessageBox(wxT("Error opening output file, file may be in use by another program"),wxT("Results"));
		ifile.close();
		return;
	}

	
	

	switch(option)
	{
	case compress:
		{
			
			vector<unsigned char> compressedData =Compress(ifile,in_offset,filesize,size);
			ofile.seekp(out_offset);
			vector<unsigned char>::iterator beg=compressedData.begin();
			vector<unsigned char>::iterator end=compressedData.end();
			while(beg !=end)
			{
				ofile.put(*beg);
				beg++;
			}
			while( (ofile.tellp()%4) !=0)
				ofile.put(0);
			break;
		}
	case uncompress:
		{
			bool can_be_uncompressed=true;
			vector<unsigned char> UncompressedData =Uncompress(ifile,in_offset,can_be_uncompressed, filesize);
			if(can_be_uncompressed)
			{
				ofile.seekp(out_offset);
				vector<unsigned char>::iterator beg=UncompressedData.begin();
				vector<unsigned char>::iterator end=UncompressedData.end();
				while(beg !=end)
				{
					ofile.put(*beg);
					beg++;
				}
				
			}
			else 
			{
				message=wxT("The data at input offset can not be uncompressed");
			}
			break;
		}

	}

	if (message.length()>0)
		wxMessageBox(message,wxT("Results"));
	else
		wxMessageBox(wxT("Finished"),wxT("Results"));

	
	ofile.close();
}
