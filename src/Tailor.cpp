#include <BMPFile.h>
//  裁剪图像
//  图像信息
void WriteToBMPInfo(std::vector<uint8_t> &imageData, int32_t cropHeight, int32_t cropWidth, BMPInfo &newBmpInfo, BMP &bmp)
{
    newBmpInfo.SetHeight(cropHeight);
    newBmpInfo.SetWidth(cropWidth);
    newBmpInfo.SetImageSize(newBmpInfo.GetWidth() * newBmpInfo.GetHeight() * 3);  // 120000
    bmp.SetFileSize(bmp.GetDataOffset() + newBmpInfo.GetImageSize());
    imageData.resize(newBmpInfo.GetImageSize());
}
// 内容处理
void TailorImg(int32_t cropX,
               int32_t cropY,
               int32_t cropHeight,
               int32_t cropWidth,
               std::vector<uint8_t> &imageData,
               BMPInfo &newBmpInfo,
               BMP &bmp,
               uint32_t originWidth)
{
    WriteToBMPInfo(imageData, cropHeight, cropWidth, newBmpInfo, bmp);
    for (int32_t y = cropY; y < cropY + cropHeight; y++) {
        for (int32_t x = cropX; x < cropX + cropWidth; x++) {
            uint32_t originIndex = (y * originWidth + x) * 3;
            int32_t index = ((y - cropY) * cropWidth + (x - cropX)) * 3;
            imageData[index] = imageData[originIndex];          // Red
            imageData[index + 1] = imageData[originIndex + 1];  // Green
            imageData[index + 2] = imageData[originIndex + 2];  // Blue
        }
    }
}

int32_t main()
{
    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    int32_t cropX = 100;       // Begin X
    int32_t cropY = 100;       // Begin Y
    int32_t cropWidth = 100;   // Width
    int32_t cropHeight = 100;  // Height

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread tailorThread(TailorImg, std::ref(cropX), std::ref(cropY), std::ref(cropHeight), std::ref(cropWidth),
                             std::ref(imageData), std::ref(myValue.bmpInfo), std::ref(myValue.bmp), std::ref(width));
    tailorThread.join();
    auto afterTime = std::chrono::steady_clock::now();

    MYFunction::WriteBMPFile("outColorTailor.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
