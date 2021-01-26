#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>
#include <math.h>

namespace sofs19
{
    void grpFillReferenceDataBlocks(uint32_t ntotal, uint32_t itotal, uint32_t nbref)
    {
        soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, nbref);
        
        uint32_t first = 1 + itotal / IPB  + 1; //1ª end físico da zona ref 
        uint32_t fbr = 1 + nbref + HEAD_CACHE_SIZE; //indice da 1ª posiçao lógica do FreeDataBlock
        uint32_t ref[RPB];
        if (nbref == 0){ // se o nbref for 0 não faz nada(cabem todos na head cache)

        }
        else{
            for(uint32_t i = 1; i < nbref; i++){
                ref[0] = i+1;
                for(uint32_t k = 1; k<RPB; k++){
                    ref[k] = fbr++;
                }
                soWriteRawBlock(first+i-1, ref); // escreve bloco a bloco as referências 
            }
        
            for(uint32_t j = 1; j < RPB; j++){ //último bloco de ref
                ref[0] = NullReference; //1ª pos último bloco
                if ( (fbr + first)  > ntotal){
                    ref[j] = NullReference;
                }
                else{
                    ref[j] = fbr++;
                }
            }
            soWriteRawBlock(first + nbref -1, ref);
        }
        //return binFillReferenceDataBlocks(ntotal, itotal, nbref);

    }
}
