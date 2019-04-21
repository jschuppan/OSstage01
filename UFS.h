/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/
#ifndef UFS_H
#define UFS_H

#include <string>
#include <fstream>
#include "iNode.h"

class UFS {
    /*struct iNode {
        std::string fileName;
        int ownerTaskID;
        int startingBlock;
        int size;
        int sequence;
        int nextIndex;

        char permission;
        unsigned int blocks[4];
        time_t createdOn;
        time_t modifiedOn;
    };*/

    iNode *inodes;  // dynamic array
    std::string fsName;
    std::string metaFileName;
    int fsBlockSize;
    int numberOfBlocks;
    int nextFileHandle;
    char initChar;

    int available, used;

    public:

    // permission constants to be passed to createFile()
    static const char OWNER_READ  = 0b1000,
                      OWNER_WRITE = 0b0100,
                      OTHER_READ  = 0b0010,
                      OTHER_WRITE = 0b0001;

    // mode constants to be passed to openFile()
    static const char READ  = 0b10,
                      WRITE = 0b01;

    UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar, std::fstream &fileSystem);
    void format();

    int openFile(int threadID, int fileHandle, std::string fileName, char mode);
    int closeFile(int threadID, int fileID);

    int readChar(int threadID, std::string fileName, char &c);
    int writeChar(int threadID, std::string fileName, char c);

    int createFile(int threadID, std::string fileName, int fileSize, char permission);
    int deleteFile(int threadID, std::string fileName);

    int changePermission(int threadID, std::string fileName, char newPermission);

    void dir();
    void dir(int threadID);
    void dump();
};

#endif
