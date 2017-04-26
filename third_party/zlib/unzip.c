#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef STDC
#  include <stddef.h>
#  include <string.h>
#  include <stdlib.h>
#endif

#define UNZ_BUFSIZE (16384)
#define UNZ_MAXFILENAMEINZIP (256)

# define ALLOC(size) (malloc(size))
# define TRYFREE(p) {if (p) free(p);}

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)
//-------------------------------------------------------------------------头-----------------------------------------------------------------------------------------------------------

#ifndef _ZLIB_H
#include "zlib.h"
#endif
#include <stdlib.h>

#define Z_BZIP2ED 12


#ifdef _MSC_VER
#define fopen64 _wfopen
#if (_MSC_VER >= 1400) && (!(defined(NO_MSCVER_FILE64_FUNC)) && !NDEBUG)
#define ftello64 _ftelli64
#define fseeko64 _fseeki64
#else // old MSC
#define ftello64 ftell
#define fseeko64 fseek
#endif
#endif

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)
typedef struct TagunzFile__ { int unused; } unzFile__;
typedef unzFile__ *unzFile;
#else
typedef voidp unzFile;
#endif



#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 ZPOS64_T;
#else
typedef unsigned long long int ZPOS64_T;
#endif

typedef struct unz_global_info64_s
{
	ZPOS64_T number_entry;         /* total number of entries in
								   the central dir on this disk */
	uLong size_comment;         /* size of the global comment of the zipfile */
} unz_global_info64;

#ifndef ZCALLBACK
#if (defined(WIN32) || defined(_WIN32) || defined (WINDOWS) || defined (_WINDOWS)) && defined(CALLBACK) && defined (USEWINDOWS_CALLBACK)
#define ZCALLBACK CALLBACK
#else
#define ZCALLBACK
#endif
#endif

#define UNZ_OK                          (0)
#define UNZ_END_OF_LIST_OF_FILE         (-100)
#define UNZ_ERRNO                       (Z_ERRNO)
#define UNZ_EOF                         (0)
#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_INTERNALERROR               (-104)
#define UNZ_CRCERROR                    (-105)

#define ZLIB_FILEFUNC_SEEK_CUR (1)
#define ZLIB_FILEFUNC_SEEK_END (2)
#define ZLIB_FILEFUNC_SEEK_SET (0)

#define ZLIB_FILEFUNC_MODE_READ      (1)
#define ZLIB_FILEFUNC_MODE_WRITE     (2)
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (3)

#define ZLIB_FILEFUNC_MODE_EXISTING (4)
#define ZLIB_FILEFUNC_MODE_CREATE   (8)

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

typedef struct unz_file_info_s
{
	uLong version;              /* version made by                 2 bytes */
	uLong version_needed;       /* version needed to extract       2 bytes */
	uLong flag;                 /* general purpose bit flag        2 bytes */
	uLong compression_method;   /* compression method              2 bytes */
	uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
	uLong crc;                  /* crc-32                          4 bytes */
	uLong compressed_size;      /* compressed size                 4 bytes */
	uLong uncompressed_size;    /* uncompressed size               4 bytes */
	uLong size_filename;        /* filename length                 2 bytes */
	uLong size_file_extra;      /* extra field length              2 bytes */
	uLong size_file_comment;    /* file comment length             2 bytes */

	uLong disk_num_start;       /* disk number start               2 bytes */
	uLong internal_fa;          /* internal file attributes        2 bytes */
	uLong external_fa;          /* external file attributes        4 bytes */

	tm_unz tmu_date;
} unz_file_info;

typedef struct unz_file_pos_s
{
	uLong pos_in_zip_directory;   /* offset in zip file directory */
	uLong num_of_file;            /* # of file */
} unz_file_pos;

typedef struct unz64_file_pos_s
{
	ZPOS64_T pos_in_zip_directory;   /* offset in zip file directory */
	ZPOS64_T num_of_file;            /* # of file */
} unz64_file_pos;

typedef uLong(ZCALLBACK *read_file_func)      OF((voidpf stream, void* buf, uLong size));
typedef uLong(ZCALLBACK *write_file_func)     OF((voidpf stream, const void* buf, uLong size));
typedef int(ZCALLBACK *close_file_func)     OF((voidpf stream));
typedef int(ZCALLBACK *testerror_file_func) OF((voidpf stream));
typedef ZPOS64_T(ZCALLBACK *tell64_file_func)    OF((voidpf stream));
typedef long(ZCALLBACK *seek64_file_func)    OF((voidpf stream, ZPOS64_T offset, int origin));
typedef voidpf(ZCALLBACK *open64_file_func)    OF((const void* filename, int mode));

typedef struct zlib_filefunc64_def_s
{
	open64_file_func    zopen64_file;
	read_file_func      zread_file;
	tell64_file_func    ztell64_file;
	seek64_file_func    zseek64_file;
	close_file_func     zclose_file;
	testerror_file_func zerror_file;
} zlib_filefunc64_def;

#define ZREAD64(filefunc,filestream,buf,size)	((*((filefunc).zread_file))   (filestream,buf,size))
#define ZCLOSE64(filefunc,filestream)			((*((filefunc).zclose_file))  (filestream))
#define ZERROR64(filefunc,filestream)			((*((filefunc).zerror_file))  (filestream))
#define ZOPEN64(filefunc,filename,mode)			((*((filefunc).zopen64_file)) ((filename),(mode)))
#define ZTELL64(filefunc,filestream)            ((*((filefunc).ztell64_file)) ((filestream)))
#define ZSEEK64(filefunc,filestream,pos,mode)   ((*((filefunc).zseek64_file)) ((filestream),(pos),(mode)))

//-------------------------------------------------------------------------头-----------------------------------------------------------------------------------------------------------
typedef struct unz_file_info64_internal_s
{
    ZPOS64_T offset_curfile;
} unz_file_info64_internal;

typedef struct
{
    char  *read_buffer;         /* internal buffer for compressed data */
    z_stream stream;            /* zLib stream structure for inflate */

    ZPOS64_T pos_in_zipfile;       /* position in byte on the zipfile, for fseek*/
    uLong stream_initialised;   /* flag set if stream structure is initialised*/

    ZPOS64_T offset_local_extrafield;/* offset of the local extra field */
    uInt  size_local_extrafield;/* size of the local extra field */
    ZPOS64_T pos_local_extrafield;   /* position in the local extra field in read*/
    ZPOS64_T total_out_64;

    uLong crc32;                /* crc32 of all data uncompressed */
    uLong crc32_wait;           /* crc32 we must obtain after decompress all */
    ZPOS64_T rest_read_compressed; /* number of byte to be decompressed */
    ZPOS64_T rest_read_uncompressed;/*number of byte to be obtained after decomp*/
    zlib_filefunc64_def z_filefunc;
    voidpf filestream;        /* io structore of the zipfile */
    uLong compression_method;   /* compression method (0==store) */
    ZPOS64_T byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
    int   raw;
} file_in_zip64_read_info_s;

