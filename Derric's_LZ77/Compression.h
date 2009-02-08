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
const int MIN_MATCH=2;
const int BlockSize=8;

struct length_offset
{
	int length;
	int offset;

};

vector<unsigned char> Compress(ifstream &infile,int offset,int filelength,int length);

vector<unsigned char> Uncompress(ifstream &infile, int offset, bool &compressible, int inputsize);


length_offset Search(unsigned char* data, int position, int size);
length_offset Search(string &data, int position,int size);

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
			if(searchResult.length > MIN_MATCH)
			{
				unsigned char temp=(unsigned char)((((searchResult.length - 3) & 0xF) << 4) + (((searchResult.offset - 1) >> 8) & 0xF));
				tempBytes.push_back(temp);//Pushed length to compressed bytes yet to be writtem
				
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
	if( (position <3)||( (size-position) <3) )

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
I use the string rfind function to drastically speed up the search function
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
	if( (position <3)||( (size-position) <3) )
		return results;

	int search_position=0;
	int match_results=0;
	string search_window;

	int temp=(position - SlidingWindow);
	if(temp>0)
	{	search_window=data.substr(temp,SlidingWindow);
		search_position=SlidingWindow;
	}
	else
	{
		search_window=data.substr(0,position);
		search_position=position;
	}
	
	
	int matched=0; //Holds the number of bytes matched
	string str_to_match;

	for (int i=0;matched<ReadAheadBuffer && (position+i)<size;i++)
	{
		str_to_match.append(1,data[position+i]);
		
		if((temp=search_window.rfind(str_to_match,search_position-MIN_MATCH)) != string::npos)
		{
			matched++;
			//search_position++;
			match_results=search_position-temp;
			search_window.append(1,str_to_match[str_to_match.length()-1]);
			
			
		}
		else
			break;



	}

	
	results.length=matched;
	results.offset=match_results;

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
				 unsigned short position=(unsigned short) ((((first << 8) + second) & 0xFFF) + 1);
				 
				 //Number of bytes to copy to output stream
				 unsigned char num_to_copy=(unsigned char) (3+((first >> 4) & 0xF));

				 if(position > uncompPosition)//If the position to uncompressed data is passed t
				 {
					 compressible = false;
					 return UncompressedBytes;
				 }

				 for(int j=0;j< num_to_copy;j++)
					 UncompressedBytes[uncompPosition +j]=UncompressedBytes[uncompPosition-position+(j % position)];
				 
				 uncompPosition+=num_to_copy;
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

