
#include <BMPFile.h>

std::vector<uint8_t>
SmallImage(const std::vector<uint8_t> &imageData, int32_t width, int32_t height, int32_t newWidth, int32_t newHeight)
{
    std::vector<uint8_t> resizedImage(newWidth * newHeight * 3);

    // 计算缩放因子
    double scaleX = static_cast<double>(width) / newWidth;
    double scaleY = static_cast<double>(height) / newHeight;

    for (int32_t y = 0; y < newHeight; y++) {
        for (int32_t x = 0; x < newWidth; x++) {
            // 原图坐标
            auto srcX = static_cast<int32_t>(x * scaleX);
            auto srcY = static_cast<int32_t>(y * scaleY);
            // 复制到目标
            int32_t srcIndex = (srcY * width + srcX) * 3;
            int32_t destIndex = (y * newWidth + x) * 3;
            resizedImage[destIndex] = imageData[srcIndex];
            resizedImage[destIndex + 1] = imageData[srcIndex + 1];
            resizedImage[destIndex + 2] = imageData[srcIndex + 2];
        }
    }

    return resizedImage;
}

std::vector<uint8_t>
LargeImage(const std::vector<uint8_t> &imageData, int32_t width, int32_t height, int32_t newWidth, int32_t newHeight)
{
    std::vector<uint8_t> resizedImage(newHeight * newWidth * 3);

    // 放大因子
    double scaleX = static_cast<double>(newWidth) / width;
    double scaleY = static_cast<double>(newHeight) / height;

    for (int32_t y = 0; y < newHeight; y++) {
        for (int32_t x = 0; x < newWidth; x++) {
            // 原图坐标
            auto srcX = static_cast<int32_t>(x / scaleX);
            auto srcY = static_cast<int32_t>(y / scaleY);
            int32_t srcIndex = (srcY * width + srcX) * 3;
            int32_t destIndex = (y * newWidth + x) * 3;
            // 复制到目标
            resizedImage[destIndex] = imageData[srcIndex];
            resizedImage[destIndex + 1] = imageData[srcIndex + 1];
            resizedImage[destIndex + 2] = imageData[srcIndex + 2];
        }
    }

    return resizedImage;
}

int main()
{

    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t width = myValue.bmpInfo.GetWidth();
    int32_t height = myValue.bmpInfo.GetHeight();
    std::vector<uint8_t> imageData = myValue.imageData;

    std::vector<uint8_t> smallImageData;
    std::vector<uint8_t> largeImageData;

    auto beforeTime = std::chrono::steady_clock::now();
    // 创建两个线程分别处理缩小和放大操作
    std::thread threadSmall([&]() {
        int32_t newWidth = width / 2;
        int32_t newHeight = height / 2;
        smallImageData = SmallImage(imageData, width, height, newWidth, newHeight);
        MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, newWidth, newHeight, myValue.bmpInfo.GetBitsPerPixel());
        MYFunction::WriteBMPFile("outputNearestSmall.bmp", smallImageData, myValue.bmp, myValue.bmpInfo);
    });

    std::thread threadLarge([&]() {
        int32_t newWidth = width * 2;
        int32_t newHeight = height * 2;
        largeImageData = LargeImage(imageData, width, height, newWidth, newHeight);
        MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, newWidth, newHeight, myValue.bmpInfo.GetBitsPerPixel());
        MYFunction::WriteBMPFile("outputNearestLarge.bmp", largeImageData, myValue.bmp, myValue.bmpInfo);
    });
    auto afterTime = std::chrono::steady_clock::now();

    // 等待两个线程完成
    threadSmall.join();
    threadLarge.join();

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
