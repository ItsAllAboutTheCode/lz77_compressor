#ifndef __LZ77BASE__
#define __LZ77BASE__

#include "wx/string.h"
#include "wx/ffile.h"
#include "enum.h"
typedef unsigned char byte;

struct length_offset
{	
	int length;//The number of bytes compressed
	unsigned short offset;//How far back in sliding window where bytes that match the lookAheadBuffer is located

};
enum class compressedType:int{
		None=-1,
		LZ00,
		LZ01,
		LZ10,
		LZ11
};

class lzBase {
public:
	lzBase(int ReadAheadBuffer=18,int MinimumOffset=1,int SlidingWindow=4096,int MinimumMatch=3,int BlockSize=8);
	virtual enumCompressionResult Compress(const wxString& inStr, const wxString& outStr,unsigned long offset,unsigned long length)=0;
	virtual enumCompressionResult Decompress(const wxString& inStr, const wxString& outStr,unsigned long offset)=0;
	bool FileIsCompressed(const wxString& inStr, byte encoding, unsigned long offset=0);
	unsigned int decompressedFileLength(const wxString& inStr,unsigned long offset=0);
	static compressedType FileType(const wxString& inStr,unsigned long offset=0);
	
	void setSlidingWindow(int SlidingWindow){m_iSlidingWindow=SlidingWindow;}
	void setReadAheadBuffer(int ReadAheadBuffer){m_iReadAheadBuffer=ReadAheadBuffer;}
	void setMIN_MATCH(int MinimumMatch){m_iMIN_MATCH=MinimumMatch;}
	void setBlockSize(int BlockSize){m_iBlockSize=BlockSize;}
	void setMinimumOffset(unsigned int MinimumOffset){m_uiMinOffset=MinimumOffset;}
	int getSlidingWindow(){return m_iSlidingWindow;}
	int getReadAheadBuffer(){return m_iReadAheadBuffer;}
	int getMIN_MATCH(){return m_iMIN_MATCH;}
	int getBlockSize(){return m_iBlockSize;}
	unsigned getMinimumOffset(){return m_uiMinOffset;}
	
	
private:
	
	int submatch(const byte* str1,const byte* str2,const int& len);
	length_offset window_search(byte* beginSearchPtr, byte* searchPosPtr,byte* endLABufferPtr, byte* startLBPtr);
protected:
	length_offset Search(byte* data,byte* posPtr, byte* sizePtr);
	unsigned int seekPosition;
	
	int m_iSlidingWindow;
	int m_iReadAheadBuffer;
	int m_iMIN_MATCH;//Minimum number of bytes that have to matched to go through with compression
	int m_iBlockSize;
	unsigned int m_uiMinOffset;


};

#endif
