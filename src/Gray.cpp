
#include <BMPFile.h>

void ConvertToGrayPartial(std::vector<uint8_t> &imageData, int start, int end)
{
    for (int i = start; i < end; i += 3) {
        // uint8_t gray = static_cast<uint8_t>(0.299 * imageData[i] + 0.587 * imageData[i + 1] + 0.114 * imageData[i + 2]);
        // imageData[i] = gray;
        // imageData[i + 1] = gray;
        // imageData[i + 2] = gray;

        uint8_t r_radio = imageData[i] * 0.299;
        uint8_t g_radio = imageData[i + 1] * 0.587;
        uint8_t b_radio = imageData[i + 2] * 0.114;
        uint8_t temp = r_radio + g_radio + b_radio;
        imageData[i] = temp;
        imageData[i + 1] = temp;
        imageData[i + 2] = temp;
    }
}

int main()
{
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    std::vector<int> diffData(imageData.size() - 1);
    size_t imageSize = imageData.size();

    int num_threads = 4;

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
        threads.emplace_back(ConvertToGrayPartial, std::ref(imageData), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outColorGray.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
