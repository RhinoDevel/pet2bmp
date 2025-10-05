
// MT, 2016mar19

#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include "Deb.h"
#include "FileSys.h"

/** Source: http://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
 */
off_t FileSys_GetFileSize(char const * const inPath)
{
    struct stat s;

    assert(inPath!=NULL);

    if(stat(inPath, &s)==0)
    {
        return s.st_size; // *** RETURN ***
    }

    return -1;
}

unsigned char * FileSys_loadFile(
    char const * const inPath, off_t * const inOutSize)
{
    *inOutSize = -1;

    off_t const signed_size = FileSys_GetFileSize(inPath);

    if(signed_size==-1)
    {
        Deb_line("Error: Failed to get size of file \"%s\"!", inPath)
        return NULL;
    }

    FILE * const file = fopen(inPath, "rb");

    if(file==NULL)
    {
        Deb_line("Error: Failed to open source file \"%s\"!", inPath)
        return NULL;
    }

    size_t const size = (size_t)signed_size;
    unsigned char * const buf = malloc(size*sizeof(*buf));

    if(fread(buf, sizeof(*buf), size, file)!=size)
    {
        Deb_line("Error: Failed to completely load character ROM file content!")
        return NULL;
    }

    fclose(file);
    *inOutSize = signed_size;
    return buf;
}

bool FileSys_saveFile(
    char const * const inPath,
    unsigned char const * const inBytes,
    int const inByteCount)
{
    FILE* fp = NULL;

    if(inPath == NULL || inPath[0] == '\0')
    {
        Deb_line("Error: No or empty file path given!")
        return false;
    }
    if(0 < inByteCount && inBytes == NULL)
    {
        Deb_line("Error: Byte count given, but no bytes!")
        return false;
    }
    if(inByteCount < 0)
    {
        Deb_line("Error: Negative byte count given!")
    }

    fp = fopen(inPath, "wb");

    if(0 < inByteCount)
    {
        assert(inBytes != NULL); // Checked above.
        
        assert(sizeof *inBytes == 1);
        if(fwrite(inBytes, 1, (size_t)inByteCount, fp) != (size_t)inByteCount)
        {
            Deb_line("Error: Not all bytes were written!")
            fclose(fp);
            fp = NULL;
            return false;
        }
    }

    fclose(fp);
    return true;
}

bool FileSys_saveTxtFile(
    char const * const inPath,
    char const * const inChars,
    int const inCharCount)
{
    FILE* fp = NULL;

    if(inPath == NULL || inPath[0] == '\0')
    {
        Deb_line("Error: No or empty file path given!")
        return false;
    }
    if(0 < inCharCount && inChars == NULL)
    {
        Deb_line("Error: Char. count given, but no characters!")
        return false;
    }
    if(inCharCount < 0)
    {
        Deb_line("Error: Negative character count given!")
    }

    fp = fopen(inPath, "w");

    if(0 < inCharCount)
    {
        assert(inChars != NULL); // Checked above.
        
        if(fwrite(inChars, sizeof *inChars, (size_t)inCharCount, fp)
            != (size_t)inCharCount)
        {
            Deb_line("Error: Not all characters were written!")
            fclose(fp);
            fp = NULL;
            return false;
        }
    }

    fclose(fp);
    return true;
}
