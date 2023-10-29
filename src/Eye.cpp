#include <BMPFile.h>
// 0.0043
void EyePartial(std::vector<uint8_t> &imageData,
                int32_t width,
                int32_t height,
                int32_t centerX,
                int32_t centerY,
                int32_t radius,
                double intensity,
                int startRow,
                int endRow)
{
    for (int y = startRow; y < endRow; y++) {
        for (int x = 0; x < width; x++) {
            int pixelIndex = (y * width + x) * 3;

            double distance = std::sqrt(static_cast<double>((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY)));

            if (distance < radius) {
                double warpAmount = intensity * std::sin(distance / radius * 3.14159265);
                double newX = x + warpAmount;
                double newY = y + warpAmount;

                int x0 = static_cast<int>(newX);
                int y0 = static_cast<int>(newY);
                int x1 = std::min(x0 + 1, width - 1);
                int y1 = std::min(y0 + 1, height - 1);
                double dx = newX - x0;
                double dy = newY - y0;

                int targetPixelIndex00 = (y0 * width + x0) * 3;
                int targetPixelIndex01 = (y0 * width + x1) * 3;
                int targetPixelIndex10 = (y1 * width + x0) * 3;
                int targetPixelIndex11 = (y1 * width + x1) * 3;

                for (int c = 0; c < 3; c++) {
                    double interpolatedValue = imageData[targetPixelIndex00 + c] * (1 - dx) * (1 - dy) +
                                               imageData[targetPixelIndex01 + c] * dx * (1 - dy) +
                                               imageData[targetPixelIndex10 + c] * (1 - dx) * dy +
                                               imageData[targetPixelIndex11 + c] * dx * dy;

                    imageData[pixelIndex + c] = static_cast<uint8_t>(interpolatedValue);
                }
            }
        }
    }
}

int main()
{

    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t width = myValue.bmpInfo.GetWidth();
    uint32_t height = myValue.bmpInfo.GetHeight();
    std::vector<uint8_t> imageData = myValue.imageData;

    int32_t leftEyeCenterX = 265;
    int32_t leftEyeCenterY = 243;
    int32_t eyeRadius = 20;
    double warpIntensity = 5;
    int32_t rightEyeCenterX = 332;
    int32_t rightEyeCenterY = 243;

    int num_threads = 4; 
    std::vector<std::thread> threads;
    std::mutex mtx; // 用于线程同步

    // 分段处理图像数据
    std::vector<size_t> segmentStarts;             
    size_t segmentSize = height / num_threads; 
    auto beforeTime = std::chrono::steady_clock::now();
    for (int i = 0; i < num_threads; i++) {
        size_t start = i * segmentSize;
        size_t end = (i == num_threads - 1) ? height : start + segmentSize;
        segmentStarts.push_back(start);
        threads.emplace_back([&imageData, width, height, leftEyeCenterX, leftEyeCenterY,rightEyeCenterX,rightEyeCenterY ,eyeRadius, warpIntensity, &mtx, start, end] {
            EyePartial(imageData, width, height, leftEyeCenterX, leftEyeCenterY, eyeRadius, warpIntensity, start, end);
            EyePartial(imageData, width, height, rightEyeCenterX, rightEyeCenterY, eyeRadius, warpIntensity, start, end);
            mtx.lock();
            mtx.unlock();
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outputResizeEye.bmp", imageData, myValue.bmp, myValue.bmpInfo);

    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
