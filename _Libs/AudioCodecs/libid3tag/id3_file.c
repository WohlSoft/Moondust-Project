/*
 * libid3tag - ID3 tag manipulation library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: file.c,v 1.21 2004/01/23 09:41:32 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int close(int);
# endif

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# include "id3tag.h"
# include "file.h"
# include "tag.h"
# include "field.h"

#if !defined(__MINGW32__) && !defined(__MINGW64__) && !defined(_WIN32)
extern char *strdup(const char *);
extern FILE *fdopen(int, const char *);
#endif

struct filetag
{
    struct id3_tag *tag;
    unsigned long location;
    id3_length_t length;
};

struct id3_file
{
    FILE *iofile;
    SDL_RWops *iorwops;
    enum id3_file_mode mode;
    char *path;

    int flags;

    struct id3_tag *primary;

    unsigned int ntags;
    struct filetag *tags;
};

/* Add support for SDL_RWops*/
size_t id3f_read(void *data, size_t count, size_t size, struct id3_file *ctx)
{
    if(ctx->iorwops)
        return SDL_RWread(ctx->iorwops, data, count, size);
    else
        return fread(data, size, count, ctx->iofile);
}

size_t id3f_write(const void *data, size_t count, size_t size, struct id3_file *ctx)
{
    if(ctx->iorwops)
        return SDL_RWwrite(ctx->iorwops, data, count, size);
    else
        return fwrite(data, size, count, ctx->iofile);
}

Sint64 id3f_tell(struct id3_file *ctx)
{
    if(ctx->iorwops)
        return SDL_RWtell(ctx->iorwops);
    else
        return ftell(ctx->iofile);
}

Sint64 id3f_seek(struct id3_file *ctx, Sint64 offset, int whence)
{
    if(ctx->iorwops)
        return SDL_RWseek(ctx->iorwops, offset, whence) != -1 ? 0 : -1;
    else
        return fseek(ctx->iofile, offset, whence);
}

void id3f_rewind(struct id3_file *ctx)
{
    if(ctx->iorwops)
        SDL_RWseek(ctx->iorwops, 0, RW_SEEK_SET);
    else
        rewind(ctx->iofile);
}

typedef struct id3f_pos_t_ttt
{
    Sint64 rwoffset;
    fpos_t real_fpos;
} id3f_pos_t;

int id3f_getpos(struct id3_file *ctx, id3f_pos_t *pos)
{
    if(!ctx || !pos)
        return -1;
    if(ctx->iorwops)
    {
        pos->rwoffset = (Sint64)id3f_tell(ctx);
        return pos->rwoffset != -1 ? 0 : -1;
    }
    return fgetpos(ctx->iofile, &pos->real_fpos);
}

int id3f_setpos(struct id3_file *ctx, const id3f_pos_t *pos)
{
    if(!ctx || !pos)
        return -1;
    if(ctx->iorwops)
        return id3f_seek(ctx, pos->rwoffset, SEEK_SET) == 0 ? 0 : -1;
    return fsetpos(ctx->iofile, &pos->real_fpos);
}

int id3f_flush(struct id3_file *ctx)
{
    if(ctx->iorwops)
        return 0;
    else
        return fflush(ctx->iofile);
}

void id3f_clearerr(struct id3_file *ctx)
{
    if(!ctx->iorwops)
        clearerr(ctx->iofile);
}


enum
{
    ID3_FILE_FLAG_ID3V1 = 0x0001
};

/*
 * NAME:    query_tag()
 * DESCRIPTION: check for a tag at a file's current position
 */
static
signed long query_tag(struct id3_file *iofile)
{
    id3f_pos_t save_position;
    id3_byte_t query[ID3_TAG_QUERYSIZE];
    signed long size;

    if(id3f_getpos(iofile, &save_position) == -1)
        return 0;

    size = id3_tag_query(query, id3f_read(query, 1, sizeof(query), iofile));

    if(id3f_setpos(iofile, &save_position) == -1)
        return 0;

    return size;
}

