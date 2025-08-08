/**
 * @file vfont.c
 * @brief 矢量字体渲染实现
 */

#include "vfont.h"
#include "freetype.h"
#include <stdlib.h>
#include <string.h>

// 内部状态结构体
typedef struct
{
    void *library;    ///< FreeType库实例
    void *face;       ///< 字体face对象
    int current_size; ///< 当前缓存字号
} VFontInternal;

VFontHandle vfont_init(const char *ttf_path, int size)
{
    if (!ttf_path || size <= 0)
        return NULL;

    VFontInternal *font = (VFontInternal *)malloc(sizeof(VFontInternal));
    if (!font)
        return NULL;

    memset(font, 0, sizeof(VFontInternal));

    // 初始化FreeType
    if (FT_Init_FreeType((FT_Library *)&font->library))
    {
        free(font);
        return NULL;
    }

    // 加载字体文件
    if (FT_New_Face((FT_Library)font->library, ttf_path, 0, (FT_Face *)&font->face))
    {
        FT_Done_FreeType((FT_Library)font->library);
        free(font);
        return NULL;
    }

    // 配置字体参数
    FT_Select_Charmap((FT_Face)font->face, FT_ENCODING_UNICODE);
    FT_Set_Pixel_Sizes((FT_Face)font->face, 0, size);
    font->current_size = size;

    return (VFontHandle)font;
}

int vfont_set_size(VFontHandle handle, int size)
{
    if (!handle || size <= 0)
        return -1;

    VFontInternal *font = (VFontInternal *)handle;

    if (font->current_size != size)
    {
        if (FT_Set_Pixel_Sizes((FT_Face)font->face, 0, size))
            return -1;
        font->current_size = size;
    }

    return 0;
}

void vfont_render_text(
    VFontHandle handle,
    const char *text,
    uint32_t *buffer,
    int buf_width,
    int buf_height,
    int x,
    int y,
    uint32_t color,
    int size)
{
    if (!handle || !text || !buffer ||
        buf_width <= 0 || buf_height <= 0)
        return;

    VFontInternal *font = (VFontInternal *)handle;

    // 如果指定了字号且与当前不同，则更新字号
    if (size > 0 && font->current_size != size)
    {
        FT_Set_Pixel_Sizes((FT_Face)font->face, 0, size);
        font->current_size = size;
    }
    else if (size <= 0)
    {
        size = font->current_size;
    }

    FT_Face face = (FT_Face)font->face;
    FT_GlyphSlot glyph = face->glyph;

    // 计算基线位置（FreeType坐标系）
    int pen_x = x;
    int pen_y = y + size; // 转换为基线坐标

    const unsigned char *p = (const unsigned char *)text;
    while (*p)
    {
        // UTF-8解码（保持原样）
        FT_ULong codepoint = 0;
        if ((*p & 0x80) == 0)
        {
            codepoint = *p++;
        }
        else if ((*p & 0xE0) == 0xC0)
        {
            codepoint = ((p[0] & 0x1F) << 6) | (p[1] & 0x3F);
            p += 2;
        }
        else if ((*p & 0xF0) == 0xE0)
        {
            codepoint = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
            p += 3;
        }
        else if ((*p & 0xF8) == 0xF0)
        {
            codepoint = ((p[0] & 0x07) << 18) | ((p[1] & 0x3F) << 12) |
                        ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
            p += 4;
        }
        else
        {
            p++;
            continue;
        }

        // 加载并渲染字形
        if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT))
            continue;

        FT_Bitmap *bitmap = &glyph->bitmap;
        int x_start = pen_x + glyph->bitmap_left;
        int y_start = pen_y - glyph->bitmap_top;

        // 像素混合（保持原样）
        for (int row = 0; row < bitmap->rows; row++)
        {
            for (int col = 0; col < bitmap->width; col++)
            {
                int px = x_start + col;
                int py = y_start + row;

                if (px < 0 || px >= buf_width || py < 0 || py >= buf_height)
                    continue;

                uint8_t alpha = 0;
                if (bitmap->pixel_mode == FT_PIXEL_MODE_MONO)
                {
                    uint8_t byte = bitmap->buffer[row * bitmap->pitch + (col / 8)];
                    alpha = (byte >> (7 - (col % 8))) & 1 ? 255 : 0;
                }
                else
                {
                    alpha = bitmap->buffer[row * bitmap->pitch + col];
                }

                if (alpha > 0)
                {
                    size_t index = (size_t)py * buf_width + px;
                    uint32_t bg = buffer[index];

                    uint8_t r = ((color >> 16) & 0xFF) * alpha / 255 +
                                ((bg >> 16) & 0xFF) * (255 - alpha) / 255;
                    uint8_t g = ((color >> 8) & 0xFF) * alpha / 255 +
                                ((bg >> 8) & 0xFF) * (255 - alpha) / 255;
                    uint8_t b = (color & 0xFF) * alpha / 255 +
                                (bg & 0xFF) * (255 - alpha) / 255;

                    buffer[index] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }

        pen_x += (glyph->advance.x >> 6);
    }
}

void vfont_free(VFontHandle handle)
{
    if (!handle)
        return;

    VFontInternal *font = (VFontInternal *)handle;

    if (font->face)
        FT_Done_Face((FT_Face)font->face);
    if (font->library)
        FT_Done_FreeType((FT_Library)font->library);

    free(font);
}