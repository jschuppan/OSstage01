#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <string>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include "linkedlist.h"
//#include <bits/stdc++.h> //Needed for memset

using namespace std;
void format();
void dump();
void dir();
int createFile(int threadID, char* fileName, int fileSize, char permission);
void testOutput();
string charToBin(unsigned char c);

class iNode {
    public:
//ddss
    char fileName[8];
    int ownerTaskID;
    int startingBlock;
    int size;
    int sequence;
    int nextIndex;
	int handle;

    char permission;
    unsigned int blocks[4];
    time_t createdOn;
    time_t modifiedOn;
};

struct openFiles
{
	//T_ID is the thread that opened file
	int T_ID;
	std::string filename;
	int fileID;
	char status;
};

linkedList<openFiles> openFileList;

int numberOfBlocks = 16;
iNode inodes[16];
string metaFileName = "metaTestFile";
string fsName = "dataTestFile";
int fsBlockSize = 128;
int nextFileHandle;
char initChar = '.';
int available = 0, used = 0;

int main()
{

 ifstream metaFile;
 metaFile.open("metaTestFile",ios::in);
 ifstream dataFile;
 dataFile.open("dataTestFile",ios::in);


 if (metaFile.fail() || dataFile.fail()) {  // one or more files don't exist
        printf("\nCouldn't open meta or data file, initializing with default values.\n");
        metaFile.close();
        format();  // format file system with initCha
    }
    else {  // file exists
		
        for (int i = 0; i < numberOfBlocks; i++) 
		{
			//Read from file
			metaFile.read((char*) &(inodes[i]), sizeof(iNode));

            if (inodes[i].ownerTaskID == -1) {
                available++;
            }
            else {
                used++;
            }
			
        }
        metaFile.close();
    }
	
	//metaFile.read((char*) &(inodes[i]), sizeof(iNode));
	//OUTPUT INODES READ FROM FILE WORKS
	//testOutput();
	
	//TESTING CREATEFILE
	int ownerID = 3;
	char* fileName = "sdf";
	int fileSize = 1500;
	char permission =0b0100;
	//testOutput();
	//cout<<"-----------------------------------------------------------"<<endl;
	if(createFile(ownerID, fileName, fileSize, permission))
		cout<<"FileCreated"<<endl;
	else
		cout<<"File Failed tp Create"<<endl;

	cout<<"size = " <<fileSize / 128.0<<endl;
	dump();
	dir();

	//testOutput();
}

void format() {
    // initialize and write to meta file
    std::ofstream metaFile(metaFileName.c_str());
    for (int i = 0; i < numberOfBlocks; i++) {

        // initialize
        memset(inodes[i].fileName, 0, sizeof( inodes[i].fileName ));
        inodes[i].ownerTaskID = -1;
        inodes[i].startingBlock = fsBlockSize * i;
        inodes[i].size = fsBlockSize;
        inodes[i].sequence = 0;
        inodes[i].nextIndex = -1;
        inodes[i].permission = 0b0000;
        memset(inodes[i].blocks, 0, sizeof( inodes[i].blocks ));
        inodes[i].createdOn = time(NULL);
        inodes[i].modifiedOn = time(NULL);

        // write to metaFile
        char *buffer = (char *) &(inodes[i]);
        int inodeSize = sizeof(iNode);
        for (int j = 0; j < inodeSize; j++) {
            metaFile.put( buffer[j] );
        }
    }
    metaFile.close();

    available = numberOfBlocks;
    used = 0;

    // initialize data file
    std::ofstream dataFile(fsName.c_str());
    int fileSize = numberOfBlocks * fsBlockSize;
    for (int i = 0; i < fileSize; i++) {
        dataFile << initChar;
    }
    dataFile.close();
}

int createFile(int ownerID, char* fileName, int fileSize, char permission) {
	std::fstream metaFile(metaFileName.c_str());
    if ( fileSize <=0)
		return 0;
	else if (fileSize < fsBlockSize && available > 0) 
	{
		for (int i = 0; i < numberOfBlocks; i++) 
		{	
		    if (inodes[i].ownerTaskID == -1) 
			{
		       	inodes[i].ownerTaskID = ownerID;
			   	inodes[i].permission = permission;
				for(int k=0; k<8; k++)
					inodes[i].fileName[k] = fileName[k];
				//ADD SEMAPHORE HERE
				metaFile.seekp(sizeof(iNode) * i);
				metaFile.write((char *) &(inodes[i]), sizeof(iNode));
				available--;
				used++;
		        return 1;
		    }
		}
	}
	else if(available >= fileSize / 128.0)
	{
		iNode* temp;
		int count = 1;
		for (int i = 0; i < numberOfBlocks; i++) 
		{	
		    if (inodes[i].ownerTaskID == -1 && count == 1) 
			{
				cout<<"IF number1"<<endl;
				//store previous iNode to store nextIndex
				temp = &inodes[i];
		       	inodes[i].ownerTaskID = ownerID;
			   	inodes[i].permission = permission;
				for(int k=0; k<8; k++)
					inodes[i].fileName[k] = fileName[k];
				count++;
				available--;
				used++;
				metaFile.seekp(sizeof(iNode) * i);
				metaFile.write((char *) &(inodes[i]), sizeof(iNode));
		    }
			//Second or more iNode
			else if(inodes[i].ownerTaskID == -1)
			{
				cout<<"IF number2"<<endl;
				temp->nextIndex = i;
				temp = &inodes[i];
				inodes[i].ownerTaskID = ownerID;
			   	inodes[i].permission = permission;
				inodes[i].sequence = count -1;
				//ADD SEMAPHORE HERE
				metaFile.seekp(sizeof(iNode) * i);
				metaFile.write((char *) &(inodes[i]), sizeof(iNode));
				available--;
				used++;
				for(int k=0; k<8; k++)
					inodes[i].fileName[k] = fileName[k];
				if(count >= fileSize / 128.0)
				{
					return 1;
				}
				count++;
			}
		}
	}
	return 0;
}

