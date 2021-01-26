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

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    void grpFreeInode(uint32_t in)
    {
        soProbe(402, "%s(%u)\n", __FUNCTION__, in);

        /* change the following line by your code */
       	
       	// aceder superblock
        SOSuperBlock *superblock = soGetSuperBlockPointer();

       	// aceder ao inode
       	int ihandler = soOpenInode(in);
       	SOInode *inode = soGetInodePointer(ihandler);

        // falta esta verificação no binário do prof (não se pode libertar um inode que já está livre) 
        if(inode->mode==INODE_FREE) {
          // fecha só o inode
          soCloseInode(ihandler);
        } else {
          //atualizar inode
          inode->mode = INODE_FREE;
          inode->lnkcnt = 0;
          inode->owner = 0;
          inode->group = 0;
          inode->size = 0;
          inode->blkcnt = 0;
          inode->ctime = 0;
          inode->mtime = 0;
          inode->next=NullReference;
          uint32_t i;
          for(i=0; i<N_DIRECT; i++)
              inode->d[i] = NullReference;
          for(i=0; i<N_INDIRECT; i++)
              inode->i1[i] = NullReference;
          for(i=0; i<N_DOUBLE_INDIRECT; i++)
              inode->i2[i] = NullReference;
          // guardar alterações ao inode
          soSaveInode(ihandler);
          // fechar inode
          soCloseInode(ihandler);
        
          if(superblock->ifree>0) {
            // alterar o next do ultimo inode da FIFO (se existir)
            ihandler = soOpenInode(superblock->itail);
            inode = soGetInodePointer(ihandler);
            inode->next=in;
            // guardar alterações ao inode
            soSaveInode(ihandler);
            // fechar inode
            soCloseInode(ihandler);
          } else if(superblock->ifree==0) {
           // se o disco não tiver nenhum inode livre o libertado passa a ser head e tail da FIFO
           superblock->ihead=in;
          }
          // inode libertado passa a ser tail da FIFO
          superblock->itail=in;
          superblock->ifree++;
          // guardar alteraçãos ao superblock
          soSaveSuperBlock();
          //binFreeInode(in);
        }
    }
};

