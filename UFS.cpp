/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/

#include "UFS.h"
#include "iNode.h"
#include "window.h"
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <iomanip>

// constant array corresponding to iNode bitmap
static unsigned short int B_ALLOC[16] = { 0b1000000000000000,
                                                0b0100000000000000,
                                                0b0010000000000000,
                                                0b0001000000000000,
                                                0b0000100000000000,
                                                0b0000010000000000,
                                                0b0000001000000000,
                                                0b0000000100000000,
                                                0b0000000010000000,
                                                0b0000000001000000,
                                                0b0000000000100000,
                                                0b0000000000010000,
                                                0b0000000000001000,
                                                0b0000000000000100,
                                                0b0000000000000010,
                                                0b0000000000000001 };


UFS::UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar) {
    this->fsName = fsName + ".txt";  // ex: ultima.txt
    this->metaFileName = "meta" + fsName + ".txt";  // ex: metaultima.txt
    this->numberOfBlocks = numberOfBlocks;
    this->fsBlockSize = fsBlockSize;
    this->initChar = initChar;

    this->available = 0;
    this->used = 0;
    this->nextFileID = 0;
    this->nextFileHandle = 0;

    // create iNode corresponding to UFS here
        // iNode crINODE();
    inodes = new iNode[ numberOfBlocks ];  // like this?

    std::ifstream metaFile(metaFileName.c_str());
    std::ifstream dataFile(fsName.c_str());

    if (metaFile.fail() || dataFile.fail()) {  // one or more files don't exist
        printf("\nCouldn't open meta or data file, initializing with default values.\n");
        metaFile.close();
        format();  // format file system with initChar
    }
    else {  // file exists
        
        // read iNode from metaFile
        metaFile.read((char *) inodes, sizeof(inodes));
        metaFile.close();

        for (int i = 0; i < numberOfBlocks; i++) {

            inodes[i].blocks = B_ALLOC[i];
            if (inodes[i].ownerTaskID == -1) {
                (this->available)++;
            }
            else {
                (this->used)++;
            }
        }
    }
}


