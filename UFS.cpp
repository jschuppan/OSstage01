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

// not sure what to do with std::fstream &fileSystem
UFS::UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar, std::fstream &fileSystem) {
    this->fsName = fsName + ".txt";  // ex: ultima.txt
    this->metaFileName = "meta" + fsName;  // ex: metaultima.txt
    this->numberOfBlocks = numberOfBlocks;
    this->fsBlockSize = fsBlockSize;
    this->initChar = initChar;

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
            // remember to put new line after fileName in case it was empty string
            // rest are separated by spaces
            // each inode is separated by a new line
            getline(metaFile, inodes[i].fileName);
            metaFile >> inodes[i].ownerTaskID;
            metaFile >> inodes[i].startingBlock;
            metaFile >> inodes[i].size;

            metaFile >> inodes[i].permission;
            for (int j = 0; j < 4; j++) {
                metaFile >> inodes[i].blocks[j];
            }
            metaFile >> inodes[i].createdOn;
            metaFile >> inodes[i].modifiedOn;
            metaFile.ignore(1);  // ignore new line
        }
        metaFile.close();
    }
}


void UFS::format() {
    // initialize and write to meta file
    std::ofstream metaFile(metaFileName.c_str());
    for (int i = 0; i < numberOfBlocks; i++) {
        metaFile << (inodes[i].fileName = "") << '\n';
        metaFile << (inodes[i].ownerTaskID = -1) << ' ';
        metaFile << (inodes[i].startingBlock = fsBlockSize * i) << ' ';
        metaFile << (inodes[i].size = fsBlockSize) << ' ';

        metaFile << (inodes[i].permission = 0b0000) << ' ';
        for (int j = 0; j < 4; j++) {
            metaFile << (inodes[i].blocks[j] = -1) << ' ';
        }
        metaFile << (inodes[i].createdOn = time(NULL)) << ' ';
        metaFile << (inodes[i].modifiedOn = time(NULL)) << '\n';
    }
    metaFile.close();

    // initialize data file
    std::ofstream dataFile(fsName.c_str());
    int fileSize = numberOfBlocks * fsBlockSize;
    for (int i = 0; i < fileSize; i++) {
        dataFile << initChar;
    }
    dataFile.close();
}