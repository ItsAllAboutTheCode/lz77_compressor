#ifndef __LZ77TYPE10__
#define __LZ77TYPE10__

#include "lzBase.h"

class lz77Type10 : public lzBase {
public:
	lz77Type10(int ReadAheadBuffer=18,int MinimumOffset=1,int SlidingWindow=4096,int MinimumMatch=3,int BlockSize=8);
	int Compress(const wxString& inStr, const wxString& outStr,unsigned long offset,unsigned long length);
	int Decompress(const wxString& inStr, const wxString& outStr,unsigned long offset);
	bool FileIsCompressed(const wxString& inStr, byte encoding=0x10, unsigned long offset=0);
	
};

#endif
