//
// Created by Obj4ct on 2023/9/21.
// 边缘检测
#include <BMPFile.h>
// 0.0103
//  Sobel卷积核，用于边缘检测
//  索伯算子（sobel operator）常用于边缘检测，在粗精度下，是最常用的边缘检测算子，
//  边缘检测时: Gx用于检测纵向边缘, Gy用于检测横向边缘.
//  计算法线时: Gx用于计算法线的横向偏移, Gy用于计算法线的纵向偏移.
//  值得注意的是，一般卷积操作会伴随卷积核的翻转，而sobel却不需要翻转，因为绕中心点旋转180°，并不影响结果。
int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
void SobelEdge(const std::vector<uint8_t> &imageData, std::promise<std::vector<uint8_t>> &result, int width, int height)
{
    std::vector<uint8_t> edgeImageData(imageData.size());

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0;
            int sumY = 0;

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < 3; i++) {
                    int pixelX = x + i - 1;
                    int pixelY = y + j - 1;
                    int pixelIndex = (pixelY * width + pixelX) * 3;
                    int grayValue = static_cast<int>(imageData[pixelIndex]);
                    sumX += grayValue * sobelX[j][i];
                    sumY += grayValue * sobelY[j][i];
                }
            }
            // edge
            // 梯度的强度，通常是水平和垂直梯度的绝对值之和。这个值用来表示像素的边缘强度。
            int edgeValue = std::min(std::max(std::abs(sumX) + std::abs(sumY), 0), 255);
            int index = (y * width + x) * 3;
            edgeImageData[index] = static_cast<uint8_t>(edgeValue);
            edgeImageData[index + 1] = static_cast<uint8_t>(edgeValue);
            edgeImageData[index + 2] = static_cast<uint8_t>(edgeValue);
        }
    }

    result.set_value(edgeImageData);
}

int main()
{

    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    std::promise<std::vector<uint8_t>> promise;
    std::future<std::vector<uint8_t>> future = promise.get_future();

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread edgeThread(SobelEdge, std::ref(imageData),std::ref(promise),std::ref(width), std::ref(height));
    // 等待异步任务完成并获取值
    std::vector<uint8_t> value = future.get();
    edgeThread.join();
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outputEdge.bmp", value, myValue.bmp, myValue.bmpInfo);

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
