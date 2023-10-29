#include <BMPFile.h>
// 0.026
void Complementary(std::vector<uint8_t> &imageData, size_t start, size_t end)
{
    for (size_t i = start; i < end; i += 3) {
        size_t r = imageData[i];
        size_t g = imageData[i + 1];
        size_t b = imageData[i + 2];
        size_t maxRgb = std::max(std::max(r, g), b);
        size_t minRgb = std::min(std::min(r, g), b);
        imageData[i] = maxRgb + minRgb - r;
        imageData[i + 1] = maxRgb + minRgb - g;
        imageData[i + 2] = maxRgb + minRgb - b;
    }
}

int main()
{
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;
    size_t imageSize = imageData.size();
    int num_threads = 4;  // 获取可用的线程数
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
        threads.emplace_back(Complementary, std::ref(imageData), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
    MYFunction::WriteBMPFile("outColorComplementary.bmp", imageData, myValue.bmp, myValue.bmpInfo);
}
