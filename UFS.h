/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/
#ifndef UFS_H
#define UFS_H

//k


#include <string>
#include <fstream>
#include "iNode.h"
#include "linkedlist.h"

class UFS {

    iNode* inodes;
    MCB* mcb;
    std::string fsName;
    std::string metaFileName;
    int fsBlockSize;
    int numberOfBlocks;
    int nextFileHandle;
    char initChar;
    int nextFileID;

    int available, used;

    struct openFiles
    {
        //T_ID is the thread that opened file
        int T_ID;
        int ownerID;
        std::string filename;
        int fileID;
        char status;
        bool operator==(const openFiles& rhs)
        {
          return (fileID == rhs.fileID);
        }
    };

    void writeToThreadWindow(int, char*);
    linkedList<openFiles> openFileList;
    public:

    // permission constants to be passed to createFile()
    static const char OWNER_READ  = 0b1000,
                      OWNER_WRITE = 0b0100,
                      OTHER_READ  = 0b0010,
                      OTHER_WRITE = 0b0001;

    // mode constants to be passed to openFile()
    static const char READ   = 0b10,
                      WRITE  = 0b01,
                      CLOSED = 0;


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


    UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar);
    void format();

    int openFile(int threadID, int fileHandle, std::string fileName, char mode);
    int closeFile(int threadID, int fileID);

    int readChar(int threadID, int fileID, char &c, int offset);
    int writeChar(int threadID, int fileID, char c, int offset);

    int createFile(int threadID, std::string fileName, int fileSize, char permission);
    int deleteFile(int threadID, std::string fileName);

    int changePermission(int threadID, std::string fileName, char newPermission);

    void dir(Window* Win);
    std::string intToBin(unsigned short int val);
    void dir(Window* Win, int threadID);
    void dump(Window* Win);
    void setMCB(MCB* mcb);
};

#endif
