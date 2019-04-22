/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of an iNode
============================================================================*/
#ifndef INODE_H
#define INODE_H

#include <ctime>
#include <string>
#include <fstream>


class iNode {
    public:

    char fileName[8];
    int ownerTaskID;
    int startingBlock;
    int size;
    int sequence;
    int nextIndex;
    int handle;

    char permission;
    unsigned short int blocks;
    time_t createdOn;
    time_t modifiedOn;
};

#endif
