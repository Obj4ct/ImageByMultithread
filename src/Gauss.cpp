#include <BMPFile.h>

// 0.0711
inline float_t Gaussian(float_t sigma, int8_t x, int8_t y);

void Gauss(const std::vector<uint8_t> &imageData,
           std::promise<std::vector<uint8_t>> &result,
           int width,
           int height,
           float_t sigma)
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

inline float_t Gaussian(float_t sigma, int8_t x, int8_t y)
{
    return exp(-(x * x + y * y) / (2.0 * sigma * sigma)) / (2.0 * M_PI * sigma * sigma);
}

int main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    // sigma 模糊程度
    float_t sigma = 1.5;
    std::promise<std::vector<uint8_t>> promise;
    std::future<std::vector<uint8_t>> future = promise.get_future();
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread gauss(Gauss, std::ref(imageData), std::ref(promise), std::ref(width), std::ref(height), std::ref(sigma));

    gauss.join();  // 等待线程执行完成

    auto afterTime = std::chrono::steady_clock::now();
    std::vector<uint8_t> resultImage = future.get();
    MYFunction::WriteBMPFile("outputGauss.bmp", resultImage, myValue.bmp, myValue.bmpInfo);
    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;

    return 0;
}
