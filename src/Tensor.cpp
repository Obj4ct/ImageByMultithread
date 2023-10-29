#include <BMPFile.h>

// 定义结构体表示像素
struct Pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Sobel卷积核
int32_t sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
int32_t sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

// 边缘检测函数
void SobelEdge(const std::vector<Pixel> &imageData,
               std::vector<Pixel> &edgeImageData,
               int32_t width,
               int32_t height,
               size_t start,
               size_t end)
{
    for (size_t i = start; i < end; i++) {
        // 计算像素的坐标
        int32_t x = i % width;
        int32_t y = i / width;

        if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
            int32_t sumXRed = 0, sumXGreen = 0, sumXBlue = 0;
            int32_t sumYRed = 0, sumYGreen = 0, sumYBlue = 0;

            for (int32_t j = 0; j < 3; j++) {
                for (int32_t i = 0; i < 3; i++) {
                    int32_t pixelX = x + i - 1;
                    int32_t pixelY = y + j - 1;
                    int32_t pixelIndex = pixelY * width + pixelX;
                    Pixel pixel = imageData[pixelIndex];

                    sumXRed += pixel.red * sobelX[j][i];
                    sumXGreen += pixel.green * sobelX[j][i];
                    sumXBlue += pixel.blue * sobelX[j][i];

                    sumYRed += pixel.red * sobelY[j][i];
                    sumYGreen += pixel.green * sobelY[j][i];
                    sumYBlue += pixel.blue * sobelY[j][i];
                }
            }

            // 计算梯度强度
            int32_t edgeValueRed = std::min(std::max(std::abs(sumXRed) + std::abs(sumYRed), 0), 255);
            int32_t edgeValueGreen = std::min(std::max(std::abs(sumXGreen) + std::abs(sumYGreen), 0), 255);
            int32_t edgeValueBlue = std::min(std::max(std::abs(sumXBlue) + std::abs(sumYBlue), 0), 255);

            edgeImageData[i].red = static_cast<uint8_t>(edgeValueRed);
            edgeImageData[i].green = static_cast<uint8_t>(edgeValueGreen);
            edgeImageData[i].blue = static_cast<uint8_t>(edgeValueBlue);
        }
    }
}

int32_t main()
{

    const uint8_t num_threads = 4;  // 设置线程数
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();

    // 将原始数据从 uint8_t 转换为 Pixel 结构
    std::vector<Pixel> imageData(reinterpret_cast<Pixel *>(myValue.imageData.data()),
                                 reinterpret_cast<Pixel *>(myValue.imageData.data() + myValue.imageData.size()));

    std::vector<Pixel> edgeImageData(imageData.size());

    // 分段处理图像数据
    std::vector<std::thread> threads;
    std::vector<size_t> segmentStarts;
    size_t imageSize = imageData.size();
    size_t segmentSize = imageSize / num_threads;
    auto beforeTime = std::chrono::steady_clock::now();

    for (uint8_t i = 0; i < num_threads; i++) {
        size_t start = i * segmentSize;
        size_t end = (i == num_threads - 1) ? imageSize : start + segmentSize;
        segmentStarts.push_back(start);
        threads.emplace_back(SobelEdge, std::ref(imageData), std::ref(edgeImageData), width, height, start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();

    // 将处理后的数据写入输出图像
    std::vector<uint8_t> edgeImageDataVector(edgeImageData.size() * sizeof(Pixel));
    std::memcpy(edgeImageDataVector.data(), edgeImageData.data(), edgeImageDataVector.size());

    MYFunction::WriteBMPFile("outputTensorEdge.bmp", edgeImageDataVector, myValue.bmp, myValue.bmpInfo);

    float_t duration_second = std::chrono::duration<float>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
