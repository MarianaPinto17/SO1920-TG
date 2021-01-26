#include "grp_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>
//#include <string.h>
#include <iostream>

namespace sofs19
{


    static uint32_t grpAllocIndirectFileBlock(SOInode * ip, uint32_t afbn);
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn);


    /* ********************************************************* */
    // ih -> inode handler; fbn -> file block number
    uint32_t grpAllocFileBlock(int ih, uint32_t fbn)
    {
        soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

        /* change the following two lines by your code */
        //creates a pointer for a inode
        SOInode *inode;
        //get pointer to an open inode
        inode = soGetInodePointer(ih);

        uint32_t allocblock;
       
        //if fbn<N_DIRECT(number of direct block references in the inode)
        // allocDataBlock
        if(fbn<N_DIRECT){
            allocblock = soAllocDataBlock();
            //d[N_DIRECT] -> number of direct block references in the inode
            (*inode).d[fbn] = allocblock;
            //blkcnt -> block count: total number of blocks used by the file
            (*inode).blkcnt++;
        // if fbn < (N_DIRECT + N_INDIRECT (number of indirect
        // block references in the inode)* RPB) -> INDIRECT ALLOCATED
        }else if(fbn <(N_DIRECT + N_INDIRECT*RPB)){
            allocblock = grpAllocIndirectFileBlock(inode, fbn - N_DIRECT);
        // if fbn < (N_DIRECT + N_INDIRECT * RPB*2) -> DOUBLE INDIRECT ALLOCATED   
        }else if(fbn < N_DIRECT + N_INDIRECT*RPB*N_INDIRECT){
            allocblock = grpAllocDoubleIndirectFileBlock(inode, fbn - N_INDIRECT +N_INDIRECT*RPB);
        //Error EINVAL is thrown if fbn is not valid;
        }else{
            throw SOException(EINVAL, __FUNCTION__);
            return 0;
        }

        //Save an open inode to disk
        soSaveInode(ih);

        //return the number of the allocated block
        return allocblock;
        //return binAllocFileBlock(ih, fbn);
    }
    /* ********************************************************* */


    /*
    */
    static uint32_t grpAllocIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        //indirect position on the i1 array
        uint32_t ipos = afbn/RPB;
        //indirect block
        uint32_t iblock;
        uint32_t ref[RPB];

        //if there's no direct references
        if((*ip).d[ipos]==NullReference){
            //alloc free block
            iblock=soAllocDataBlock();
            //fil a block sized array (ref) with NullReference
            //memset(ref,NullReference,RPB);
            for (uint32_t i=0; i<RPB; i++){
                ref[i] =NullReference;
            }
            //saves the new block reference
            (*ip).d[ipos]=iblock;
            //increments the block count
            (*ip).blkcnt++;
        //if there are direct references 
        }else{
            //gets the reference
            iblock=(*ip).i2[ipos];
            //reads the data block
            soReadDataBlock(iblock,ref);
        }

        uint32_t allocblock=soAllocDataBlock();

        ref[afbn%RPB]=allocblock;
        
        (*ip).blkcnt++;

        soWriteDataBlock(iblock,ref);
        return allocblock;
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;

    }


    /* ********************************************************* */


    /*
    */
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);
        //double indirect position/index on the i2 array
        uint32_t dipos = afbn/(RPB*RPB);
        //double indirect block
        uint32_t diblock;
        uint32_t iref[RPB];

        //if there's no double indrect references
        if((*ip).i2[dipos]==NullReference){
            //alloc free block
            diblock = soAllocDataBlock();
            //saves the new block reference
            (*ip).i2[dipos]=diblock;
            //increments the block count
            (*ip).blkcnt++;
            //fil a block sized array (iref) with NullReference
            for(u_int32_t i=0;i<RPB;i++){
                iref[i]=NullReference;
            }
            //memset(iref,NullReference,RPB);

        //if there are double indrect references
        }else{
            //gets the reference
            diblock=(*ip).i2[dipos];
            //reads the data block
            soReadDataBlock(diblock,iref);
        }

        u_int32_t allocblock=grpAllocIndirectFileBlock(ip, afbn-dipos*(RPB*RPB));

        soWriteDataBlock(diblock,iref);

        return allocblock;
        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;
    }

};

