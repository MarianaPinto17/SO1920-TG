#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

namespace sofs19
{
    void grpRenameDirEntry(int pih, const char *name, const char *newName)
    {
        soProbe(204, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name, newName);

        /* change the following line by your code */

        // aceder ao inode -> "Assumes pih is a valid inode handler of a directory where the user has write access"
        SOInode *pinode = soGetInodePointer(pih);

        uint32_t i;
        SODirEntry pdirentry[DPB];

        uint32_t nblock = pinode->blkcnt;
        for(i=0; i<nblock; i++) {
            soReadFileBlock(pih, i, pdirentry);
            for(uint32_t j=0; j<DPB; j++){
                if(strcmp(name, pdirentry[j].name) == 0) {
                    strcpy(pdirentry[j].name, newName);
                    // escrever nova direntry
                    soWriteFileBlock(pih, i, pdirentry);
                    printf("Minha2\n");
                    return ;
                }
            }
        }
        throw SOException(ENOENT, __FUNCTION__);
        
        //binRenameDirEntry(pih, name, newName);
    }
};

