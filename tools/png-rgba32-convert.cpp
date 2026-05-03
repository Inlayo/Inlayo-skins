#include <iostream>
#include <filesystem>
#include <vector>
#include <png.h>

namespace fs = std::filesystem;

bool convert(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (!png || !info) { fclose(fp); return false; }
    if (setjmp(png_jmpbuf(png))) { fclose(fp); return false; }

    png_init_io(png, fp);
    png_read_info(png, info);

    int w = png_get_image_width(png, info);
    int h = png_get_image_height(png, info);
    int color = png_get_color_type(png, info);
    int depth = png_get_bit_depth(png, info);

    if (depth == 16) png_set_strip_16(png);
    if (color == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color == PNG_COLOR_TYPE_GRAY && depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

    if (color == PNG_COLOR_TYPE_RGB || color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    std::vector<png_bytep> rows(h);
    for (int y = 0; y < h; y++)
        rows[y] = (png_bytep)malloc(png_get_rowbytes(png, info));

    png_read_image(png, rows.data());
    fclose(fp);

    png_destroy_read_struct(&png, &info, nullptr);

    fp = fopen(path, "wb");
    if (!fp) return false;

    png_structp out = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop out_info = png_create_info_struct(out);
    if (!out || !out_info) { fclose(fp); return false; }
    if (setjmp(png_jmpbuf(out))) { fclose(fp); return false; }

    png_init_io(out, fp);
    png_set_IHDR(out, out_info, w, h, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(out, out_info);
    png_write_image(out, rows.data());
    png_write_end(out, nullptr);

    for (int y = 0; y < h; y++) free(rows[y]);

    fclose(fp);
    png_destroy_write_struct(&out, &out_info);

    return true;
}

int main() {
    for (auto& f : fs::directory_iterator(fs::current_path())) {
        if (f.path().extension() == ".png") {
            std::cout << "[PROCESS] " << f.path().filename().string();
            if (convert(f.path().string().c_str()))
                std::cout << " -> OK\n";
            else
                std::cout << " -> FAIL\n";
        }
    }
}