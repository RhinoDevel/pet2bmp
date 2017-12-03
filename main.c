
// MT, 2017nov30

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

static struct Dim const char_dim = (struct Dim){.w = 8, .h = 8};

int main(int argc, char* argv[])
{
    off_t size = -1;

    if(argc!=3)
    {
        Deb_line(
            "Error: Exactly two arguments (src. & dest. files) must be given!")
        return 1;
    }

    unsigned char * const char_rom = FileSys_loadFile(argv[1], &size);

    if(char_rom==NULL)
    {
        return 2;
    }
    assert((off_t)((int)size)==size);

    Sys_log_line(true, true, "Character ROM size = %d bytes.", (int)size);
    Sys_log_line(
        true, true,
        "Character count = %d.", (int)size/((char_dim.w*char_dim.h)/8));

    struct Bmp * const b = Bmp_create(8/*char_dim.w*/, (int)size);

    Sys_log_line(
        true, true,
        "Bitmap resolution = %d x %d pixels.", b->d.w, b->d.h);

    assert(b->d.w*b->d.h==8/*char_dim.w*/*(int)size);

    for(int row = 0, c = -1;row<b->d.h;++row)
    {
        for(int col = 0;col<b->d.w;++col)
        {
            int const offset = row*b->d.w+col,
                bit = offset%8;

            if(bit==0)
            {
                ++c;
            }

            b->p[3*offset+0] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
            b->p[3*offset+1] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
            b->p[3*offset+2] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
        }
    }

    Bmp_save(b, argv[2]);

    free(char_rom);
    Bmp_delete(b);
    return 0;
}
