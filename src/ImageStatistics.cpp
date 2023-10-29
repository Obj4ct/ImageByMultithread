//
// Created by ZThenG on 2023/9/16/0016.
//
// 图像数据统计
#include <BMPFile.h>

void CalSquareSum(const std::vector<uint8_t>& imageData)
{
    uint32_t sum = 0;
    for (const auto& pixVal : imageData) {
        sum += pixVal * pixVal;
    }
    std::cout << "平方和是:" << sum << std::endl;
}

void CalVariance(const std::vector<uint8_t>& imageData)
{
    float_t sum = 0.0;
    float_t variance = 0.0;

    for (const auto& pixelValue : imageData) {
        sum += pixelValue;
    }

    float_t aver = sum / imageData.size();

    for (const auto& pixelValue : imageData) {
        float_t diff = pixelValue - aver;
        variance += diff * diff;
    }

    variance /= imageData.size();

    std::cout << "方差:" << variance << std::endl;
}


void CalStandard(const std::vector<uint8_t>& imageData)
{
    uint32_t sum = 0, tempSum = 0, variance;
    
    for (const auto pixelValue : imageData) {
        sum += pixelValue;
    }
    
    uint32_t aver = sum / imageData.size();
    
    for (const auto pixelValue : imageData) {
        tempSum += (pixelValue - aver) * (pixelValue - aver);
    }
    
    variance = tempSum / imageData.size();
    
    float_t standard = std::sqrt(variance);
    std::cout << "标准差:" << standard << std::endl;
}

void SaveHistogram(const std::vector<uint8_t> &imageData, std::string fileName)
{
    std::vector<int> histogram(256); // 255应该改成256以包含像素值为255的情况

    for (const auto pixel : imageData) {
        histogram[pixel]++;
    }

    std::ofstream outputFile("../../test/OutImages/" + fileName);
    if (!outputFile.is_open()) {
        std::cout << "无法创建直方图文件" << std::endl;
        return;
    }

    for (int i = 0; i < histogram.size(); ++i) {
        outputFile << "直方图数据" << i << ": " << histogram[i] << std::endl;
    }

    outputFile.close();
    std::cout << "直方图数据保存到" << fileName << std::endl;
}


int main()
{
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;
    std::string fileName="Histogram.txt";
    // 在这里运行你的程序
    // 2782078788
    auto beforeTime = std::chrono::steady_clock::now();
    std::thread calSquareSum(CalSquareSum, std::ref(imageData));

    // 在这里运行你的程序
    // 3479
    std::thread calVariance(CalVariance, std::ref(imageData));

    // 在这里运行你的程序
    // 58.983
    std::thread calStandard(CalStandard, std::ref(imageData));

    // 在这里运行你的程序
    std::thread saveHistogram(SaveHistogram, std::ref(imageData),std::ref(fileName));


    calSquareSum.join();
    calVariance.join();
    calStandard.join();
    saveHistogram.join();

    auto afterTime = std::chrono::steady_clock::now();
    float_t duration_second = std::chrono::duration<float_t>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
