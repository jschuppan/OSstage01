/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
File          : iNode.h
Date          : April 16, 2019
Purpose       : Implementation of our file system
============================================================================*/

#include "UFS.h"
#include "iNode.h"
#include "window.h"
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <fstream>

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


UFS::UFS(std::string fsName, int numberOfBlocks, int fsBlockSize, char initChar) {
    this->fsName = fsName + ".txt";  // ex: ultima.txt
    this->metaFileName = "meta" + fsName + ".txt";  // ex: metaultima.txt
    this->numberOfBlocks = numberOfBlocks;
    this->fsBlockSize = fsBlockSize;
    this->initChar = initChar;

    this->available = 0;
    this->used = 0;
    this->nextFileID = 0;
    this->nextFileHandle = 0;

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
        inodes[i].handle = -1;
        inodes[i].nextIndex = -1;
        inodes[i].permission = 0b0000;
        inodes[i].blocks = B_ALLOC[i];
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
        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0) {
          writeToThreadWindow(threadID, "  TEST\n");

            // owner wants to open
            if (inodes[i].ownerTaskID == threadID || fileHandle == inodes[i].handle) {
                // asking for read without read permission
                if ((mode & READ) && !(inodes[i].permission & OWNER_READ)) {
                    writeToThreadWindow(threadID, "  Invalid Read Permission\n");
                    return -1;
                }

                // asking for write without write permission
                if ((mode & WRITE) && !(inodes[i].permission & OWNER_WRITE)) {
                    writeToThreadWindow(threadID, "  Invalid Write Permission\n");
                    return -1;
                }
            }

            // other wants to open
            else {

                // asking for read without read permission
                if ((mode & READ) && !(inodes[i].permission & OTHER_READ)) {
                    writeToThreadWindow(threadID, "  Invalid Read Permission\n");
                    return -1;
                }

                // asking for write without write permission
                if ((mode & WRITE) && !(inodes[i].permission & OTHER_WRITE)) {
                    writeToThreadWindow(threadID, "  Invalid Write Permission\n");
                    return -1;
                }
            }
            //success (unique int file_id?)
            openFiles tempNode;
            tempNode.T_ID = threadID;
            tempNode.filename = fileName;
            tempNode.fileID = nextFileID;
            tempNode.status = mode;
            tempNode.ownerID = inodes[i].ownerTaskID;
            openFileList.addToFront(tempNode, nextFileID);
            writeToThreadWindow(threadID, "  Success Open\n");
            return nextFileID++;
        }
    }
    // file doesn't exist
    writeToThreadWindow(threadID, "  File Does Not Exist\n");
    return -1;
}


