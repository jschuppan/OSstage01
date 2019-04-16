/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/
#ifndef UFS_H
#define UFS_H

#include <string>

class UFS {
    std::string fsName;
    int fsBlockSize;
    int numberOfBlocks;
    int nextFileHandle;
    char initChar;

    UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar);
    void format();

    int openFile(int threadID, int fileHandle, std::string fileName, char mode);
    int closeFile(int threadID, int fileID);

    
};

#endif