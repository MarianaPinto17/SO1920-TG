#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "direntries.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

namespace sofs19
{
    uint32_t grpTraversePath(char *path)
    {
        soProbe(221, "%s(%s)\n", __FUNCTION__, path);
 
        /* change the following line by your code */
        //return binTraversePath(path);

        uint32_t n; // numero do inode
        char *base = basename(strdupa(path));
        char *dir = dirname(strdupa(path));

        if (strcmp(dir,"/") == 0){
            int ih = soOpenInode(0); //raiz
            SOInode *node = soGetInodePointer(ih); 
            if(!soCheckInodeAccess(ih,X_OK)) throw SOException(EACCES,__FUNCTION__);// ver as permissôes
            if((node->mode & S_IFDIR) != S_IFDIR) throw SOException(ENOTDIR,__FUNCTION__); // verificar se é diretório
            n = soGetDirEntry(ih, base);
            return n;
        }
        
        uint32_t in = soTraversePath(dir);
        int ih = soOpenInode(in);
        SOInode *node = soGetInodePointer(ih); 
        if(!soCheckInodeAccess(ih,X_OK)) throw SOException(EACCES,__FUNCTION__);
        if((node->mode & S_IFDIR) != S_IFDIR) throw SOException(ENOTDIR,__FUNCTION__);
        n = soGetDirEntry(ih, base);
        return n; 
    }
}

