#ifndef FILELIB_H
#define FILELIB_H

#include <stdint.h>
#include "options.h"
#include "access.h"
#include "list.h"

//=========================================================================
// Defines
//=========================================================================
#ifndef SEEK_CUR
	#define SEEK_CUR	1
#endif

#ifndef SEEK_END
	#define SEEK_END	2
#endif

#ifndef SEEK_SET
	#define SEEK_SET	0
#endif

#ifndef EOF
	#define EOF	(-1)
#endif

//=========================================================================
// Structures
//=========================================================================
struct sFL_FILE;

struct cluster_lookup
{
	uint32_t cluster_index;
	uint32_t current_cluster;
};

typedef struct sFL_FILE
{
	uint32_t parent_cluster;
	uint32_t start_cluster;
	uint32_t byte_num;
	uint32_t file_length;
	int file_length_changed;
	char path[FATLIB_MAX_LONG_FILENAME];
	char filename[FATLIB_MAX_LONG_FILENAME];
	uint8_t short_filename[11];

#ifdef FAT_FAT_CLUSTER_CACHE_ENTRIES
	uint32_t cluster_cache_index[FAT_CLUSTER_CACHE_ENTRIES];
	uint32_t cluster_cache_data[FAT_CLUSTER_CACHE_ENTIRES];
#endif

	struct cluster_lookup last_fat_lookup;

	uint8_t file_data_sector[FAT_SECTOR_SIZE];
	uint32_t file_data_address;
	int file_data_dirty;

	uint8_t flags;
#define FILE_READ	(1 << 0)
#define FILE_WRITE	(1 << 1)
#define FILE_APPEND	(1 << 2)
#define FILE_BINARY	(1 << 3)
#define FILE_ERASE	(1 << 4)
#define FILE_CREATE	(1 << 5)

	struct fat_node list_node;
} FL_FILE;

//=========================================================================
// Functions
//=========================================================================

// External
void fl_init();
void fl_attach_locks(void (*lock)(mutex_r_t*), void (*unlock)(mutex_r_t*));
int fl_attach_media(fn_diskio_read rd, fn_diskio_write wr);
void fl_shutdown();

// Standard API
void *fl_fopen(const char *path, const char *modifiers);
void fl_flclose(void *file);
int fl_fflush(void *file);
int fl_fgetc(void *file);
char *fl_fgets(char *s, int n, void *f);
int fl_fputc(int c, void *file);
int fl_fputs(const char *str, void *file);
int fl_fwrite(const void *data, int size, int count, void *file);
int fl_fread(void *data, int size, int count, void *file );
int fl_fseek(void *file, long offset, int origin);
int fl_fgetpos(void *file, uint32_t *position);
long fl_ftell(void *f);
int fl_feof(void *f);
int fl_remove(const char *filename);

// dirent.h
typedef struct fs_dir_list_status FL_DIR;
typedef struct fs_dir_ent fl_dirent;

FL_DIR *fl_opendir(const char *path, FL_DIR *dir);
int fl_readdir(FL_DIR *dirls, fl_dirent *entry);
int fl_closedir(FL_DIR *dir);

// Extensions
void fl_list_directory(const char *path);
int fl_created_directory(const char *path);
int fl_is_dir(const char *path);

int fl_format(uint32_t volume_sectors, const char *name);

// Test hooks
#ifdef FATLIB_INC_TEST_HOOKS
struct fatfs *fl_get_fs();
#endif

//=========================================================================
// stdio file I/O names
//=========================================================================
#ifdef USE_FILELIB_STDIO_COMPAT_NAMES

#define FILE		FL_FILE

#define fopen(a,b)	fl_fopen(a, b)
#define fclose(a)	fl_fclose(a)
#define fflush(a)	fl_fflush(a)
#define fgetc(a)	fl_fgetc(a)
#define fgets(a,b,c)	fl_fgets(a, b, c)
#define fputc(a,b)	fl_fputc(a, b)
#define fputs(a,b)	fl_fputs(a, b)
#define fwrite(a,b,c,d)	fl_fwrite(a, b, c, d)
#define fread(a,b,c,d)	fl_fread(a, b, c, d)
#define fseek(a,b,c)	fl_fseek(a, b, c)
#define fgetpos(a,b)	fl_fgetpos(a, b)
#define ftell(a)	fl_ftell(a)
#define feof(a)		fl_feof(a)
#define remove(a)	fl_remove(a)
#define mkdir(a)	fl_created_directory(a)
#define rmdir(a)	0

#endif

#endif