/*
 * NAME:    read_tag()
 * DESCRIPTION: read and parse a tag at a file's current position
 */
static
struct id3_tag *read_tag(struct id3_file *iofile, id3_length_t size)
{
    id3_byte_t *data;
    struct id3_tag *tag = 0;

    data = (id3_byte_t *)malloc(size);
    if(data)
    {
        if(id3f_read(data, size, 1, iofile) == 1)
            tag = id3_tag_parse(data, size);

        free(data);
    }

    return tag;
}

/*
 * NAME:    update_primary()
 * DESCRIPTION: update the primary tag with data from a new tag
 */
static
int update_primary(struct id3_tag *tag, struct id3_tag const *newf)
{
    unsigned int i;
    struct id3_frame *frame;

    if(newf)
    {
        if(!(newf->extendedflags & ID3_TAG_EXTENDEDFLAG_TAGISANUPDATE))
            id3_tag_clearframes(tag);

        i = 0;
        while((frame = id3_tag_findframe(newf, 0, i++)))
        {
            if(id3_tag_attachframe(tag, frame) == -1)
                return -1;
        }
    }

    return 0;
}

/*
 * NAME:    tag_compare()
 * DESCRIPTION: tag sort function for qsort()
 */
static
int tag_compare(const void *a, const void *b)
{
    struct filetag const *tag1 = (struct filetag const *)a,
                          *tag2 = (struct filetag const *)b;

    if(tag1->location < tag2->location)
        return -1;
    else if(tag1->location > tag2->location)
        return +1;

    return 0;
}

/*
 * NAME:    add_filetag()
 * DESCRIPTION: add a new file tag entry
 */
static
int add_filetag(struct id3_file *file, struct filetag const *filetag)
{
    struct filetag *tags;

    tags = (struct filetag *)realloc(file->tags, (file->ntags + 1) * sizeof(*tags));
    if(tags == 0)
        return -1;

    file->tags = tags;
    file->tags[file->ntags++] = *filetag;

    /* sort tags by location */

    if(file->ntags > 1)
        qsort(file->tags, file->ntags, sizeof(file->tags[0]), tag_compare);

    return 0;
}

/*
 * NAME:    del_filetag()
 * DESCRIPTION: delete a file tag entry
 */
#if defined(HAVE_FTRUNCATE)
static
void del_filetag(struct id3_file *file, unsigned int index)
{
    assert(index < file->ntags);

    while(index < file->ntags - 1)
    {
        file->tags[index] = file->tags[index + 1];
        ++index;
    }

    --file->ntags;
}
#endif
/*
 * NAME:    add_tag()
 * DESCRIPTION: read, parse, and add a tag to a file structure
 */
static
struct id3_tag *add_tag(struct id3_file *file, id3_length_t length)
{
    long location;
    unsigned int i;
    struct filetag filetag;
    struct id3_tag *tag;

    location = id3f_tell(file);

    if(location == -1)
        return 0;

    /* check for duplication/overlap */
    {
        unsigned long begin1, end1, begin2, end2;

        begin1 = (unsigned long)location;
        end1   = begin1 + length;

        for(i = 0; i < file->ntags; ++i)
        {
            begin2 = file->tags[i].location;
            end2   = begin2 + file->tags[i].length;

            if(begin1 == begin2 && end1 == end2)
                return file->tags[i].tag;  /* duplicate */

            if(begin1 < end2 && end1 > begin2)
                return 0;  /* overlap */
        }
    }

    tag = read_tag(file, length);

    filetag.tag      = tag;
    filetag.location = (unsigned long)location;
    filetag.length   = length;

    if(add_filetag(file, &filetag) == -1 ||
       update_primary(file->primary, tag) == -1)
    {
        if(tag)
            id3_tag_delete(tag);
        return 0;
    }

    if(tag)
        id3_tag_addref(tag);

    return tag;
}

