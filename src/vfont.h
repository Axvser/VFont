/**
 * @file vfont.h
 * @brief 矢量字体渲染接口
 *
 * 提供基于FreeType的字体渲染功能，支持动态调整字号，多字库加载，UTF-8编码和抗锯齿渲染
 */

#ifndef VFONT_H
#define VFONT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief 字体句柄
     *
     * 通过vfont_init创建，使用完毕后必须调用vfont_free释放
     */
    typedef struct VFontInternal *VFontHandle;

    /**
     * @brief 初始化字体系统
     * @param ttf_path 字体文件路径（支持.ttf/.ttc）
     * @param size 初始字号
     * @return 成功返回字体句柄，失败返回NULL
     *
     * @note 实际加载的字号可能受字体本身限制
     */
    VFontHandle vfont_init(const char *ttf_path, int size);

    /**
     * @brief 设置当前字号
     * @param handle 字体句柄
     * @param size 要设置的字号
     * @return 成功返回0，失败返回非0
     */
    int vfont_set_size(VFontHandle handle, int size);

    /**
     * @brief 渲染UTF-8文本到缓冲区
     * @param handle 字体句柄
     * @param text UTF-8编码的文本
     * @param buffer RGBA 像素缓冲区
     * @param buf_width 缓冲区宽度
     * @param buf_height 缓冲区高度
     * @param x 起始X坐标
     * @param y 基线Y坐标
     * @param color ARGB 颜色值
     * @param size 字号（0表示使用当前字号）
     *
     * @warning 缓冲区必须足够大
     */
    void vfont_render_text(
        VFontHandle handle,
        const char *text,
        uint32_t *buffer,
        int buf_width,
        int buf_height,
        int x,
        int y,
        uint32_t color,
        int size);

    /**
     * @brief 释放字体资源
     * @param handle 要释放的字体句柄
     *
     * @note 调用后handle将变为无效指针
     */
    void vfont_free(VFontHandle handle);
#ifdef __cplusplus
}
#endif

#endif