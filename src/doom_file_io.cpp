#include <Arduino.h>
#include <SDHCI.h>

extern "C" {
#include "w_file.h"
#include "z_zone.h"
#include "spresense_debug.h"
}

extern "C" {

// The wad_file_t struct holds information about an open WAD file
typedef struct
{
    wad_file_t wad;
    File file;
} spresense_wad_file_t;

// The wad_file_class_t struct holds function pointers for file operations
extern wad_file_class_t stdc_wad_file;

static wad_file_t* W_Spresense_OpenFile(char* path)
{
    spresense_wad_file_t* result;
    File f;

    // Use the global sdCard object from spresense_sd.cpp
    extern SDClass sdCard;

    f = sdCard.open(path, FILE_READ);

    if (!f)
    {
        spresense_printf("Failed to open WAD file: %s\n", path);
        return NULL;
    }

    spresense_printf("Successfully opened WAD file: %s\n", path);

    result = (spresense_wad_file_t*)Z_Malloc(sizeof(spresense_wad_file_t), PU_STATIC, 0);
    result->wad.file_class = &stdc_wad_file;
    result->wad.mapped = NULL;
    result->wad.length = f.size();
    result->file = f;

    return &result->wad;
}

static void W_Spresense_CloseFile(wad_file_t* wad)
{
    spresense_wad_file_t* spresense_wad = (spresense_wad_file_t*)wad;
    spresense_wad->file.close();
    Z_Free(spresense_wad);
}

static size_t W_Spresense_Read(wad_file_t* wad, unsigned int offset, void* buffer, size_t buffer_len)
{
    spresense_wad_file_t* spresense_wad = (spresense_wad_file_t*)wad;
    
    if (!spresense_wad->file.seek(offset))
    {
        return 0;
    }
    
    return spresense_wad->file.read(buffer, buffer_len);
}

wad_file_class_t stdc_wad_file = 
{
    W_Spresense_OpenFile,
    W_Spresense_CloseFile,
    W_Spresense_Read,
};

} // extern "C"