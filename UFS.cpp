/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/

#include <fstream>
#include "UFS.h"
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <stdio.h>


// not sure what to do with std::fstream &fileSystem
UFS::UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar, std::fstream &fileSystem) {
    this->fsName = fsName + ".txt";  // ex: ultima.txt
    this->metaFileName = "meta" + fsName;  // ex: metaultima.txt
    this->numberOfBlocks = numberOfBlocks;
    this->fsBlockSize = fsBlockSize;
    this->initChar = initChar;

    this->available = 0;
    this->used = 0;

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
        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0 ) {
            
            // owner wants to open
            if (inodes[i].ownerTaskID == threadID) {

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
            return 0;
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
Returns       : 
Details       : 
------------------------------------------------------------------*/
int UFS::changePermission(int threadID, std::string fileName, char newPermission) {
  
}


/*-----------------------------------------------------------------
Function      : dir()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
void UFS::dir() {
    for (int i = 0; i < numberOfBlocks; i++) {
        
    }
}


/*-----------------------------------------------------------------
Function      : dir()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
void UFS::dir(int threadID) {

}


/*-----------------------------------------------------------------
Function      : dump()
Parameters    : 
Returns       : 
Details       : 
------------------------------------------------------------------*/
void UFS::dump() {

}