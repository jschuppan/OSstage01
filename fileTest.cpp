#include<iostream>
#include<cstdlib>
#include<fstream>
#include <ctime>
#include <string>
#include <stdio.h>
#include <cstring>
//#include <bits/stdc++.h> //Needed for memset

using namespace std;
void format();
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

    char permission;
    unsigned int blocks[4];
    time_t createdOn;
    time_t modifiedOn;
};


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
	testOutput();
	cout<<"-----------------------------------------------------------"<<endl;
	if(createFile(ownerID, fileName, fileSize, permission))
		cout<<"FileCreated"<<endl;
	else
		cout<<"File Failed tp Create"<<endl;

	cout<<"size = " <<fileSize / 128.0<<endl;

	testOutput();
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
