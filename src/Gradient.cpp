#include <BMPFile.h>

// 优化后的计算梯度函数
void ParallelGradient(const std::vector<uint8_t> &imageData, std::vector<int> &diffData, int start, int end)
{
    for (int i = start; i < end - 1; ++i) {
        int diff = imageData[i + 1] - imageData[i];
        diffData[i] = diff;
    }
}

void Save(const std::vector<int> &data, const std::string &filename)
{
    std::ofstream outputFile("../../test/OutImages/" + filename);
    if (!outputFile.is_open()) {
        std::cout << "Unable to create file" << std::endl;
        return;
    }

    for (int i = 0; i < data.size(); ++i) {
        outputFile << "梯度数据" << i << ": " << data[i] << std::endl;
    }
    std::cout << "Success" << std::endl;
    outputFile.close();
}

int main()
{

    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    std::vector<int> diffData(imageData.size() - 1);

    int num_threads =12;
    size_t imageSize = imageData.size();

    // 分段处理图像数据
    std::vector<std::thread> threads;
    std::vector<size_t> segmentStarts;
    size_t segmentSize = imageSize / num_threads;
    auto beforeTime = std::chrono::steady_clock::now();
    for (uint8_t i = 0; i < num_threads; i++) {
        size_t start = i * segmentSize;
        size_t end = (i == num_threads - 1) ? imageSize : start + segmentSize;
        segmentStarts.push_back(start);
        threads.emplace_back(ParallelGradient, std::ref(imageData), std::ref(diffData), start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto afterTime = std::chrono::steady_clock::now();
    Save(diffData, "Gradient.txt");

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