/*
 * NAME:    search_tags()
 * DESCRIPTION: search for tags in a file
 */
static
int search_tags(struct id3_file *file)
{
    id3f_pos_t save_position;
    signed long size;

    /*
     * save the current seek position
     *
     * We also verify the stream is seekable by calling fsetpos(), since
     * fgetpos() alone is not reliable enough for this purpose.
     *
     * [Apparently not even fsetpos() is sufficient under Win32.]
     */

    if(id3f_getpos(file, &save_position) == -1 ||
       id3f_setpos(file, &save_position) == -1)
        return -1;

    /* look for an ID3v1 tag */

    if(id3f_seek(file, -128, SEEK_END) == 0)
    {
        size = query_tag(file);
        if(size > 0)
        {
            struct id3_tag const *tag;

            tag = add_tag(file, (id3_length_t)size);

            /* if this is indeed an ID3v1 tag, mark the file so */

            if(tag && (ID3_TAG_VERSION_MAJOR(id3_tag_version(tag)) == 1))
                file->flags |= ID3_FILE_FLAG_ID3V1;
        }
    }

    /* look for a tag at the beginning of the file */

    id3f_rewind(file);

    size = query_tag(file);
    if(size > 0)
    {
        struct id3_tag const *tag;
        struct id3_frame const *frame;

        tag = add_tag(file, (id3_length_t)size);

        /* locate tags indicated by SEEK frames */

        while(tag && (frame = id3_tag_findframe(tag, "SEEK", 0)))
        {
            long seek;

            seek = id3_field_getint(id3_frame_field(frame, 0));
            if(seek < 0 || id3f_seek(file, seek, SEEK_CUR) == -1)
                break;

            size = query_tag(file);
            tag  = (size > 0) ? add_tag(file, (id3_length_t)size) : 0;
        }
    }

    /* look for a tag at the end of the file (before any ID3v1 tag) */

    if(id3f_seek(file, ((file->flags & ID3_FILE_FLAG_ID3V1) ? -128 : 0) +
             -10, SEEK_END) == 0)
    {
        size = query_tag(file);
        if(size < 0 && id3f_seek(file, size, SEEK_CUR) == 0)
        {
            size = query_tag(file);
            if(size > 0)
                add_tag(file, (id3_length_t)size);
        }
    }

    id3f_clearerr(file);

    /* restore seek position */

    if(id3f_setpos(file, &save_position) == -1)
        return -1;

    /* set primary tag options and target padded length for convenience */

    if((file->ntags > 0 && !(file->flags & ID3_FILE_FLAG_ID3V1)) ||
       (file->ntags > 1 && (file->flags & ID3_FILE_FLAG_ID3V1)))
    {
        if(file->tags[0].location == 0)
            id3_tag_setlength(file->primary, file->tags[0].length);
        else
            id3_tag_options(file->primary, ID3_TAG_OPTION_APPENDEDTAG, ~0);
    }

    return 0;
}

/*
 * NAME:    finish_file()
 * DESCRIPTION: release memory associated with a file
 */
static
void finish_file(struct id3_file *file)
{
    unsigned int i;

    if(file->path)
        free(file->path);

    if(file->primary)
    {
        id3_tag_delref(file->primary);
        id3_tag_delete(file->primary);
    }

    for(i = 0; i < file->ntags; ++i)
    {
        struct id3_tag *tag;

        tag = file->tags[i].tag;
        if(tag)
        {
            id3_tag_delref(tag);
            id3_tag_delete(tag);
        }
    }

    if(file->tags)
        free(file->tags);

    free(file);
}

