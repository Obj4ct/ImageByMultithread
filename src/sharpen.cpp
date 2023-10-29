#include <BMPFile.h>

// 锐化
// 0.0697
inline float_t Gaussian(float_t sigma, int8_t x, int8_t y);

void Gauss(const std::vector<uint8_t> &imageData, std::promise<std::vector<uint8_t>> &result, int width, int height, float_t sigma)
{
    std::vector<uint8_t> blurImageData(imageData.size());

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x= 0; x < width; x++) {
            float_t r = 0.0;
            float_t g = 0.0;
            float_t b = 0.0;
            float_t weightSum = 0.0;

            for (int8_t j = -2; j <= 2; j++) {
                for (int8_t i = -2; i <= 2; i++) {
                    int pixelX = x + i;
                    int pixelY = y + j;
                    if (pixelX >= 0 && pixelX < width && pixelY >= 0 && pixelY < height) {
                        int pixelIndex = (pixelY * width + pixelX) * 3;
                        float_t weight = Gaussian(sigma, i, j);
                        r += static_cast<float_t>(imageData[pixelIndex]) * weight;
                        g += static_cast<float_t>(imageData[pixelIndex + 1]) * weight;
                        b += static_cast<float_t>(imageData[pixelIndex + 2]) * weight;
                        weightSum += weight;
                    }
                }
            }

            int index = (y * width + x) * 3;
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

void HighContrast(const std::vector<uint8_t> &imageData,
                  std::promise<std::vector<uint8_t>> &result,
                  const std::vector<uint8_t> &blurImageData)
{
    std::vector<uint8_t> highContrastImageData(imageData.size());

    for (size_t i = 0; i < imageData.size(); i++) {
        int between = static_cast<int>(imageData[i]) - static_cast<int>(blurImageData[i]);
        highContrastImageData[i] = static_cast<uint8_t>(between + 128);  // 0-255
    }

    result.set_value(highContrastImageData);
}

void Sharpen(const std::vector<uint8_t> &imageData,
             std::promise<std::vector<uint8_t>> &result,
             const std::vector<uint8_t> &highContrastImageData)
{
    std::vector<uint8_t> sharpenImageData(imageData.size());

    for (size_t i = 0; i < imageData.size(); i++) {
        int addValue = static_cast<int>(imageData[i]) + static_cast<int>(highContrastImageData[i]);
        sharpenImageData[i] = static_cast<uint8_t>(std::max(std::min(addValue - 170, 255), 0));
    }

    result.set_value(sharpenImageData);
}

int main()
{
    // 在这里运行你的程序
    // 锐化=原图+高反差
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;
    // sigma 模糊程度
    float_t sigma = 1.5;

    std::promise<std::vector<uint8_t>> blurPromise;
    std::promise<std::vector<uint8_t>> highPromise;
    std::promise<std::vector<uint8_t>> sharpenPromise;

    std::future<std::vector<uint8_t>> blurValue=blurPromise.get_future();
    std::future<std::vector<uint8_t>> highValue=highPromise.get_future();
    std::future<std::vector<uint8_t>> sharpenValue=sharpenPromise.get_future();
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread gauss(Gauss, std::ref(imageData), std::ref(blurPromise), std::ref(width), std::ref(height), std::ref(sigma));
    std::vector<uint8_t> gaussValue = blurValue.get();
    gauss.join();
    std::thread highContrast(HighContrast, std::ref(imageData), std::ref(highPromise), std::ref(gaussValue));
    std::vector<uint8_t> highContrastValue = highValue.get();
    highContrast.join();

    std::thread sharpen(Sharpen, std::ref(imageData), std::ref(sharpenPromise), std::ref(highContrastValue));
    std::vector<uint8_t> finalResult = sharpenValue.get();
    sharpen.join();
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outputSharpen.bmp", finalResult, myValue.bmp, myValue.bmpInfo);

    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
