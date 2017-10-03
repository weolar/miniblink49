/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "curl_setup.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "strtoofft.h"
#include "urldata.h"
#include <curl/curl.h>
#include "progress.h"
#include "sendf.h"
#include "escape.h"
#include "file.h"
#include "speedcheck.h"
#include "getinfo.h"
#include "transfer.h"
#include "url.h"
#include "parsedate.h" /* for the week day and month names */
#include "warnless.h"
/* The last 3 #include files should be in this order */
#include "curl_printf.h"
#include "curl_memory.h"
#include "memdebug.h"

#if defined(WIN32) || defined(MSDOS) || defined(__EMX__) || defined(__SYMBIAN32__)
#define DOS_FILESYSTEM 1
#endif

#ifdef OPEN_NEEDS_ARG3
#  define open_readonly(p,f) open((p),(f),(0))
#else
#  define open_readonly(p,f) open((p),(f))
#endif


////////////////////////////////////////////////////////////////////////////////////////////////(ewe+)

#define EweUserFileIO  0x12345123

typedef struct userFILE{
	int        Flag;
	void*      fp;
}userFILE;

#define UserFD(fd)  ((userFILE*)fd)

#define IsUserFileIO( fd )  ( UserFD(fd)->Flag == EweUserFileIO )

bool    (__fastcall *userExistsFile)( const char * _Filename ) = 0;
void*   (__fastcall *userOpenFile)( const char * _Filename ) = 0;
int     (__fastcall *userCloseFile)( void* userFileHandle ) = 0;
int     (__fastcall *userReadFile)( void* userFileHandle, void *buf, size_t count ) = 0;
int     (__fastcall *userSeekFile)( void* userFileHandle, __int64 _Offset ) = 0;
__int64 (__fastcall *userFileSize)( void* userFileHandle ) = 0;

void SetFileSystemHook(
	bool  (__fastcall *lpExistsFile)( const char * _Filename ),
	void* (__fastcall *lpOpenFile)( const char * _Filename ),
	int   (__fastcall *lpCloseFile)( void* userFileHandle ),
	int   (__fastcall *lpReadFile)( void* userFileHandle, void *buf, size_t count ),
	int   (__fastcall *lpSeekFile)( void* userFileHandle, __int64 _Offset ),
	__int64 (__fastcall *lpFileSize)( void* userFileHandle ) )
{
	userExistsFile = lpExistsFile;
	userOpenFile = lpOpenFile;
	userCloseFile = lpCloseFile;
	userReadFile = lpReadFile;
	userSeekFile = lpSeekFile;
	userFileSize = lpFileSize;
}

wchar_t* utf8_to_wide_char(const char * inStr) {
    int cbMultiByte = 0;
    DWORD dwMinSize = 0;
    wchar_t* outStr = 0;

    FILE* fp = 0;

    cbMultiByte = strlen(inStr);
    if (0 == cbMultiByte)
        return 0;

    dwMinSize = MultiByteToWideChar(CP_UTF8, 0, inStr, cbMultiByte, NULL, 0);
    if (0 == dwMinSize)
        return 0;

    outStr = (wchar_t*)malloc((dwMinSize + 1) * 2);
    memset(outStr, 0, (dwMinSize + 1) * 2);

    // Convert headers from ASCII to Unicode.
    MultiByteToWideChar(CP_UTF8, 0, inStr, cbMultiByte, outStr, dwMinSize);

    return outStr;
}

FILE* fopen_wrap(/*_In_z_*/ const char * _Filename, /*_In_*/ const char * _OpenFlag) {
 wchar_t* filenameW = 0;
 wchar_t* openFlagW = 0;

 FILE* fp = 0;

 filenameW = utf8_to_wide_char(_Filename);
 openFlagW = utf8_to_wide_char(_OpenFlag);

 fp = _wfopen(filenameW, openFlagW);

 free(filenameW);
 free(openFlagW);

 if (fp)
     return fp;

 fp = fopen(_Filename, _OpenFlag);
 return fp;
}

