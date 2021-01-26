/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    void grpReplenishHeadCache(void)
    {
        soProbe(443, "%s()\n", __FUNCTION__);

        /* change the following line by your code */
        //binReplenishHeadCache();
        
        SOSuperBlock *sb = soGetSuperBlockPointer();
        uint32_t ref[RPB];
        
        //Se a Head Cache nao estiver vazia não faz nada
        if(sb->head_cache.idx != HEAD_CACHE_SIZE){
            return ;
        }
        //Is there enough references in head reference block to fill the head cache?
        int check = 0;
        soReadDataBlock(sb->head_blk,ref);
        int count;
        //ver se tem referencias suf para preencher a cache
        
        for(int i = 1;i < RPB;i++){
            if(ref[i] == NullReference){
                

            }else{ 
                count = i;
                break;
            }


        }
        int n_pos = RPB - count;//numero de referencias no head reference data block
        // se tiver
        if(n_pos > HEAD_CACHE_SIZE){
            for(int i = 0; i < HEAD_CACHE_SIZE ; i++){
                sb->head_cache.ref[i] = ref[count + i];
                ref[count + i] = NullReference;
            }
            sb->head_cache.idx = 0;
        }
        else{//senao tiver
            
            int start = HEAD_CACHE_SIZE - n_pos;
            sb->head_cache.idx = start;
            for(start; start < HEAD_CACHE_SIZE; start++){
                sb->head_cache.ref[start] = ref[count];
                ref[count] = NullReference;
                count++;
            }
            check = 1;
        }

        
        
        soWriteDataBlock(sb->head_blk,ref);
        if(check){
            
            sb->head_blk = ref[0];
            soFreeDataBlock(sb->head_blk);
        }
        soSaveSuperBlock();

    }
    
    
}

