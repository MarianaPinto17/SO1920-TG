/*
/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace sofs19
{
    /* only fill the current block to its end */
    void grpDepleteTailCache(void)
    {
        soProbe(444, "%s()\n", __FUNCTION__);
        soOpenSuperBlock ( );
        SOSuperBlock* sb = soGetSuperBlockPointer ();
        uint32_t ref[RPB];
        uint32_t nfree = RPB - sb->tail_idx;
        soReadDataBlock(sb->tail_blk, ref); // le do bloco para ref 

        if(sb->tail_cache.idx != TAIL_CACHE_SIZE){//se a tail cache não estiver cheia n faz nada

        }
        else{
            if(nfree==0){ // se a tail reference tiver cheio cria outro bloco
                uint32_t num = soAllocDataBlock(); // atualizar o superblock
                ref[0] = num;
                soWriteDataBlock(sb->tail_blk,ref);
                uint32_t new_ref[RPB];
                for(uint32_t i = 0; i<RPB; i++){
                    new_ref[i] = NullReference;
                }
                soWriteDataBlock(num,new_ref);
                sb->tail_blk = num;
                sb->tail_idx = 1;
            }

            nfree = RPB - sb->tail_idx;

            if(nfree >= TAIL_CACHE_SIZE){
                soReadDataBlock(sb->tail_blk, ref);
                sb->tail_cache.idx = 0; // cache esvazia totalmente, logo 1º indicie livre é 0
                for(uint32_t i=0; i<TAIL_CACHE_SIZE; i++){
                    ref[sb->tail_idx] = sb->tail_cache.ref[i]; // copia para a ref os valores da tail cache
                    sb->tail_idx++;
                    sb->tail_cache.ref[i] = NullReference; // liberta tail cache
                }
                soWriteDataBlock(sb->tail_blk,ref); // escreve na tail reference data block
            }

            else{
                uint32_t idx = nfree + 1;
                for (uint32_t i = 0; i < nfree; i++)
                {
                    ref[sb->tail_idx] = sb->tail_cache.ref[i]; // copia para a ref os valores da tail cache
                    sb->tail_idx++;
                    sb->tail_cache.ref[i] = NullReference; //limpa a cache
                }
                soWriteDataBlock(sb->tail_blk,ref); // escreve no bloco até á última posiçao

                for(uint32_t j = 0; j < TAIL_CACHE_SIZE; j++) { 
                    
                    if(j < TAIL_CACHE_SIZE-nfree){ // preenche a cache
                        sb->tail_cache.ref[j] = idx++;
                    }
                    else{// liberta
                        sb->tail_cache.ref[j] = NullReference;
                    }
                }
                sb->tail_cache.idx = TAIL_CACHE_SIZE - nfree; // atualiza a primeira posiçao livre
            }
        }
        soSaveSuperBlock();
        /* change the following line by your code */
        //binDepleteTailCache();
    }
}