__int64 ewefs_open(/*_In_z_*/ const char * _Filename, /*_In_*/ int _OpenFlag ){
	FILE* fp = 0;

	if( O_WRONLY & _OpenFlag ){
		if( O_APPEND & _OpenFlag ){
			fp = fopen_wrap( _Filename, "ab" );
		}else{
			fp = fopen_wrap( _Filename, "wb" );
		}
	}else{
		if( userOpenFile ){
			void* fp = userOpenFile( _Filename );
			if( fp ){
				userFILE* user = malloc(sizeof(userFILE));
				if( !user ){ userCloseFile(fp); }
				user->Flag = EweUserFileIO;
				user->fp = fp;
				return user ? user : -1;
			}
		}
		if( !fp ){
			fp = fopen_wrap( _Filename, "rb" );
		}
	}
	return fp ? fp : -1;
}

int ewefs_close( __int64 fd ){
	if( IsUserFileIO(fd) ){
		userCloseFile( UserFD(fd)->fp );
		free( UserFD(fd) );
		return 0;
	}
	return fclose( (FILE*)fd );
}

ssize_t ewefs_read(__int64 fd, void *buf, size_t count){
	return IsUserFileIO(fd) ? userReadFile( UserFD(fd)->fp, buf, count ) : read( fileno((FILE*)fd), buf, count );
}

ssize_t ewefs_write(__int64 fd, const void *buf, size_t count){
	return IsUserFileIO(fd) ? -1 : write( fileno((FILE*)fd), buf, count );
}

curl_off_t ewefs_lseek( __int64 fd, curl_off_t _Offset, int _Origin ){
	return IsUserFileIO(fd) ? userSeekFile( UserFD(fd)->fp, _Offset ) : _lseeki64( fileno((FILE*)fd), _Offset, _Origin );
}

int ewefs_fstat(/*_In_*/ __int64 fd, struct _stati64* _Stat){
	if( IsUserFileIO(fd) ){
		_Stat->st_size = userFileSize( UserFD(fd)->fp );
		return _Stat->st_size >= 0 ? 0 : -1;
	}
	return _fstati64( fileno((FILE*)fd), _Stat );
}

////////////////////////////////////////////////////////////////////////////////////////////////(ewe-)


/*
 * Forward declarations.
 */

static CURLcode file_do(struct connectdata *, bool *done);
static CURLcode file_done(struct connectdata *conn,
                          CURLcode status, bool premature);
static CURLcode file_connect(struct connectdata *conn, bool *done);
static CURLcode file_disconnect(struct connectdata *conn,
                                bool dead_connection);
static CURLcode file_setup_connection(struct connectdata *conn);

/*
 * FILE scheme handler.
 */

const struct Curl_handler Curl_handler_file = {
  "FILE",                               /* scheme */
  file_setup_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_done,                            /* done */
  ZERO_NULL,                            /* do_more */
  file_connect,                         /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_getsock */
  ZERO_NULL,                            /* doing_getsock */
  ZERO_NULL,                            /* domore_getsock */
  ZERO_NULL,                            /* perform_getsock */
  file_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* readwrite */
  0,                                    /* defport */
  CURLPROTO_FILE,                       /* protocol */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY /* flags */
};


static CURLcode file_setup_connection(struct connectdata *conn)
{
  /* allocate the FILE specific struct */
  conn->data->req.protop = calloc(1, sizeof(struct FILEPROTO));
  if(!conn->data->req.protop)
    return CURLE_OUT_OF_MEMORY;

  return CURLE_OK;
}

 /*
  Check if this is a range download, and if so, set the internal variables
  properly. This code is copied from the FTP implementation and might as
  well be factored out.
 */
