/*
Credits for this LZ77Compression algorithm belongs to Nintenlord

Changes have been made to the source code (along with it obviously being c++)
that allows exact compression of the Wii files.
Also changes have been made to make the code more memory efficient
*/

#include "wx/string.h"
#include "wx/ffile.h"

typedef unsigned char byte;



const int SlidingWindow=4096;
const int ReadAheadBuffer=18;
const int DictionaryBufffer=(1<<16)+275;
const int MIN_MATCH=3;//Minimum number of bytes that have to matched to go through with compression
const int BlockSize=8;

//To hold data about compressed bytes take 2 bytes or 16 bits
//Length represents the top 4 bits and have to be between 0 and 15=(2^4-1)
//Offset represents the next 12 bits and have to be between 4095(2^12-1) and 0
struct length_offset
{	
	unsigned int length;//The number of bytes compressed
	unsigned short offset;//How far back in sliding window where bytes that match the lookAheadBuffer is located

};

int Compress_LZ77_10(wxString inStr, wxString outStr,unsigned long offset,unsigned long length,unsigned int minOffset=1);
int DeCompress_LZ77_10(wxString inStr, wxString outStr,unsigned long offset);
int Compress_LZ77_11(wxString inStr, wxString outStr,unsigned long offset,unsigned long length,unsigned int minOffset=1);
int DeCompress_LZ77_11(wxString inStr, wxString outStr,unsigned long offset);


/*
Paramaters are
infile - Name of the input file
outfile - name of the output file
offset - Offset to start compressing data
length - length of bytes from offset to attempt to compress data

Return 0 on success
*/
int Compress_LZ77_10(wxString inStr,wxString outStr,unsigned long offset,unsigned long length,unsigned int minOffset)
{
	wxFFile infile,outfile;
	infile.Open(inStr,wxT("rb"));
	infile.Seek(offset);
	byte* filedata=new byte[length];
	size_t bytesRead;
	//Read file from the offset into filedata
	bytesRead=infile.Read(filedata,length);
	infile.Close();
	if(bytesRead!=length){
		delete []filedata;
		filedata=NULL;
		return -2;
	}


	outfile.Open(outStr, wxT("wb"));
	if(outfile.IsOpened()==false)	{
		delete []filedata;
		filedata=NULL;
		return -1;
	}
	
	int encodeSize=(length<<8)|(0x10);
	
	outfile.Write(&encodeSize,4);
	
	byte *ptrStart=filedata;
	byte *ptrEnd=(byte*)(filedata+length);
	
	//At most their will be two bytes written if the bytes can be compressed. So if all bytes in the block can be compressed it would take blockSize*2 bytes
	byte *compressedBytes=new byte[BlockSize *2];//Holds the compressed bytes yet to be written
	while( ptrStart< ptrEnd )
	{
		byte num_of_compressed_bytes=0;
		//In Binary represents 1 if byte is compressed or 0 if not compressed
		//For example 01001000 means that the second and fifth byte in the blockSize from the left is compressed
		byte *ptrBytes=compressedBytes;
		for(int i=0;i < BlockSize;i++)
		{
			length_offset searchResult=Search(filedata,ptrStart,ptrEnd,minOffset);
			//If the number of bytes to be compressed is at least the size of the Minimum match 
			if(searchResult.length >= MIN_MATCH)
			{	//Gotta swap the bytes since system is wii is big endian and most computers are little endian
				unsigned short len_off=wxINT16_SWAP_ON_LE( (((searchResult.length - MIN_MATCH) & 0xF) << 12) + ((searchResult.offset - 1) & 0xFFF) );
				memcpy(ptrBytes,&len_off,sizeof(short));
				ptrBytes+=sizeof(short);
								
				ptrStart+=searchResult.length;
				
				num_of_compressed_bytes |=(1 << (7-i));
				//Stores which of the next 8 bytes is compressed
				//bit 1 for compress and bit 0 for not compressed
			}
			else if(searchResult.length >= 0)
			{
				*ptrBytes++=*ptrStart++;
				
			
			}
			else
				break;
		}
		outfile.Write(&num_of_compressed_bytes,1);
		outfile.Write(compressedBytes,(size_t)(ptrBytes-compressedBytes));

	}
	delete []compressedBytes;
	delete []filedata;
	compressedBytes=NULL;
	filedata=NULL;
	int div4;
	//Add zeros until the file is a multiple of 4
	if((div4=outfile.Tell()%4) !=0 )
		outfile.Write("\0",4-div4);
	outfile.Close();
	return 0;

}

