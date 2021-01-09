#ifndef COMPRESS_H
#define COMPRESS_H

#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <utime.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <linux/kdev_t.h>
#include <sys/sysmacros.h>

#define REGULAR      0
#define NORMAL      '0'
#define HARDLINK    '1'
#define SYMLINK     '2'
#define CHAR        '3'
#define BLOCK       '4'
#define DIRECTORY   '5'
#define FIFO        '6'
#define LONGNAME    'L'
#define LINKLONG    'K'

typedef union record
{
    union
    {
        // Pre-POSIX.1-1988 format
        struct
        {
            char name[100];      // file name
            char mode[8];        // permissions
            char uid[8];         // user id (octal)
            char gid[8];         // group id (octal)
            char size[12];       // size (octal)
            char mtime[12];      // modefication time (octal)
            char check[8];       // sum of unsigned characters in block (octal)
            char link;           // link indicator
            char link_name[100]; // name of linked file
        };

        // UStar format (POSIX IEEE P1003.1)
        struct
        {
            char old[156];            // first 156 octets of Pre-POSIX.1-1998 format
            char type;                // file type
            char also_link_name[100]; // name of linked file
            char ustar[8];            // ustar\000
            char owner[32];           // user name (string)
            char group[32];           // group name (string)
            char major[8];            // device major number
            char minor[8];            // device minor number
            char prefix[155];
        };
    };

    char block[512]; // raw memory (padded to 1 block)
}Record;

typedef struct inode
{
    u_int64_t inode;
    char* path;
    struct inode* next;
} iNode;

typedef struct huffmannode
{
    int ch;
    struct huffmannode* left;
    struct huffmannode* right;
} huffmanNode;

typedef struct linknode
{
    u_int64_t frequency;
    huffmanNode* node;
    struct linknode* next;
} linkNode;

typedef struct huffmanitem
{
    unsigned char length;
    char* huffmanCode;
} huffmanItem;

static iNode iNodeHead;

static linkNode linkNodeHead;

static u_int64_t Frequency[256] = { 0 };

static huffmanItem huffmanTable[256];

char* mallocAndReset(size_t length, int n);
void freeINode();
void printOneBlock(Record* block, FILE* fout);
int tar(char* path, FILE* fout);
int untar(char *path, FILE* fin);
int countFileFrequency(char *path);
int huffman();
int compress(FILE* fin, FILE* fout);
int uncompress(FILE* fin, FILE* fout);

#endif // COMPRESS_H