static CURLcode file_range(struct connectdata *conn)
{
  curl_off_t from, to;
  curl_off_t totalsize=-1;
  char *ptr;
  char *ptr2;
  struct Curl_easy *data = conn->data;

  if(data->state.use_range && data->state.range) {
    from=curlx_strtoofft(data->state.range, &ptr, 0);
    while(*ptr && (ISSPACE(*ptr) || (*ptr=='-')))
      ptr++;
    to=curlx_strtoofft(ptr, &ptr2, 0);
    if(ptr == ptr2) {
      /* we didn't get any digit */
      to=-1;
    }
    if((-1 == to) && (from>=0)) {
      /* X - */
      data->state.resume_from = from;
      DEBUGF(infof(data, "RANGE %" CURL_FORMAT_CURL_OFF_T " to end of file\n",
                   from));
    }
    else if(from < 0) {
      /* -Y */
      data->req.maxdownload = -from;
      data->state.resume_from = from;
      DEBUGF(infof(data, "RANGE the last %" CURL_FORMAT_CURL_OFF_T " bytes\n",
                   -from));
    }
    else {
      /* X-Y */
      totalsize = to-from;
      data->req.maxdownload = totalsize+1; /* include last byte */
      data->state.resume_from = from;
      DEBUGF(infof(data, "RANGE from %" CURL_FORMAT_CURL_OFF_T
                   " getting %" CURL_FORMAT_CURL_OFF_T " bytes\n",
                   from, data->req.maxdownload));
    }
    DEBUGF(infof(data, "range-download from %" CURL_FORMAT_CURL_OFF_T
                 " to %" CURL_FORMAT_CURL_OFF_T ", totally %"
                 CURL_FORMAT_CURL_OFF_T " bytes\n",
                 from, to, data->req.maxdownload));
  }
  else
    data->req.maxdownload = -1;
  return CURLE_OK;
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.  We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
static CURLcode file_connect(struct connectdata *conn, bool *done)
{
  struct Curl_easy *data = conn->data;
  char *real_path;
  struct FILEPROTO *file = data->req.protop;
  __int64 fd;
#ifdef DOS_FILESYSTEM
  int i;
  char *actual_path;
#endif
  int real_path_len;

  real_path = curl_easy_unescape(data, data->state.path, 0, &real_path_len);
  if(!real_path)
    return CURLE_OUT_OF_MEMORY;

#ifdef DOS_FILESYSTEM
  /* If the first character is a slash, and there's
     something that looks like a drive at the beginning of
     the path, skip the slash.  If we remove the initial
     slash in all cases, paths without drive letters end up
     relative to the current directory which isn't how
     browsers work.

     Some browsers accept | instead of : as the drive letter
     separator, so we do too.

     On other platforms, we need the slash to indicate an
     absolute pathname.  On Windows, absolute paths start
     with a drive letter.
  */
  actual_path = real_path;
  if((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  for(i=0; i < real_path_len; ++i)
    if(actual_path[i] == '/')
      actual_path[i] = '\\';
    else if(!actual_path[i]) { /* binary zero */
      Curl_safefree(real_path);
      return CURLE_URL_MALFORMAT;
    }

  fd = ewefs_open(actual_path, O_RDONLY|O_BINARY);
  file->path = actual_path;
#else
  if(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    Curl_safefree(real_path);
    return CURLE_URL_MALFORMAT;
  }

  fd = ewefs_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
  if(!data->set.upload && (fd == -1)) {
    failf(data, "Couldn't open file %s", data->state.path);
    file_done(conn, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    return CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  return CURLE_OK;
}

static CURLcode file_done(struct connectdata *conn,
                               CURLcode status, bool premature)
{
  struct FILEPROTO *file = conn->data->req.protop;
  (void)status; /* not used */
  (void)premature; /* not used */

  if(file) {
    Curl_safefree(file->freepath);
    file->path = NULL;
    if(file->fd != -1)
      ewefs_close(file->fd);
    file->fd = -1;
  }

  return CURLE_OK;
}

static CURLcode file_disconnect(struct connectdata *conn,
                                bool dead_connection)
{
  struct FILEPROTO *file = conn->data->req.protop;
  (void)dead_connection; /* not used */

  if(file) {
    Curl_safefree(file->freepath);
    file->path = NULL;
    if(file->fd != -1)
      ewefs_close(file->fd);
    file->fd = -1;
  }

  return CURLE_OK;
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#else
#define DIRSEP '/'
#endif

static CURLcode file_upload(struct connectdata *conn)
{
  struct FILEPROTO *file = conn->data->req.protop;
  const char *dir = strchr(file->path, DIRSEP);
  __int64 fd;
  int mode;
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  char *buf = data->state.buffer;
  size_t nread;
  size_t nwrite;
  curl_off_t bytecount = 0;
  struct timeval now = Curl_tvnow();
  struct _stati64 file_stat;
  const char* buf2;

  /*
   * Since FILE: doesn't do the full init, we need to provide some extra
   * assignments here.
   */
  conn->data->req.upload_fromhere = buf;

  if(!dir)
    return CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  if(!dir[1])
    return CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

#ifdef O_BINARY
#define MODE_DEFAULT O_WRONLY|O_CREAT|O_BINARY
#else
#define MODE_DEFAULT O_WRONLY|O_CREAT
#endif

  if(data->state.resume_from)
    mode = MODE_DEFAULT|O_APPEND;
  else
    mode = MODE_DEFAULT|O_TRUNC;

  // fd = open(file->path, mode, conn->data->set.new_file_perms);
  fd = ewefs_open(file->path, mode );
  if(fd < 0) {
    failf(data, "Can't open %s for writing", file->path);
    return CURLE_WRITE_ERROR;
  }

  if(-1 != data->state.infilesize)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  if(data->state.resume_from < 0) {
    if(ewefs_fstat(fd, &file_stat)) {
      ewefs_close(fd);
      failf(data, "Can't get the size of %s", file->path);
      return CURLE_WRITE_ERROR;
    }
    else
      data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  while(!result) {
    int readcount;
    result = Curl_fillreadbuffer(conn, BUFSIZE, &readcount);
    if(result)
      break;

    if(readcount <= 0)  /* fix questionable compare error. curlvms */
      break;

    nread = (size_t)readcount;

    /*skip bytes before resume point*/
    if(data->state.resume_from) {
      if((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        buf2 = buf;
      }
      else {
        buf2 = buf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    else
      buf2 = buf;

    /* write the data to the target */
    nwrite = ewefs_write(fd, buf2, nread);
    if(nwrite != nread) {
      result = CURLE_SEND_ERROR;
      break;
    }

    bytecount += nread;

    Curl_pgrsSetUploadCounter(data, bytecount);

    if(Curl_pgrsUpdate(conn))
      result = CURLE_ABORTED_BY_CALLBACK;
    else
      result = Curl_speedcheck(data, now);
  }
  if(!result && Curl_pgrsUpdate(conn))
    result = CURLE_ABORTED_BY_CALLBACK;

  ewefs_close(fd);

  return result;
}

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support don't allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
static CURLcode file_do(struct connectdata *conn, bool *done)
{
  /* This implementation ignores the host name in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL
  */
  CURLcode result = CURLE_OK;
  struct _stati64 file_stat; /* struct_stat instead of struct stat just to allow the
                          Windows version to have a different struct without
                          having to redefine the simple word 'stat' */
  curl_off_t expected_size=0;
  bool size_known;
  bool fstated=FALSE;
  ssize_t nread;
  struct Curl_easy *data = conn->data;
  char *buf = data->state.buffer;
  curl_off_t bytecount = 0;
  __int64 fd;
  struct timeval now = Curl_tvnow();
  struct FILEPROTO *file;

  *done = TRUE; /* unconditionally */

  Curl_initinfo(data);
  Curl_pgrsStartNow(data);

  if(data->set.upload)
    return file_upload(conn);

  file = conn->data->req.protop;

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  if(-1 != ewefs_fstat(fd, &file_stat)) {
    /* we could stat it, then read out the size */
    expected_size = file_stat.st_size;
    /* and store the modification time */
    data->info.filetime = (long)file_stat.st_mtime;
    fstated = TRUE;
  }

  if(fstated && !data->state.range && data->set.timecondition) {
    if(!Curl_meets_timecondition(data, (time_t)data->info.filetime)) {
      *done = TRUE;
      return CURLE_OK;
    }
  }

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which for FILE can't be much more than the file size and
     date. */
  if(data->set.opt_no_body && data->set.include_header && fstated) {
    time_t filetime;
    struct tm buffer;
    const struct tm *tm = &buffer;
    snprintf(buf, sizeof(data->state.buffer),
             "Content-Length: %" CURL_FORMAT_CURL_OFF_T "\r\n", expected_size);
    result = Curl_client_write(conn, CLIENTWRITE_BOTH, buf, 0);
    if(result)
      return result;

    result = Curl_client_write(conn, CLIENTWRITE_BOTH,
                               (char *)"Accept-ranges: bytes\r\n", 0);
    if(result)
      return result;

    filetime = (time_t)file_stat.st_mtime;
    result = Curl_gmtime(filetime, &buffer);
    if(result)
      return result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    snprintf(buf, BUFSIZE-1,
             "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
             Curl_wkday[tm->tm_wday?tm->tm_wday-1:6],
             tm->tm_mday,
             Curl_month[tm->tm_mon],
             tm->tm_year + 1900,
             tm->tm_hour,
             tm->tm_min,
             tm->tm_sec);
    result = Curl_client_write(conn, CLIENTWRITE_BOTH, buf, 0);
    if(!result)
      /* set the file size to make it available post transfer */
      Curl_pgrsSetDownloadSize(data, expected_size);
    return result;
  }

  /* Check whether file range has been specified */
  file_range(conn);

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream iff the filesize could be determined */
  if(data->state.resume_from < 0) {
    if(!fstated) {
      failf(data, "Can't get the size of file.");
      return CURLE_READ_ERROR;
    }
    else
      data->state.resume_from += (curl_off_t)file_stat.st_size;
  }

  if(data->state.resume_from <= expected_size)
    expected_size -= data->state.resume_from;
  else {
    failf(data, "failed to resume file:// transfer");
    return CURLE_BAD_DOWNLOAD_RESUME;
  }

  /* A high water mark has been specified so we obey... */
  if(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  if(!fstated || (expected_size == 0))
    size_known = FALSE;
  else
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  if(fstated)
    Curl_pgrsSetDownloadSize(data, expected_size);

  if(data->state.resume_from) {
    if(data->state.resume_from !=
       ewefs_lseek(fd, data->state.resume_from, SEEK_SET))
      return CURLE_BAD_DOWNLOAD_RESUME;
  }

  Curl_pgrsTime(data, TIMER_STARTTRANSFER);

  while(!result) {
    /* Don't fill a whole buffer if we want less than all data */
    size_t bytestoread;

    if(size_known) {
      bytestoread =
        (expected_size < CURL_OFF_T_C(BUFSIZE) - CURL_OFF_T_C(1)) ?
        curlx_sotouz(expected_size) : BUFSIZE - 1;
    }
    else
      bytestoread = BUFSIZE-1;

    nread = ewefs_read(fd, buf, bytestoread);

    if(nread > 0)
      buf[nread] = 0;

    if(nread <= 0 || (size_known && (expected_size == 0)))
      break;

    bytecount += nread;
    if(size_known)
      expected_size -= nread;

    result = Curl_client_write(conn, CLIENTWRITE_BODY, buf, nread);
    if(result)
      return result;

    Curl_pgrsSetDownloadCounter(data, bytecount);

    if(Curl_pgrsUpdate(conn))
      result = CURLE_ABORTED_BY_CALLBACK;
    else
      result = Curl_speedcheck(data, now);
  }
  if(Curl_pgrsUpdate(conn))
    result = CURLE_ABORTED_BY_CALLBACK;

  return result;
}

#endif
