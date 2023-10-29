//
// Created by ZThenG on 2023/9/24/0024.
//- L*代表明度，取值0~100
//
//- a*代表从绿色到红色的分量 ，取值-128~127
//
//- b*代表从蓝色到黄色的分量，取值-128~127

#include <BMPFile.h>
// 0.0399
void RGB2LAB(std::vector<uint8_t> &imageData, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int index = i * 3;

        uint8_t r = imageData[index];
        uint8_t g = imageData[index + 1];
        uint8_t b = imageData[index + 2];
        // 归一化
        double R = r / 255.0;
        double G = g / 255.0;
        double B = b / 255.0;

        double X = 0.4124564 * R + 0.3575761 * G + 0.1804375 * B;
        double Y = 0.2126729 * R + 0.7151522 * G + 0.0721750 * B;
        double Z = 0.0193339 * R + 0.1191920 * G + 0.9503041 * B;

        X = X / 0.95047;
        Z = Z / 1.08883;

        // 将X和Z通道的值进行缩放，以将它们从CIEXYZ颜色空间的D65白点参考值（通常是1.0）映射到不同的白点参考值上
        double epsilon = 0.008856;
        double kappa = 903.3;
        double Xr = 0.9642;
        double Yr = 1.0;
        double Zr = 0.8249;

        double fx = (X > epsilon) ? pow(X, 1.0 / 3.0) : ((kappa * X + 16.0) / 116.0);
        double fy = (Y > epsilon) ? pow(Y, 1.0 / 3.0) : ((kappa * Y + 16.0) / 116.0);
        double fz = (Z > epsilon) ? pow(Z, 1.0 / 3.0) : ((kappa * Z + 16.0) / 116.0);

        double L = (116.0 * fy) - 16.0;
        double a = (fx - fy) * 500.0;
        double bValue = (fy - fz) * 200.0;

        L = std::max(0.0, std::min(255.0, L));
        a = std::max(0.0, std::min(255.0, a + 128.0));
        bValue = std::max(0.0, std::min(255.0, b + 128.0));

        imageData[index] = static_cast<uint8_t>(L);
        imageData[index + 1] = static_cast<uint8_t>(a);
        imageData[index + 2] = static_cast<uint8_t>(bValue);
    }
}

void LAB2RGB(std::vector<uint8_t> &imageData, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int index = i * 3;
        // 128中心编码
        double L = imageData[index];
        double a = imageData[index + 1] - 128.0;
        double b = imageData[index + 2] - 128.0;
        // 防止分母为零的情况
        double epsilon = 0.008856;
        double kappa = 903.3;

        // 表示标准观察条件下的白色参考
        double Xr = 0.9642;
        double Yr = 1.0;
        double Zr = 0.8249;
        // Y通道的标准化值
        double fy = (L + 16.0) / 116.0;
        // X和Z通道的标准化值
        double fx = a / 500.0 + fy;
        double fz = fy - b / 200.0;

        double X = (fx > epsilon) ? pow(fx, 3.0) : ((116.0 * fx - 16.0) / kappa);
        double Y = (fy > epsilon) ? pow(fy, 3.0) : ((116.0 * fy - 16.0) / kappa);
        double Z = (fz > epsilon) ? pow(fz, 3.0) : ((116.0 * fz - 16.0) / kappa);

        // 对X和Z通道的值进行归一化，
        // 将它们从CIEXYZ颜色空间的某个白点参考值（通常是D65白点）映射到标准观察条件下的白点参考值。
        // 这是颜色空间转换的一部分，以确保颜色表示的一致性。
        X = X * Xr;
        Z = Z * Zr;

        double R = 3.2404542 * X - 1.5371385 * Y - 0.4985314 * Z;
        double G = -0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z;
        double B = 0.0556434 * X - 0.2040259 * Y + 1.0572252 * Z;

        R = std::max(0.0, std::min(1.0, R));
        G = std::max(0.0, std::min(1.0, G));
        B = std::max(0.0, std::min(1.0, B));

        imageData[index] = static_cast<uint8_t>(R * 255.0);
        imageData[index + 1] = static_cast<uint8_t>(G * 255.0);
        imageData[index + 2] = static_cast<uint8_t>(B * 255.0);
    }
}

int main()
{

    // 在这里运行你的程序
    MyValue myValue = MYFunction::ReadBMPFile(FILENAME);
    uint32_t height = myValue.bmpInfo.GetHeight();
    uint32_t width = myValue.bmpInfo.GetWidth();
    std::vector<uint8_t> imageData = myValue.imageData;

    auto beforeTime = std::chrono::steady_clock::now();
    std::thread labThread([&]() {

        RGB2LAB(imageData, width, height);

        MYFunction::WriteBMPFile("outputRGBtoLAB.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    });

    labThread.join();
    std::thread rgbThread([&]() {
        LAB2RGB(imageData, width, height);
        MYFunction::WriteBMPFile("outputLABtoRGB.bmp", imageData, myValue.bmp, myValue.bmpInfo);
    });

    rgbThread.join();
    auto afterTime = std::chrono::steady_clock::now();
    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;
}
