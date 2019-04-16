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

class iNode {
    std::string fileName;
    int ownerTaskID;
    int startingBlock;
    int size;

    char permission[4];
    unsigned int blocks[4];
    time_t createdOn;
    time_t modifiedOn;
};

#endif