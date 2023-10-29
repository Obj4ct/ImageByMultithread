#include <BMPFile.h>
// done
uint32_t GetSum(const std::vector<uint8_t> &imageData)
{
    return std::accumulate(imageData.begin(), imageData.end(), 0);
}

float_t CalAver(const std::vector<uint8_t> &imageData)
{
    uint32_t sum = GetSum(imageData);
    return sum / imageData.size();
}

float_t CalStandard(const std::vector<uint8_t> &imageData, float_t aver)
{
    float_t tempSum = 0.0;
    for (uint8_t i : imageData) {
        tempSum += (i - aver) * (i - aver);
    }
    float_t variance = tempSum / static_cast<float_t>(imageData.size());
    return std::sqrt(variance);
}

void AutoContrastSegment(std::vector<uint8_t> &imageData, float_t aver, float_t standard, size_t start, size_t end)
{
    float_t factor = 128.0 / standard;

    for (size_t i = start; i < end; i += 3) {
        imageData[i] = std::max(0, std::min(255, static_cast<int32_t>(factor * (imageData[i] - aver) + 128)));
        imageData[i + 1] = std::max(0, std::min(255, static_cast<int32_t>(factor * (imageData[i + 1] - aver) + 128)));
        imageData[i + 2] = std::max(0, std::min(255, static_cast<int32_t>(factor * (imageData[i + 2] - aver) + 128)));
    }
}

int main()
{
    const uint8_t num_threads = 10;  // 设置线程数
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    size_t imageSize = imageData.size();
    float_t aver = CalAver(imageData);
    float_t standard = CalStandard(imageData, aver);
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
        threads.emplace_back(AutoContrastSegment, std::ref(imageData), std::ref(aver), std::ref(standard), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outputAutoContrast.bmp", imageData, myValue.bmp, myValue.bmpInfo);

    float_t duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;

    return 0;
}
