typedef unsigned char byte;

int submatch(const byte* str1,const byte* str2,const int& len);
length_offset Search(byte* data,byte* posPtr, byte* sizePtr,unsigned int minOffset);
length_offset window_search(byte* beginSearchPtr, byte* searchPosPtr,byte* endLABufferPtr, byte* startLBPtr);


/*This search function is my own work and is no way affilated with any one else
  I use the my own window_search function to drastically speed up the search function
  Normally a search for one byte is matched, then two, then three, all the way up
  to the size of the LookAheadBuffer. So I decided to skip the incremental search
  and search for the entire LookAheadBuffer and if I don't find the bytes are equal I return
  the next best match(which means if I look for 18 bytes and they are not found 18 characters did not match, and 17 characters did m

*/
length_offset Search(byte* data,byte* posPtr, byte* sizePtr,unsigned int minOffset)
{
	length_offset results={0,0};

	//Returns negative 1 for Search failures since the current position is passed the size to be compressed
	if(posPtr >=sizePtr)
	{
		results.length=-1;
		return results;
	}
	//Returns if in the first three or last three bytes of the file
	if( (posPtr-data<MIN_MATCH)||( sizePtr-posPtr <MIN_MATCH) )
		return results;
	
	byte* search_window;
	//LookAheadBuffer is ReadAheadBuffer Size if there are more bytes than ReadAheadBufferSize waiting
	//to be compressed else the number of remaining bytes is the LookAheadBuffer
	int lookAheadBuffer_len=((int)(sizePtr-posPtr)<ReadAheadBuffer) ? (int)(sizePtr-posPtr) :ReadAheadBuffer;
	int sliding_buffer=(int)(posPtr - data)-SlidingWindow;
	if(sliding_buffer>0)
		search_window=data+sliding_buffer;	
	else
		search_window=data;
	
	byte* endPos=posPtr+lookAheadBuffer_len;
	
	results=window_search(search_window,posPtr,endPos,posPtr-minOffset);
	return results;
}


//Returns the full length of string2 if they are equal else
//Return the number of characters that were equal before they weren't equal
int submatch(const byte* str1,const byte* str2,const int& len){
	for(int i=0;i<len;++i)
		if(str1[i]!=str2[i])
			return i;
	
	return len;
}

/*
Normally a search for one byte is matched, then two, then three, all the way up
  to the size of the LookAheadBuffer. So I decided to skip the incremental search
  and search for the entire LookAheadBuffer and if the function doesn't find the bytes are 
  equal the function return the next best match(which means if the function look for 18 bytes and they are not found, return
  the number of bytes that did match before it failed to match. The submatch is function returns the number of bytes that
  were equal, which can result up to the bytes total length if both byte strings are equal.
  
  
  ...[][][][][][][][][][][][]|[][][][][][][][][][][][][][]
                             |
     Search Window       Current Pos   LookAheadBuffer
    Up to 4096 bytes                   Up to 18 bytes
    			Sliding Window
    			Up to 4114 bytes
*/
length_offset window_search(byte* beginSearchPtr, byte* searchPosPtr,byte* endLABufferPtr, byte* startLBPtr){
	int size=endLABufferPtr-beginSearchPtr;//Size of the entire sliding window
	int n=endLABufferPtr-searchPosPtr;
	length_offset result={0,0};
	int temp=0;
	if(n>size)//If the string that is being looked for is bigger than the string that is being searched
		return result;

	/*This makes sure that search for the searchPosPtr can be searched if an invalid position is given
	  An invalid position occurs if the amount of characters to search in_beginSearchPtr is less than the size
	  of searchPosPtr. In other words there has to be at least n characters left in the string 
	  to have a chance to find n characters*/
	
	do{	
		temp=submatch(startLBPtr,searchPosPtr,n);
		if(result.length<temp){
			result.length=temp;
			result.offset=(int)(searchPosPtr-startLBPtr);
		}
		if(result.length==n)
			return result;
		
		//ReadAheadBuffer is the maximum size of a character match
		
						
	
	}while(startLBPtr-->beginSearchPtr);
	return result;
}

