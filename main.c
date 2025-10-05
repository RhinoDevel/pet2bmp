
// MT, 2017nov30

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

// Hard-coded (also see Makefile):
static char const * const executable = "pet2bmp";

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

static bool save_txt_as_rom(
    char const * const text_file_path, char const * const rom_file_path)
{
    return false; // TODO: Implement!
}

static bool save_rom_as_txt(
    char const * const rom_file_path, char const * const txt_file_path)
{
    return false; // TODO: Implement!
}

static bool save_bmp_as_rom(
    char const * const bmp_file_path, char const * const rom_file_path)
{
    struct Bmp * const bmp = Bmp_load(bmp_file_path);

    if(bmp == NULL)
    {
        return false; // (called method debug-logs)
    }

    assert(char_dim.w == 8); // One row of a char must be a byte.
    int const char_byte_count = char_dim.h;

    // The bitmap must hold complete characters.
    assert((bmp->d.w * bmp->d.h) % (char_dim.w * char_dim.h) == 0);
    int const char_count = (bmp->d.w * bmp->d.h) / (char_dim.w * char_dim.h);

    Deb_line("Character count: %d", char_count)

    size_t const rom_byte_count = (size_t)(char_count * char_byte_count);

    Deb_line("ROM byte count: %zu", rom_byte_count)

    assert(sizeof(unsigned char) == 1);
    unsigned char * const rom = malloc(rom_byte_count);

    if(rom == NULL)
    {
        Deb_line(
            "Error: Failed to allocate ROM memory (%zu bytes)!",
            rom_byte_count)
        Bmp_delete(bmp);
        return false;
    }

    // One bitmap row must be exactly wide enough to hold complete characters.
    assert(bmp->d.w % char_dim.w == 0);
    int const row_char_count = bmp->d.w / char_dim.w;
    int const col_char_count = bmp->d.h / char_dim.h;

    Deb_line("Row char. count: %d", row_char_count)

    // TODO: Hard-coded, read this from bitmap file!
    static int const bytes_per_pixel = 3;

    memset(rom, 0, rom_byte_count); // TODO: Do in loop!

    for(int char_i = 0; char_i < char_count; ++char_i)
    {
        int const bmp_row_beg = char_dim.h * (char_i / row_char_count);
        int const bmp_row_lim = bmp_row_beg + char_dim.h;

        int const bmp_col_beg = char_dim.w * (char_i % col_char_count);
        int const bmp_col_lim = bmp_col_beg + char_dim.w;

        Deb_line(
            "Char.: %d, rows: [%d, %d), columns: [%d, %d)",
            char_i, bmp_row_beg, bmp_row_lim, bmp_col_beg, bmp_col_lim)

        for(int bmp_row = bmp_row_beg; bmp_row < bmp_row_lim; ++bmp_row)
        {
            // Assuming row to be completely filled with pixel data (no stride).
            int const bmp_row_offset = bmp_row * bmp->d.w;

            for(int bmp_col = bmp_col_beg; bmp_col < bmp_col_lim; ++bmp_col)
            {
                int const bmp_col_offset = bmp_row_offset + bmp_col;

                int const bmp_col_channel_offset =
                        bytes_per_pixel * bmp_col_offset;

                // Assuming black to always be the background color!
                if(bmp->p[bmp_col_channel_offset + 0] != 0
                    || bmp->p[bmp_col_channel_offset + 1] != 0
                    || bmp->p[bmp_col_channel_offset + 2] != 0)
                {
                    int const shift_count = 7 - (bmp_col - bmp_col_beg);
                    unsigned char const mask =
                        (unsigned char)(1 << shift_count);

                    rom[char_i * char_byte_count + bmp_row - bmp_row_beg] =
                        rom[char_i * char_byte_count + bmp_row - bmp_row_beg]
                            | mask;
                }
            }
        }
    }

    if(!FileSys_saveFile(rom_file_path, rom, rom_byte_count))
    {
        free(rom);
        Bmp_delete(bmp);
        return false; // Called method debug-logs on error.
    }
    free(rom);
    Bmp_delete(bmp);
    return true;
}

