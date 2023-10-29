//
// Created by ZThenG on 2023/9/24/0024.
// YIQ电视系统标准
// 在YIQ系统中，Y分量代表图像的亮度信息，I、Q两个分量则携带颜色信息，I分量代表从橙色到青色的颜色变化，而Q分量则代表从紫色到黄绿色的颜色变化。
#include <BMPFile.h>
// 0.0083
void RGB2YIQ(std::vector<uint8_t> &imageData, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int index = i * 3;

        double r = imageData[index];
        double g = imageData[index + 1];
        double b = imageData[index + 2];

        double y = 0.299 * r + 0.587 * g + 0.114 * b;
        double iValue = 0.596 * r - 0.275 * g - 0.321 * b;
        double q = 0.212 * r - 0.523 * g + 0.311 * b;

        if (y < 0) {
            y = 0;
        }
        else if (y > 255) {
            y = 255;
        }

        if (iValue < 0) {
            iValue = 0;
        }
        else if (iValue > 255) {
            iValue = 255;
        }

        if (q < 0) {
            q = 0;
        }
        else if (q > 255) {
            q = 255;
        }
        imageData[index] = static_cast<uint8_t>(y);
        imageData[index + 1] = static_cast<uint8_t>(iValue);
        imageData[index + 2] = static_cast<uint8_t>(q);
    }
}

void YIQ2RGB(std::vector<uint8_t> &imageData, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int index = i * 3;

        double y = imageData[index];
        double iValue = imageData[index + 1];
        double q = imageData[index + 2];

        double r = y + 0.956 * iValue + 0.621 * q;
        double g = y - 0.272 * iValue - 0.647 * q;
        double b = y - 1.106 * iValue + 1.703 * q;

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

        imageData[index] = static_cast<uint8_t>(r);
        imageData[index + 1] = static_cast<uint8_t>(g);
        imageData[index + 2] = static_cast<uint8_t>(b);
    }
}

int main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread yiqThread([&]() {
        RGB2YIQ(imageData, width, height);
        MYFunction::WriteBMPFile("outputRGBtoYIQ.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    });
    yiqThread.join();
    std::thread rgbThread([&]() {
        YIQ2RGB(imageData, width, height);
        MYFunction::WriteBMPFile("outputYIQtoRGB.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    });

    rgbThread.join();
    auto afterTime = std::chrono::steady_clock::now();
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
