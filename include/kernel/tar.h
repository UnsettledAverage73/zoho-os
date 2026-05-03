#ifndef TAR_H
#define TAR_H

#include <stdint.h>

typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
} __attribute__((packed)) tar_header_t;

void tar_init_disk(uint32_t start_lba);
int tar_find_file(const char* name, uint32_t* out_lba, uint32_t* out_size);
int tar_get_file_by_index(int index, char* out_name);

void cached_write_sector(uint32_t lba, uint8_t* buffer);

#endif
