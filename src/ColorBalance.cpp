//
// Created by ztheng on 2023/9/5.
// done
// 色彩平衡
#include <BMPFile.h>


void ColorBalance(std::vector<uint8_t> &imageData, int32_t width, int32_t height, size_t start, size_t end)
{
    for (size_t i = start / (width * 3); i < end / (width * 3); ++i) {
        for (size_t j = (start % (width * 3)) / 3; j < (end % (width * 3)) / 3; ++j) {
            size_t index = (i * width + j) * 3;
            uint8_t r = imageData[index];
            uint8_t g = imageData[index + 1];
            uint8_t b = imageData[index + 2];

            // 颜色平衡的逻辑
            // 在这里执行颜色平衡的逻辑，你可以使用r、g、b变量进行计算

            // 计算平均颜色值
            double_t totalRed = 0.0;
            double_t totalGreen = 0.0;
            double_t totalBlue = 0.0;

            for (int k = 0; k < height; ++k) {
                for (int l = 0; l < width; ++l) {
                    int idx = (k * width + l) * 3;
                    uint8_t red = imageData[idx];
                    uint8_t green = imageData[idx + 1];
                    uint8_t blue = imageData[idx + 2];
                    totalRed += static_cast<double_t>(red);
                    totalGreen += static_cast<double_t>(green);
                    totalBlue += static_cast<double_t>(blue);
                }
            }

            double_t avgRed = totalRed / (width * height);
            double_t avgGreen = totalGreen / (width * height);
            double_t avgBlue = totalBlue / (width * height);

            // 计算颜色平衡因子
            double_t redFactor = avgRed > 0.0 ? avgRed / 255.0 : 1.0;
            double_t greenFactor = avgGreen > 0.0 ? avgGreen / 255.0 : 1.0;
            double_t blueFactor = avgBlue > 0.0 ? avgBlue / 255.0 : 1.0;

            // 使用颜色平衡因子调整颜色
            r = static_cast<uint8_t>(r * redFactor);
            g = static_cast<uint8_t>(g * greenFactor);
            b = static_cast<uint8_t>(b * blueFactor);

            // 更新像素
            imageData[index] = r;
            imageData[index + 1] = g;
            imageData[index + 2] = b;
        }
    }
}

int main()
{
    const uint8_t num_threads = 4;  // 设置线程数

    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> &imageData = myValue.imageData;
    size_t imageSize = imageData.size();
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();

    std::vector<std::thread> threads;              // 存储线程对象，每个线程对象将处理图像数据的不同部分
    std::vector<size_t> segmentStarts;             // 用来存储每个数据段的起始位置
    size_t segmentSize = imageSize / num_threads;  // 均分处理

    auto beforeTime = std::chrono::steady_clock::now();
    for (uint8_t i = 0; i < num_threads; i++)  // 创建多个线程
    {
        size_t start = i * segmentSize;  // 计算当前线程负责的数据段的起始位置
        size_t end = (i == num_threads - 1) ? imageSize : start + segmentSize;  // 结尾,同上
        segmentStarts.push_back(start);
        threads.emplace_back(ColorBalance, std::ref(imageData), std::ref(width), std::ref(height), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto afterTime = std::chrono::steady_clock::now();
    // ImgInfo();
    //  create file
    MYFunction::WriteBMPFile("outColorBalance.bmp", imageData, myValue.bmp, myValue.bmpInfo);

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;

    return 0;
}
