//
// Created by Obj4ct on 2023/9/21.
//
// 高反差保留
// 高反差保留 = 原图 - 高斯模糊图
// 高斯核，用于模糊像素值的权重矩阵

// 0.0706
#include <BMPFile.h>

inline float_t Gaussian(float_t sigma, uint8_t x, uint8_t y);

void Gauss(const std::vector<uint8_t> &imageData, std::promise<std::vector<uint8_t>> &result, int width, int height, float_t sigma)
{
    std::vector<uint8_t> blurImageData(imageData.size());

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            float_t r = 0.0;
            float_t g = 0.0;
            float_t b = 0.0;
            float_t weightSum = 0.0;

            for (int8_t j = -2; j <= 2; j++) {
                for (int8_t i = -2; i <= 2; i++) {
                    uint16_t pixelX = x + i;
                    uint16_t pixelY = y + j;
                    if (pixelX >= 0 && pixelX < width && pixelY >= 0 && pixelY < height) {
                        uint32_t pixelIndex = (pixelY * width + pixelX) * 3;
                        float_t weight = Gaussian(sigma, i, j);
                        r += static_cast<float_t>(imageData[pixelIndex]) * weight;
                        g += static_cast<float_t>(imageData[pixelIndex + 1]) * weight;
                        b += static_cast<float_t>(imageData[pixelIndex + 2]) * weight;
                        weightSum += weight;
                    }
                }
            }

            uint32_t index = (y * width + x) * 3;
            blurImageData[index] = static_cast<uint8_t>(r / weightSum);
            blurImageData[index + 1] = static_cast<uint8_t>(g / weightSum);
            blurImageData[index + 2] = static_cast<uint8_t>(b / weightSum);
        }
    }

    result.set_value(blurImageData);
}

inline float_t Gaussian(float_t sigma, uint8_t x, uint8_t y)
{

    return exp(-(x * x + y * y) / (2.0 * sigma * sigma)) / (2.0 * M_PI * sigma * sigma);
}

void HighContrast(const std::vector<uint8_t> &imageData,
                  std::promise<std::vector<uint8_t>> &result,
                   std::vector<uint8_t> &blurImageData)
{
    std::vector<uint8_t> highContrastImageData(imageData.size());
    for (size_t i = 0; i < imageData.size(); i++) {
        uint32_t between = static_cast<uint32_t>(imageData[i]) - static_cast<uint32_t>(blurImageData[i]);
        highContrastImageData[i] = static_cast<uint8_t>(between + 128);  // 调整到 0-255 范围
    }

    result.set_value(highContrastImageData);
}

int main()
{
    // 在这里运行你的程序
    // 高反差=原图-高斯
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;
    // sigma 模糊程度
    float_t sigma = 1.5;

    std::promise<std::vector<uint8_t>> gaussPromise;
    std::promise<std::vector<uint8_t>> highContrastPromise;

    std::future<std::vector<uint8_t>> gaussFuture = gaussPromise.get_future();
    std::future<std::vector<uint8_t>> highContrastFuture = highContrastPromise.get_future();

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread gaussThread(Gauss, std::ref(imageData), std::ref(gaussPromise), width, height, sigma);

    // 等待异步任务完成并获取值
    std::vector<uint8_t> gaussValue = gaussFuture.get();
    gaussThread.join();
    std::thread highContrastThread(HighContrast, std::ref(imageData), std::ref(highContrastPromise), std::ref(gaussValue));
    std::vector<uint8_t> value = highContrastFuture.get();
    highContrastThread.join();

    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outputHighContrast.bmp", value, myValue.bmp, myValue.bmpInfo);
    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
