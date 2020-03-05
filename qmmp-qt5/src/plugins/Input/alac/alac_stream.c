#include "alac_stream.h"
#include <errno.h>

#define _Swap32(v) do { \
                   v = (((v) & 0x000000FF) << 0x18) | \
                       (((v) & 0x0000FF00) << 0x08) | \
                       (((v) & 0x00FF0000) >> 0x08) | \
                       (((v) & 0xFF000000) >> 0x18); } while(0)

#define _Swap16(v) do { \
                   v = (((v) & 0x00FF) << 0x08) | \
                       (((v) & 0xFF00) >> 0x08); } while (0)

extern int host_bigendian;

struct stream_tTAG {
    FILE *f;
    int bigendian;
    int eof;
};

void stream_read(stream_t *stream, size_t size, void *buf)
{
    size_t ret;

    ret = fread(buf, 4, size >> 2, stream->f) * 4;
    ret += fread((char*)buf + ret, 1, size - ret, stream->f);

    if (ret == 0 && size != 0) stream->eof = 1;
}

int32_t stream_read_int32(stream_t *stream)
{
    int32_t v;
    stream_read(stream, 4, &v);
    if ((stream->bigendian && !host_bigendian) || (!stream->bigendian && host_bigendian))
    {
        _Swap32(v);
    }
    return v;
}

uint32_t stream_read_uint32(stream_t *stream)
{
    uint32_t v;
    stream_read(stream, 4, &v);
    if ((stream->bigendian && !host_bigendian) || (!stream->bigendian && host_bigendian))
    {
        _Swap32(v);
    }
    return v;
}

int16_t stream_read_int16(stream_t *stream)
{
    int16_t v;
    stream_read(stream, 2, &v);
    if ((stream->bigendian && !host_bigendian) || (!stream->bigendian && host_bigendian))
    {
        _Swap16(v);
    }
    return v;
}

uint16_t stream_read_uint16(stream_t *stream)
{
    uint16_t v;
    stream_read(stream, 2, &v);
    if ((stream->bigendian && !host_bigendian) ||
            (!stream->bigendian && host_bigendian))
    {
        _Swap16(v);
    }
    return v;
}

int8_t stream_read_int8(stream_t *stream)
{
    int8_t v;
    stream_read(stream, 1, &v);
    return v;
}

uint8_t stream_read_uint8(stream_t *stream)
{
    uint8_t v;
    stream_read(stream, 1, &v);
    return v;
}


void stream_skip(stream_t *stream, size_t skip)
{
    if (fseek(stream->f, (long)skip, SEEK_CUR) == 0)
        return;

    if (errno == ESPIPE)
    {
        char *buffer = malloc(skip);
        stream_read(stream, skip, buffer);
        free(buffer);
    }
}

int stream_eof(stream_t *stream)
{
    return stream->eof;
}

long stream_tell(stream_t *stream)
{
    return ftell(stream->f); /* returns -1 on error */
}

int stream_setpos(stream_t *stream, long pos)
{
    return fseek(stream->f, pos, SEEK_SET);
}

stream_t *stream_create_file(FILE *file, int bigendian)
{
    stream_t *new_stream;

    new_stream = (stream_t*)malloc(sizeof(stream_t));
    new_stream->f = file;
    new_stream->bigendian = bigendian;
    new_stream->eof = 0;

    return new_stream;
}

void stream_destroy(stream_t *stream)
{
    free(stream);
}

