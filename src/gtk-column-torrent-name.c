/*
 * gtk-column-torrent-name.c
 *
 * Copyright (C) 2016 Alessandro Grassi <alessandro.g89@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include "bencode.h"

FM_DEFINE_MODULE(gtk_folder_col, torrent_name)

GType get_type()
{
    return G_TYPE_STRING;
}

char* get_torrent_name(FmFileInfo *fi)
{
    char* torrent_name = NULL;
    char* file_name;
    FILE* file_handle;
    int file_size;
    const int max_file_size = 102400;
    char *file_content;
    struct bencode ctx[1];
    int result;
    int name_found = 0;
    const char key_name[] = "name";
    const int key_length = 4;

    file_name = fm_path_to_str(fm_file_info_get_path(fi));
    file_handle = fopen(file_name, "rb");
    if (file_handle == NULL)
    {
        return NULL;
    }
    fseek(file_handle, 0L, SEEK_END);
    file_size = ftell(file_handle);
    if (file_size > max_file_size)
    {
        file_size = max_file_size;
    }
    file_content = malloc(file_size);
    if (file_content == NULL)
    {
        return NULL;
    }
    fseek(file_handle, 0L, SEEK_SET);
    fread(file_content, 1, file_size, file_handle);
    fclose(file_handle);

    bencode_init(ctx, file_content, file_size);
    do
    {
        result = bencode_next(ctx);
        if (result == BENCODE_STRING)
        {
            if (ctx->toklen == key_length &&
                    strncmp(ctx->tok, key_name, key_length) == 0)
            {
                name_found = 1;
            }
        }
    } while (result != BENCODE_DONE && result >= 0 && name_found == 0);

    if (name_found == 1)
    {
        result = bencode_next(ctx);
        if (result == BENCODE_STRING)
        {
            torrent_name = malloc(ctx->toklen + 1);
            if (torrent_name != NULL)
            {
                snprintf(torrent_name, ctx->toklen + 1, "%s", ctx->tok);
            }
        }
    }

    free(file_content);
    bencode_free(ctx);
    return torrent_name;
}

void get_value(FmFileInfo *fi, GValue *value)
{
    char* torrent_name = get_torrent_name(fi);
    if (torrent_name == NULL)
    {
        return;
    }
    else
    {
        g_value_set_string(value, torrent_name);
        free(torrent_name);
    }
};

gint sort(FmFileInfo *fi1, FmFileInfo *fi2)
{    
    char* torrent_name_1 = get_torrent_name(fi1);
    char* torrent_name_2 = get_torrent_name(fi2);

    if (torrent_name_1 == NULL && torrent_name_2 == NULL)
    {
        free(torrent_name_1);
        free(torrent_name_2);
        return 0;
    }
    else if (torrent_name_1 != NULL && torrent_name_2 == NULL)
    {
        free(torrent_name_2);
        return 1;
    }
    else if (torrent_name_1 == NULL && torrent_name_2 != NULL)
    {
        free(torrent_name_1);
        return -1;
    }
    else
    {
        return strcmp(torrent_name_1, torrent_name_2);
    }
}

FmFolderModelColumnInit fm_module_init_gtk_folder_col = {
    "Torrent name",
    0, 
    &get_type,
    &get_value,
    &sort
};
