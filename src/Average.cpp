#include <BMPFile.h>

void AverageBlurSegment(std::vector<uint8_t> &imageData, uint16_t width, uint16_t height, size_t start, size_t end)
{
    for (size_t i = start; i < end; i += 3) {
        size_t y = i / (width * 3);
        size_t x = (i / 3) % width;
        size_t index = i;

        uint32_t sum = 0;
        size_t count = 0;

        for (int8_t dy = -1; dy <= 1; dy++) {
            for (int8_t dx = -1; dx <= 1; dx++) {
                if (y + dy >= 0 && y + dy < height && x + dx >= 0 && x + dx < width) {
                    size_t neighborIndex = ((y + dy) * width + (x + dx)) * 3;
                    sum += imageData[neighborIndex];
                    count++;
                }
            }
        }

        uint8_t averagePixel = static_cast<uint8_t>(sum / count);
        imageData[index] = averagePixel;
    }
}

int main()
{
    const uint8_t num_threads = 9;  // 设置线程数

    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint16_t height = static_cast<uint16_t>(myValue.bmpInfo.GetHeight());
    uint16_t width = static_cast<uint16_t>(myValue.bmpInfo.GetWidth());
    std::vector<uint8_t> imageData = myValue.imageData;

    std::vector<std::thread> threads;   // 存储线程对象，每个线程对象将处理图像数据的不同部分
    std::vector<size_t> segmentStarts;  // 用来存储每个数据段的起始位置
    size_t segmentSize = imageData.size() / num_threads;  // 均分处理

    auto beforeTime = std::chrono::steady_clock::now();
    for (uint8_t i = 0; i < num_threads; i++)  // 创建多个线程
    {
        size_t start = i * segmentSize;  // 计算当前线程负责的数据段的起始位置
        size_t end = (i == num_threads - 1) ? imageData.size() : start + segmentSize;  // 结尾
        segmentStarts.push_back(start);
        threads.emplace_back(AverageBlurSegment, std::ref(imageData), width, height, start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outputAverBlur.bmp", imageData, myValue.bmp, myValue.bmpInfo);

    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;

    return 0;
}
