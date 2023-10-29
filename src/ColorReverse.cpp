
#include <BMPFile.h>

void InvertColorsParallel(std::vector<uint8_t> &imageData, int start, int end)
{
    for (int i = start; i < end; i += 3) {
        imageData[i] = 255 - imageData[i];
        imageData[i + 1] = 255 - imageData[i + 1];
        imageData[i + 2] = 255 - imageData[i + 2];
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
    for (uint8_t i = 0; i < num_threads; i++)      // 创建多个线程
    {
        size_t start = i * segmentSize;  // 计算当前线程负责的数据段的起始位置
        size_t end = (i == num_threads - 1) ? imageSize : start + segmentSize;  // 结尾,同上
        segmentStarts.push_back(start);
        threads.emplace_back(InvertColorsParallel, std::ref(imageData), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outColorReverse.bmp", imageData, myValue.bmp, myValue.bmpInfo);


    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
