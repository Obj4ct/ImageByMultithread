//
// Created by Obj4ct on 2023/9/19.
//
// #include "Debug.h"
// just a simple shadow
#include <BMPFile.h>
// 亮的地方变暗
void HighLight(std::vector<uint8_t> &imageData, uint8_t &pixel, size_t start, size_t end)
{
    for (int i = start; i < end; i++) {
        if (imageData[i] > pixel) {
            // 在指定像素值之上的像素降低值，确保颜色值不会小于0
            uint8_t newValue = imageData[i] - 100;
            imageData[i] = (newValue >= 0) ? static_cast<uint8_t>(newValue) : 0;
        }
    }
}
int main()
{
    uint8_t pixel = 100;

    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    size_t imageSize = imageData.size();
    int num_threads = 4;

    // 分段处理图像数据
    std::vector<std::thread> threads;              // 存储线程对象，每个线程对象将处理图像数据的不同部分
    std::vector<size_t> segmentStarts;             // 用来存储每个数据段的起始位置
    size_t segmentSize = imageSize / num_threads;  // 均分处理
    auto beforeTime = std::chrono::steady_clock::now();
    for (uint8_t i = 0; i < num_threads; i++)      // 创建多个线程
    {
        size_t start = i * segmentSize;  // 计算当前线程负责的数据段的起始位置
        size_t end = (i == num_threads - 1) ? imageSize : start + segmentSize;  // 结尾,同上
        segmentStarts.push_back(start);
        threads.emplace_back(HighLight, std::ref(imageData), std::ref(pixel), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("HighLight.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
