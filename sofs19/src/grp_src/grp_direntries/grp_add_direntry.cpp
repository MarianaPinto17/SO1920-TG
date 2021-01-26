#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs19
{
    //Assumes pih is a valid inode handler of a directory where the user has write access
    //Assumes cin is a valid inode number 
    //The lnkcnt of the child inode is not changed by this function!!!!
    void grpAddDirEntry(int pih, const char *name, uint32_t cin)
    {
        soProbe(202, "%s(%d, %s, %u)\n", __FUNCTION__, pih, name, cin);

        /* change the following line by your code */
        uint32_t freefound = 0;
        uint32_t freeblock, freeinode;
        uint32_t blocknumber;

         //the whole data block is formated as an array of DPB directory entries
        SODirEntry dirblock[DPB];

        //creates a pointer for a inode
        SOInode *inode;
        //get pointer to an open inode
        inode = soGetInodePointer(pih);
        //blocos usados
        uint32_t blocksused = (*inode).size/BlockSize;
        
        // Error EINVAL if name is empty
        if(name == NULL || (name && !name[0])){
            throw SOException(EINVAL,__FUNCTION__);
        }

        //verify inode number
        // mode -> it stores the file type and permissions
        //if it's not a diretctory
        if(!S_ISDIR((*inode).mode)){
            throw SOException(ENOTDIR, __FUNCTION__);
        }
        
        //iteration for a free space and checks if name already exists
        for (blocknumber = 0; blocknumber<blocksused; blocknumber++){
            soReadFileBlock(pih,blocknumber,dirblock);

            for(uint32_t i=0; i<DPB;i++){
                // finds a free space
                if(!freefound && dirblock[i].in == NullReference){
                    freefound=1;
                    freeblock=blocknumber;
                    freeinode=i;
                }
                //Error EEXIST is thrown if name already exists 
                if(dirblock[i].name==name){
                    throw SOException(EEXIST,__FUNCTION__);
                }
            }
        }
        //if there's no free space allocate and format a new block
        if(!freefound){
            //alloc file block
            soAllocFileBlock(pih,blocksused);
            //memset -> fill a block of memory with a particular value
            //dirblock -> starting adress
            //0 -> value to be field
            //blocksize -> number of bytes to be filled
            memset(dirblock,0,BlockSize);
            for(uint32_t i=0;i<DPB;i++){
                dirblock[i].in=NullReference;
            }

            //write changes to disk
            soWriteFileBlock(pih,blocksused,dirblock);

            //increment inode size
            (*inode).size = (((*inode).size) + BlockSize);

            //free space points to a new block
            freeinode=0;
            freeblock=blocksused;
        }

        //read block with free space
        soReadFileBlock(pih,freeblock,dirblock);

        //new node in free space
        dirblock[freeinode].in=cin;
        //puts name on sb
        strcpy(dirblock[freeinode].name,name);

        //write changes to disk 
        soWriteFileBlock(pih,freeblock,dirblock);
        //binAddDirEntry(pih, name, cin);
    }
};

