//
// Created by Obj4ct on 2023/9/22.
// 局部磨皮
// 局部磨皮通常涉及到平滑（模糊）图像中的特定区域，以减少细微的皮肤瑕疵。可以使用简单的均值滤波或高斯滤波等技术来实现这一效果
#include <BMPFile.h>
// 0.0069
double Gaussian(double sigma, int x, int y);

void LocalSkinSmoothing(std::vector<uint8_t> &imageData,
                                        std::promise<std::vector<uint8_t>>& result,
                                        int width,
                                        int height,
                                        int startX,
                                        int startY,
                                        int endX,
                                        int endY,
                                        int radius)
{
    std::vector<uint8_t> smoothedData = imageData;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            double r = 0.0;
            double g = 0.0;
            double b = 0.0;
            double weightSum = 0.0;

            for (int j = -radius; j <= radius; j++) {
                for (int i = -radius; i <= radius; i++) {
                    int pixelX = x + i;
                    int pixelY = y + j;
                    if (pixelX >= startX && pixelX < endX && pixelY >= startY && pixelY < endY) {
                        int pixelIndex = (pixelY * width + pixelX) * 3;
                        double weight = Gaussian(radius, i, j);
                        r += static_cast<double>(imageData[pixelIndex]) * weight;
                        g += static_cast<double>(imageData[pixelIndex + 1]) * weight;
                        b += static_cast<double>(imageData[pixelIndex + 2]) * weight;
                        weightSum += weight;
                    }
                }
            }

            int index = (y * width + x) * 3;
            smoothedData[index] = static_cast<uint8_t>(r / weightSum);
            smoothedData[index + 1] = static_cast<uint8_t>(g / weightSum);
            smoothedData[index + 2] = static_cast<uint8_t>(b / weightSum);
        }
    }

    result.set_value(smoothedData);
}

double Gaussian(double sigma, int x, int y)
{
    double exponent = -(x * x + y * y) / (2.0 * sigma * sigma);
    return exp(exponent) / (2.0 * M_PI * sigma * sigma);
}

int main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t width = myValue.bmpInfo.GetWidth();
    int32_t height = myValue.bmpInfo.GetHeight();
    std::vector<uint8_t> imageData = myValue.imageData;
    int startX = 100;
    int startY = 100;
    int endX = 300;
    int endY = 300;
    int radius = 1;
    std::promise<std::vector<uint8_t>> promise;
    std::future<std::vector<uint8_t>> future = promise.get_future();

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread localSkinSmoothing(LocalSkinSmoothing, std::ref(imageData),std::ref(promise),std::ref(width),std::ref(height),std::ref(startX),std::ref(startY),std::ref(endX),std::ref(endY),std::ref(radius));

    // 等待异步任务完成并获取值
    std::vector<uint8_t> value = future.get();

    localSkinSmoothing.join();
    auto afterTime = std::chrono::steady_clock::now();


    MYFunction::WriteBMPFile("outputLocalSkinSmooth.bmp", value, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
    return 0;
}
