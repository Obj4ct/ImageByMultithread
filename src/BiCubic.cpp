#include <BMPFile.h>

float_t cubicWeight(float_t t)
{
    // Cubic interpolation kernel
    if (std::abs(t) <= 1.0f) {
        return 1.5f * std::abs(t) * std::abs(t) * std::abs(t) - 2.5f * t * t + 1.0f;
    }
    else if (std::abs(t) <= 2.0f) {
        return -0.5f * std::abs(t) * std::abs(t) * std::abs(t) + 2.5f * t * t - 4.0f * std::abs(t) + 2.0f;
    }
    else {
        return 0.0f;
    }
}
std::vector<uint8_t>
LargeImage(const std::vector<uint8_t> &imageData, int32_t width, int32_t height, int32_t newWidth, int32_t newHeight)
{
    std::vector<uint8_t> resizedImage(newWidth * newHeight * 3);
    std::vector<std::thread> threads;

    for (uint16_t y = 0; y < newHeight; ++y) {
        threads.emplace_back(
            [y, &resizedImage, &imageData, width, height, newWidth, newHeight]() {  // 在 lambda 函数参数列表中添加 newHeight
                for (uint16_t x = 0; x < newWidth; ++x) {
                    int32_t srcX = static_cast<float_t>(x) * width / newWidth;
                    int32_t srcY = static_cast<float_t>(y) * height / newHeight;

                    srcX = std::max(0, std::min(srcX, static_cast<int32_t>(width)));
                    srcY = std::max(0, std::min(srcY, static_cast<int32_t>(height)));

                    float_t weights[4][4];

                    // 权重计算
                    for (int8_t j = -1; j <= 2; ++j) {
                        for (int8_t i = -1; i <= 2; ++i) {
                            int32_t xi = static_cast<float_t>(std::floor(srcX)) + i;
                            int32_t yj = static_cast<float_t>(std::floor(srcY)) + j;

                            xi = std::max(0, std::min(xi, width));
                            yj = std::max(0, std::min(yj, height));

                            float_t wx = cubicWeight(srcX - (xi + 0.5f));
                            float_t wy = cubicWeight(srcY - (yj + 0.5f));
                            weights[j + 1][i + 1] = wx * wy;
                        }
                    }

                    // 插值
                    for (uint8_t channel = 0; channel < 3; ++channel) {
                        float_t interpolatedValue = 0.0f;
                        for (uint8_t j = 0; j < 4; ++j) {
                            for (uint8_t i = 0; i < 4; ++i) {
                                int32_t xi = static_cast<float_t>(std::floor(srcX)) + i - 1;
                                int32_t yj = static_cast<float_t>(std::floor(srcY)) + j - 1;

                                xi = std::max(0, std::min(xi, width - 1));
                                yj = std::max(0, std::min(yj, height - 1));

                                interpolatedValue += weights[j][i] * imageData[(yj * width + xi) * 3 + channel];
                            }
                        }

                        interpolatedValue = std::max(0.0f, std::min(255.0f, interpolatedValue));
                        resizedImage[(y * newWidth + x) * 3 + channel] = static_cast<uint8_t>(interpolatedValue);
                    }
                }
            });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    return resizedImage;
}

std::vector<uint8_t>
SmallImage(const std::vector<uint8_t> &imageData, int32_t width, int32_t height, int32_t newWidth, int32_t newHeight)
{
    std::vector<uint8_t> resizedImage(newWidth * newHeight * 3);
    std::vector<std::thread> threads;

    for (uint16_t y = 0; y < newHeight; ++y) {
        threads.emplace_back(
            [y, &resizedImage, &imageData, width, height, newWidth, newHeight]() {  // 在 lambda 函数参数列表中添加 newHeight
                for (uint16_t x = 0; x < newWidth; ++x) {
                    int32_t srcX = static_cast<float_t>(x) * width / newWidth;
                    int32_t srcY = static_cast<float_t>(y) * height / newHeight;

                    srcX = std::max(0, std::min(srcX, static_cast<int32_t>(width)));
                    srcY = std::max(0, std::min(srcY, static_cast<int32_t>(height)));  

                    float_t weights[4][4];

                    // 权重计算
                    for (int8_t j = -1; j <= 2; ++j) {
                        for (int8_t i = -1; i <= 2; ++i) {
                            int32_t xi = static_cast<float_t>(std::floor(srcX)) + i;
                            int32_t yj = static_cast<float_t>(std::floor(srcY)) + j;  // 修改此行的括号

                            xi = std::max(0, std::min(xi, width - 1));
                            yj = std::max(0, std::min(yj, height - 1));

                            float_t wx = cubicWeight(srcX - (xi + 0.5f));
                            float_t wy = cubicWeight(srcY - (yj + 0.5f));
                            weights[j + 1][i + 1] = wx * wy;
                        }
                    }

                    // 插值
                    for (uint8_t channel = 0; channel < 3; ++channel) {
                        float_t interpolatedValue = 0.0f;
                        for (uint8_t j = 0; j < 4; ++j) {
                            for (uint8_t i = 0; i < 4; ++i) {
                                int32_t xi = static_cast<float_t>(std::floor(srcX)) + i - 1;
                                int32_t yj = static_cast<float_t>(std::floor(srcY)) + j - 1;  // 修改此行的括号

                                xi = std::max(0, std::min(xi, width - 1));
                                yj = std::max(0, std::min(yj, height - 1));

                                interpolatedValue += weights[j][i] * imageData[(yj * width + xi) * 3 + channel];
                            }
                        }

                        interpolatedValue = std::max(0.0f, std::min(255.0f, interpolatedValue));
                        resizedImage[(y * newWidth + x) * 3 + channel] = static_cast<uint8_t>(interpolatedValue);
                    }
                }
            });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    return resizedImage;
}

int main()
{
    const uint8_t num_threads = 4;  // 设置线程数

    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    int32_t largeWidth = width * 2;
    int32_t largeHeigh = height * 2;
    int32_t SmallWidth = width / 2;
    int32_t SmallHeight = height / 2;
    std::vector<uint8_t> imageData = myValue.imageData;

    // 图像放大
    auto beforeTime = std::chrono::steady_clock::now();
    std::vector<uint8_t> largeImageData = LargeImage(imageData, width, height, largeWidth, largeHeigh);
    // 图像缩小
    std::vector<uint8_t> smallImageData = SmallImage(imageData, width, height, SmallWidth, SmallHeight);
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, largeWidth, largeHeigh, 24);
    MYFunction::WriteBMPFile("outputBiCubicLarge.bmp", largeImageData, myValue.bmp, myValue.bmpInfo);
    MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, SmallWidth, SmallHeight, 24);
    MYFunction::WriteBMPFile("outputBiCubicSmall.bmp", smallImageData, myValue.bmp, myValue.bmpInfo);

    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;

    return 0;
}
