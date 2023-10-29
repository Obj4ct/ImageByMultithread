//
// Created by ZThenG on 2023/9/21/0021.
// 双线性插值法 效果要好于最近邻插值，只是计算量稍大一些，算法复杂些
// 0.0011
#include <BMPFile.h>
std::mutex mtx;
std::vector<uint8_t>
SmallImage(const std::vector<uint8_t> &imageData, int32_t width, int32_t height, int32_t newWidth, int32_t newHeight)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<uint8_t> resizedImage(newWidth * newHeight * 3);
    std::vector<std::thread> threads;
    // 计算缩小因子
    float_t scaleX = static_cast<float_t>(width) / newWidth;
    float_t scaleY = static_cast<float_t>(height) / newHeight;
    for (int32_t y = 0; y < newHeight; y++) {
        threads.emplace_back([y, &resizedImage, &imageData, width, height, newWidth, newHeight, scaleX, scaleY]() {
            for (int32_t x = 0; x < newWidth; x++) {
                // 原图坐标
                auto srcX = x * scaleX;
                auto srcY = y * scaleY;
                // 计算最近的像素点坐标?
                auto x1 = static_cast<int32_t>(srcX);
                auto x2 = static_cast<int32_t>(x1 + 1);
                auto y1 = static_cast<int32_t>(srcY);
                auto y2 = static_cast<int32_t>(y1 + 1);
                // 权重? 计算目标像素的权重 w1、w2、w3 和 w4，这些权重表示了目标像素与最近的四个原始像素之间的关系。
                auto tx = srcX - x1;
                auto ty = srcY - y1;
                auto w1 = (1.0 - tx) * (1.0 - ty);
                auto w2 = tx * (1.0 - ty);
                auto w3 = (1.0 - tx) * ty;
                auto w4 = tx * ty;
                // 新像素值?
                int32_t destIndex = (y * newWidth + x) * 3;
                int32_t srcIndex1 = (y1 * width + x1) * 3;
                int32_t srcIndex2 = (y1 * width + x2) * 3;
                int32_t srcIndex3 = (y2 * width + x1) * 3;
                int32_t srcIndex4 = (y2 * width + x2) * 3;
                for (int32_t channel = 0; channel < 3; channel++) {
                    resizedImage[destIndex + channel] =
                        static_cast<uint8_t>(w1 * imageData[srcIndex1 + channel] + w2 * imageData[srcIndex2 + channel] +
                                             w3 * imageData[srcIndex3 + channel] + w4 * imageData[srcIndex4 + channel]);
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
LargeImage(const std::vector<uint8_t> &imageData, int32_t width, int32_t height, int32_t newWidth, int32_t newHeight)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<uint8_t> resizedImage(newWidth * newHeight * 3);
    std::vector<std::thread> threads;
    // 计算放大因子
    float_t scaleX = static_cast<float_t>(newWidth) / width;
    float_t scaleY = static_cast<float_t>(newHeight) / height;
    for (int32_t y = 0; y < newHeight; y++) {
        threads.emplace_back([y, &resizedImage, &imageData, width, height, newWidth, newHeight, scaleX, scaleY]() {
            for (int32_t x = 0; x < newWidth; x++) {
                // 原图坐标
                auto srcX = x / scaleX;
                auto srcY = y / scaleY;
                // 计算最近的像素点坐标
                auto x1 = static_cast<int32_t>(srcX);
                auto x2 = static_cast<int32_t>(x1 + 1);
                auto y1 = static_cast<int32_t>(srcY);
                auto y2 = static_cast<int32_t>(y1 + 1);
                // 权重?
                auto tx = srcX - x1;
                auto ty = srcY - y1;
                auto w1 = (1.0 - tx) * (1.0 - ty);
                auto w2 = tx * (1.0 - ty);
                auto w3 = (1.0 - tx) * ty;
                auto w4 = tx * ty;
                // 新像素值
                int32_t destIndex = (y * newWidth + x) * 3;
                int32_t srcIndex1 = (y1 * width + x1) * 3;
                int32_t srcIndex2 = (y1 * width + x2) * 3;
                int32_t srcIndex3 = (y2 * width + x1) * 3;
                int32_t srcIndex4 = (y2 * width + x2) * 3;
                for (int32_t channel = 0; channel < 3; channel++) {
                    resizedImage[destIndex + channel] =
                        static_cast<uint8_t>(w1 * imageData[srcIndex1 + channel] + w2 * imageData[srcIndex2 + channel] +
                                             w3 * imageData[srcIndex3 + channel] + w4 * imageData[srcIndex4 + channel]);
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
    int32_t width = myValue.bmpInfo.GetWidth();
    int32_t height = myValue.bmpInfo.GetHeight();
    std::vector<uint8_t> imageData = myValue.imageData;
    std::vector<uint8_t> smallImageData;
    std::vector<uint8_t> largeImageData;
    // 缩小
    int32_t largeWidth = width / 2;
    int32_t largeHeight = height / 2;
    int32_t smalllWidth = width * 2;
    int32_t smalllHeight = height * 2;
    auto beforeTime = std::chrono::steady_clock::now();
    smallImageData = SmallImage(myValue.imageData, width, height, smalllWidth, smalllHeight);
    // 放大
    largeImageData = LargeImage(myValue.imageData, width, height, largeWidth, largeHeight);
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, smalllWidth, smalllHeight, myValue.bmpInfo.GetBitsPerPixel());
    MYFunction::WriteBMPFile("outputBilinearSmall.bmp", smallImageData, myValue.bmp, myValue.bmpInfo);
    MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, largeWidth, largeHeight, myValue.bmpInfo.GetBitsPerPixel());
    MYFunction::WriteBMPFile("outputBilinearLarge.bmp", largeImageData, myValue.bmp, myValue.bmpInfo);

    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
