
#ifndef FILESTRM_H
#define FILESTRM_H

#include "unicode/utypes.h"

typedef struct _FileStream FileStream;

U_CAPI FileStream* U_EXPORT2
T_FileStream_open(const char* filename, const char* mode);

U_CAPI void U_EXPORT2
T_FileStream_close(FileStream* fileStream);

U_CAPI UBool U_EXPORT2
T_FileStream_file_exists(const char* filename);


U_CAPI int32_t U_EXPORT2
T_FileStream_read(FileStream* fileStream, void* addr, int32_t len);

U_CAPI int32_t U_EXPORT2
T_FileStream_write(FileStream* fileStream, const void* addr, int32_t len);

U_CAPI void U_EXPORT2
T_FileStream_rewind(FileStream* fileStream);

/*Added by Bertrand A. D. */
U_CAPI char * U_EXPORT2
T_FileStream_readLine(FileStream* fileStream, char* buffer, int32_t length);

U_CAPI int32_t U_EXPORT2
T_FileStream_writeLine(FileStream* fileStream, const char* buffer);

U_CAPI int32_t U_EXPORT2
T_FileStream_putc(FileStream* fileStream, int32_t ch);

U_CAPI int U_EXPORT2
T_FileStream_getc(FileStream* fileStream);

U_CAPI int32_t U_EXPORT2
T_FileStream_ungetc(int32_t ch, FileStream *fileStream);

U_CAPI int32_t U_EXPORT2
T_FileStream_peek(FileStream* fileStream);

U_CAPI int32_t U_EXPORT2
T_FileStream_size(FileStream* fileStream);

U_CAPI int U_EXPORT2
T_FileStream_eof(FileStream* fileStream);

U_CAPI int U_EXPORT2
T_FileStream_error(FileStream* fileStream);


U_CAPI FileStream* U_EXPORT2
T_FileStream_stdin(void);

U_CAPI FileStream* U_EXPORT2
T_FileStream_stdout(void);

U_CAPI FileStream* U_EXPORT2
T_FileStream_stderr(void);

U_CAPI UBool U_EXPORT2
T_FileStream_remove(const char* fileName);

#endif /* _FILESTRM*/