typedef struct
{
	zlib_filefunc64_def z_filefunc;/*io函数*/
    voidpf filestream;        /* io流句柄 */
    unz_global_info64 gi;       /* public global information */
    ZPOS64_T byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
    ZPOS64_T num_file;             /* number of the current file in the zipfile*/
    ZPOS64_T pos_in_central_dir;   /* pos of the current file in the central dir*/
    ZPOS64_T current_file_ok;      /* flag about the usability of the current file*/
    ZPOS64_T central_pos;          /* position of the beginning of the central dir*/

    ZPOS64_T size_central_dir;     /* size of the central directory  */
    ZPOS64_T offset_central_dir;   /* offset of start of central directory with
                                   respect to the starting disk number */

    unz_file_info64 cur_file_info; /* public info about the current file in zip*/
    unz_file_info64_internal cur_file_info_internal; /* private info about it*/
    file_in_zip64_read_info_s* pfile_in_zip_read; /* structure about the current
                                        file if we are decompressing it */
    int encrypted;

    int isZip64;

    unsigned long keys[3];     /* keys defining the pseudo-random sequence */
    const unsigned long* pcrc_32_tab;

} unz64_s;
//-------------------------------------------------------------------------io-----------------------------------------------------------------------------------------------------------

static voidpf ZCALLBACK fopen64_file_func(const void* filename, int mode)
{
	FILE* file = NULL;
	const wchar_t* mode_fopen = NULL;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
		mode_fopen = L"rb";
	else
		if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
			mode_fopen = L"r+b";
		else
			if (mode & ZLIB_FILEFUNC_MODE_CREATE)
				mode_fopen = L"wb";

	if ((filename != NULL) && (mode_fopen != NULL))
		file = fopen64((const wchar_t*)filename, mode_fopen);
	return file;
}

static uLong ZCALLBACK fread_file_func(voidpf stream, void* buf, uLong size)
{
	uLong ret;
	ret = (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
	return ret;
}

static ZPOS64_T ZCALLBACK ftell64_file_func(voidpf stream)
{
	ZPOS64_T ret;
	ret = ftello64((FILE *)stream);
	return ret;
}

static long ZCALLBACK fseek64_file_func(voidpf stream, ZPOS64_T offset, int origin)
{
	int fseek_origin = 0;
	long ret;
	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR:
		fseek_origin = SEEK_CUR;
		break;
	case ZLIB_FILEFUNC_SEEK_END:
		fseek_origin = SEEK_END;
		break;
	case ZLIB_FILEFUNC_SEEK_SET:
		fseek_origin = SEEK_SET;
		break;
	default: return -1;
	}
	ret = 0;

	if (fseeko64((FILE *)stream, offset, fseek_origin) != 0)
		ret = -1;

	return ret;
}

static int ZCALLBACK fclose_file_func(voidpf stream)
{
	int ret;
	ret = fclose((FILE *)stream);
	return ret;
}

static int ZCALLBACK ferror_file_func(voidpf stream)
{
	int ret;
	ret = ferror((FILE *)stream);
	return ret;
}
//--------------------------内存流
typedef struct
{
	char *buff;
	ZPOS64_T len;
	ZPOS64_T p;
} unz64_stream;
static uLong ZCALLBACK fread_stream_func(unz64_stream *stream, void* buf, uLong size)
{
	if (stream->len == stream->p){
		return -1;
	}
	uLong l = (stream->len - stream->p >= size) ? size:stream->len - stream->p > size;
	memcpy(buf, stream->buff + stream->p, l);
	stream->p += l;
	return l;
}

static ZPOS64_T ZCALLBACK ftell64_stream_func(unz64_stream *stream)
{
	return stream->p;
}

static long ZCALLBACK fseek64_stream_func(unz64_stream *stream, ZPOS64_T offset, int origin)
{
	//成功返回0 失败返回-1
	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR://当前位置
		if (stream->p + offset > stream->len || stream->p + offset<0)
			return -1;
		stream->p += offset;
		break;
	case ZLIB_FILEFUNC_SEEK_END://文件尾
		if (stream->len - offset > stream->len || stream->len - offset < 0)
			return -1;
		stream->p = stream->len - offset;
		break;
	case ZLIB_FILEFUNC_SEEK_SET://文件头
		if (offset > stream->len || offset < 0)
			return -1;
		stream->p = offset;
		break;
	default: return -1;
	}
	return 0;
}

static int ZCALLBACK fclose_stream_func(unz64_stream *stream)
{
	free(stream->buff);
	return 0;
}

static int ZCALLBACK ferror_stream_func(unz64_stream *stream)
{
	return 0;
}
//-------------------------------------------------------------------------crypt-----------------------------------------------------------------------------------------------------------
#define CRC32(c, b) ((*(pcrc_32_tab+(((int)(c) ^ (b)) & 0xff))) ^ ((c) >> 8))

