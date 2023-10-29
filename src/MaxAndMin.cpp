//
// Created by ZThenG on 2023/9/20/0020.
// 图像的最大和最小
// 0.0005
#include <BMPFile.h>


int main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    uint8_t minValue = imageData[0];
    uint8_t maxValue = imageData[0];

    auto beforeTime = std::chrono::steady_clock::now();
    for (uint8_t pixelValue : imageData) {
        minValue = std::min(minValue, pixelValue);
        maxValue = std::max(maxValue, pixelValue);
    }
    auto afterTime = std::chrono::steady_clock::now();

    std::cout << "Min pixel value: " << static_cast<int>(minValue) << std::endl;
    std::cout << "Max pixel value: " << static_cast<int>(maxValue) << std::endl;
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
// 多线程版本 效果不好
//  Created by ZThenG on 2023/9/20/0020.
//  图像的最大和最小
//  0.0005
//  #include <BMPFile.h>

// #include "Debug.h"

// int main()
// {
//     auto beforeTime = std::chrono::steady_clock::now();

//     // 在这里运行你的程序
//     MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
//     uint32_t height = myValue.bmpInfo.GetHeight();
//     uint32_t width = myValue.bmpInfo.GetWidth();
//     std::vector<uint8_t> imageData = myValue.imageData;

//     uint8_t minValue = imageData[0];
//     uint8_t maxValue = imageData[0];
//     std::thread minThread([&]{
//         for (uint8_t pixelValue : imageData) {
//             minValue = std::min(minValue, pixelValue);
//         }
//     });
//     std::thread maxThread([&](){
//         for (uint8_t pixelValue : imageData) {
//             maxValue = std::max(maxValue, pixelValue);
//         }
//     }
//     );

//     minThread.join();
//     maxThread.join();

// std::cout << "Min pixel value: " << static_cast<int>(minValue) << std::endl;
// std::cout << "Max pixel value: " << static_cast<int>(maxValue) << std::endl;
// auto afterTime = std::chrono::steady_clock::now();
// double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
// float_t duration_milliseconds = duration_second * 1000;
//    std::cout << duration_milliseconds << "毫秒" << std::endl;
// }