/*
	
	inStr- Input file to decompress
	outStr-Output file to decompress
	offset-position in infile to start de-compression
*/
int DeCompress_LZ77_10(wxString inStr,wxString outStr,unsigned long offset)
{
	wxFFile infile,outfile;
	infile.Open(inStr,wxT("rb"));
	
	infile.Seek(offset);
	int filesize;
	infile.Read(&filesize,4); //Size of data when it is uncompressed
	
	if((filesize & 0xFF)!=0x10)
	{
		infile.Close();
		return 1;//Not compressible
	}
	filesize>>=8; //Gets rid of the 0x10 that starts all compressed files and also get the true file length
	long inputsize=infile.Length()-offset-4;
	byte* filedata=new byte[inputsize];
	byte* buffer=filedata;
	size_t bytesRead;
	//Read file from the offset into filedata
	while((bytesRead=infile.Read(buffer,4096))>0){
		buffer+=bytesRead;
	}
	infile.Close();
		
	byte *uncompressedData=new byte[filesize];
	byte *outputPtr=uncompressedData;
	byte *outputEndPtr=uncompressedData+filesize;
	byte *inputPtr=filedata;
	byte *inputEndPtr=filedata+inputsize;
	while(inputPtr<inputEndPtr && outputPtr<outputEndPtr)
	{
	
		byte isCompressed=*inputPtr++;

		for(int i=0;i < BlockSize; i++)
		{
			//Checks to see if the next byte is compressed by looking 
			//at its binary representation - E.g 10010000
			//This says that the first extracted byte and the four extracted byte is compressed
			 if ((isCompressed>>(7-i)) & 0x1) 					
			 {
				 unsigned short len_off;
				 memcpy(&len_off,inputPtr,sizeof(short));
				 inputPtr+=sizeof(short);//Move forward two bytes
				 
				 len_off=wxINT16_SWAP_ON_LE(len_off);
								 
				 //length offset pair has bes been decoded.
				 length_offset decoding={(len_off>>12)+MIN_MATCH, (len_off & 0xFFF) + 1};
				 

				if((outputPtr - decoding.offset) < uncompressedData){//If the offset to look for uncompressed is passed the current uncompresed data then the data is not compressed
				 	delete []uncompressedData;
				 	delete []filedata;
					uncompressedData=NULL;
				 	filedata=NULL;
				 	return 2;
				 }
				for(int j=0;j<decoding.length;++j)
					outputPtr[j]=(outputPtr-decoding.offset)[j];
				outputPtr+=decoding.length;
			 }
			 else
				*outputPtr++=*inputPtr++;
			 		 
			 if(!(inputPtr<inputEndPtr && outputPtr<outputEndPtr))
			 	break;
			 	
			 
		}
	}
	
	outfile.Open(outStr, wxT("wb"));
	if(outfile.IsOpened()==false){
	
		delete []uncompressedData;
		delete []filedata;
		uncompressedData=NULL;
		filedata=NULL;
		return -1;
	}
	outfile.Write(uncompressedData,filesize);
	outfile.Close();
	delete []uncompressedData;
	delete []filedata;
	uncompressedData=NULL;
	filedata=NULL;
	return 0;
	
	
}

/*
Paramaters are
infile - Name of the input file\
outfile - name of the output file
offset - Offset to start compressing data
length - length of bytes from offset to attempt to compress data

Return 0 on success
*/
int Compress_LZ77_11(wxString inStr,wxString outStr,unsigned long offset,unsigned long length,unsigned int minOffset)
{
	wxFFile infile,outfile;
	infile.Open(inStr,wxT("rb"));
	infile.Seek(offset);
	byte* filedata=new byte[length];
	size_t bytesRead;
	//Read file from the offset into filedata
	bytesRead=infile.Read(filedata,length);
	infile.Close();
	if(bytesRead!=length){
		delete []filedata;
		filedata=NULL;
		return -2;
	}


	outfile.Open(outStr, wxT("wb"));
	if(outfile.IsOpened()==false)	{
		delete []filedata;
		filedata=NULL;
		return -1;
	}
	
	if (length>0xFFFFFF){// If length is greater than 24 bits
		int encodeFlag=0x11;
		outfile.Write(&encodeFlag,4);
		outfile.Write(&length,4);
		
	}
	else{
		int encodeSize=(length<<8)|(0x11);
		outfile.Write(&encodeSize,4);
	}
		
	byte *ptrStart=filedata;
	byte *ptrEnd=(byte*)(filedata+length);
	
	//At most their will be two bytes written if the bytes can be compressed. So if all bytes in the block can be compressed it would take blockSize*2 bytes
	byte *compressedBytes=new byte[BlockSize *2];//Holds the compressed bytes yet to be written
	while( ptrStart< ptrEnd )
	{
		byte num_of_compressed_bytes=0;
		//In Binary represents 1 if byte is compressed or 0 if not compressed
		//For example 01001000 means that the second and fifth byte in the blockSize from the left is compressed
		byte *ptrBytes=compressedBytes;
		for(int i=0;i < BlockSize;i++)
		{
			length_offset searchResult=Search(filedata,ptrStart,ptrEnd,minOffset);
			//If the number of bytes to be compressed is at least the size of the Minimum match 
			if(searchResult.length >= MIN_MATCH)
			{	//Gotta swap the bytes since system is wii is big endian and most computers are little endian
				unsigned short len_off=wxINT16_SWAP_ON_LE( (((searchResult.length - MIN_MATCH) & 0xF) << 12) + ((searchResult.offset - 1) & 0xFFF) );
				memcpy(ptrBytes,&len_off,sizeof(short));
				ptrBytes+=sizeof(short);
								
				ptrStart+=searchResult.length;
				
				num_of_compressed_bytes |=(1 << (7-i));
				//Stores which of the next 8 bytes is compressed
				//bit 1 for compress and bit 0 for not compressed
			}
			else if(searchResult.length >= 0)
			{
				*ptrBytes++=*ptrStart++;
				
			
			}
			else
				break;
		}
		outfile.Write(&num_of_compressed_bytes,1);
		outfile.Write(compressedBytes,(size_t)(ptrBytes-compressedBytes));

	}
	delete []compressedBytes;
	delete []filedata;
	compressedBytes=NULL;
	filedata=NULL;
	int div4;
	//Add zeros until the file is a multiple of 4
	if((div4=outfile.Tell()%4) !=0 )
		outfile.Write("\0",4-div4);
	outfile.Close();
	return 0;

}

