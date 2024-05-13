
#include <stdio.h>
#include <stdlib.h>

#include "files.h"

int main()
{
    char  FILE_PATH[] = "test.txt";

    Registry R[] =
    {
        {.name = "Teste25656",  .id = "id0",.code = 2},
        {.name = "Teste56565",  .id = "id1",.code = 3},
        {.name = "Teste5677f",  .id = "id2",.code = 4},
        {.name = "Teste76dfd",  .id = "id3",.code = 6},
        {.name = "Teste57fdd",  .id = "id4",.code = 8},
        {.name = "Teste7fdff",  .id = "id5",.code = 9},
        {.name = "Testando  ",  .id = "id6",.code = 1},
        {.name = "Strings_em_C_sao_problematicas",  .id = "id687",.code = 1} // testing width
    };

    for(int i=0 ; i < 4; i++) 
        write_sequential(FILE_PATH, true, &R[i]);
    printf("Insertion with Sequential\n");
    display_all_regs(FILE_PATH);

    for (int i = 4; i < 8; i++)
        write_direct(FILE_PATH, false, &R[i]);
    
    printf("Insertion with Direct Input\n");
    display_all_regs(FILE_PATH);
    

    return 0;
}