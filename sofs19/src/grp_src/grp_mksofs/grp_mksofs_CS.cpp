#include "grp_mksofs.h"

#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <iostream>

namespace sofs19
{
    void grpComputeStructure(uint32_t ntotal, uint32_t & itotal, uint32_t & nbref)
    {
        soProbe(601, "%s(%u, %u, ...)\n", __FUNCTION__, ntotal, itotal);

        /* change the following line by your code */
        
        // if it is zero initially, the value ntotal/16 should be used as the start value for itotal, where / stands for the integer division
        if(itotal==0) itotal=ntotal/16;
        /* Aviso!! O codigo binário dos profs parece usar itotal/4, mas a documentação diz itotal/8 ...
        	 No código dos profs arredonda tb por cima depois da operação... 
        	 Tudo em casos de teste limites
           Usei o valor que ia dar de acordo com os testes realizados, mas se for pedido é só mudar as 2 divisões a baixo por /8
		    */

        //itotal is always lower than or equal to ntotal/8 (ntotal/4 nos testes, encolhe ombros)
        if(itotal>ntotal/4) {
        	itotal=ntotal/4;
        }

        // "itotal is always greater than or equal to "... IPB 
        if(itotal<IPB) itotal=IPB;
        
        //itotal must be rounded up to be multiple of IPB
       	//if(itotal%IPB!=0) itotal=((itotal/IPB)+1)*IPB;
       	itotal = (1 + ((itotal - 1) / IPB)) * IPB;

       	/* Continhas aos números de nbref:
			. Começar de ntotal
			. Retirar superblock (1)
			. Retirar InodeTable (itotal/IPB)
			. Retirar o Root (1) (Já não conta como bloco livre)
			. Retirar número de referências que já existam na Head Cache no SuperBlock 
       	*/
       	uint32_t freeDB = ntotal - 2 - itotal/IPB;
       	// evitar overflow
       	if(freeDB <= HEAD_CACHE_SIZE) {
       		nbref = 0;
       	} else {
       		freeDB -= HEAD_CACHE_SIZE;
    		for(nbref=1; nbref*RPB<freeDB; nbref++) { 
    			//"if, after splitting data blocks between reference data blocks and free data blocks, a single data block remains, it is assigned to the inode table"; 
    			if(freeDB-nbref*RPB==1) {
    				itotal += IPB;
    				break;
    			}
    		}
    	}
        //binComputeStructure(ntotal, itotal, nbref);      
    }
};

