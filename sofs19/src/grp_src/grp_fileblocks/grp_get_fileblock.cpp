#include "grp_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

namespace sofs19
{
    /* ********************************************************* */


    static uint32_t grpGetIndirectFileBlock(SOInode * ip, uint32_t fbn);
    static uint32_t grpGetDoubleIndirectFileBlock(SOInode * ip, uint32_t fbn);


    /* ********************************************************* */

    uint32_t grpGetFileBlock(int ih, uint32_t fbn)
    {
        soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);
        uint32_t res;
        /* change the following line by your code */
        //return binGetFileBlock(ih, fbn);
        SOInode *inode = soGetInodePointer(ih);
        
        if(fbn < 0){
            throw SOException(EINVAL,__FUNCTION__);
        }
        if(fbn < N_DIRECT){
            res = inode->d[fbn];
        
        }else if(fbn < (N_INDIRECT * RPB + N_DIRECT)){
                res = grpGetIndirectFileBlock(inode,fbn);
            }else{
                res = grpGetDoubleIndirectFileBlock(inode,fbn);
            }

        soSaveInode(ih);
        return res;





    }

    /* ********************************************************* */


    static uint32_t grpGetIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        throw SOException(ENOSYS, __FUNCTION__); 
        uint32_t res;
        uint32_t indice_i1 = afbn/RPB;//indice de i1 
        uint32_t indice_in_Block = afbn%RPB;//indice dentro do bloco
        uint32_t ref[RPB];
        soReadDataBlock(ip->i1[indice_i1],ref);

        if(ip->i1 == (uint32_t*)NullReference){
                res = NullReference;  
                return res;
        }else{
            res = ref[indice_in_Block];
            return res;
        }
        
        
    }


    /* ********************************************************* */


    static uint32_t grpGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        uint32_t res;
        uint32_t tmp;
        uint32_t indice_i2 = afbn/RPB;//indice de i1 
        uint32_t indice_in_Block = afbn%RPB;//indice dentro do bloco
        uint32_t ref[RPB];
        soReadDataBlock(ip->i2[indice_i2],ref);
        
        if(ip->i2 == (uint32_t*)NullReference){
            res = NullReference;
            return res;
        }else{
            res = ref[indice_in_Block];
            return res;


        }
        throw SOException(ENOSYS, __FUNCTION__); 
        
    }

};

