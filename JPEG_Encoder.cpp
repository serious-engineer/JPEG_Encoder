#include <iostream>
#include <fstream>
#include <vector>
#include <jpeglib.h>

struct YUVImage {
    int width;
    int height;
    std::vector<unsigned char> Y;
    std::vector<unsigned char> U;
    std::vector<unsigned char> V;
};

// Function to convert YUV to RGB
void YUVtoRGB(const YUVImage& yuv, std::vector<unsigned char>& rgb) {
    int index = 0;
    for (int i = 0; i < yuv.height; ++i) {
        for (int j = 0; j < yuv.width; ++j) {
            int Y = yuv.Y[i * yuv.width + j];
            int U = yuv.U[i / 2 * yuv.width / 2 + j / 2] - 128; // 4:2:0 subsampling
            int V = yuv.V[i / 2 * yuv.width / 2 + j / 2] - 128;

            int R = Y + 1.402 * V;
            int G = Y - 0.344136 * U - 0.714136 * V;
            int B = Y + 1.772 * U;

            rgb[index++] = std::min(std::max(R, 0), 255);
            rgb[index++] = std::min(std::max(G, 0), 255);
            rgb[index++] = std::min(std::max(B, 0), 255);
        }
    }
}

// Function to write JPEG
void writeJPEG(const char* filename, const std::vector<unsigned char>& rgb, int width, int height) {
    FILE* outfile = fopen(filename, "wb");
    if (!outfile) {
        std::cerr << "Error opening output file." << std::endl;
        return;
    }

    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;  // RGB has 3 components
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 85, TRUE);  // Set quality to 85
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = width * 3;  // 3 bytes per pixel
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (unsigned char*)&rgb[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    std::cout << "JPEG saved to " << filename << std::endl;
}

int main() {
    // Input YUV file (assume YUV420 with known width/height)
    const char* inputFile = "input.yuv";
    const char* outputFile = "output.jpg";
    int width = 640;
    int height = 480;

    std::ifstream input(inputFile, std::ios::binary);
    if (!input) {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    YUVImage yuv;
    yuv.width = width;
    yuv.height = height;
    yuv.Y.resize(width * height);
    yuv.U.resize(width * height / 4);  // 4:2:0 subsampling
    yuv.V.resize(width * height / 4);

    input.read(reinterpret_cast<char*>(yuv.Y.data()), yuv.Y.size());
    input.read(reinterpret_cast<char*>(yuv.U.data()), yuv.U.size());
    input.read(reinterpret_cast<char*>(yuv.V.data()), yuv.V.size());
    input.close();

    std::vector<unsigned char> rgb(width * height * 3);
    YUVtoRGB(yuv, rgb);

    writeJPEG(outputFile, rgb, width, height);

    return 0;
}
