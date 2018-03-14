//#include "safe_iop.h"
//#include "zlib.h"

//#include <errno.h>
//#include <fcntl.h>
//#include <limits.h>
//#include <stdint.h>     // for uintptr_t
#include <stdlib.h>
//#include <sys/stat.h>   // for S_ISLNK()
//#include <unistd.h>

#define LOG_TAG "minzip_mrom"
#ifdef USE_MINZIP
#include "../minzip/Zip.h"
//#include "../minzip/Bits.h"
#include "../minzip/Log.h"
#endif
#include "multirom_Zip.h"
#include "../common.h"
//#include "../minzip/DirUtil.h"

#undef NDEBUG   // do this after including Log.h
//#include <assert.h>

#ifndef USE_MINZIP
typedef struct {
    char *buf;
    int bufLen;
} CopyProcessArgs;

static bool copyProcessFunction(const uint8_t *buf, size_t buf_size,
        void *cookie)
{
    CopyProcessArgs *args = (CopyProcessArgs *)cookie;
    if (buf_size <= args->bufLen) {
        memcpy(args->buf, buf, buf_size);
        args->buf += buf_size;
        args->bufLen -= buf_size;
        return true;
    }
    return false;
}

bool readZipEntry(ZipArchiveHandle archive, ZipEntry* pEntry,
        char *buf, int bufLen)
{
    CopyProcessArgs args;
    bool ret;

    args.buf = buf;
    args.bufLen = bufLen;
    ret = ProcessZipEntryContents(archive, pEntry, copyProcessFunction,
            (void *)&args);
    if (!ret) {
        LOGE("Can't extract entry to buffer.\n");
        return false;
    }
    return true;
}
#endif

#ifdef USE_MINZIP
int read_data(ZipArchive *zip, const ZipEntry *entry,char** ppData, int* pLength)
#else
int read_data(ZipArchiveHandle *zip, const ZipEntry *entry,char** ppData, int* pLength)
#endif
{
#ifdef USE_MINZIP
    int len = (int)mzGetZipEntryUncompLen(entry);
#else
    int len = entry->uncompressed_length;
#endif
    bool ok;
    if (len <= 0) {
        LOGE("Bad data length %d\n", len);
        return -1;
    }
    char *data = (char*)malloc(len + 1);
    if (data == NULL) {
        LOGE("Can't allocate %d bytes for data\n", len + 1);
        return -2;
    }
#ifdef USE_MINZIP
    ok = mzReadZipEntry(zip, entry, data, len);
#else
    ok = readZipEntry(*zip, (ZipEntry*)entry, data, len);
#endif
    if (!ok) {
        LOGE("Error while reading data\n");
        free(data);
        return -3;
    }
    data[len] = '\0';     // not necessary, but just to be safe
    *ppData = data;
    if (pLength) {
        *pLength = len;
    }
    return 0;
}
