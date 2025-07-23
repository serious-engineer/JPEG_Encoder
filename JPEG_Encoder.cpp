#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <jpeglib.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

struct YUVImage {
    int width;
    int height;
    std::vector<unsigned char> Y;
    std::vector<unsigned char> U;
    std::vector<unsigned char> V;
};

void YUVtoRGB(const YUVImage& yuv, std::vector<unsigned char>& rgb) {
    int index = 0;
    for (int i = 0; i < yuv.height; ++i) {
        for (int j = 0; j < yuv.width; ++j) {
            int Y = yuv.Y[i * yuv.width + j];
            int U = yuv.U[i / 2 * (yuv.width / 2) + j / 2] - 128;
            int V = yuv.V[i / 2 * (yuv.width / 2) + j / 2] - 128;

            int R = Y + 1.402 * V;
            int G = Y - 0.344136 * U - 0.714136 * V;
            int B = Y + 1.772 * U;

            rgb[index++] = std::clamp(R, 0, 255);
            rgb[index++] = std::clamp(G, 0, 255);
            rgb[index++] = std::clamp(B, 0, 255);
        }
    }
}

void RGBtoYUV420(const std::vector<unsigned char>& rgb, int width, int height,
                 std::vector<unsigned char>& Y, std::vector<unsigned char>& U, std::vector<unsigned char>& V) {
    Y.resize(width * height);
    U.resize(width * height / 4);
    V.resize(width * height / 4);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int idx = (i * width + j) * 3;
            int R = rgb[idx];
            int G = rgb[idx + 1];
            int B = rgb[idx + 2];

            int y = 0.299 * R + 0.587 * G + 0.114 * B;
            int u = -0.169 * R - 0.331 * G + 0.5 * B + 128;
            int v = 0.5 * R - 0.419 * G - 0.081 * B + 128;

            Y[i * width + j] = std::clamp(y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0) {
                int uvIndex = (i / 2) * (width / 2) + (j / 2);
                U[uvIndex] = std::clamp(u, 0, 255);
                V[uvIndex] = std::clamp(v, 0, 255);
            }
        }
    }
}

void encodeJPEG(const char* filename, const std::vector<unsigned char>& rgb, int width, int height, int quality = 85) {
    FILE* outfile = fopen(filename, "wb");
    if (!outfile) {
        cerr << "Error opening output file." << endl;
        return;
    }

    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (unsigned char*)&rgb[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    cout << "JPEG saved to " << filename << endl;
}

bool loadJPEG(const char* filename, std::vector<unsigned char>& rgb, int& width, int& height) {
    FILE* infile = fopen(filename, "rb");
    if (!infile) {
        cerr << "Error opening input file: " << filename << endl;
        return false;
    }

    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    int row_stride = width * cinfo.output_components;
    rgb.resize(width * height * 3);

    while (cinfo.output_scanline < height) {
        unsigned char* row_ptr = &rgb[cinfo.output_scanline * row_stride];
        jpeg_read_scanlines(&cinfo, &row_ptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage:\n";
        cout << "  Encode: " << argv[0] << " encode input.yuv output.jpg width height\n";
        cout << "  Decode: " << argv[0] << " decode input.jpg output.yuv\n";
        return 1;
    }

    string mode = argv[1];

    if (mode == "encode") {
        const char* inputFile = argv[2];
        const char* outputFile = argv[3];
        int width = atoi(argv[4]);
        int height = atoi(argv[5]);

        ifstream in(inputFile, ios::binary);
        if (!in) {
            cerr << "Failed to open input YUV file\n";
            return 1;
        }

        YUVImage yuv{width, height};
        yuv.Y.resize(width * height);
        yuv.U.resize(width * height / 4);
        yuv.V.resize(width * height / 4);

        in.read(reinterpret_cast<char*>(yuv.Y.data()), yuv.Y.size());
        in.read(reinterpret_cast<char*>(yuv.U.data()), yuv.U.size());
        in.read(reinterpret_cast<char*>(yuv.V.data()), yuv.V.size());
        in.close();

        std::vector<unsigned char> rgb(width * height * 3);
        YUVtoRGB(yuv, rgb);
        encodeJPEG(outputFile, rgb, width, height);
    }

    else if (mode == "decode") {
        const char* inputFile = argv[2];
        const char* outputFile = argv[3];

        std::vector<unsigned char> rgb;
        int width, height;

        if (!loadJPEG(inputFile, rgb, width, height)) {
            return 1;
        }

        std::vector<unsigned char> Y, U, V;
        RGBtoYUV420(rgb, width, height, Y, U, V);

        ofstream out(outputFile, ios::binary);
        out.write(reinterpret_cast<char*>(Y.data()), Y.size());
        out.write(reinterpret_cast<char*>(U.data()), U.size());
        out.write(reinterpret_cast<char*>(V.data()), V.size());
        out.close();

        cout << "YUV output saved to " << outputFile << " (" << width << "x" << height << ")\n";
    }

    else {
        cerr << "Unknown mode: " << mode << endl;
        return 1;
    }

    return 0;
}