static
struct id3_file *new_file_private(SDL_RWops *rwops_src,
                                  FILE *iofile,
                                  enum id3_file_mode mode,
                                  char const *path)
{
    struct id3_file *file;
    char *nullP = 0;

    file = (struct id3_file *)malloc(sizeof(*file));
    if(file == 0)
        goto fail;

    file->iofile  = iofile;
    file->iorwops = rwops_src;
    file->mode    = mode;
    file->path    = path ? (char*)strdup(path) : nullP;

    file->flags   = 0;

    file->ntags   = 0;
    file->tags    = 0;

    file->primary = id3_tag_new();
    if(file->primary == 0)
        goto fail;

    id3_tag_addref(file->primary);

    /* load tags from the file */

    if(search_tags(file) == -1)
        goto fail;

    id3_tag_options(file->primary, ID3_TAG_OPTION_ID3V1,
                    (file->flags & ID3_FILE_FLAG_ID3V1) ? ~0 : 0);

    if(0)
    {
fail:
        if(file)
        {
            finish_file(file);
            file = 0;
        }
    }

    return file;
}

static
struct id3_file *new_file_rwops(SDL_RWops *src, enum id3_file_mode mode)
{
    return new_file_private(src, NULL, mode, NULL);
}

/*
 * NAME:    new_file()
 * DESCRIPTION: create a new file structure and load tags
 */
static
struct id3_file *new_file(FILE *iofile, enum id3_file_mode mode,
                          char const *path)
{
    return new_file_private(NULL, iofile, mode, path);
}

struct id3_file *id3_file_from_rwops(SDL_RWops *src, enum id3_file_mode mode)
{
    struct id3_file *file;
    if(src == 0)
        return 0;

    SDL_RWseek(src, 0, SEEK_SET);

    file = new_file_rwops(src, mode);
    if(file == 0)
        return 0;

    return file;
}

/*
 * NAME:    file->open()
 * DESCRIPTION: open a file given its pathname
 */
struct id3_file *id3_file_open(char const *path, enum id3_file_mode mode)
{
    FILE *iofile;
    struct id3_file *file;

    assert(path);

    iofile = fopen(path, (mode == ID3_FILE_MODE_READWRITE) ? "r+b" : "rb");
    if(iofile == 0)
        return 0;

    file = new_file(iofile, mode, path);
    if(file == 0)
        fclose(iofile);

    return file;
}

/*
 * NAME:    file->fdopen()
 * DESCRIPTION: open a file using an existing file descriptor
 */
struct id3_file *id3_file_fdopen(int fd, enum id3_file_mode mode)
{
    # if 1 || defined(HAVE_UNISTD_H)
    FILE *iofile;
    struct id3_file *file;

    iofile = (FILE *)fdopen(fd, (mode == ID3_FILE_MODE_READWRITE) ? "r+b" : "rb");
    if(iofile == 0)
        return 0;

    file = new_file(iofile, mode, 0);
    if(file == 0)
    {
        int save_fd;

        /* close iofile without closing fd */

        save_fd = dup(fd);

        fclose(iofile);

        dup2(save_fd, fd);
        close(save_fd);
    }

    return file;
    # else
    return 0;
    # endif
}

/*
 * NAME:    file->close()
 * DESCRIPTION: close a file and delete its associated tags
 */
int id3_file_close(struct id3_file *file)
{
    int result = 0;

    assert(file);

    if(file->iorwops)
    {
        //Don't close RWops, just remove id3-tag descriptor
        id3f_rewind(file);
        result = 0;
    }
    else if(fclose(file->iofile) == EOF)
        result = -1;

    finish_file(file);

    return result;
}

/*
 * NAME:    file->tag()
 * DESCRIPTION: return the primary tag structure for a file
 */
struct id3_tag *id3_file_tag(struct id3_file const *file)
{
    assert(file);

    return file->primary;
}

/*
 * NAME:    v1_write()
 * DESCRIPTION: write ID3v1 tag modifications to a file
 */
