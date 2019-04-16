/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of an iNode
============================================================================*/
#ifndef UFS_H
#define UFS_H

#include <string>

class UFS {
    std::string fsName;
    int fsBlockSize;
    int numberOfBlocks;
};

#endif