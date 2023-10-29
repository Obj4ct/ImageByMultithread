//
// Created by Obj4ct on 2023/9/19.
//
// #include "Debug.h"
// just a simple shadow
#include <BMPFile.h>
// 阴影
void MakeShadow(std::vector<uint8_t> &imageData, uint8_t &shadowValue)
{
    for (size_t i = 0; i < imageData.size(); i += 3) {
        uint8_t r = imageData[i];
        uint8_t g = imageData[i + 1];
        uint8_t b = imageData[i + 2];
        imageData[i] = (r >= shadowValue) ? r - shadowValue : 0;
        imageData[i + 1] = (g >= shadowValue) ? g - shadowValue : 0;
        imageData[i + 2] = (b >= shadowValue) ? b - shadowValue : 0;
    }

}
int main()
{
    uint8_t shadow = 100;
    uint8_t pixel = 100;
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();

    // 在这里运行你的程序
    // std::cout << "input shadow:" << std::endl;
    // std::cin >> shadow;
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread tMakeShadow(MakeShadow, std::ref(imageData), std::ref(shadow));

    tMakeShadow.join();
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("MakeShadow.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
