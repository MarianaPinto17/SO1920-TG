/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    void grpFreeDataBlock(uint32_t bn)
    {
        soProbe(442, "%s(%u)\n", __FUNCTION__, bn);

        /* change the following line by your code */
        
         //creates a pointer for a superblock
        SOSuperBlock *superblock;

        //load the superblock if not done yet
        superblock = soGetSuperBlockPointer();  //soGetSuperBlockPoint returns
                                                //a pointer to the superblock
        // bn>total number of blocks or bn<0 throw EINVAL
        if(bn>((*superblock).ntotal) || bn<0){
            throw SOException(EINVAL,__FUNCTION__);
        }

        //if the cache is full, it is depleted 
        //before the insertion takes place;
        if(((*superblock).tail_cache.idx) == TAIL_CACHE_SIZE){
            soDepleteTailCache();
        }

        //The data block reference is inserted into the tail cache:
        ((*superblock).tail_cache.ref)[(*superblock).tail_cache.idx]=bn;
        
        //increment of the free block in data zone
        (*superblock).dz_free++;

        //increase of the cache position
        (*superblock).tail_cache.idx++;

        //save superblock to disk 
        soSaveSuperBlock();
        
        //binFreeDataBlock(bn);
    }

};

