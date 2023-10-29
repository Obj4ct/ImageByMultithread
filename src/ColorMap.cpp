//
// Created by Obj4ct on 2023/9/22.
// 颜色映射 颜色映射 （colormap）是一系列颜色，它们从起始颜色渐变到结束颜色 对灰度图像重新上色
// 在一些计算机视觉应用程序中，算法的输出结果是灰度图像。但是，人眼在感知彩色图像的变化时更加敏感，而不擅长观察灰度图像的变化。
// 因此我们常常需要将 灰度图像  重新着色转换为 等效 的伪彩色图像
// done

#include <BMPFile.h>
// 映射表 随便写几个
std::vector<uint8_t> colorMap = {100, 10, 200, 255, 100, 190, 100, 200, 190, 105, 0, 255, 100, 200, 0, 255, 0, 255, 0, 150, 5};
void ConvertToGray(std::vector<uint8_t> &imageData, size_t start, size_t end)
{
    // OutputToFile(imageData, "GrayBefore");

    for (size_t i = start; i < end; i += 3) {
        uint8_t r = imageData[i];
        uint8_t g = imageData[i + 1];
        uint8_t b = imageData[i + 2];
        // cal gray
        auto gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
        // gary to every chanel
        imageData[i] = gray;
        imageData[i + 1] = gray;
        imageData[i + 2] = gray;
    }
}
void ColorMap(std::vector<uint8_t> &imageData, std::vector<uint8_t> &colorMap)
{
    // 计算颜色映射表中颜色的数量,3表示channel
    int numColors = colorMap.size() / 3;

    for (size_t i = 0; i < imageData.size(); i += 3) {
        uint8_t gray = imageData[i];
        // 先归一化然后执行 乘法
        int index = static_cast<int>(static_cast<float>(gray) / 255.0f * (numColors - 1));

        imageData[i] = colorMap[index * 3];
        imageData[i + 1] = colorMap[index * 3 + 1];
        imageData[i + 2] = colorMap[index * 3 + 2];
    }
}

int main()
{
    const uint8_t num_threads = 4;  // 设置线程数
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    size_t imageSize = imageData.size();
    int32_t width = myValue.bmpInfo.GetWidth();
    int32_t height = myValue.bmpInfo.GetHeight();
    // 分段处理图像数据
    std::vector<std::thread> threads;              // 存储线程对象，每个线程对象将处理图像数据的不同部分
    std::vector<size_t> segmentStarts;             // 用来存储每个数据段的起始位置
    size_t segmentSize = imageSize / num_threads;  // 均分处理

    auto beforeTime = std::chrono::steady_clock::now();
    for (uint8_t i = 0; i < num_threads; i++)  // 创建多个线程
    {
        size_t start = i * segmentSize;  // 计算当前线程负责的数据段的起始位置
        size_t end = (i == num_threads - 1) ? imageSize : start + segmentSize;  // 结尾,同上
        segmentStarts.push_back(start);
        threads.emplace_back(ConvertToGray, std::ref(imageData), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    ColorMap(imageData, colorMap);
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outputColorMapped.bmp", imageData, myValue.bmp, myValue.bmpInfo);

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
