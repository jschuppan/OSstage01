/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/
#include "UFS.h"

UFS::UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar) {
    this->fsName = fsName;
    this->numberOfBlocks = numberOfBlocks;
    this->fsBlockSize = fsBlockSize;
    this->initChar = initChar;

    // create iNode corresponding to UFS here
        // iNode crINODE();

    // format file system with initChar
    
}