/*-----------------------------------------------------------------
Function      : format();
Parameters    : 
Returns       : 
Details       : initializes the iNodes, metaFile and dataFile with default values
------------------------------------------------------------------*/
void UFS::format() {
    // initialize and write to meta file
    for (int i = 0; i < numberOfBlocks; i++) {

        // initialize
        memset(inodes[i].fileName, 0, sizeof( inodes[i].fileName ));
        inodes[i].ownerTaskID = -1;
        inodes[i].startingBlock = fsBlockSize * i;
        inodes[i].size = fsBlockSize;
        inodes[i].sequence = 0;
        inodes[i].handle = -1;
        inodes[i].nextIndex = -1;
        inodes[i].permission = 0b0000;
        inodes[i].blocks = 0;
        inodes[i].createdOn = time(NULL);
        inodes[i].modifiedOn = time(NULL);
    }

    // write to metaFile
    std::ofstream metaFile(metaFileName.c_str());
    metaFile.write((char *) inodes, sizeof(inodes));
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


/*-----------------------------------------------------------------
Function      : openFile()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::openFile(int threadID, int fileHandle, std::string fileName, char mode) {
    for (int i = 0; i < numberOfBlocks; i++) {
        
        // if file exists
        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0) {
            
            // owner wants to open
            if (inodes[i].ownerTaskID == threadID || fileHandle == inodes[i].handle) {

                // asking for read without read permission
                if ((mode & READ) && !(inodes[i].permission & OWNER_READ)) {
                    return -1;
                }

                // asking for write without write permission
                if ((mode & WRITE) && !(inodes[i].permission & OWNER_WRITE)) {
                    return -1;
                }
            }
        
            // other wants to open
            else {

                // asking for read without read permission
                if ((mode & READ) && !(inodes[i].permission & OTHER_READ)) {
                    return -1;
                }

                // asking for write without write permission
                if ((mode & WRITE) && !(inodes[i].permission & OTHER_WRITE)) {
                    return -1;
                }
            }
            //success (unique int file_id?)
            openFiles tempNode;
            tempNode.T_ID = threadID;
            tempNode.filename = fileName;
            tempNode.fileID = nextFileID;
            tempNode.status = mode;
            openFileList.addToFront(tempNode, nextFileID);
            return nextFileID++;
        }
    }
    // file doesn't exist
    return -1;
}


/*-----------------------------------------------------------------
Function      : closeFile()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::closeFile(int threadID, int fileID) {
    
}


/*-----------------------------------------------------------------
Function      : readChar()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::readChar(int threadID, std::string fileName, char &c) {

}


/*-----------------------------------------------------------------
Function      : writeChar()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::writeChar(int threadID, std::string fileName, char c) {

}


/*-----------------------------------------------------------------
Function      : createFile()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::createFile(int threadID, std::string fileName, int fileSize, char permission) {
    	std::fstream metaFile(metaFileName.c_str());
    if ( fileSize <=0)
		return 0;
    //Requested Block is less that 128
	else if (fileSize < fsBlockSize && available > 0) 
	{
		for (int i = 0; i < numberOfBlocks; i++) 
		{	
		    if (inodes[i].ownerTaskID == -1) 
			{
		       	inodes[i].ownerTaskID = threadID;
			   	inodes[i].permission = permission;
				for(int k=0; k<8; k++)
					inodes[i].fileName[k] = fileName[k];
				//ADD SEMAPHORE HERE
				metaFile.seekp(sizeof(iNode) * i);
				metaFile.write((char *) &(inodes[i]), sizeof(iNode));
				available--;
				used++;
		        return nextFileHandle++;
		    }
		}
	}
    //Requested Block is greater than 128 and enough space is available
	else if(available >= fileSize / 128.0)
	{
		iNode* temp;
		int count = 1;
		for (int i = 0; i < numberOfBlocks; i++) 
		{	
		    if (inodes[i].ownerTaskID == -1 && count == 1) 
			{
				//store previous iNode to store nextIndex
				temp = &inodes[i];
		       	inodes[i].ownerTaskID = threadID;
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
				temp->nextIndex = i;
				temp = &inodes[i];
				inodes[i].ownerTaskID = threadID;
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
					return nextFileHandle++;
				}
				count++;
			}
		}
	}
    //Not enough space
	return -1;

}


/*-----------------------------------------------------------------
Function      : deleteFile()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::deleteFile(int threadID, std::string fileName) {
    for (int i = 0; i < numberOfBlocks; i++) {
        
        // file exists
        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0
             && inodes[i].ownerTaskID == threadID ) {
            
            // has permission
            if (inodes[i].permission & 0b0100) {
                    
                std::fstream dataFile(fsName.c_str(), std::ios::in | std::ios::out);
                std::fstream metaFile(metaFileName.c_str(), std::ios::in | std::ios::out);

                // for all inodes corresponding to this file
                // reset dataFile block and reset inode
                int current = i;
				int nextIndex = 0;
                while (current != -1) {

                    // reset dataFile block
                    dataFile.seekp( inodes[ current ].startingBlock );
                    for (int j = 0; j < inodes[ current ].size; j++) {
                        dataFile.put('$');
                    }

					nextIndex = inodes[ current ].nextIndex;
                    // reset inode
                    memset(inodes[ current ].fileName, 0, sizeof( inodes[ current ].fileName ));
                    inodes[ current ].ownerTaskID = -1;
                    inodes[ current ].sequence = 0;
                    inodes[ current ].nextIndex = -1;
                    inodes[ current ].permission = 0b0000;
                    inodes[ current ].blocks = 0;
                    inodes[ current ].handle = -1;
                    inodes[ current ].createdOn = time(NULL);
                    inodes[ current ].modifiedOn = time(NULL);

                    // write to metaFile
                    int inodeSize = sizeof(iNode);
                    metaFile.seekp( current * inodeSize );
                    metaFile.write((char *) &(inodes[ current ]), inodeSize);

                    current = nextIndex;
                }

                dataFile.close();
                metaFile.close();

                return 0;  // success
            }

            // doesn't have permission
            else {
                return -1;
            }
        }
    }
    return -1;
}


/*-----------------------------------------------------------------
Function      : changePermission()
Parameters    : 
Returns       : ???????????????????????
Details       : 
------------------------------------------------------------------*/
int UFS::changePermission(int threadID, std::string fileName, char newPermission) {

    for (int i = 0; i < numberOfBlocks; i++) {
        
        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0
             && inodes[i].ownerTaskID == threadID ) {
            
            int current = i;
            while (current != -1) {
                inodes[ current ].permission = newPermission;

                current = inodes[ current ].nextIndex;
            }
            return 1;
        }
    }

    return -1;
}


/*-----------------------------------------------------------------
Function      : dir()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
void UFS::dir() {
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
			std::cout << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  inodes[i].handle << colSep;
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


/*-----------------------------------------------------------------
Function      : dir()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
void UFS::dir(int threadID) {
    // next index
}


/*-----------------------------------------------------------------
Function      : dump()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
void UFS::dump(Window* Win) {
  char mBuff[16384];
  std::string tempString; 
  char* chr;

  //Suspend scheduler
  mcb->s->SCHEDULER_SUSPENDED = true;


  std::string fRow;
  std::fstream dataFile(fsName.c_str(), std::ios::in | std::ios::out);

  // make sure file was successfully opened
  if (dataFile.is_open())
  {
    // get one line at a time until EOF
    while (getline (dataFile, fRow))
    {
      sprintf(mBuff + strlen(mBuff),"%s \t\t", fRow);
    }
  dataFile.close();
  }

  // write to window
  Win->write_window(mBuff);

  //Resume scheduler
  mcb->s->SCHEDULER_SUSPENDED = true;
}

std::string UFS::intToBin(unsigned short int val)
{
   unsigned short int mask = 1 << (sizeof(short int) * 8 - 1);
   char c[17];
   for(int i = 0; i < sizeof(short int) * 8; i++)
   {
      if( (val & mask) == 0 )
         c[i] =  '0' ;
      else
         c[i] = '1' ;

      mask  >>= 1;
   }
	c[16] = '\0';
	std::string result(c);
	return result;
}

/*-----------------------------------------------------------------
Function      : stMCB
Parameters    : MCB*
Returns       :
Details       : Sets the MCB pointer within the class
------------------------------------------------------------------*/
void UFS::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}