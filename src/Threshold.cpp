// //
// // Created by ZThenG on 2023/9/19/0019.
// // 阈值
#include <BMPFile.h>
// done
//  0.0262
void ApplyThreshold(std::vector<uint8_t> &imageData, uint32_t threshold, size_t start, size_t end)
{
    for (size_t i = start; i < end; i += 3) {
        uint8_t r = imageData[i];
        uint8_t g = imageData[i + 1];
        uint8_t b = imageData[i + 2];

        // 计算灰度值并使用double类型保存
        float_t gray = 0.299 * r + 0.587 * g + 0.114 * b;

        if (gray > threshold) {
            // 使用比例因子来调整颜色通道值
            float_t factor = (gray - threshold) / (255.0 - threshold);
            imageData[i] = std::min(255, static_cast<int>(r + 100 * factor));
            imageData[i + 1] = std::min(255, static_cast<int>(g + 100 * factor));
            imageData[i + 2] = std::min(255, static_cast<int>(b + 100 * factor));
        }
        else {
            // 使用比例因子来调整颜色通道值
            float_t factor = gray / threshold;
            imageData[i] = std::max(0, static_cast<int>(r * factor));
            imageData[i + 1] = std::max(0, static_cast<int>(g * factor));
            imageData[i + 2] = std::max(0, static_cast<int>(b * factor));
        }
    }
}

int main()
{

    const uint8_t num_threads = 4;   // 设置线程数
    const uint32_t threshold = 100;  // 阈值
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> &imageData = myValue.imageData;
    size_t imageSize = imageData.size();

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
        threads.emplace_back(ApplyThreshold, std::ref(imageData), threshold, start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outputThreshold.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
    return 0;
}
