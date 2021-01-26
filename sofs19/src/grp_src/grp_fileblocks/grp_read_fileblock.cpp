#include "grp_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "fileblocks.h"
#include "bin_fileblocks.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{
    void grpReadFileBlock(int ih, uint32_t fbn, void *buf)
    {
        soProbe(331, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);

        /* change the following line by your code */
        //binReadFileBlock(ih, fbn, buf);

        if(soGetFileBlock(ih,fbn) == NullReference){ // return null caracter 
            uint32_t temp[BlockSize] = {0x00000000};
            buf = temp;															
        }
        else{
            uint32_t n_blk = soGetFileBlock(ih,fbn);
            soReadDataBlock(n_blk, buf);
        }
    }

}