static int decrypt_byte(unsigned long* pkeys, const unsigned long* pcrc_32_tab)
{
	unsigned temp;  /* POTENTIAL BUG:  temp*(temp^1) may overflow in an
					* unpredictable manner on 16-bit systems; not a problem
					* with any known compiler so far, though */

	temp = ((unsigned)(*(pkeys + 2)) & 0xffff) | 2;
	return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

static int update_keys(unsigned long* pkeys, const unsigned long* pcrc_32_tab, int c)
{
	(*(pkeys + 0)) = CRC32((*(pkeys + 0)), c);
	(*(pkeys + 1)) += (*(pkeys + 0)) & 0xff;
	(*(pkeys + 1)) = (*(pkeys + 1)) * 134775813L + 1;
	{
		register int keyshift = (int)((*(pkeys + 1)) >> 24);
		(*(pkeys + 2)) = CRC32((*(pkeys + 2)), keyshift);
	}
	return c;
}

static void init_keys(const char* passwd, unsigned long* pkeys, const unsigned long* pcrc_32_tab)
{
	*(pkeys + 0) = 305419896L;
	*(pkeys + 1) = 591751049L;
	*(pkeys + 2) = 878082192L;
	while (*passwd != '\0') {
		update_keys(pkeys, pcrc_32_tab, (int)*passwd);
		passwd++;
	}
}

#define zdecode(pkeys,pcrc_32_tab,c) \
    (update_keys(pkeys,pcrc_32_tab,c ^= decrypt_byte(pkeys,pcrc_32_tab)))

#define zencode(pkeys,pcrc_32_tab,c,t) \
    (t=decrypt_byte(pkeys,pcrc_32_tab), update_keys(pkeys,pcrc_32_tab,c), t^(c))

//-------------------------------------------------------------------------unzip-----------------------------------------------------------------------------------------------------------
int unz64local_getByte(const zlib_filefunc64_def* pzlib_filefunc_def, voidpf filestream, int *pi)
{
	unsigned char c;
	int err = (int)ZREAD64(*pzlib_filefunc_def, filestream, &c, 1);
	if (err == 1)
	{
		*pi = (int)c;
		return UNZ_OK;
	}
	else
	{
		if (ZERROR64(*pzlib_filefunc_def, filestream))
			return UNZ_ERRNO;
		else
			return UNZ_EOF;
	}
}

int unz64local_getShort(const zlib_filefunc64_def* pzlib_filefunc_def, voidpf filestream, uLong *pX)
{
	uLong x;
	int i = 0;
	int err;

	err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x = (uLong)i;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((uLong)i) << 8;

	if (err == UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}

int unz64local_getLong(const zlib_filefunc64_def* pzlib_filefunc_def, voidpf filestream, uLong *pX)
{
	uLong x;
	int i = 0;
	int err;

	err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x = (uLong)i;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((uLong)i) << 8;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((uLong)i) << 16;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x += ((uLong)i) << 24;

	if (err == UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}

int unz64local_getLong64(const zlib_filefunc64_def* pzlib_filefunc_def, voidpf filestream, ZPOS64_T *pX)
{
	ZPOS64_T x;
	int i = 0;
	int err;

	err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x = (ZPOS64_T)i;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 8;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 16;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 24;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 32;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 40;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 48;

	if (err == UNZ_OK)
		err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
	x |= ((ZPOS64_T)i) << 56;

	if (err == UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}

int strcmpcasenosensitive_internal (const char* fileName1, const char* fileName2)
{
    for (;;)
    {
        char c1=*(fileName1++);
        char c2=*(fileName2++);
        if ((c1>='a') && (c1<='z'))
            c1 -= 0x20;
        if ((c2>='a') && (c2<='z'))
            c2 -= 0x20;
        if (c1=='\0')
            return ((c2=='\0') ? 0 : -1);
        if (c2=='\0')
            return 1;
        if (c1<c2)
            return -1;
        if (c1>c2)
            return 1;
    }
}

#define CASESENSITIVITYDEFAULTVALUE 2
#define STRCMPCASENOSENTIVEFUNCTION strcmpcasenosensitive_internal

int ZEXPORT unzStringFileNameCompare(const char*  fileName1, const char*  fileName2, int iCaseSensitivity)

{
	if (iCaseSensitivity == 0)
		iCaseSensitivity = CASESENSITIVITYDEFAULTVALUE;

	if (iCaseSensitivity == 1)
		return strcmp(fileName1, fileName2);

	return STRCMPCASENOSENTIVEFUNCTION(fileName1, fileName2);
}

#define BUFREADCOMMENT (0x400)

ZPOS64_T unz64local_SearchCentralDir(const zlib_filefunc64_def* pzlib_filefunc_def, voidpf filestream)
{
    unsigned char* buf;
    ZPOS64_T uSizeFile;
    ZPOS64_T uBackRead;
    ZPOS64_T uMaxBack=0xffff; /* maximum size of global comment */
    ZPOS64_T uPosFound=0;

    if (ZSEEK64(*pzlib_filefunc_def,filestream,0,ZLIB_FILEFUNC_SEEK_END) != 0)
        return 0;


    uSizeFile = ZTELL64(*pzlib_filefunc_def,filestream);

    if (uMaxBack>uSizeFile)
        uMaxBack = uSizeFile;

    buf = (unsigned char*)ALLOC(BUFREADCOMMENT+4);
    if (buf==NULL)
        return 0;

    uBackRead = 4;
    while (uBackRead<uMaxBack)
    {
        uLong uReadSize;
        ZPOS64_T uReadPos ;
        int i;
        if (uBackRead+BUFREADCOMMENT>uMaxBack)
            uBackRead = uMaxBack;
        else
            uBackRead+=BUFREADCOMMENT;
        uReadPos = uSizeFile-uBackRead ;

        uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ?
                     (BUFREADCOMMENT+4) : (uLong)(uSizeFile-uReadPos);
        if (ZSEEK64(*pzlib_filefunc_def,filestream,uReadPos,ZLIB_FILEFUNC_SEEK_SET)!=0)
            break;

        if (ZREAD64(*pzlib_filefunc_def,filestream,buf,uReadSize)!=uReadSize)
            break;

        for (i=(int)uReadSize-3; (i--)>0;)
            if (((*(buf+i))==0x50) && ((*(buf+i+1))==0x4b) &&
                ((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))
            {
                uPosFound = uReadPos+i;
                break;
            }

        if (uPosFound!=0)
            break;
    }
    TRYFREE(buf);
    return uPosFound;
}

ZPOS64_T unz64local_SearchCentralDir64(const zlib_filefunc64_def* pzlib_filefunc_def, voidpf filestream)
{
	unsigned char* buf;
	ZPOS64_T uSizeFile;
	ZPOS64_T uBackRead;
	ZPOS64_T uMaxBack = 0xffff; /* maximum size of global comment */
	ZPOS64_T uPosFound = 0;
	uLong uL;
	ZPOS64_T relativeOffset;

	if (ZSEEK64(*pzlib_filefunc_def, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0)
		return 0;


	uSizeFile = ZTELL64(*pzlib_filefunc_def, filestream);

	if (uMaxBack > uSizeFile)
		uMaxBack = uSizeFile;

	buf = (unsigned char*)ALLOC(BUFREADCOMMENT + 4);
	if (buf == NULL)
		return 0;

	uBackRead = 4;
	while (uBackRead < uMaxBack)
	{
		uLong uReadSize;
		ZPOS64_T uReadPos;
		int i;
		if (uBackRead + BUFREADCOMMENT > uMaxBack)
			uBackRead = uMaxBack;
		else
			uBackRead += BUFREADCOMMENT;
		uReadPos = uSizeFile - uBackRead;

		uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ?
			(BUFREADCOMMENT + 4) : (uLong)(uSizeFile - uReadPos);
		if (ZSEEK64(*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
			break;

		if (ZREAD64(*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
			break;

		for (i = (int)uReadSize - 3; (i--) > 0;)
			if (((*(buf + i)) == 0x50) && ((*(buf + i + 1)) == 0x4b) &&
				((*(buf + i + 2)) == 0x06) && ((*(buf + i + 3)) == 0x07))
			{
				uPosFound = uReadPos + i;
				break;
			}

		if (uPosFound != 0)
			break;
	}
	TRYFREE(buf);
	if (uPosFound == 0)
		return 0;

	/* Zip64 end of central directory locator */
	if (ZSEEK64(*pzlib_filefunc_def, filestream, uPosFound, ZLIB_FILEFUNC_SEEK_SET) != 0)
		return 0;

	/* the signature, already checked */
	if (unz64local_getLong(pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
		return 0;

	/* number of the disk with the start of the zip64 end of  central directory */
	if (unz64local_getLong(pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
		return 0;
	if (uL != 0)
		return 0;

	/* relative offset of the zip64 end of central directory record */
	if (unz64local_getLong64(pzlib_filefunc_def, filestream, &relativeOffset) != UNZ_OK)
		return 0;

	/* total number of disks */
	if (unz64local_getLong(pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
		return 0;
	if (uL != 1)
		return 0;

	/* Goto end of central directory record */
	if (ZSEEK64(*pzlib_filefunc_def, filestream, relativeOffset, ZLIB_FILEFUNC_SEEK_SET) != 0)
		return 0;

	/* the signature */
	if (unz64local_getLong(pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
		return 0;

	if (uL != 0x06064b50)
		return 0;

	return relativeOffset;
}
/*
* 打开压缩文件
* 参数一路径
* 参数二内存流
*/
unzFile unzOpen(const wchar_t *path, void *buff, int len)
{
	unz64_s us;
	unz64_s *s;
	ZPOS64_T central_pos;
	uLong   uL;

	uLong number_disk;          /* number of the current dist, used for
								   spaning ZIP, unsupported, always 0*/
	uLong number_disk_with_CD;  /* number the the disk with central dir, used
								   for spaning ZIP, unsupported, always 0*/
	ZPOS64_T number_entry_CD;      /* total number of entries in
								   the central dir
								   (same than number_entry on nospan) */

	int err = UNZ_OK;
	us.z_filefunc.zseek64_file = NULL;
	us.z_filefunc.ztell64_file = NULL;
	if (path){
		us.z_filefunc.zopen64_file = fopen64_file_func;
		us.z_filefunc.zread_file = fread_file_func;
		us.z_filefunc.ztell64_file = ftell64_file_func;
		us.z_filefunc.zseek64_file = fseek64_file_func;
		us.z_filefunc.zclose_file = fclose_file_func;
		us.z_filefunc.zerror_file = ferror_file_func;

		/*打开文件*/
		us.filestream = ZOPEN64(us.z_filefunc, path, ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING);
		if (us.filestream == NULL)
			return NULL;
	}
	else if (buff && len > 0){
		us.z_filefunc.zread_file = fread_stream_func;
		us.z_filefunc.ztell64_file = ftell64_stream_func;
		us.z_filefunc.zseek64_file = fseek64_stream_func;
		us.z_filefunc.zclose_file = fclose_stream_func;
		us.z_filefunc.zerror_file = ferror_stream_func;

		unz64_stream *p = malloc(sizeof(unz64_stream));
		memset(p, 0, sizeof(unz64_stream));
		p->buff = malloc(len);
		p->len = len;
		memcpy(p->buff, buff, len);
		us.filestream = (void *)p;
	}


	central_pos = unz64local_SearchCentralDir64(&us.z_filefunc, us.filestream);
	if (central_pos)
	{
		uLong uS;
		ZPOS64_T uL64;

		us.isZip64 = 1;

		if (ZSEEK64(us.z_filefunc, us.filestream,
			central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
			err = UNZ_ERRNO;

		/* the signature, already checked */
		if (unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
			err = UNZ_ERRNO;

		/* size of zip64 end of central directory record */
		if (unz64local_getLong64(&us.z_filefunc, us.filestream, &uL64) != UNZ_OK)
			err = UNZ_ERRNO;

		/* version made by */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &uS) != UNZ_OK)
			err = UNZ_ERRNO;

		/* version needed to extract */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &uS) != UNZ_OK)
			err = UNZ_ERRNO;

		/* number of this disk */
		if (unz64local_getLong(&us.z_filefunc, us.filestream, &number_disk) != UNZ_OK)
			err = UNZ_ERRNO;

		/* number of the disk with the start of the central directory */
		if (unz64local_getLong(&us.z_filefunc, us.filestream, &number_disk_with_CD) != UNZ_OK)
			err = UNZ_ERRNO;

		/* total number of entries in the central directory on this disk */
		if (unz64local_getLong64(&us.z_filefunc, us.filestream, &us.gi.number_entry) != UNZ_OK)
			err = UNZ_ERRNO;

		/* total number of entries in the central directory */
		if (unz64local_getLong64(&us.z_filefunc, us.filestream, &number_entry_CD) != UNZ_OK)
			err = UNZ_ERRNO;

		if ((number_entry_CD != us.gi.number_entry) ||
			(number_disk_with_CD != 0) ||
			(number_disk != 0))
			err = UNZ_BADZIPFILE;

		/* size of the central directory */
		if (unz64local_getLong64(&us.z_filefunc, us.filestream, &us.size_central_dir) != UNZ_OK)
			err = UNZ_ERRNO;

		/* offset of start of central directory with respect to the
		  starting disk number */
		if (unz64local_getLong64(&us.z_filefunc, us.filestream, &us.offset_central_dir) != UNZ_OK)
			err = UNZ_ERRNO;

		us.gi.size_comment = 0;
	}
	else
	{
		central_pos = unz64local_SearchCentralDir(&us.z_filefunc, us.filestream);
		if (central_pos == 0)
			err = UNZ_ERRNO;

		us.isZip64 = 0;

		if (ZSEEK64(us.z_filefunc, us.filestream,
			central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
			err = UNZ_ERRNO;

		/* the signature, already checked */
		if (unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
			err = UNZ_ERRNO;

		/* number of this disk */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &number_disk) != UNZ_OK)
			err = UNZ_ERRNO;

		/* number of the disk with the start of the central directory */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &number_disk_with_CD) != UNZ_OK)
			err = UNZ_ERRNO;

		/* total number of entries in the central dir on this disk */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
			err = UNZ_ERRNO;
		us.gi.number_entry = uL;

		/* total number of entries in the central dir */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
			err = UNZ_ERRNO;
		number_entry_CD = uL;

		if ((number_entry_CD != us.gi.number_entry) ||
			(number_disk_with_CD != 0) ||
			(number_disk != 0))
			err = UNZ_BADZIPFILE;

		/* size of the central directory */
		if (unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
			err = UNZ_ERRNO;
		us.size_central_dir = uL;

		/* offset of start of central directory with respect to the
			starting disk number */
		if (unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
			err = UNZ_ERRNO;
		us.offset_central_dir = uL;

		/* zipfile comment length */
		if (unz64local_getShort(&us.z_filefunc, us.filestream, &us.gi.size_comment) != UNZ_OK)
			err = UNZ_ERRNO;
	}

	if ((central_pos < us.offset_central_dir + us.size_central_dir) &&
		(err == UNZ_OK))
		err = UNZ_BADZIPFILE;

	if (err != UNZ_OK)
	{
		ZCLOSE64(us.z_filefunc, us.filestream);
		return NULL;
	}

	us.byte_before_the_zipfile = central_pos -
		(us.offset_central_dir + us.size_central_dir);
	us.central_pos = central_pos;
	us.pfile_in_zip_read = NULL;
	us.encrypted = 0;


	s = (unz64_s*)ALLOC(sizeof(unz64_s));
	if (s != NULL)
	{
		*s = us;
		unzGoToFirstFile((unzFile)s);
	}
	return (unzFile)s;
}

int unzClose (unzFile file)
{
    unz64_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s*)file;

    if (s->pfile_in_zip_read!=NULL)
        unzCloseCurrentFile(file);

    ZCLOSE64(s->z_filefunc, s->filestream);
    TRYFREE(s);
    return UNZ_OK;
}

int unzGetGlobalInfo64 (unzFile file, unz_global_info64* pglobal_info)
{
    unz64_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s*)file;
    *pglobal_info=s->gi;
    return UNZ_OK;
}

int unzGetGlobalComment(unzFile file, char * szComment, uLong uSizeBuf)
{
	unz64_s* s;
	uLong uReadThis;
	if (file == NULL)
		return (int)UNZ_PARAMERROR;
	s = (unz64_s*)file;

	uReadThis = uSizeBuf;
	if (uReadThis>s->gi.size_comment)
		uReadThis = s->gi.size_comment;

	if (ZSEEK64(s->z_filefunc, s->filestream, s->central_pos + 22, ZLIB_FILEFUNC_SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (uReadThis>0)
	{
		*szComment = '\0';
		if (ZREAD64(s->z_filefunc, s->filestream, szComment, uReadThis) != uReadThis)
			return UNZ_ERRNO;
	}

	if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment))
		*(szComment + s->gi.size_comment) = '\0';
	return (int)uReadThis;
}

void unz64local_DosDateToTmuDate (ZPOS64_T ulDosDate, tm_unz* ptm)
{
    ZPOS64_T uDate;
    uDate = (ZPOS64_T)(ulDosDate>>16);
    ptm->tm_mday = (uInt)(uDate&0x1f) ;
    ptm->tm_mon =  (uInt)((((uDate)&0x1E0)/0x20)-1) ;
    ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980) ;

    ptm->tm_hour = (uInt) ((ulDosDate &0xF800)/0x800);
    ptm->tm_min =  (uInt) ((ulDosDate&0x7E0)/0x20) ;
    ptm->tm_sec =  (uInt) (2*(ulDosDate&0x1f)) ;
}

int unz64local_GetCurrentFileInfoInternal(unzFile file, unz_file_info64 *pfile_info, unz_file_info64_internal*pfile_info_internal, char *szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize, char *szComment, uLong commentBufferSize)
{
	unz64_s* s;
	unz_file_info64 file_info;
	unz_file_info64_internal file_info_internal;
	int err = UNZ_OK;
	uLong uMagic;
	long lSeek = 0;
	uLong uL;

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz64_s*)file;
	if (ZSEEK64(s->z_filefunc, s->filestream,
		s->pos_in_central_dir + s->byte_before_the_zipfile,
		ZLIB_FILEFUNC_SEEK_SET) != 0)
		err = UNZ_ERRNO;


	/* we check the magic */
	if (err == UNZ_OK)
	{
		if (unz64local_getLong(&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x02014b50)
			err = UNZ_BADZIPFILE;
	}

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.version) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.version_needed) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.flag) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.compression_method) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &file_info.dosDate) != UNZ_OK)
		err = UNZ_ERRNO;

	unz64local_DosDateToTmuDate(file_info.dosDate, &file_info.tmu_date);

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &file_info.crc) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
		err = UNZ_ERRNO;
	file_info.compressed_size = uL;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
		err = UNZ_ERRNO;
	file_info.uncompressed_size = uL;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.size_filename) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.size_file_extra) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.size_file_comment) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.disk_num_start) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.internal_fa) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &file_info.external_fa) != UNZ_OK)
		err = UNZ_ERRNO;

	// relative offset of local header
	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
		err = UNZ_ERRNO;
	file_info_internal.offset_curfile = uL;

	lSeek += file_info.size_filename;
	if ((err == UNZ_OK) && (szFileName != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_filename < fileNameBufferSize)
		{
			*(szFileName + file_info.size_filename) = '\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;

		if ((file_info.size_filename>0) && (fileNameBufferSize > 0))
			if (ZREAD64(s->z_filefunc, s->filestream, szFileName, uSizeRead) != uSizeRead)
				err = UNZ_ERRNO;
		lSeek -= uSizeRead;
	}

	// Read extrafield
	if ((err == UNZ_OK) && (extraField != NULL))
	{
		ZPOS64_T uSizeRead;
		if (file_info.size_file_extra < extraFieldBufferSize)
			uSizeRead = file_info.size_file_extra;
		else
			uSizeRead = extraFieldBufferSize;

		if (lSeek != 0)
		{
			if (ZSEEK64(s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		}

		if ((file_info.size_file_extra>0) && (extraFieldBufferSize > 0))
			if (ZREAD64(s->z_filefunc, s->filestream, extraField, (uLong)uSizeRead) != uSizeRead)
				err = UNZ_ERRNO;

		lSeek += file_info.size_file_extra - (uLong)uSizeRead;
	}
	else
		lSeek += file_info.size_file_extra;


	if ((err == UNZ_OK) && (file_info.size_file_extra != 0))
	{
		uLong acc = 0;

		// since lSeek now points to after the extra field we need to move back
		lSeek -= file_info.size_file_extra;

		if (lSeek != 0)
		{
			if (ZSEEK64(s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		}

		while (acc < file_info.size_file_extra)
		{
			uLong headerId;
			uLong dataSize;

			if (unz64local_getShort(&s->z_filefunc, s->filestream, &headerId) != UNZ_OK)
				err = UNZ_ERRNO;

			if (unz64local_getShort(&s->z_filefunc, s->filestream, &dataSize) != UNZ_OK)
				err = UNZ_ERRNO;

			/* ZIP64 extra fields */
			if (headerId == 0x0001)
			{
				uLong uL;

				if (file_info.uncompressed_size == (ZPOS64_T)(unsigned long)-1)
				{
					if (unz64local_getLong64(&s->z_filefunc, s->filestream, &file_info.uncompressed_size) != UNZ_OK)
						err = UNZ_ERRNO;
				}

				if (file_info.compressed_size == (ZPOS64_T)(unsigned long)-1)
				{
					if (unz64local_getLong64(&s->z_filefunc, s->filestream, &file_info.compressed_size) != UNZ_OK)
						err = UNZ_ERRNO;
				}

				if (file_info_internal.offset_curfile == (ZPOS64_T)(unsigned long)-1)
				{
					/* Relative Header offset */
					if (unz64local_getLong64(&s->z_filefunc, s->filestream, &file_info_internal.offset_curfile) != UNZ_OK)
						err = UNZ_ERRNO;
				}

				if (file_info.disk_num_start == (unsigned long)-1)
				{
					/* Disk Start Number */
					if (unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
						err = UNZ_ERRNO;
				}

			}
			else
			{
				if (ZSEEK64(s->z_filefunc, s->filestream, dataSize, ZLIB_FILEFUNC_SEEK_CUR) != 0)
					err = UNZ_ERRNO;
			}

			acc += 2 + 2 + dataSize;
		}
	}

	if ((err == UNZ_OK) && (szComment != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_file_comment < commentBufferSize)
		{
			*(szComment + file_info.size_file_comment) = '\0';
			uSizeRead = file_info.size_file_comment;
		}
		else
			uSizeRead = commentBufferSize;

		if (lSeek != 0)
		{
			if (ZSEEK64(s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		}

		if ((file_info.size_file_comment>0) && (commentBufferSize > 0))
			if (ZREAD64(s->z_filefunc, s->filestream, szComment, uSizeRead) != uSizeRead)
				err = UNZ_ERRNO;
		lSeek += file_info.size_file_comment - uSizeRead;
	}
	else
		lSeek += file_info.size_file_comment;


	if ((err == UNZ_OK) && (pfile_info != NULL))
		*pfile_info = file_info;

	if ((err == UNZ_OK) && (pfile_info_internal != NULL))
		*pfile_info_internal = file_info_internal;

	return err;
}

int unzGetCurrentFileInfo64(unzFile file, unz_file_info64 * pfile_info, char * szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
{
	return unz64local_GetCurrentFileInfoInternal(file, pfile_info, NULL,
		szFileName, fileNameBufferSize,
		extraField, extraFieldBufferSize,
		szComment, commentBufferSize);
}

int unzGoToFirstFile (unzFile file)
{
    int err=UNZ_OK;
    unz64_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s*)file;
    s->pos_in_central_dir=s->offset_central_dir;
    s->num_file=0;
    err=unz64local_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                             &s->cur_file_info_internal,
                                             NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

int unzGoToNextFile (unzFile  file)
{
    unz64_s* s;
    int err;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;
    if (s->gi.number_entry != 0xffff)    /* 2^16 files overflow hack */
      if (s->num_file+1==s->gi.number_entry)
        return UNZ_END_OF_LIST_OF_FILE;

    s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
            s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment ;
    s->num_file++;
    err = unz64local_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                               &s->cur_file_info_internal,
                                               NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

int unzLocateFile (unzFile file, const char *szFileName, int iCaseSensitivity)
{
    unz64_s* s;
    int err;

    /* We remember the 'current' position in the file so that we can jump
     * back there if we fail.
     */
    unz_file_info64 cur_file_infoSaved;
    unz_file_info64_internal cur_file_info_internalSaved;
    ZPOS64_T num_fileSaved;
    ZPOS64_T pos_in_central_dirSaved;


    if (file==NULL)
        return UNZ_PARAMERROR;

    if (strlen(szFileName)>=UNZ_MAXFILENAMEINZIP)
        return UNZ_PARAMERROR;

    s=(unz64_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;

    /* Save the current state */
    num_fileSaved = s->num_file;
    pos_in_central_dirSaved = s->pos_in_central_dir;
    cur_file_infoSaved = s->cur_file_info;
    cur_file_info_internalSaved = s->cur_file_info_internal;

    err = unzGoToFirstFile(file);

    while (err == UNZ_OK)
    {
        char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
        err = unzGetCurrentFileInfo64(file,NULL,
                                    szCurrentFileName,sizeof(szCurrentFileName)-1,
                                    NULL,0,NULL,0);
        if (err == UNZ_OK)
        {
            if (unzStringFileNameCompare(szCurrentFileName,
                                            szFileName,iCaseSensitivity)==0)
                return UNZ_OK;
            err = unzGoToNextFile(file);
        }
    }

    /* We failed, so restore the state of the 'current file' to where we
     * were.
     */
    s->num_file = num_fileSaved ;
    s->pos_in_central_dir = pos_in_central_dirSaved ;
    s->cur_file_info = cur_file_infoSaved;
    s->cur_file_info_internal = cur_file_info_internalSaved;
    return err;
}

int unz64local_CheckCurrentFileCoherencyHeader(unz64_s* s, uInt* piSizeVar, ZPOS64_T * poffset_local_extrafield, uInt  * psize_local_extrafield)
{
	uLong uMagic, uData, uFlags;
	uLong size_filename;
	uLong size_extra_field;
	int err = UNZ_OK;

	*piSizeVar = 0;
	*poffset_local_extrafield = 0;
	*psize_local_extrafield = 0;

	if (ZSEEK64(s->z_filefunc, s->filestream, s->cur_file_info_internal.offset_curfile +
		s->byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET) != 0)
		return UNZ_ERRNO;


	if (err == UNZ_OK)
	{
		if (unz64local_getLong(&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x04034b50)
			err = UNZ_BADZIPFILE;
	}

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &uData) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &uFlags) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.compression_method))
		err = UNZ_BADZIPFILE;

	if ((err == UNZ_OK) && (s->cur_file_info.compression_method != 0) &&
		/* #ifdef HAVE_BZIP2 */
		(s->cur_file_info.compression_method != Z_BZIP2ED) &&
		/* #endif */
		(s->cur_file_info.compression_method != Z_DEFLATED))
		err = UNZ_BADZIPFILE;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* date/time */
		err = UNZ_ERRNO;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* crc */
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.crc) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* size compr */
		err = UNZ_ERRNO;
	else if (uData != 0xFFFFFFFF && (err == UNZ_OK) && (uData != s->cur_file_info.compressed_size) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* size uncompr */
		err = UNZ_ERRNO;
	else if (uData != 0xFFFFFFFF && (err == UNZ_OK) && (uData != s->cur_file_info.uncompressed_size) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &size_filename) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (size_filename != s->cur_file_info.size_filename))
		err = UNZ_BADZIPFILE;

	*piSizeVar += (uInt)size_filename;

	if (unz64local_getShort(&s->z_filefunc, s->filestream, &size_extra_field) != UNZ_OK)
		err = UNZ_ERRNO;
	*poffset_local_extrafield = s->cur_file_info_internal.offset_curfile +
		SIZEZIPLOCALHEADER + size_filename;
	*psize_local_extrafield = (uInt)size_extra_field;

	*piSizeVar += (uInt)size_extra_field;

	return err;
}

int unzOpenCurrentFile3(unzFile file, int* method, int* level, int raw, const char* password)
{
	int err = UNZ_OK;
	uInt iSizeVar;
	unz64_s* s;
	file_in_zip64_read_info_s* pfile_in_zip_read_info;
	ZPOS64_T offset_local_extrafield;  /* offset of the local extra field */
	uInt  size_local_extrafield;    /* size of the local extra field */
	char source[12];

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz64_s*)file;
	if (!s->current_file_ok)
		return UNZ_PARAMERROR;

	if (s->pfile_in_zip_read != NULL)
		unzCloseCurrentFile(file);

	if (unz64local_CheckCurrentFileCoherencyHeader(s, &iSizeVar, &offset_local_extrafield, &size_local_extrafield) != UNZ_OK)
		return UNZ_BADZIPFILE;

	pfile_in_zip_read_info = (file_in_zip64_read_info_s*)ALLOC(sizeof(file_in_zip64_read_info_s));
	if (pfile_in_zip_read_info == NULL)
		return UNZ_INTERNALERROR;

	pfile_in_zip_read_info->read_buffer = (char*)ALLOC(UNZ_BUFSIZE);
	pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
	pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
	pfile_in_zip_read_info->pos_local_extrafield = 0;
	pfile_in_zip_read_info->raw = raw;

	if (pfile_in_zip_read_info->read_buffer == NULL)
	{
		TRYFREE(pfile_in_zip_read_info);
		return UNZ_INTERNALERROR;
	}

	pfile_in_zip_read_info->stream_initialised = 0;

	if (method != NULL)
		*method = (int)s->cur_file_info.compression_method;

	if (level != NULL)
	{
		*level = 6;
		switch (s->cur_file_info.flag & 0x06)
		{
		case 6: *level = 1; break;
		case 4: *level = 2; break;
		case 2: *level = 9; break;
		}
	}

	if ((s->cur_file_info.compression_method != 0) &&
		/* #ifdef HAVE_BZIP2 */
		(s->cur_file_info.compression_method != Z_BZIP2ED) &&
		/* #endif */
		(s->cur_file_info.compression_method != Z_DEFLATED))

		err = UNZ_BADZIPFILE;

	pfile_in_zip_read_info->crc32_wait = s->cur_file_info.crc;
	pfile_in_zip_read_info->crc32 = 0;
	pfile_in_zip_read_info->total_out_64 = 0;
	pfile_in_zip_read_info->compression_method = s->cur_file_info.compression_method;
	pfile_in_zip_read_info->filestream = s->filestream;
	pfile_in_zip_read_info->z_filefunc = s->z_filefunc;
	pfile_in_zip_read_info->byte_before_the_zipfile = s->byte_before_the_zipfile;

	pfile_in_zip_read_info->stream.total_out = 0;

	if ((s->cur_file_info.compression_method == Z_BZIP2ED) && (!raw))
	{
		pfile_in_zip_read_info->raw = 1;
	}
	else if ((s->cur_file_info.compression_method == Z_DEFLATED) && (!raw))
	{
		pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
		pfile_in_zip_read_info->stream.zfree = (free_func)0;
		pfile_in_zip_read_info->stream.opaque = (voidpf)0;
		pfile_in_zip_read_info->stream.next_in = 0;
		pfile_in_zip_read_info->stream.avail_in = 0;

		err = inflateInit2(&pfile_in_zip_read_info->stream, -MAX_WBITS);
		if (err == Z_OK)
			pfile_in_zip_read_info->stream_initialised = Z_DEFLATED;
		else
		{
			TRYFREE(pfile_in_zip_read_info);
			return err;
		}
	}
	pfile_in_zip_read_info->rest_read_compressed =
		s->cur_file_info.compressed_size;
	pfile_in_zip_read_info->rest_read_uncompressed =
		s->cur_file_info.uncompressed_size;


	pfile_in_zip_read_info->pos_in_zipfile =
		s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
		iSizeVar;

	pfile_in_zip_read_info->stream.avail_in = (uInt)0;

	s->pfile_in_zip_read = pfile_in_zip_read_info;
	s->encrypted = 0;

	if (password != NULL)
	{
		int i;
		s->pcrc_32_tab = (const unsigned long *)get_crc_table();
		init_keys(password, s->keys, s->pcrc_32_tab);
		if (ZSEEK64(s->z_filefunc, s->filestream,
			s->pfile_in_zip_read->pos_in_zipfile +
			s->pfile_in_zip_read->byte_before_the_zipfile,
			SEEK_SET) != 0)
			return UNZ_INTERNALERROR;
		if (ZREAD64(s->z_filefunc, s->filestream, source, 12) < 12)
			return UNZ_INTERNALERROR;

		for (i = 0; i < 12; i++)
			zdecode(s->keys, s->pcrc_32_tab, source[i]);

		s->pfile_in_zip_read->pos_in_zipfile += 12;
		s->encrypted = 1;
	}
	return UNZ_OK;
}

int unzOpenCurrentFilePassword (unzFile file, const char*  password)
{
    return unzOpenCurrentFile3(file, NULL, NULL, 0, password);
}

int unzReadCurrentFile  (unzFile file, voidp buf, unsigned len)
{
    int err=UNZ_OK;
    uInt iRead = 0;
    unz64_s* s;
    file_in_zip64_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;


    if ((pfile_in_zip_read_info->read_buffer == NULL))
        return UNZ_END_OF_LIST_OF_FILE;
    if (len==0)
        return 0;

    pfile_in_zip_read_info->stream.next_out = (Bytef*)buf;

    pfile_in_zip_read_info->stream.avail_out = (uInt)len;

    if ((len>pfile_in_zip_read_info->rest_read_uncompressed) &&
        (!(pfile_in_zip_read_info->raw)))
        pfile_in_zip_read_info->stream.avail_out =
            (uInt)pfile_in_zip_read_info->rest_read_uncompressed;

    if ((len>pfile_in_zip_read_info->rest_read_compressed+
           pfile_in_zip_read_info->stream.avail_in) &&
         (pfile_in_zip_read_info->raw))
        pfile_in_zip_read_info->stream.avail_out =
            (uInt)pfile_in_zip_read_info->rest_read_compressed+
            pfile_in_zip_read_info->stream.avail_in;

    while (pfile_in_zip_read_info->stream.avail_out>0)
    {
        if ((pfile_in_zip_read_info->stream.avail_in==0) &&
            (pfile_in_zip_read_info->rest_read_compressed>0))
        {
            uInt uReadThis = UNZ_BUFSIZE;
            if (pfile_in_zip_read_info->rest_read_compressed<uReadThis)
                uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
            if (uReadThis == 0)
                return UNZ_EOF;
            if (ZSEEK64(pfile_in_zip_read_info->z_filefunc,
                      pfile_in_zip_read_info->filestream,
                      pfile_in_zip_read_info->pos_in_zipfile +
                         pfile_in_zip_read_info->byte_before_the_zipfile,
                         ZLIB_FILEFUNC_SEEK_SET)!=0)
                return UNZ_ERRNO;
            if (ZREAD64(pfile_in_zip_read_info->z_filefunc,
                      pfile_in_zip_read_info->filestream,
                      pfile_in_zip_read_info->read_buffer,
                      uReadThis)!=uReadThis)
                return UNZ_ERRNO;
            if(s->encrypted)
            {
                uInt i;
                for(i=0;i<uReadThis;i++)
                  pfile_in_zip_read_info->read_buffer[i] =
                      zdecode(s->keys,s->pcrc_32_tab,
                              pfile_in_zip_read_info->read_buffer[i]);
            }


            pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

            pfile_in_zip_read_info->rest_read_compressed-=uReadThis;

            pfile_in_zip_read_info->stream.next_in =
                (Bytef*)pfile_in_zip_read_info->read_buffer;
            pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
        }

        if ((pfile_in_zip_read_info->compression_method==0) || (pfile_in_zip_read_info->raw))
        {
            uInt uDoCopy,i ;

            if ((pfile_in_zip_read_info->stream.avail_in == 0) &&
                (pfile_in_zip_read_info->rest_read_compressed == 0))
                return (iRead==0) ? UNZ_EOF : iRead;

            if (pfile_in_zip_read_info->stream.avail_out <
                            pfile_in_zip_read_info->stream.avail_in)
                uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
            else
                uDoCopy = pfile_in_zip_read_info->stream.avail_in ;

            for (i=0;i<uDoCopy;i++)
                *(pfile_in_zip_read_info->stream.next_out+i) =
                        *(pfile_in_zip_read_info->stream.next_in+i);

            pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uDoCopy;

            pfile_in_zip_read_info->crc32 = crc32(pfile_in_zip_read_info->crc32,
                                pfile_in_zip_read_info->stream.next_out,
                                uDoCopy);
            pfile_in_zip_read_info->rest_read_uncompressed-=uDoCopy;
            pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
            pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
            pfile_in_zip_read_info->stream.next_out += uDoCopy;
            pfile_in_zip_read_info->stream.next_in += uDoCopy;
            pfile_in_zip_read_info->stream.total_out += uDoCopy;
            iRead += uDoCopy;
        }
        else if (pfile_in_zip_read_info->compression_method==Z_BZIP2ED)
        {
        } // end Z_BZIP2ED
        else
        {
            ZPOS64_T uTotalOutBefore,uTotalOutAfter;
            const Bytef *bufBefore;
            ZPOS64_T uOutThis;
            int flush=Z_SYNC_FLUSH;

            uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
            bufBefore = pfile_in_zip_read_info->stream.next_out;

            err=inflate(&pfile_in_zip_read_info->stream,flush);

            if ((err>=0) && (pfile_in_zip_read_info->stream.msg!=NULL))
              err = Z_DATA_ERROR;

            uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
            uOutThis = uTotalOutAfter-uTotalOutBefore;

            pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uOutThis;

            pfile_in_zip_read_info->crc32 =
                crc32(pfile_in_zip_read_info->crc32,bufBefore,
                        (uInt)(uOutThis));

            pfile_in_zip_read_info->rest_read_uncompressed -=
                uOutThis;

            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

            if (err==Z_STREAM_END)
                return (iRead==0) ? UNZ_EOF : iRead;
            if (err!=Z_OK)
                break;
        }
    }

    if (err==Z_OK)
        return iRead;
    return err;
}

int unzCloseCurrentFile (unzFile file)
{
    int err=UNZ_OK;

    unz64_s* s;
    file_in_zip64_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;


    if ((pfile_in_zip_read_info->rest_read_uncompressed == 0) &&
        (!pfile_in_zip_read_info->raw))
    {
        if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
            err=UNZ_CRCERROR;
    }


    TRYFREE(pfile_in_zip_read_info->read_buffer);
    pfile_in_zip_read_info->read_buffer = NULL;
    if (pfile_in_zip_read_info->stream_initialised == Z_DEFLATED)
        inflateEnd(&pfile_in_zip_read_info->stream);


    pfile_in_zip_read_info->stream_initialised = 0;
    TRYFREE(pfile_in_zip_read_info);

    s->pfile_in_zip_read=NULL;

    return err;
}