//
// Created by ztheng on 2023/9/5.
// done
// 色阶
// 色阶调整分输入色阶调整和输出色阶调整，其中输入色阶调整有3个调整点，即通常所说的黑场、白场及灰场调整
#include <BMPFile.h>

void ColorLevelChanel_R(std::vector<uint8_t> &rImageData, int32_t width, int32_t height, double_t brightness, double_t contrast)
{
    for (int i = 0; i < rImageData.size(); i += 3) {
        uint8_t r = rImageData[i];

        // brightness
        r = std::min(255, std::max(0, static_cast<int>(r + brightness)));
        // contrast
        // 128：midGray
        r = std::min(255, std::max(0, static_cast<int>((r - 128) * contrast + 128)));
        rImageData[i] = r;
    }
}

void ColorLevelChanel_G(std::vector<uint8_t> &gImageData, int32_t width, int32_t height, double_t brightness, double_t contrast)
{
    for (int i = 0; i < gImageData.size(); i += 3) {
        uint8_t G = gImageData[i + 1];

        // brightness
        G = std::min(255, std::max(0, static_cast<int>(G + brightness)));
        // contrast
        // 128：midGray
        G = std::min(255, std::max(0, static_cast<int>((G - 128) * contrast + 128)));
        gImageData[i + 1] = G;
    }
}

void ColorLevelChanel_B(std::vector<uint8_t> &bImageData, int32_t width, int32_t height, double_t brightness, double_t contrast)
{
    for (int i = 0; i < bImageData.size(); i += 3) {
        uint8_t B = bImageData[i + 2];

        // brightness
        B = std::min(255, std::max(0, static_cast<int>(B + brightness)));
        // contrast
        // 128：midGray
        B = std::min(255, std::max(0, static_cast<int>((B - 128) * contrast + 128)));
        bImageData[i + 2] = B;
    }
}

void ColorLevelChanel_RGB(std::vector<uint8_t> &rgbImageData,
                          int32_t width,
                          int32_t height,
                          double_t brightness,
                          double_t contrast)
{
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int index = (i * width + j) * 3;
            uint8_t r = rgbImageData[index];
            uint8_t g = rgbImageData[index + 1];
            uint8_t b = rgbImageData[index + 2];
            // brightness
            r = std::min(255, std::max(0, static_cast<int>(r + brightness)));
            g = std::min(255, std::max(0, static_cast<int>(g + brightness)));
            b = std::min(255, std::max(0, static_cast<int>(b + brightness)));
            // contrast
            r = std::min(255, std::max(0, static_cast<int>((r - 128) * contrast + 128)));
            g = std::min(255, std::max(0, static_cast<int>((g - 128) * contrast + 128)));
            b = std::min(255, std::max(0, static_cast<int>((b - 128) * contrast + 128)));

            // update
            rgbImageData[index] = r;
            rgbImageData[index + 1] = g;
            rgbImageData[index + 2] = b;
        }
    }
}

int main()
{
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    int32_t height = myValue.bmpInfo.GetHeight();
    int32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;
    // CreateNewBmp();
    std::vector<uint8_t> rImageData(imageData.size());
    rImageData = imageData;
    std::vector<uint8_t> gImageData(imageData.size());
    gImageData = imageData;
    std::vector<uint8_t> bImageData(imageData.size());
    bImageData = imageData;
    std::vector<uint8_t> rgbImageData(imageData.size());
    rgbImageData = imageData;
    double_t brightness = 100, contrast = 100;
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread rThread(ColorLevelChanel_R, std::ref(rImageData), std::ref(width), std::ref(height), std::ref(brightness),
                        std::ref(contrast));
    std::thread gThread(ColorLevelChanel_G, std::ref(gImageData), std::ref(width), std::ref(height), std::ref(brightness),
                        std::ref(contrast));
    std::thread bThread(ColorLevelChanel_B, std::ref(bImageData), std::ref(width), std::ref(height), std::ref(brightness),
                        std::ref(contrast));
    std::thread rgbThread(ColorLevelChanel_RGB, std::ref(rgbImageData), std::ref(width), std::ref(height), std::ref(brightness),
                          std::ref(contrast));

    rThread.join();
    gThread.join();
    bThread.join();
    rgbThread.join();
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("changeColorLever_R.bmp", rImageData, myValue.bmp, myValue.bmpInfo);
    MYFunction::WriteBMPFile("changeColorLever_G.bmp", gImageData, myValue.bmp, myValue.bmpInfo);
    MYFunction::WriteBMPFile("changeColorLever_B.bmp", bImageData, myValue.bmp, myValue.bmpInfo);
    MYFunction::WriteBMPFile("changeColorLever_RGB.bmp", rgbImageData, myValue.bmp, myValue.bmpInfo);


    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
