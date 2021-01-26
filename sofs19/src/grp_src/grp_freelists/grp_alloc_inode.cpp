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
#include <sys/stat.h>
#include <string.h>

#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    uint32_t grpAllocInode(uint32_t type, uint32_t perm)
    {
        soProbe(401, "%s(0x%x, 0%03o)\n", __FUNCTION__, type, perm);

        /* change the following line by your code */
        
        // "type must represent either a file (S_IFREG), a directory (S_IFDIR), or a symbolic link (S_IFLNK); if not, error EINVAL is thrown;"
        if(type!=S_IFREG && type!=S_IFDIR && type!=S_IFLNK)
            throw SOException(EINVAL, __FUNCTION__);
        
        // "perm must represent a valid permission pattern (a octal value in the range 0000 to 0777); if not, error EINVAL is thrown;"
        if(perm<0 || perm>0777)
        	throw SOException(EINVAL, __FUNCTION__);

        // aceder superblock
        SOSuperBlock *superblock = soGetSuperBlockPointer();

        // "if there are no free inodes, error ENOSPC is thrown;"
        if(superblock->ifree == 0)
        	throw SOException(ENOSPC, __FUNCTION__);

        // obter Inode handler
        int inumber = superblock->ihead;
       	int ihandler = soOpenInode(inumber);
       	// obter Inode Pointer
       	SOInode *inode = soGetInodePointer(ihandler);

       	// superblock: atualizar head, tail e num de inodes
       	if(superblock->ihead == superblock->itail) {
       		superblock->itail=NullReference;
       	}
       	superblock->ihead = inode->next;
       	superblock->ifree--;
       	// guardar alteraçãos ao superblock
       	soSaveSuperBlock();

       	// Mudar info do inode (voltei a colocar alguns destes a 0 por redundância, mas possivelmente é desnecessário porque o inode já era livre)
       	inode->mode = type | perm;
        inode->lnkcnt = 0;
        inode->owner = 1000;
        inode->group = 1000;
        inode->size = 0;
        inode->blkcnt = 0;
        time_t system_time;
        time(&system_time);
        uint32_t time = system_time;
        inode->ctime=time;
        inode->mtime=time;
        inode->atime=time;
        // guardar alterações ao inode
        soSaveInode(ihandler);
        // fechar inode
        soCloseInode(ihandler);
        return inumber;
        // return binAllocInode(type, perm);
    }
};