/*-----------------------------------------------------------------
Function      : closeFile()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
int UFS::closeFile(int threadID, int fileID)
{
    openFiles *tempOpenFile = openFileList.getDatumById(fileID);
	if(!tempOpenFile)
	{
		//Print error file not found
		return -1;
	}
	else
	{
		if(threadID == tempOpenFile->T_ID)
		{
			openFileList.removeNodeByElement(fileID);
			return 1;
		}
	}
	//ERROR ID didnt match
	return -1;

}


/*-----------------------------------------------------------------
Function      : readChar()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
int UFS::readChar(int threadID, int fileID, char &c,int offset) {
    openFiles *ptr = openFileList.getDatumById(fileID);

    // file is NOT open
    if (!ptr) {
        return -1;
    }

    // task didn't open file
    if ( ptr->T_ID != threadID ) {
        return -1;
    }

    // file is not open for read
    if ( ! (ptr->status & READ) ) {
        return -1;
    }

    // calculate how many blocks to traverse before starting to read
    int skips = offset / fsBlockSize;

    for (int i = 0; i < numberOfBlocks; i++) {

        // found the file
        if ( strcmp(ptr->filename.c_str(), inodes[i].fileName) == 0 ) {

            // go to the appropriate block
            int current = i;
            while (current != -1 && skips > 0) {
                skips--;
                current = inodes[ current ].nextIndex;
            }

            // out of boundaries
            if (skips > 0) {
                return -1;
            }

            // make offset local to that block
            offset %= fsBlockSize;

            // write to appropriate location
            std::ifstream dataFile(fsName.c_str());
            dataFile.seekg( offset + inodes[ current ].startingBlock );
            dataFile.get(c);
            dataFile.close();

            return 1;
        }
    }
    // shouldn't reach this return because file should be found
    return -99;
}

/*-----------------------------------------------------------------
Function      : writeChar()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
int UFS::writeChar(int threadID, int fileID, char c,int offset) {
    openFiles *ptr = openFileList.getDatumById(fileID);

    // file is NOT open
    if (!ptr) {
          writeToThreadWindow(threadID, "  File Does Not Exist\n");
        return -1;
    }

    // task didn't open file
    if ( ptr->T_ID != threadID ) {
        writeToThreadWindow(threadID, "  Do Not Have Correct Permissions\n");
        return -1;
    }

    // file is not open for write
    if ( ! (ptr->status & WRITE) ) {
        writeToThreadWindow(threadID, "  File Not Open For Write\n");
        return -1;
    }

    // calculate how many blocks to traverse before starting to write
    int skips = offset / fsBlockSize;

    for (int i = 0; i < numberOfBlocks; i++) {

        // found the file
        if ( strcmp(ptr->filename.c_str(), inodes[i].fileName) == 0 ) {

            // go to the appropriate block
            int current = i;
            while (current != -1 && skips > 0) {
                skips--;
                current = inodes[ current ].nextIndex;
            }

            // out of boundaries
            if (skips > 0) {
                writeToThreadWindow(threadID, "  Error Writing Out Of Bounds\n");
                return -1;
            }

            // make offset local to that block
            offset %= fsBlockSize;

            // write to appropriate location
            std::fstream dataFile(fsName.c_str(), std::ios::in | std::ios::out);
            dataFile.seekp( offset + inodes[ current ].startingBlock );
            dataFile.put(c);
            dataFile.close();

            writeToThreadWindow(threadID, "  Successful Write\n");
            return 1;
        }
    }
    // shouldn't reach this return because file should be found
    return -99;
}


/*-----------------------------------------------------------------
Function      : createFile()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
int UFS::createFile(int threadID, std::string fileName, int fileSize, char permission) {
    	std::fstream metaFile(metaFileName.c_str());
    if ( fileSize <=0 || fileSize >= 4*fsBlockSize)
    {
        writeToThreadWindow(threadID, "  Invalid File Size\n");
		return -1;
    }
    //Requested Block is less that 128
	else if (fileSize < fsBlockSize && available > 0)
	{
		for (int i = 0; i < numberOfBlocks; i++)
		{
		    if (inodes[i].ownerTaskID == -1)
			{
		       	inodes[i].ownerTaskID = threadID;
			   	inodes[i].permission = permission;
				for(int k=0; k<8; k++)
					inodes[i].fileName[k] = fileName[k];
				//ADD SEMAPHORE HERE
				metaFile.seekp(sizeof(iNode) * i);
				metaFile.write((char *) &(inodes[i]), sizeof(iNode));
				available--;
				used++;
                writeToThreadWindow(threadID, "  File Created\n");
                inodes[i].handle = ++nextFileHandle;
		        return inodes[i].handle;
		    }
		}
	}
    //Requested Block is greater than 128 and enough space is available
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
		       	inodes[i].ownerTaskID = threadID;
			   	inodes[i].permission = permission;
                inodes[i].handle = ++nextFileHandle;
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
				inodes[i].ownerTaskID = threadID;
			   	inodes[i].permission = permission;
				inodes[i].sequence = count -1;
                inodes[i].handle = nextFileHandle;
				//ADD SEMAPHORE HERE
				metaFile.seekp(sizeof(iNode) * i);
				metaFile.write((char *) &(inodes[i]), sizeof(iNode));
				available--;
				used++;
				for(int k=0; k<8; k++)
					inodes[i].fileName[k] = fileName[k];
				if(count >= fileSize / 128.0)
				{
                    writeToThreadWindow(threadID, "  File Created\n");
		            return inodes[i].handle;
				}
				count++;
			}
		}
	}
    //Not enough space
    writeToThreadWindow(threadID, "  Not Enough Free Space\n");
	return -1;

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
                    inodes[ current ].handle = -1;
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

    for (int i = 0; i < numberOfBlocks; i++) {

        if ( strcmp(inodes[i].fileName, fileName.c_str()) == 0
             && inodes[i].ownerTaskID == threadID ) {

            int current = i;
            while (current != -1) {
                inodes[ current ].permission = newPermission;

                current = inodes[ current ].nextIndex;
            }
            return 1;
        }
    }

    return -1;
}


/*-----------------------------------------------------------------
Function      : dir()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
void UFS::dir(Window* Win) {
    const char colFill        = ' ';
    const std::string colSep  = " | ";
	const int  colNameSm      = 4;
    const int  colNameMd      = 7;
	const int  colNameLg      = 12;
	const int  colNameXLg     = 16;
	char permBuff[5];
    char outBuff[65536];
    std::string outString;
    std::stringstream sOutput;
    char* chr;
    std::ofstream yoo;
    unsigned short int cmbBlock; // combined block
    int j;
    int fileSize;

	  std::time_t cDate, mDate;
	  std::time_t initTime;
  	struct tm * timeStruct;

    //Loop through linked list
    openFiles* Ofile = NULL;
    int k =0;
    char tempStatus = 'c';
    while(Ofile = openFileList.getNextElementUntilEnd(Ofile))
    {
      if(Ofile->ownerID == inodes[k].ownerTaskID && strcmp(Ofile->filename.c_str(),inodes[k].fileName) == 0)
      {
        if(Ofile->status == READ)
            tempStatus = 'r';
        else if(Ofile->status == WRITE)
        {
          //Highest priority status stop looking
          tempStatus = 'w';
          break;
        }
      }
      k++;
    }
    mcb->s->SCHEDULER_SUSPENDED = true;


	// setting up the table
    sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Hndle" << colSep;
    sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Name" << colSep;
	sOutput << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "Blocks#" << colSep;
	sOutput << std::left << std::setw(colNameSm) << std::setfill(colFill) <<  "Size" << colSep;
	sOutput << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "Starting block" << colSep;
	sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Status" << colSep;
	sOutput << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  "Perm." << colSep;
	sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  "Owner#" << colSep;
	sOutput << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  "Create Time" << colSep;
	sOutput << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  "Mod. Time" << std::endl;

    outString = sOutput.str();
    chr = strdup(outString.c_str());

    sprintf(outBuff, "  \n  %s", chr);
    Win->write_window(outBuff);
    sprintf(outBuff, "");
    sOutput.str("");
    sOutput.clear();




    for (int i = 0; i < numberOfBlocks; i++) {
		// time conversions
		char cDateBuff[100];
		char mDateBuff[100];

		// figure out time
        timeStruct = localtime (&inodes[i].createdOn);
        strftime (cDateBuff, 100 ,"%x %X", timeStruct);
		timeStruct = localtime (&inodes[i].modifiedOn);
        strftime (mDateBuff, 100 ,"%x %X", timeStruct);

		// figure out file permissions
		if (inodes[i].permission & 0b1000)
			permBuff[0] = 'r';
		else
			permBuff[0] = '-';

		if (inodes[i].permission & 0b0100)
			permBuff[1] = 'w';
		else
			permBuff[1] = '-';

		if (inodes[i].permission & 0b0010)
			permBuff[2] = 'r';
		else
			permBuff[2] = '-';

		if (inodes[i].permission & 0b0001)
			permBuff[3] = 'w';
		else
			permBuff[3] = '-';

		permBuff[4] = '\0';


        // only list actual files
        if (inodes[i].ownerTaskID != -1)
        {

            cmbBlock = inodes[i].blocks;
            fileSize = inodes[i].size;
            j = i;

            while (inodes[j].nextIndex != -1)
            {
                j = inodes[j].nextIndex;
                cmbBlock =  cmbBlock | inodes[j].blocks;
                fileSize += inodes[j].size;
            }

            if (inodes[i].sequence == 0)
            {
                sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  inodes[i].handle << colSep;
			    sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  inodes[i].fileName << colSep;
			    sOutput << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  intToBin(cmbBlock) << colSep;
			    sOutput << std::left << std::setw(colNameSm) << std::setfill(colFill) <<  fileSize << colSep;
			    sOutput << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  inodes[i].startingBlock << colSep;
			    sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  tempStatus << colSep;
			    sOutput << std::left << std::setw(colNameLg) << std::setfill(colFill) <<  permBuff << colSep;
			    sOutput << std::left << std::setw(colNameMd) << std::setfill(colFill) <<  inodes[i].ownerTaskID << colSep;
			    sOutput << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  cDateBuff << colSep;
			    sOutput << std::left << std::setw(colNameXLg) << std::setfill(colFill) <<  mDateBuff << std::endl;
            }
        }

        outString = sOutput.str();
        chr = strdup(outString.c_str());
        sprintf(outBuff, "  %s ", chr);
        Win->write_window(outBuff);
        sOutput.str("");
        sOutput.clear();

    }
    free(chr);

    mcb->s->SCHEDULER_SUSPENDED = false;


}


/*-----------------------------------------------------------------
Function      : dir()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
void UFS::dir(Window* Win, int threadID) {
    // next index
}


/*-----------------------------------------------------------------
Function      : dump()
Parameters    :
Returns       :
Details       :
------------------------------------------------------------------*/
void UFS::dump(Window* Win) {
  char mBuff[16384];
  std::string tempString;
  char* chr;

  //Suspend scheduler
  mcb->s->SCHEDULER_SUSPENDED = true;


  std::string fRow;
  char* tmpChar;
  std::fstream dataFile(fsName.c_str(), std::ios::in | std::ios::out);

  // make sure file was successfully opened
  if (dataFile.is_open())
  {
    // get one line at a time until EOF
    while (getline (dataFile, fRow))
    {
      tmpChar = strdup(fRow.c_str());
      sprintf(mBuff + strlen(mBuff),"%s \t\t", tmpChar);
    }
  dataFile.close();
  }

  // write to window
  Win->write_window(mBuff);

  //Resume scheduler
  mcb->s->SCHEDULER_SUSPENDED = true;
}

std::string UFS::intToBin(unsigned short int val)
{
   unsigned short int mask = 1 << (sizeof(short int) * 8 - 1);
   char c[17];
   for(int i = 0; i < sizeof(short int) * 8; i++)
   {
      if( (val & mask) == 0 )
         c[i] =  '0' ;
      else
         c[i] = '1' ;

      mask  >>= 1;
   }
	c[16] = '\0';
	std::string result(c);
	return result;
}

/*-----------------------------------------------------------------
Function      : stMCB
Parameters    : MCB*
Returns       :
Details       : Sets the MCB pointer within the class
------------------------------------------------------------------*/
void UFS::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}

void UFS::writeToThreadWindow(int threadID, char* text)
{
    mcb->s->getThreadInfo().getDatumById(threadID)->getThreadWin()->write_window(text);
}
