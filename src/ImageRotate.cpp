#include <BMPFile.h>
std::mutex myMutex;

// 图像旋转
// RotateImage
void RotateImage(std::vector<uint8_t> &imageData, int32_t width, int32_t height, double_t angle)
{
    double_t radians = angle * M_PI / 180.0;
    std::vector<uint8_t> rotatedImageData(width * height * 3);

    int32_t centerX = width / 2;
    int32_t centerY = height / 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double_t rotatedX = std::cos(radians) * (x - centerX) - std::sin(radians) * (y - centerY) + centerX;
            double_t rotatedY = std::sin(radians) * (x - centerX) + std::cos(radians) * (y - centerY) + centerY;

            if (rotatedX >= 0 && rotatedX < width && rotatedY >= 0 && rotatedY < height) {
                int originalIndex =
                    static_cast<int>(std::round(rotatedY)) * width * 3 + static_cast<int>(std::round(rotatedX)) * 3;
                int newIndex = y * width * 3 + x * 3;
                rotatedImageData[newIndex] = imageData[originalIndex];
                rotatedImageData[newIndex + 1] = imageData[originalIndex + 1];
                rotatedImageData[newIndex + 2] = imageData[originalIndex + 2];
            }
        }
    }

    imageData = rotatedImageData;
}
void RotateReverse(std::vector<uint8_t> &imageData, int32_t width, int32_t height, double_t angle)
{
    double_t clockwiseAngle = 360.0 - angle;
    RotateImage(imageData, width, height, clockwiseAngle);
}
int main()
{

    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;
    std::vector<uint8_t> rotateImage(imageData.size());
    rotateImage = imageData;
    std::vector<uint8_t> reverseImage(imageData.size());
    reverseImage = imageData;

    double_t angle = 30;
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread rotateThread(RotateImage, std::ref(rotateImage), width, height, angle);
    std::thread reverseThread(RotateReverse, std::ref(reverseImage), width, height, angle);
    rotateThread.join();
    reverseThread.join();
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outputRotate.bmp", rotateImage, myValue.bmp, myValue.bmpInfo);
    MYFunction::WriteBMPFile("outputRotateReverse.bmp", reverseImage, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    std::cout << duration_second << " seconds" << std::endl;

    return 0;
}
