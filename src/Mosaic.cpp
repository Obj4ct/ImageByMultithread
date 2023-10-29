#include <BMPFile.h>


void FullMosaic(std::vector<uint8_t> &imageData, uint32_t width, uint32_t height, uint32_t degree)
{
    for (uint32_t y = 0; y < height; y += degree) {
        for (uint32_t x = 0; x < width; x += degree) {
            uint32_t totalR = 0;
            uint32_t totalG = 0;
            uint32_t totalB = 0;
            int mosaic = 0;

            for (uint32_t dy = 0; dy < degree && y + dy < height; dy++) {
                for (uint32_t dx = 0; dx < degree && x + dx < width; dx++) {
                    uint32_t index = ((y + dy) * width + (x + dx)) * 3;
                    totalR += imageData[index];
                    totalG += imageData[index + 1];
                    totalB += imageData[index + 2];
                    mosaic++;
                }
            }
            auto averageR = totalR / mosaic;
            auto averageG = totalG / mosaic;
            auto averageB = totalB / mosaic;

            for (uint32_t dy = 0; dy < degree && y + dy < height; dy++) {
                for (uint32_t dx = 0; dx < degree && x + dx < width; dx++) {
                    uint32_t index = ((y + dy) * width + (x + dx)) * 3;
                    imageData[index] = averageR;
                    imageData[index + 1] = averageG;
                    imageData[index + 2] = averageB;
                }
            }
        }
    }
}

int AreaMosaic(std::vector<uint8_t> &imageData,
               uint32_t width,
               uint32_t height,
               uint32_t beginX,
               uint32_t beginY,
               uint32_t blockWidth,
               uint32_t blockHeight,
               uint32_t degree)
{
    uint32_t endX = beginX + blockWidth;
    uint32_t endY = beginY + blockHeight;

    if (endX > width) {
        std::cout << "Error: Mosaic width exceeds image width." << std::endl;
        return 1;
    }
    else if (endY > height) {
        std::cout << "Error: Mosaic height exceeds image height." << std::endl;
        return 1;
    }

    for (uint32_t y = beginY; y < endY; y += degree) {
        for (uint32_t x = beginX; x < endX; x += degree) {
            int totalR = 0;
            int totalG = 0;
            int totalB = 0;
            int pixelCount = 0;

            for (int dy = 0; dy < degree && y + dy < height; dy++) {
                for (int dx = 0; dx < degree && x + dx < width; dx++) {
                    uint32_t pixelIndex = ((y + dy) * width + (x + dx)) * 3;
                    totalR += imageData[pixelIndex];
                    totalG += imageData[pixelIndex + 1];
                    totalB += imageData[pixelIndex + 2];
                    pixelCount++;
                }
            }

            int averageR = totalR / pixelCount;
            int averageG = totalG / pixelCount;
            int averageB = totalB / pixelCount;

            for (int dy = 0; dy < degree && y + dy < height; dy++) {
                for (int dx = 0; dx < degree && x + dx < width; dx++) {
                    uint32_t pixelIndex = ((y + dy) * width + (x + dx)) * 3;
                    imageData[pixelIndex] = averageR;
                    imageData[pixelIndex + 1] = averageG;
                    imageData[pixelIndex + 2] = averageB;
                }
            }
        }
    }
    return 0;
}

int main()
{

    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    uint32_t degree = 10;
    std::vector<uint8_t> imageDataFull = imageData;
    std::vector<uint8_t> imageDataArea = imageData;

    std::vector<std::thread> threads;

    auto beforeTime = std::chrono::steady_clock::now();
    // 创建多个线程来并行处理全图马赛克和区域马赛克任务
    threads.emplace_back(FullMosaic, std::ref(imageDataFull), width, height, degree);
    threads.emplace_back(AreaMosaic, std::ref(imageDataArea), width, height, width / 4, height / 4, 100, 100, degree);

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outputFullMosaic.bmp", imageDataFull, myValue.bmp, myValue.bmpInfo);
    MYFunction::WriteBMPFile("outputAreaMosaic.bmp", imageDataArea, myValue.bmp, myValue.bmpInfo);

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
