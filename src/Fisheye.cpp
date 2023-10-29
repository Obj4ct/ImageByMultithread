//
// Created by Obj4ct on 2023/9/22.
// 鱼眼镜头捕捉到的图像中，远离中心的部分会被扭曲成弯曲的形状，
// 而靠近中心的部分则相对较正常。这种效果可以通过极坐标变换来实现，
// 将直角坐标系下的图像映射到极坐标系下，然后根据所需的鱼眼效果进行变换。
#include <BMPFile.h>
// 0.0062
void Fisheye(const std::vector<uint8_t> &imageData,std::promise<std::vector<uint8_t>>& result, int32_t width, int32_t height)
{
    // 首先确定图像的中心点坐标，以及最大半径
    int32_t centerX = width / 2;
    int32_t centerY = height / 2;
    int32_t maxRadius = std::min(centerX, centerY);

    std::vector<uint8_t> fisheyeImage(width * height * 3);
    // 遍历每个像素，计算像素到图像中心的距离，并检查是否小于最大半径，以确保像素位于鱼眼效果的范围内。
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int32_t offsetX = x - centerX;
            int32_t offsetY = y - centerY;
            float distance = std::sqrt(static_cast<float>(offsetX * offsetX + offsetY * offsetY));
            // 小于=应用鱼眼
            if (distance < maxRadius) {
                // 当前像素相对于图像中心点的极坐标角度
                float angle = std::atan2(static_cast<float>(offsetY), static_cast<float>(offsetX));
                // 用于计算每个像素的新半径
                float normalizedRadius = distance / maxRadius;
                // 进行非线性变换
                float mappedRadius = normalizedRadius * normalizedRadius;

                // 极坐标变换
                int32_t newX = static_cast<int32_t>(centerX + maxRadius * mappedRadius * std::cos(angle));
                int32_t newY = static_cast<int32_t>(centerY + maxRadius * mappedRadius * std::sin(angle));

                for (int channel = 0; channel < 3; ++channel) {
                    fisheyeImage[(y * width + x) * 3 + channel] = imageData[(newY * width + newX) * 3 + channel];
                }
            }
            else {
                // 超出范围保持原始颜色
                for (int channel = 0; channel < 3; ++channel) {
                    fisheyeImage[(y * width + x) * 3 + channel] = 0;
                }
            }
        }
    }

    result.set_value(fisheyeImage);
}

int main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    std::promise<std::vector<uint8_t>> promise;
    std::future<std::vector<uint8_t>> future=promise.get_future();
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread faceEyeThread(Fisheye,std::ref(imageData),std::ref(promise),std::ref(width),std::ref(height));
    std::vector<uint8_t> value=future.get();
    faceEyeThread.join();
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::SetBMPHeaderValues(myValue.bmp, myValue.bmpInfo, width, height, 24);
    MYFunction::WriteBMPFile("outputFisheye.bmp", value, myValue.bmp, myValue.bmpInfo);
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
