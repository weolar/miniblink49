#ifndef _unz64_H
#define _unz64_H

#ifdef __cplusplus
extern "C" {
#endif
#    define OF(args)  args

typedef void* unzFile;
typedef unsigned long  uLong; /* 32 bits or more */
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned __int64 ZPOS64_T;

typedef struct unz_global_info64_s
{
    ZPOS64_T number_entry;         /* total number of entries in
                                     the central dir on this disk */
    uLong size_comment;         /* size of the global comment of the zipfile */
} unz_global_info64;

typedef struct tm_unz_s
{
	uInt tm_sec;            /* seconds after the minute - [0,59] */
	uInt tm_min;            /* minutes after the hour - [0,59] */
	uInt tm_hour;           /* hours since midnight - [0,23] */
	uInt tm_mday;           /* day of the month - [1,31] */
	uInt tm_mon;            /* months since January - [0,11] */
	uInt tm_year;           /* years - [1980..2044] */
} tm_unz;

typedef struct unz_file_info64_s
{
	uLong version;              /* version made by                 2 bytes */
	uLong version_needed;       /* version needed to extract       2 bytes */
	uLong flag;                 /* general purpose bit flag        2 bytes */
	uLong compression_method;   /* compression method              2 bytes */
	uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
	uLong crc;                  /* crc-32                          4 bytes */
	ZPOS64_T compressed_size;   /* compressed size                 8 bytes */
	ZPOS64_T uncompressed_size; /* uncompressed size               8 bytes */
	uLong size_filename;        /* filename length                 2 bytes */
	uLong size_file_extra;      /* extra field length              2 bytes */
	uLong size_file_comment;    /* file comment length             2 bytes */

	uLong disk_num_start;       /* disk number start               2 bytes */
	uLong internal_fa;          /* internal file attributes        2 bytes */
	uLong external_fa;          /* external file attributes        4 bytes */

	tm_unz tmu_date;
} unz_file_info64;

int unzStringFileNameCompare OF ((const char* fileName1, const char* fileName2, int iCaseSensitivity));
/*打开压缩文件*/
unzFile unzOpen(const wchar_t *path, void *buff, int len);
/*关闭压缩文件*/
int unzClose(unzFile file);
/*取全局信息*/
int unzGetGlobalInfo64(unzFile file, unz_global_info64 *pglobal_info);
/*取注释*/
int unzGetGlobalComment(unzFile file, char * szComment, uLong uSizeBuf);
/*到第一个文件*/
int unzGoToFirstFile(unzFile file);
/*到下一个文件*/
int unzGoToNextFile(unzFile file);
/*定位文件*/
int unzLocateFile(unzFile file, const char *szFileName, int iCaseSensitivity);
/*取当前文件信息*/
int unzGetCurrentFileInfo64(unzFile file, unz_file_info64 *pfile_info, char *szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize, char *szComment, uLong commentBufferSize);
/*打开当前文件 带密码*/
int unzOpenCurrentFilePassword(unzFile file, const char* password);
/*打开当前文件*/
int unzOpenCurrentFile3(unzFile file, int* method, int* level, int raw, const char* password);
/*关闭当前文件*/
int unzCloseCurrentFile(unzFile file);
/*读当前文件*/
int unzReadCurrentFile(unzFile file, void* buf, unsigned len);

#ifdef __cplusplus
}
#endif

#endif /* _unz64_H */
