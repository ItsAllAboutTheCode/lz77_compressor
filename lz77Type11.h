#ifndef __LZ77TYPE11__
#define __LZ77TYPE11__


#include "lzBase.h"

class lz77Type11 : public lzBase {
public:
	lz77Type11(int ReadAheadBuffer=(0xF+0xFF+0xFFFF+3),int MinimumOffset=1,int SlidingWindow=4096,int MinimumMatch=3,int BlockSize=8);
	int Compress(const wxString& inStr, const wxString& outStr,unsigned long offset,unsigned long length);
	int Decompress(const wxString& inStr, const wxString& outStr,unsigned long offset);
	bool FileIsCompressed(const wxString& inStr, byte encoding=0x11, unsigned long offset=0);
	
	
};

#endif
