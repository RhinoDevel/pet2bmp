
// MT, 2017nov30

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

static struct Dim const char_dim = (struct Dim){.w = 8, .h = 8};

// Foreground pixels' color:
//
static unsigned char const fg_red = 0;
static unsigned char const fg_green = 0xFF;
static unsigned char const fg_blue = 0;

// Background pixels' color:
//
static unsigned char const bg_red = 0;
static unsigned char const bg_green = 0;
static unsigned char const bg_blue = 0;

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

    int const char_cnt = (int)size/((char_dim.w*char_dim.h)/8);

    Sys_log_line(true, true, "Character count = %d.", char_cnt);

    // - 16 x 16 = 256 characters.
    // - 1 character <=> 8 x 8 pixels.
    //
    // => Bitmap dimensions' length = 16 x 8 pixels =  128 pixels.

    struct Bmp * const b = Bmp_create(128, 128);
    //
    // // For single-column output (also see below):
    // //
    // struct Bmp * const b = Bmp_create(8/*char_dim.w*/, (int)size);

    Sys_log_line(
        true, true,
        "Bitmap resolution = %d x %d pixels.", b->d.w, b->d.h);

    assert(b->d.w*b->d.h==8/*char_dim.w*/*(int)size);

    int const out_char_w = b->d.w/char_dim.w;

    Sys_log_line(true, true, "Characters in one bitmap row = %d.", out_char_w);

    for(int c = 0;c<char_cnt;++c) // For each character in ROM.
    {
        int const row_offset = c*1*char_dim.h;

        for(int row = 0;row<char_dim.h;++row) // For each row / each byte.
        {
            int const byte_offset = row_offset+row*1;
            unsigned char const byte = char_rom[byte_offset];

            for(int bit = 7;bit>=0;--bit) // For each bit, from left to right.
            {
                // Find out, if bit represents a foreground or background pixel:

                int const bit_val = (byte>>bit)&1;
                bool const is_background = bit_val==0;

                // Find position of pixel and its channels representing
                // character's bit in output bitmap:

                int const out_char_row = c/out_char_w,
                    out_char_col = c%out_char_w,
                    out_pix_row = out_char_row*char_dim.w+row,
                    out_pix_col = out_char_col*char_dim.h+7-bit,
                    out_pix_offset = out_pix_row*b->d.w+out_pix_col,
                    out_channel_offset = 3*out_pix_offset;

                // Set output pixel's channel values:

                b->p[out_channel_offset+2] = is_background?bg_red:fg_red;
                b->p[out_channel_offset+1] = is_background?bg_green:fg_green;
                b->p[out_channel_offset+0] = is_background?bg_blue:fg_blue;
            }
        }
    }
    //
    // // For single-column output (also see above):
    // //
    // for(int row = 0, c = -1;row<b->d.h;++row)
    // {
    //     for(int col = 0;col<b->d.w;++col)
    //     {
    //         int const offset = row*b->d.w+col,
    //             bit = offset%8;
    //
    //         if(bit==0)
    //         {
    //             ++c;
    //         }
    //
    //         b->p[3*offset+0] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
    //         b->p[3*offset+1] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
    //         b->p[3*offset+2] = ((char_rom[c]>>(7-bit))&1)==1 ? 0xFF : 0;
    //     }
    // }

    Bmp_save(b, argv[2]);

    free(char_rom);
    Bmp_delete(b);
    return 0;
}
