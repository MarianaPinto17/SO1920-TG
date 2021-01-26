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
    bool grpCheckDirEmpty(int ih)
    {
        soProbe(205, "%s(%d)\n", __FUNCTION__, ih);

        /* change the following line by your code */
        //return binCheckDirEmpty(ih);
       

        SOInode *inode = soGetInodePointer(ih);
        SODirEntry dir[DPB];
        
        uint32_t nblock = inode->size/BlockSize;

        for(uint32_t i = 0; i<nblock;i++){
            soReadDataBlock(i,dir);
            
            
            for(uint32_t j=0; j<DPB; j++){
                if(dir[j].in != NullReference){
                    if(!(strcmp(dir[j].name,"..")== 0 || strcmp(dir[j].name,".") == 0)){
                            return false;
                        }

                }
            }
        }
        return true;
    }
};