static
int v1_write(struct id3_file *file,
             id3_byte_t const *data, id3_length_t length)
{
    (void)length;
    assert(!data || length == 128);

    if(data)
    {
        long location;

        if(id3f_seek(file, (file->flags & ID3_FILE_FLAG_ID3V1) ? -128 : 0,
                 SEEK_END) == -1 ||
           (location = id3f_tell(file)) == -1 ||
           id3f_write(data, 128, 1, file) != 1 ||
           id3f_flush(file) == EOF)
            return -1;

        /* add file tag reference */

        if(!(file->flags & ID3_FILE_FLAG_ID3V1))
        {
            struct filetag filetag;

            filetag.tag      = 0;
            filetag.location = (unsigned long)location;
            filetag.length   = 128;

            if(add_filetag(file, &filetag) == -1)
                return -1;

            file->flags |= ID3_FILE_FLAG_ID3V1;
        }
    }
    # if defined(HAVE_FTRUNCATE)
    else if(file->flags & ID3_FILE_FLAG_ID3V1)
    {
        long length;

        if(fseek(file->iofile, 0, SEEK_END) == -1)
            return -1;

        length = ftell(file->iofile);
        if(length == -1 ||
           (length >= 0 && length < 128))
            return -1;

        if(ftruncate(fileno(file->iofile), length - 128) == -1)
            return -1;

        /* delete file tag reference */

        del_filetag(file, file->ntags - 1);

        file->flags &= ~ID3_FILE_FLAG_ID3V1;
    }
    # endif

    return 0;
}

/*
 * NAME:    v2_write()
 * DESCRIPTION: write ID3v2 tag modifications to a file
 */
static
int v2_write(struct id3_file *file,
             id3_byte_t const *data, id3_length_t length)
{
    assert(!data || length > 0);

    if(data &&
       ((file->ntags == 1 && !(file->flags & ID3_FILE_FLAG_ID3V1)) ||
        (file->ntags == 2 && (file->flags & ID3_FILE_FLAG_ID3V1))) &&
       file->tags[0].length == length)
    {
        /* easy special case: rewrite existing tag in-place */

        if(id3f_seek(file, (Sint64)file->tags[0].location, SEEK_SET) == -1 ||
           id3f_write(data, length, 1, file) != 1 ||
           id3f_flush(file) == EOF)
            return -1;

        goto done;
    }

    /* hard general case: rewrite entire file */

    /* ... */

done:
    return 0;
}

/*
 * NAME:    file->update()
 * DESCRIPTION: rewrite tag(s) to a file
 */
int id3_file_update(struct id3_file *file)
{
    int options, result = 0;
    id3_length_t v1size = 0, v2size = 0;
    id3_byte_t id3v1_data[128], *id3v1 = 0, *id3v2 = 0;

    assert(file);

    if(file->mode != ID3_FILE_MODE_READWRITE)
        return -1;

    options = id3_tag_options(file->primary, 0, 0);

    /* render ID3v1 */

    if(options & ID3_TAG_OPTION_ID3V1)
    {
        v1size = id3_tag_render(file->primary, 0);
        if(v1size)
        {
            assert(v1size == sizeof(id3v1_data));

            v1size = id3_tag_render(file->primary, id3v1_data);
            if(v1size)
            {
                assert(v1size == sizeof(id3v1_data));
                id3v1 = id3v1_data;
            }
        }
    }

    /* render ID3v2 */

    id3_tag_options(file->primary, ID3_TAG_OPTION_ID3V1, 0);

    v2size = id3_tag_render(file->primary, 0);
    if(v2size)
    {
        id3v2 = (id3_byte_t *)malloc(v2size);
        if(id3v2 == 0)
            goto fail;

        v2size = id3_tag_render(file->primary, id3v2);
        if(v2size == 0)
        {
            free(id3v2);
            id3v2 = 0;
        }
    }

    /* write tags */

    if(v2_write(file, id3v2, v2size) == -1 ||
       v1_write(file, id3v1, v1size) == -1)
        goto fail;

    id3f_rewind(file);

    /* update file tags array? ... */

    if(0)
    {
fail:
        result = -1;
    }

    /* clean up; restore tag options */

    if(id3v2)
        free(id3v2);

    id3_tag_options(file->primary, ~0, options);

    return result;
}
