#include "grp_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <inttypes.h>
#include <errno.h>
#include <assert.h>

namespace sofs19
{
    /* free all blocks between positions ffbn and RPB - 1
     * existing in the block of references given by i1.
     * Return true if, after the operation, all references become NullReference.
     * It assumes i1 is valid.
     */
    static bool grpFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn);

    /* free all blocks between positions ffbn and RPB**2 - 1
     * existing in the block of indirect references given by i2.
     * Return true if, after the operation, all references become NullReference.
     * It assumes i2 is valid.
     */
    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn);

    /* ********************************************************* */

    void grpFreeFileBlocks(int ih, uint32_t ffbn)
    {
        soProbe(303, "%s(%d, %u)\n", __FUNCTION__, ih, ffbn);

        // aceder ao inode -> "Assumes ih is a valid handler of an inode in use opened"
        SOInode *inode = soGetInodePointer(ih);
        
        // libertar File Blocks em d
        for(; ffbn<N_DIRECT; ffbn++) {
            if (inode->d[ffbn] != NullReference) {
                soFreeDataBlock(inode->d[ffbn]);
                inode->d[ffbn] = NullReference;
                inode->blkcnt--;
            }
        }
        // atualizar número de ffbn
        ffbn -= N_DIRECT;
        
        // libertar File Blocks em i1
        uint32_t j;
        bool empty;
        for(j=0; j<N_INDIRECT; j++) {
            if(inode->i1[j] != NullReference) {
                empty = grpFreeIndirectFileBlocks(inode, inode->i1[j], ffbn);
                // libertar i[i] caso necessário
                if(empty) {
                    soFreeDataBlock(inode->i1[j]);
                    inode->i1[j] = NullReference;
                    inode->blkcnt--;
                }
            }
            // atualizar número de ffbn
            if (ffbn > RPB) {
                ffbn -= RPB;
            } else {
                ffbn = 0;
            }
        }

        // libertar File Blocks em i2
        for(j=0; j<N_DOUBLE_INDIRECT; j++) {
            if(inode->i2[j] != NullReference) {
                empty = grpFreeDoubleIndirectFileBlocks(inode, inode->i2[j], ffbn);
                // libertar i[i] caso necessário
                if(empty) {
                    soFreeDataBlock(inode->i2[j]);
                    inode->i2[j] = NullReference;
                    inode->blkcnt--;
                }
            }
            // atualizar número de ffbn
            if (ffbn > RPB*RPB) {
                ffbn -= RPB*RPB;
            } else {
                ffbn = 0;
            }
        }

        // binFreeFileBlocks(ih, ffbn);
    }

    /* ********************************************************* */

    static bool grpFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);
        
        // array de referências a file blocks
        uint32_t ref[RPB];
        // ler as referências do bloco
        soReadDataBlock(i1, ref);
        
        uint32_t reverse = ffbn+1;
        
        for(; ffbn<RPB; ffbn++) {
            if(ref[ffbn] != NullReference) {
                soFreeDataBlock(ref[ffbn]);
                ref[ffbn] = NullReference;
                ip->blkcnt--;
            }
        }
        // escrever novo buffer    
        soWriteDataBlock(i1, ref);
        
        for(; reverse>0; reverse--) {
            if(ref[reverse-1] != NullReference) {
                return false;
            }
        }        
        return true;

        //throw SOException(ENOSYS, __FUNCTION__); 
    }

    /* ********************************************************* */

    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);

        // array de referências a file blocks
        uint32_t ref[RPB];
        // ler as referências do bloco
        soReadDataBlock(i2, ref);

        uint32_t reverse = (ffbn/RPB)+1;

        // libertar File Blocks em i2
        uint32_t j;
        bool empty;
        for(j=0; j<RPB; j++) {
            if(ref[j] != NullReference and ffbn<RPB) {
                empty = grpFreeIndirectFileBlocks(ip, ref[j], ffbn);
                // libertar i2[i] caso necessário
                if(empty) {
                    soFreeDataBlock(ref[j]);
                    ref[j] = NullReference;
                    ip->blkcnt--;
                }
            }
            // atualizar número de ffbn
            if (ffbn > RPB) {
                ffbn -= RPB;
            } else {
                ffbn = 0;
            }
        }

        // escrever novo buffer    
        soWriteDataBlock(i2, ref);

        for(; reverse>0; reverse--) {
            if(ref[reverse-1] != NullReference) {
                return false;
            }
        }       
        return true;
        //throw SOException(ENOSYS, __FUNCTION__); 
    }

    /* ********************************************************* */
};

