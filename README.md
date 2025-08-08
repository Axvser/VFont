# VFont - 基于freetype的矢量文字渲染库

📌 文档由Deepseek生成，出了问题不能怪我 ~

## 核心特性

* 🚀 **3步集成**：初始化→渲染→释放，支持同时使用多个字库
* 🌍 **原生UTF-8支持**：直接渲染"你好，世界!"，无需修改IDE的编码格式为GB2312
* 🖥️ **内存级控制**：可操作指定帧缓冲，利于实施双缓冲
* ✨ **高质量渲染**：内置抗锯齿和自动微调
* 📱 **低内存占用**：适合嵌入式设备

## 快速使用

```
// 1. 初始化指定的ttf文件为字库,推荐sim系列,24是文本默认大小
VFontHandle font = vfont_init("path/to/*.ttf", 24);
if(!font) exit(1);

// 2. 渲染到任意内存缓冲区
vfont_render_text(
    font,          // 字体
    "滑动文本效果", // 文本
    framebuffer,   // 缓冲区
    width, height, // 缓冲区大小
    x, y,          // 起始位置
    0x00FF0000     // ARGB红色
    10             // 可传入非0值以使用非默认的文字大小
);

// 3. 释放资源（必须调用）
vfont_free(font);
```

## 技巧

1. **多字体混排**：

```
VFontHandle font_small = vfont_init("font.ttf", 12);
VFontHandle font_large = vfont_init("font.ttf", 24);
// 交替使用不同字号...
```

2. **动态效果**：

```
// 实现文本滑动
for(int x_pos = -100; x_pos < 800; x_pos+=5) {
    vfont_render_text(font, "滑动文本", buffer, 800,480, x_pos,100, 0xFF0000FF);
    swap_buffers();
}
```