/*
	
	inStr- Input file to decompress
	outStr-Output file to decompress
	offset-position in infile to start de-compression
*/
int DeCompress_LZ77_11(wxString inStr,wxString outStr,unsigned long offset)
{
	wxFFile infile,outfile;
	infile.Open(inStr,wxT("rb"));
	
	infile.Seek(offset);
	int filesize;
	infile.Read(&filesize,4); //Size of data when it is uncompressed
	
	if((filesize & 0xFF)!=0x10)
	{
		infile.Close();
		return 1;//Not compressible
	}
	filesize>>=8; //Gets rid of the 0x10 that starts all compressed files and also get the true file length
	long inputsize=infile.Length()-offset-4;
	byte* filedata=new byte[inputsize];
	byte* buffer=filedata;
	size_t bytesRead;
	//Read file from the offset into filedata
	while((bytesRead=infile.Read(buffer,4096))>0){
		buffer+=bytesRead;
	}
	infile.Close();
		
	byte *uncompressedData=new byte[filesize];
	byte *outputPtr=uncompressedData;
	byte *outputEndPtr=uncompressedData+filesize;
	byte *inputPtr=filedata;
	byte *inputEndPtr=filedata+inputsize;
	while(inputPtr<inputEndPtr && outputPtr<outputEndPtr)
	{
	
		byte isCompressed=*inputPtr++;

		for(int i=0;i < BlockSize; i++)
		{
			//Checks to see if the next byte is compressed by looking 
			//at its binary representation - E.g 10010000
			//This says that the first extracted byte and the four extracted byte is compressed
			 if ((isCompressed>>(7-i)) & 0x1) 					
			 {
				 unsigned short len_off;
				 memcpy(&len_off,inputPtr,sizeof(short));
				 inputPtr+=sizeof(short);//Move forward two bytes
				 
				 len_off=wxINT16_SWAP_ON_LE(len_off);
								 
				 //length offset pair has bes been decoded.
				 length_offset decoding={(len_off>>12)+MIN_MATCH, (len_off & 0xFFF) + 1};
				 

				if((outputPtr - decoding.offset) < uncompressedData){//If the offset to look for uncompressed is passed the current uncompresed data then the data is not compressed
				 	delete []uncompressedData;
				 	delete []filedata;
					uncompressedData=NULL;
				 	filedata=NULL;
				 	return 2;
				 }
				for(int j=0;j<decoding.length;++j)
					outputPtr[j]=(outputPtr-decoding.offset)[j];
				outputPtr+=decoding.length;
			 }
			 else
				*outputPtr++=*inputPtr++;
			 		 
			 if(!(inputPtr<inputEndPtr && outputPtr<outputEndPtr))
			 	break;
			 	
			 
		}
	}
	
	outfile.Open(outStr, wxT("wb"));
	if(outfile.IsOpened()==false){
	
		delete []uncompressedData;
		delete []filedata;
		uncompressedData=NULL;
		filedata=NULL;
		return -1;
	}
	outfile.Write(uncompressedData,filesize);
	outfile.Close();
	delete []uncompressedData;
	delete []filedata;
	uncompressedData=NULL;
	filedata=NULL;
	return 0;
	
	
}