void testOutput()
{
	for(int i=0; i<numberOfBlocks; i++)
	{
		cout<<"ownerTaskID = "<<inodes[i].ownerTaskID<<endl; 
		cout<<"FileName = " << inodes[i].fileName<<endl;
		cout<<"startingBlock = "<<inodes[i].startingBlock<<endl;
		cout<<"size = "<<inodes[i].size<<endl;
		cout<<"sequennce = "<<inodes[i].sequence<<endl;
		cout<<"next index = "<<inodes[i].nextIndex<<endl; 
		cout<<"permission = "<<charToBin(inodes[i].permission)<<endl;
		cout<<"createdOn "<<inodes[i].createdOn<<endl;
		cout<<"modifiedOn = "<<inodes[i].modifiedOn<<endl;
		cout<<endl<<endl;
	}
}

string charToBin(unsigned char c) {
  unsigned char mask = 0x80;
  char binArr[9];
  for (int i = 0; i < 8; i++) {
    binArr[i] = (c & mask) ? '1' : '0';
    mask = mask >> 1;
  }
  binArr[8] = '\0';
  string result(binArr);
  return result;
}

void dump() {
	std::string fRow;
    std::fstream dataFile(fsName.c_str(), std::ios::in | std::ios::out);

	if (dataFile.is_open())
    {
      while (getline (dataFile, fRow))
      {
        std::cout << fRow << std::endl;
      }
    dataFile.close();
    }
}

void dir() {
    const char colFill        = ' ';
    const std::string colSep  = " | ";
	const int  colNameSm      = 5;
    const int  colNameMd      = 8;
	const int  colNameLg      = 14;
	const int  colNameXLg     = 17;
	char permBuff[5];

	std::time_t cDate, mDate;
	std::time_t initTime;
  	struct tm * timeStruct;
	//char* cDateOut;
	//char* mDateOut;

    //const int  colContent  = 8;

	// setting up the table
    std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Handle" << colSep;
    std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Name" << colSep;
	std::cout << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "Blocks used" << colSep;
	std::cout << std::left << std::setw(colNameSm) << std::setfill(colFill) <<  "Size" << colSep;
	std::cout << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "Starting block" << colSep;
	std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Status" << colSep;
	std::cout << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "Permission" << colSep;
	std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Owner ID" << colSep;
	std::cout << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  "Create Time" << colSep;
	std::cout << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  "Modified Time" << std::endl;


    for (int i = 0; i < numberOfBlocks; i++) {
		// time conversions
		char cDateBuff[100];
		char mDateBuff[100];

		// figure out time
        timeStruct = localtime (&inodes[i].createdOn);
        strftime (cDateBuff, 100 ,"%x %X", timeStruct);
		timeStruct = localtime (&inodes[i].modifiedOn);
        strftime (mDateBuff, 100 ,"%x %X", timeStruct);

		// figure out file permissions
		if (inodes[i].permission & 0b1000)
			permBuff[0] = 'r';
		else
			permBuff[0] = '-';

		if (inodes[i].permission & 0b0100)
			permBuff[1] = 'w';
		else
			permBuff[1] = '-';

		if (inodes[i].permission & 0b0010)
			permBuff[2] = 'r';
		else
			permBuff[2] = '-';

		if (inodes[i].permission & 0b0001)
			permBuff[3] = 'w';
		else
			permBuff[3] = '-';

		permBuff[4] = '\0';



		/* 
		cDate     =  std::time(&inodes[i].createdOn);
		cDateOut  =  std::asctime(std::localtime(&cDate));
		mDate     =  std::time(&inodes[i].modifiedOn);
		mDateOut  =  std::asctime(std::localtime(&mDate));
		*/

        // only list actual files
        if (inodes[i].ownerTaskID != -1) {
			// USE I FOR HANDLE TEMPORARILY ONLY !!!!!!!!!

			std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  i << colSep;
			std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  inodes[i].fileName << colSep;
			std::cout << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "xxxx" << colSep;
			std::cout << std::left << std::setw(colNameSm) << std::setfill(colFill) <<  inodes[i].size << colSep;
			std::cout << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  inodes[i].startingBlock << colSep;
			std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "unknown" << colSep;
			std::cout << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  permBuff << colSep;
			std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  inodes[i].ownerTaskID << colSep;
			std::cout << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  cDateBuff << colSep;
			std::cout << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  mDateBuff << std::endl;
        }
    }
}
