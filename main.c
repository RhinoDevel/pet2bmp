
// MT, 2017nov30

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "FileSys.h"
#include "Bmp.h"

int main(int argc, char* argv[])
{
    off_t size = -1;

    if(argc!=2)
    {
        Deb_line(
            "Error: Exactly one argument (the source file name) must be given!")
        return 1;
    }

    unsigned char * const char_rom = FileSys_loadFile(argv[1], &size);

    if(char_rom==NULL)
    {
        return 2;
    }
    assert((off_t)((int)size)==size);

    Deb_line("Character ROM size = %d bytes.", (int)size)

    struct Bmp const b = (struct Bmp)
    {
        .p = malloc((size_t)(3*8*size)), // RGB
        .d = (struct Dim)
        {
            .w = 8,
            .h = (int)size
        }
    };
    assert(b.d.w*b.d.h==8*(int)size);

    for(int row = 0, c = -1;row<b.d.h;++row)
    {
        for(int col = 0;col<b.d.w;++col)
        {
            int const offset = row*b.d.w+col,
                bit = offset%8;

            if(bit==0)
            {
                ++c;
            }

            b.p[3*offset+0] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
            b.p[3*offset+1] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
            b.p[3*offset+2] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
        }
    }

    Bmp_save(&b, "test.bmp");

    free(char_rom);
    free(b.p);
    return 0;
}
