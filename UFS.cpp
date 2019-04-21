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
        for (int i = 0; i < numberOfBlocks; i++) {

            // read iNode from metaFile (tested this method and it seems good)
            char *buffer = (char *) &(inodes[i]);
            int inodeSize = sizeof(iNode);
            for (int j = 0; j < inodeSize; j++) {
                metaFile.get( buffer[j] );
            }

            /*
            getline(metaFile, inodes[i].fileName);
            metaFile >> inodes[i].ownerTaskID;
            metaFile >> inodes[i].startingBlock;
            metaFile >> inodes[i].size;
            metaFile >> inodes[i].sequence;
            metaFile >> inodes[i].nextIndex;

            metaFile >> inodes[i].permission;
            for (int j = 0; j < 4; j++) {
                metaFile >> inodes[i].blocks[j];
            }
            metaFile >> inodes[i].createdOn;
            metaFile >> inodes[i].modifiedOn;
            metaFile.ignore(1);  // ignore new line
            */

            if (inodes[i].ownerTaskID == -1) {
                (this->available)++;
            }
            else {
                (this->used)++;
            }
        }
        metaFile.close();
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
    for (int i = 0; i < numberOfBlocks; i++) {
        if (true) {
            // function stub
            return 0;
        }
    }
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
        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0 ) {
            
            // has permission
            if (inodes[i].ownerTaskID == threadID
                && (inodes[i].permission & OWNER_WRITE)) {
                    
                std::fstream dataFile(fsName.c_str(), std::ios::in | std::ios::out);
                std::fstream metaFile(metaFileName.c_str(), std::ios::in | std::ios::out);

                // for all inodes corresponding to this file
                // reset dataFile block and reset inode
                int current = i;
                while (current != -1) {

                    // reset dataFile block
                    dataFile.seekp( inodes[ current ].startingBlock );
                    for (int j = 0; j < inodes[ current ].size; j++) {
                        dataFile.put('$');
                    }

                    // reset inode
                    memset(inodes[ current ].fileName, 0, sizeof( inodes[ current ].fileName ));
                    inodes[ current ].ownerTaskID = -1;
                    inodes[ current ].startingBlock = fsBlockSize * i;
                    inodes[ current ].size = fsBlockSize;
                    inodes[ current ].sequence = 0;
                    inodes[ current ].nextIndex = -1;

                    inodes[ current ].permission = 0b0000;
                    memset(inodes[ current ].blocks, 0, sizeof( inodes[ current ].blocks ));
                    inodes[ current ].createdOn = time(NULL);
                    inodes[ current ].modifiedOn = time(NULL);

                    // write to metaFile
                    char *buffer = (char *) &(inodes[ current ]);
                    int inodeSize = sizeof(iNode);
                    metaFile.seekp( current * inodeSize );
                    for (int j = 0; j < inodeSize; j++) {
                        metaFile.put( buffer[j] );
                    }

                    current = inodes[ current ].nextIndex;
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