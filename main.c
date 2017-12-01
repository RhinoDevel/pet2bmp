
// MT, 2017nov30

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "FileSys.h"
#include "Bmp.h"

int main(int argc, char* argv[])
{
    FILE* f = NULL;

    if(argc!=2)
    {
        Deb_line(
            "Error: Exactly one argument (the source file name) must be given!")
        return 1;
    }

    int const s = (int)FileSys_GetFileSize(argv[1]);

    if(s==-1)
    {
        Deb_line("Error: Failed to get size of source file!");
        return 2;
    }

    f = fopen(argv[1], "rb");
    if(f==NULL)
    {
        Deb_line("Error: Failed to open source file!")
        return 3;
    }

    struct Bmp const b = (struct Bmp)
    {
        .p = malloc(3*8*s), // RGB
        .d = (struct Dim)
        {
            .w = 8,
            .h = s
        }
    };
    assert(b.d.w*b.d.h==8*s);

    for(int row = 0;row<b.d.h;++row)
    {
        unsigned char c = 0;

        for(int col = 0;col<b.d.w;++col)
        {
            int const offset = row*b.d.w+col,
                bit = offset%8;

            if(bit==0)
            {
                fread(&c, 1, 1, f); // No error check!
            }

            b.p[3*offset+0] = (c>>(7-bit))&1==1 ? 0xFF : 0;
            b.p[3*offset+1] = (c>>(7-bit))&1==1 ? 0xFF : 0;
            b.p[3*offset+2] = (c>>(7-bit))&1==1 ? 0xFF : 0;
        }
    }
    fclose(f);

    Bmp_save(&b, "test.bmp");

    free(b.p);
    return 0;
}
