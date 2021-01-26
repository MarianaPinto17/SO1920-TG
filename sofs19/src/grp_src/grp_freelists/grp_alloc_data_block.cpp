/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2009-2019
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
    uint32_t grpAllocDataBlock()
    {
        soProbe(441, "%s()\n", __FUNCTION__);

        /* change the following line by your code */
        
        //creates a pointer for a superblock
        SOSuperBlock *superblock;

        //load the superblock if not done yet
        superblock = soGetSuperBlockPointer();  //soGetSuperBlockPoint returns
                                                //a pointer to the superblock

        // dz_free -> number of free blocks in data zone
        uint32_t freedatablocks=(*superblock).dz_free;

        //if there are no free data blocks, error ENOSPC is thrown
        if(freedatablocks==0){
            throw SOException(ENOSPC,__FUNCTION__);
        }

        //if the head cache is empty it is
        //replenished before the retrieval takes place;
        if(((*superblock).head_cache.idx)==0){
            soReplenishHeadCache();
        }

        //first cache element
        uint32_t headrefposition=(*superblock).head_cache.idx;
        uint32_t datanumber=(*superblock).head_cache.ref[headrefposition];

        // empty the cache position
        (*superblock).head_cache.ref[headrefposition]=NullReference;

        //increment cache position
        (*superblock).head_cache.idx++;
        
        //decrease free inode number
        (*superblock).dz_free--;

        // save superblock to disk
        soSaveSuperBlock();	

        //print
        //printf("Usar a minha!");
        
        //the number (reference) of the data block allocated
        return datanumber; 
        //return binAllocDataBlock();
    }
};

