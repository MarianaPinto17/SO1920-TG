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

    uint32_t grpDeleteDirEntry(int pih, const char *name)
    {
        soProbe(203, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        // aceder ao inode -> "Assumes pih is a valid inode handler of a directory where the user has write access"
        SOInode *pinode = soGetInodePointer(pih);

        uint32_t i;
        SODirEntry pdirentry[DPB];

        uint32_t nblock = pinode->blkcnt;
        for(i=0; i<nblock; i++) {
            soReadFileBlock(pih, i, pdirentry);
            for(uint32_t j=0; j<DPB; j++){
                if(strcmp(name, pdirentry[j].name) == 0) {
                    
                    /*  Apesar de ser o pedido na secção 2.2 Directories do pdf relativo ao sofs19, 
                        trocar de caracteres não parece ser o que o binário dos professores está a fazer.
                        Em vez disso ele coloca o diretório no (clean) state. Recriei isso no código.
                        O código a baixo funciona bem, só comentei por não ter a certeza se é o necessário ou pretendido.
                        Orientei-me por objetivos em vez da descrição do sistema.
                    */

                    // swapping first and last char of name
                    /*
                    char tmp = pdirentry[j].name[0];
                    uint32_t m;
                    for(m=0; pdirentry[j].name[m+1]!='\0'; m++) {}
                    pdirentry[j].name[0] = pdirentry[j].name[m];
                    pdirentry[j].name[m] = tmp;
                    */

                    uint32_t in = pdirentry[j].in;
                    pdirentry[j].in = NullReference;
                    uint32_t m;
                    for(m=0; m<=SOFS19_MAX_NAME; m++) {
                        pdirentry[j].name[m] = '\0';
                    }
                    // escrever nova direntry
                    soWriteFileBlock(pih, i, pdirentry);
                    return in;
                }
            }
        }
       	printf("Minha2\n");
        throw SOException(ENOENT, __FUNCTION__);
        //return binDeleteDirEntry(pih, name);
    }
};

