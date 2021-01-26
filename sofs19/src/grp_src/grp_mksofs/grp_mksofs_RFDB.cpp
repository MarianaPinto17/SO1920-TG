#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{
    void grpResetFreeDataBlocks(uint32_t ntotal, uint32_t itotal, uint32_t nbref)
    {
        soProbe(607, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, nbref);
        /* change the following line by your code */

        /* Organização física:
        	. Superblock ocupa posição 0
        	. Seguido da InodeTable (itotal/IPB)
        	. Seguido do primeiro Data Block -> Root (Não fazer reset!!)
        	. E pelas Reference Data Blocks (nbref)
        */
        uint32_t posFisica = (itotal/IPB) + nbref + 2;
        for(; posFisica<ntotal; posFisica++) {
        	// em C++ arrays são inicializados a 0, portanto yay for me!
        	uint32_t ref[RPB] = {};
        	soWriteRawBlock(posFisica, ref);
        }
        //binResetFreeDataBlocks(ntotal, itotal, nbref);
    }
};

