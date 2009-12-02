/*
Credits for this LZ77Compression algorithm belongs to Nintenlord

Changes have been made to the source code (along with it obviously being c++)
that allows exact compression of the Wii files.
Also changes have been made to make the code more memory efficient
*/

#include <vector>
#include <fstream>

using namespace std;



const int SlidingWindow=4096;
const int ReadAheadBuffer=18;
const int MIN_MATCH=3;//Minimum number of bytes that have to matched to go through with compression
const int BlockSize=8;

//To hold data about compressed bytes take 2 bytes or 16 bits
//Length represents the top 4 bits and have to be between MIN_MATCH and 15=(2^4-1)
//Offset represents the next 12 bits and have to be between 4096=(2^12-1) and MIN_MATCH
struct length_offset
{	
	int length;//The number of bytes compressed
	int offset;//How far back in sliding window where bytes that match the lookAheadBuffer is located

};

vector<unsigned char> Compress(ifstream &infile,int offset,int filelength,int length);

vector<unsigned char> Uncompress(ifstream &infile, int offset, bool &compressible, int inputsize);


length_offset Search(unsigned char* data, int position, int size);
length_offset Search(string &data, int position,int size);
length_offset window_search(const string& str_to_search, const string& str_to_look_for,int pos);
int rfind(const string& str_to_search, const string& str_to_look_for,int pos);
int compare(const char* str1, const char* str2, int n);

/*Paramaters are
infile - Name of the input file
outfile - name of the output file
offset - Offset to start compressing data
write_loc - offset to start writing compressed data
filelength - length of the input file
length - length of bytes from offset to attempt to compress data
*/
vector<unsigned char> Compress(ifstream &infile, int offset,int filelength,int length)
{

	//infile.seekg(offset);

/*
	unsigned char *filedata=new unsigned char[filelength];
	infile.read((char*)filedata,filelength);
	infile.close();
*/
	istreambuf_iterator<char> beg(infile);
	istreambuf_iterator<char> end;
	string filedata(beg,end);
	filedata.erase(0,offset);
	
	
	int position=0;
	
	unsigned char *pointer=(unsigned char*)&length;

	vector<unsigned char> compressedBytes;
	compressedBytes.push_back(0x10);
	compressedBytes.push_back(*(pointer++));
	compressedBytes.push_back(*(pointer++));
	compressedBytes.push_back(*(pointer++));



	while(position<length)
	{
		vector<unsigned char> tempBytes;//Holds the compressed bytes yet to be written
		unsigned char num_of_compressed_bytes=0;
		//In Binary represents 1 if byte is compressed or 0 if not compressed
		//For example 01001000 means that the second and fifth byte in the blockSize from the left is compressed



		for(int i=0;i < BlockSize;i++)
		{
			length_offset searchResult=Search(filedata,position,length);
			//If the number of bytes to be compressed is at least the size of the Minimum match 
			if(searchResult.length >= MIN_MATCH)
			{
				unsigned char temp=(unsigned char)((((searchResult.length - MIN_MATCH) & 0xF) << 4) + (((searchResult.offset - 1) >> 8) & 0xF));
				tempBytes.push_back(temp);//Pushed length to compressed bytes yet to be written
				
				temp=(unsigned char)((searchResult.offset-1) & 0xFF);
				tempBytes.push_back(temp);//Offset is now stored at the end of compressed bytes not written
				
				position+=searchResult.length;
				
				num_of_compressed_bytes |=(unsigned char)(1 << (7-i));
				//Stores which of the next 8 bytes is compressed
				//bit 1 for compress and bit 0 for not compressed
			}
			else if(searchResult.length >= 0)
			{
				tempBytes.push_back( filedata[position++] );
			
			}
			
			else
				break;
		}
		compressedBytes.push_back(num_of_compressed_bytes);
		compressedBytes.insert(compressedBytes.end(),tempBytes.begin(),tempBytes.end());

	}
	//delete [] filedata;

	return compressedBytes;

}


