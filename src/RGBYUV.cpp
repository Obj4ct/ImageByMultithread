
// Created by ZThenG on 2023/9/24/0024.
// YUV 颜色编码采用的是 明亮度 和 色度 来指定像素的颜色。 YUV 颜色编码采用的是 明亮度 和 色度 来指定像素的颜色。
#include <BMPFile.h>
#include "Debug.h"
// 0.0082
void RGB2YUV(std::vector<uint8_t> &imageData, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int index = i * 3;

        uint8_t r = imageData[index];
        uint8_t g = imageData[index + 1];
        uint8_t b = imageData[index + 2];

        // uint8_t y = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
        // uint8_t u = static_cast<uint8_t>((b - y) * 0.564 + 128);
        // uint8_t v = static_cast<uint8_t>((r - y) * 0.713 + 128);

        uint8_t y = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
        uint8_t u = static_cast<uint8_t>((b - y) * 0.564 + 128);
        uint8_t v = static_cast<uint8_t>((r - y) * 0.713 + 128);

        if (y < 0) {
            y = 0;
        }
        else if (y > 255) {
            y = 255;
        }

        if (u < 0) {
            u = 0;
        }
        else if (u > 255) {
            u = 255;
        }

        if (v < 0) {
            v = 0;
        }
        else if (v > 255) {
            v = 255;
        }
        imageData[index] = y;
        imageData[index + 1] = u;
        imageData[index + 2] = v;
    }
}

void YUV2RGB(std::vector<uint8_t> &imageData, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int index = i * 3;

        uint8_t y = imageData[index];
        uint8_t u = imageData[index + 1];
        uint8_t v = imageData[index + 2];

        uint8_t r = static_cast<uint8_t>(y + 1.13983 * (v - 128));
        uint8_t g = static_cast<uint8_t>(y - 0.39465 * (u - 128) - 0.5806 * (v - 128));
        uint8_t b = static_cast<uint8_t>(y + 2.03211 * (u - 128));

        if (r < 0) {
            r = 0;
        }
        else if (r > 255) {
            r = 255;
        }

        if (g < 0) {
            g = 0;
        }
        else if (g > 255) {
            g = 255;
        }

        if (b < 0) {
            b = 0;
        }
        else if (b > 255) {
            b = 255;
        }
        imageData[index] = r;
        imageData[index + 1] = g;
        imageData[index + 2] = b;
    }
}

int main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> YUVImageData = myValue.imageData;
    std::vector<uint8_t> RGBImageData;

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread yuvThread([&]() {
        RGB2YUV(YUVImageData, width, height);
        RGBImageData = YUVImageData;
    });
    yuvThread.join();
    std::thread rgbThread([&]() {
        YUV2RGB(RGBImageData, width, height);
    });
    rgbThread.join();
    MYFunction::WriteBMPFile("outputRGBtoYUV.bmp", YUVImageData, myValue.bmp, myValue.bmpInfo);
    MYFunction::WriteBMPFile("outputYUVtoRGB.bmp", RGBImageData, myValue.bmp, myValue.bmpInfo);
    auto afterTime = std::chrono::steady_clock::now();

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