static bool save_rom_as_bmp(
    char const * const rom_file_path, char const * const bmp_file_path)
{
    off_t size = -1;

    unsigned char * const char_rom = FileSys_loadFile(rom_file_path, &size);

    if(char_rom == NULL)
    {
        return false; // (called function debug-logs on error)
    }

    assert((off_t)((int)size) == size);

    Sys_log_line_bare("Character ROM size = %d bytes.", (int)size);

    int const char_cnt = (int)size / ((char_dim.w * char_dim.h) / 8);

    Sys_log_line_bare("Character count = %d.", char_cnt);

    // - 16 x 16 = 256 characters.
    // - 1 character <=> 8 x 8 pixels.
    //
    // => Bitmap dimensions' length = 16 x 8 pixels =  128 pixels.

    struct Bmp * const b = Bmp_create(128, 128);
    //
    // // For single-column output (also see below):
    // //
    // struct Bmp * const b = Bmp_create(8/*char_dim.w*/, (int)size);

    Sys_log_line_bare("Bitmap resolution = %d x %d pixels.", b->d.w, b->d.h);

    assert(b->d.w*b->d.h == 8/*char_dim.w*/ * (int)size);

    int const out_char_w = b->d.w / char_dim.w;

    Sys_log_line_bare("Characters in one bitmap row = %d.", out_char_w);

    for(int c = 0; c < char_cnt; ++c) // For each character in ROM.
    {
        int const row_offset = c * 1 * char_dim.h;

        for(int row = 0; row < char_dim.h; ++row) // For each row / each byte.
        {
            int const byte_offset = row_offset + row * 1;
            unsigned char const byte = char_rom[byte_offset];

            // For each bit, from left to right.
            for(int bit = 7; 0 <= bit; --bit)
            {
                // Find out, if bit represents a foreground or background pixel:

                int const bit_val = (byte >> bit) & 1;
                bool const is_background = bit_val == 0;

                // Find position of pixel and its channels representing
                // character's bit in output bitmap:

                int const out_char_row = c / out_char_w,
                    out_char_col = c % out_char_w,
                    out_pix_row = out_char_row * char_dim.w + row,
                    out_pix_col = out_char_col * char_dim.h + 7 - bit,
                    out_pix_offset = out_pix_row * b->d.w + out_pix_col,
                    out_channel_offset = 3 * out_pix_offset;

                // Set output pixel's channel values:

                b->p[out_channel_offset + 2] =
                    is_background ? bg_red : fg_red;
                b->p[out_channel_offset + 1] =
                    is_background ? bg_green : fg_green;
                b->p[out_channel_offset + 0] =
                    is_background ? bg_blue : fg_blue;
            }
        }
    }
    //
    // // For single-column output (also see above):
    // //
    // for(int row = 0, c = -1; row < b->d.h; ++row)
    // {
    //     for(int col = 0; col < b->d.w; ++col)
    //     {
    //         int const offset = row*b->d.w + col,
    //             bit = offset % 8;
    //
    //         if(bit == 0)
    //         {
    //             ++c;
    //         }
    //
    //         b->p[3 * offset + 0] =
    //             ((char_rom[c] >> (7 - bit)) & 1) == 1 ? 0xFF : 0;
    //         b->p[3 * offset + 1] =
    //             ((char_rom[c] >> (7 - bit)) & 1) == 1 ? 0xFF : 0;
    //         b->p[3 * offset + 2] =
    //             ((char_rom[c] >> (7 - bit)) & 1) == 1 ? 0xFF : 0;
    //     }
    // }

    Bmp_save(b, bmp_file_path);

    free(char_rom);
    Bmp_delete(b);
    return true;
}

int main(int argc, char* argv[])
{
    if(argc != (3 + 1)
        || (argv[1][0] != 'b'
            && argv[1][0] != 'c'
            && argv[1][0] != 't'
            && argv[1][0] != 'u')
        || argv[1][1] != '\0')
    {
        Sys_log_line_bare(
            "Please use one of the following parameter combinations:");
        Sys_log_line_bare(
            "");
        Sys_log_line_bare(
            "%s b <input ROM file path> <output bitmap file path>", executable);
        Sys_log_line_bare(
            "%s c <input bitmap file path> <output ROM file path>", executable);
        Sys_log_line_bare(
            "");
        Sys_log_line_bare(
            "%s t <input ROM file path> <output text file path>", executable);
        Sys_log_line_bare(
            "%s u <input text file path> <output ROM file path>", executable);
        return 1;
    }

    if(argv[1][0] == 'b')
    {
        // Create bitmap file from ROM file.

        if(!save_rom_as_bmp(argv[2], argv[3]))
        {
            Sys_log_line_bare(
                "Error: Failed to create bitmap file from ROM file!");
            return 2;
        }
        return 0;
    }
    if(argv[1][0] == 'c')
    {
        // Create ROM file from bitmap file.

        if(!save_bmp_as_rom(argv[2], argv[3]))
        {
            Sys_log_line_bare(
                "Error: Failed to create ROM file from bitmap file!");
            return 3;
        }
        return 0;
    }

    if(argv[1][0] == 't')
    {
        // Create text file from ROM file.

        if(!save_rom_as_txt(argv[2], argv[3]))
        {
            Sys_log_line_bare(
                "Error: Failed to create text file from ROM file!");
            return 2;
        }
        return 0;
    }
    if(argv[1][0] == 'u')
    {
        // Create ROM file from text file.

        if(!save_txt_as_rom(argv[2], argv[3]))
        {
            Sys_log_line_bare(
                "Error: Failed to create ROM file from text file!");
            return 3;
        }
        return 0;
    }
}