/*NOTE: not used
Searches for data that can be compressed using a slight modification to nintenlord algorithm
This algorithm works but it runs much slower than the other search function that takes
advantage of the string.rfind function
*/
length_offset Search(unsigned char* data, int position, int size)
{
	length_offset results={0,0};
	//Returns if in the first three or last three bytes of the file
	if( (position <MIN_MATCH)||( (size-position) <MIN_MATCH) )

		return results;

	//Returns negative 1 for Search failures since the current position is passed the size to be compressed
	if(position >=size)
	{
		results.length=-1;
		return results;
	}

	vector<int> match_results;

	for (int i = 1; ((i < SlidingWindow) && (i < position)); i++)
	{
		if (data[position - (i+1)] == data [position])
		{
			match_results.insert(match_results.begin(),(i+1));
		}
	}
	if (match_results.size() == 0)
		return results;

	int matched=1; //Holds the number of bytes matched

	string str_to_match;
	//The string that is being searched for from the look ahead buffer
	str_to_match=data[position+matched];

	//Adapted from Nintenlord LZ77 Compression source code
	//With a couple of tweaks
	while (matched < ReadAheadBuffer)
	{
		bool break_loop = false;
		for (int i = 0; i < match_results.size(); i++)
		{
			//Looks unsigned character by unsigned character forward for matches
			if (data[position + matched] != data[position - match_results[i] + ( matched % match_results[i] )] )
			{
				if (match_results.size() > 1)
				{
					match_results.erase( match_results.begin()+i );
					i--;
				}
				else
					break_loop = true;
			}
		}
		/*Break while loop if the number of results becomes 1 &&
		the number of matched bytes is less than the readAheadBuffer */
		if (break_loop)
			break;
		matched++;
	}
	results.length=matched;
	results.offset=match_results.back();

	return results;
}



/*This search function is my own work and is no way affilated with any one else
  I use the my own window_search function to drastically speed up the search function
  Normally a search for one byte is matched, then two, then three, all the way up
  to the size of the LookAheadBuffer. So I decided to skip the incremental search
  and search for the entire LookAheadBuffer and if I don't find the bytes are equal I return
  the next best match(which means if I look for 18 bytesand they are not found 18 characters did not match, and 17 characters did m

*/
length_offset Search(string &data,int position, int size)
{
	length_offset results={0,0};

	//Returns negative 1 for Search failures since the current position is passed the size to be compressed
	if(position >=size)
	{
		results.length=-1;
		return results;
	}
	//Returns if in the first three or last three bytes of the file
	if( (position <MIN_MATCH)||( (size-position) <MIN_MATCH) )
		return results;
	
	int search_position=0;//Position where lookAheadBuffer starts-The Position is relative with the Sliding Window
	int match_results=0;
	string search_window;
	//LookAheadBuffer is ReadAheadBuffer Size if the are more bytes than ReadAheadBufferSize waiting
	//to be compressed else the number of remaining bytes is the LookAheadBuffer
	int lookAheadBuffer_len=((size-position)<ReadAheadBuffer) ? (size-position) :ReadAheadBuffer;
	int sliding_buffer=(position - SlidingWindow);
	if(sliding_buffer>0)
	{	search_window=data.substr(sliding_buffer,SlidingWindow+lookAheadBuffer_len);
		search_position=SlidingWindow;
	}
	else
	{
		search_window=data.substr(0,position+lookAheadBuffer_len);
		search_position=position;
	}
	
	
	int matched=2; //Holds the number of bytes matched
	results=window_search(search_window,search_window.substr(search_position,lookAheadBuffer_len),search_position-MIN_MATCH+1);
	results.offset=search_position-results.offset;
	//A match has to be at least three bytes to be decompressed so their is no use in looking for a 
	//match that is less than 3
	/*
	string str_to_match;
	str_to_match.append(1,data[position]);
	str_to_match.append(1,data[position+1]);
	
	for (int i=2;matched<lookAheadBuffer_len;i++)
	{
		str_to_match.append(1,data[position+i]);
		
		

		if((sliding_buffer=rfind(search_window,str_to_match,search_position-MIN_MATCH+1)) != string::npos)	
		{
			matched++;
			
			match_results=search_position-sliding_buffer;
			
			
			
		}
		else
			break;



	}

	
	results.length=matched;
	results.offset=match_results;*/
	results.offset=search_position-results.offset;
	return results;
}

/*  Adapted form Nintenlord's c# uncompressed function

	infile- File to de-compress
	offset-position in infile to start de-compression
	compressible- flag to tell whether data can be de-compressed
	inputsize- size of the inputfile
	*/
