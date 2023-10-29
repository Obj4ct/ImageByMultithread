////
//// Created by ZThenG on 2023/9/21/0021.
////
// done
// 中值模糊
#include <BMPFile.h>
// 0.1133
uint8_t CalculateMedian(std::vector<uint8_t> &window)
{
    std::sort(window.begin(), window.end());
    return window[window.size() / 2];
}
void MedianBlur(std::vector<uint8_t> &imageData, uint32_t width, uint32_t height)
{
    std::vector<uint8_t> blurImage(imageData);
    for (uint32_t y = 1; y < height - 1; y++) {
        for (uint32_t x = 1; x < width - 1; x++) {
            std::vector<uint8_t> window;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    uint32_t index = ((y + dy) * width + (x + dx)) * 3;
                    window.push_back(imageData[index]);
                }
            }
            uint32_t index = (y * width + x) * 3;
            imageData[index] = CalculateMedian(window);
        }
    }
}

int main()
{
    const uint8_t num_threads = 4;  // 设置线程数
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    std::vector<uint8_t> imageData = myValue.imageData;
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread medianBlur(MedianBlur, std::ref(imageData), std::ref(width), std::ref(height));
    medianBlur.join();
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outputMedianBlur.bmp", imageData, myValue.bmp, myValue.bmpInfo);

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