vector<unsigned char> Uncompress(ifstream &infile, int offset, bool &compressible, int inputsize)
{
	infile.seekg(offset,ios_base::beg);
	char temp[4];
	infile.read(temp,4); //Size of data when it is uncompressed
	int filesize=*((int*)&temp);
	
	vector<unsigned char> UncompressedBytes;
	
	if((filesize & 0xFF)!=0x10)
	{
		compressible=false;	
		return UncompressedBytes;
	}
	
	filesize>>=8; //Gets rid of the 0x10 that starts all compressed files and also get
				 // the true file length

	UncompressedBytes.assign(filesize,0);//Holds all the uncompressed Data
	int uncompPosition=0; //Position to keep track of uncompressed data position
	
	
	while(uncompPosition < filesize && infile.tellg() < inputsize)
	{
		unsigned char isCompressed=infile.get();

		for(int i=0;i < BlockSize; i++)
		{
			//Checks to see if the next byte is compressed by looking 
			//at its binary representation - E.g 10010000
			//This says that the first extracted byte and the four extracted byte is compressed
			 if ((isCompressed & 0x80) != 0) 
									
			 {
				 unsigned char first=infile.get();
				 unsigned char second=infile.get();
				 //Offset to look for compressed data
				 unsigned short offset=(unsigned short) ((((first << 8) + second) & 0xFFF) + 1);
				 
				 //Number of bytes to copy to output stream
				 unsigned char length=(unsigned char) (MIN_MATCH+((first >> 4) & 0xF));

				 if(offset > uncompPosition)//If the offset to look for uncompressed is passed the current uncompresed data then the data is not compressed
				 {
					 compressible = false;
					 return UncompressedBytes;
				 }

				 for(int j=0;j< length;j++)
					 //UncompressedBytes[uncompPosition +j]=UncompressedBytes[uncompPosition-offset+(j % offset)];
					 UncompressedBytes[uncompPosition +j]=UncompressedBytes[uncompPosition-offset+j];
				 
				 uncompPosition+=length;
			 }
			 else
			 {
				 UncompressedBytes[uncompPosition++]=infile.get();
			 }
			 
			 if(uncompPosition >= filesize || infile.tellg() >= inputsize)
				 break;
			 
			 isCompressed<<=1;
		}
	}
	if(uncompPosition <filesize)
	{
		compressible=false;
		return UncompressedBytes;
	}
	
	return UncompressedBytes;
	
	
}

//Returns the full length of string2 if they are equal else
//Return the number of characters that were equal before they weren't equal
int submatch(const char* str1,const char* str2,const int& len){
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
  
*/
length_offset window_search(const string& str_to_search, const string& str_to_look_for,int pos){
	int size=str_to_search.length();
	int n=str_to_look_for.length();
	length_offset result={0,0};
	int temp=0;
	if(n>size)//If the string that is being looked for is bigger than the string that is being searched
		return result;
	
	/*This makes sure that search for the str_to_look_for can be searched if an invalid position is given
	  An invalid position occurs if the amount of characters to search in_str_to_search is less than the size
	  of str_to_look_for. In other words there has to be at least n characters left in the string 
	  to have a chance to find n characters*/
	pos=min(size-n,pos);
	do{	
		temp=submatch(str_to_search.c_str()+pos,str_to_look_for.c_str(),n);
		if(result.length<temp){
			result.length=temp;
			result.offset=pos;
			}
		if(result.length==n)
			return result;
		
		//ReadAheadBuffer is the maximum size of a character match
		
						
	
	}while(pos-->0);
	return result;

}

//Not in Use
int rfind(const string& str_to_search, const string& str_to_look_for,int pos){
	int size=str_to_search.length();
	int n=str_to_look_for.length();
	
	int result=0;
	if(n>size)//If the string that is being looked for is bigger than the string that is being searched
		return string::npos;
	
	/*This makes sure that search for the str_to_look_for can be searched if an invalid position is given
	  An invalid position occurs if the amount of characters to search in_str_to_search is less than the size
	  of str_to_look_for. In other words there has to be at least n characters left in the string 
	  to have a chance to find n characters*/
	pos=min(size-n,pos);
	do{	
		if(compare(str_to_search.c_str()+pos,str_to_look_for.c_str(),n)==0)
			return pos;
	
	}while(pos-->0);
	return string::npos;

}
//Not in Use
int compare(const char* str1, const char* str2, int n){
	for(int i=0;i<n;++i)
		if(str1[i]!=str2[i])
			return -1;
	return 0;
}
