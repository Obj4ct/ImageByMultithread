
#include <BMPFile.h>
// done
enum BlendMode {
    BLEND_NORMAL,
    BLEND_MULTIPLY,
    BLEND_SCREEN,
    BLEND_OVERLAY
};

void SwitchBlendMode(uint8_t &destR,
                     uint8_t &destG,
                     uint8_t &destB,
                     uint8_t srcR,
                     uint8_t srcG,
                     uint8_t srcB,
                     BlendMode blendMode,
                     double alpha)
{
    switch (blendMode) {
        // 正常模式 在“正常”模式下，“混合色”的显示与不透明度的设置有关。
        // 线性插值
        case BLEND_NORMAL: {
            destR = static_cast<uint8_t>((1.0 - alpha) * destR + alpha * srcR);
            destG = static_cast<uint8_t>((1.0 - alpha) * destG + alpha * srcG);
            destB = static_cast<uint8_t>((1.0 - alpha) * destB + alpha * srcB);
            break;
        }
            // 正片叠底 其主要特点是以正片作为底图，再将其他图像叠加于其上，并根据叠加图像的颜色来调整底图的 亮度 和
            // 饱和度，产生丰富的合成效果。
            // 这种混合模式的效果是将源像素的颜色应用于目标像素，具有"遮罩"效果，通常用于创建阴影或混合颜色的效果。
            //   相乘:这种混合模式的核心概念是模拟颜色的遮罩效应或阴影效果。这种效果是通过将源像素的颜色与目标像素的颜色相乘来实现的。
            //  /255 需要保证像素的范围
        case BLEND_MULTIPLY: {
            destR = static_cast<uint8_t>((destR * srcR) / 255);
            destG = static_cast<uint8_t>((destG * srcG) / 255);
            destB = static_cast<uint8_t>((destB * srcB) / 255);
            break;
        }
            // 滤色 ps中滤色的作用结果和正片叠底刚好相反，它是将 两个颜色的互补色
            // 的像素值相乘，然后除以255得到的最终色的像素值。通常执行滤色模式后的颜色都较浅。
        case BLEND_SCREEN: {
            destR = static_cast<uint8_t>((255 - destR) * (255 - srcR) / 255);
            destG = static_cast<uint8_t>((255 - destG) * (255 - srcG) / 255);
            destB = static_cast<uint8_t>((255 - destB) * (255 - srcB) / 255);
            break;
        }
        // “叠加”模式把图像的“基色”颜色与“混合色”颜色相混合产生一种中间色。
        // 根据目标像素的亮度选择源像素的亮度值。如果目标像素较暗（R、G、B < 128）,则使用 2 * dest * src / 255 计算；
        //  如果目标像素较亮（R、G、B >= 128），则使用 255 - (2 * (255 - dest) * (255 - src)) / 255 计算
        case BLEND_OVERLAY: {
            if (destR < 128) {
                destR = static_cast<uint8_t>((2 * destR * srcR) / 255);
            }
            else {
                destR = static_cast<uint8_t>(255 - (2 * (255 - destR) * (255 - srcR)) / 255);
            }

            if (destG < 128) {
                destG = static_cast<uint8_t>((2 * destG * srcG) / 255);
            }
            else {
                destG = static_cast<uint8_t>(255 - (2 * (255 - destG) * (255 - srcG)) / 255);
            }

            if (destB < 128) {
                destB = static_cast<uint8_t>((2 * destB * srcB) / 255);
            }
            else {
                destB = static_cast<uint8_t>(255 - (2 * (255 - destB) * (255 - srcB)) / 255);
            }
            break;
        }

        default:
            break;
    }
}

void EffectSegment(std::vector<uint8_t> &imageData,
                   const std::vector<uint8_t> &effectData,
                   int width,
                   int height,
                   BlendMode blendMode,
                   double alpha,
                   int start,
                   int end)
{
    for (int i = start; i < end; i++) {
        uint8_t &destR = imageData[i * 3];
        uint8_t &destG = imageData[i * 3 + 1];
        uint8_t &destB = imageData[i * 3 + 2];
        uint8_t srcR = effectData[i * 3];
        uint8_t srcG = effectData[i * 3 + 1];
        uint8_t srcB = effectData[i * 3 + 2];
        SwitchBlendMode(destR, destG, destB, srcR, srcG, srcB, blendMode, alpha);
    }
}

void ApplyEffectParallel(std::vector<uint8_t> &imageData,
                         const std::vector<uint8_t> &effectData,
                         int width,
                         int height,
                         BlendMode blendMode,
                         double alpha)
{
    int num_threads = 4;  // 获取CPU核心数
    std::vector<std::thread> threads(num_threads);
    int segmentSize = static_cast<int>(imageData.size() / 3 / num_threads);

    for (int i = 0; i < num_threads; i++) {
        int start = i * segmentSize;
        int end = (i == num_threads - 1) ? imageData.size() / 3 : (i + 1) * segmentSize;
        threads[i] =
            std::thread(EffectSegment, std::ref(imageData), std::cref(effectData), width, height, blendMode, alpha, start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }
}

int main()
{

    MyValue originalImage = MYFunction::ReadBMPFile(FILENAME);
    MyValue effectImage = MYFunction::ReadBMPFile(BACKFILE);

    uint32_t width = originalImage.bmpInfo.GetWidth();
    uint32_t height = originalImage.bmpInfo.GetHeight();
    std::vector<uint8_t> originalImageData = originalImage.imageData;
    std::vector<uint8_t> effectImageData = effectImage.imageData;
    // 选择混合模式和透明度
    BlendMode blendMode = BLEND_NORMAL;
    double alpha = 0.5;
    auto beforeTime = std::chrono::steady_clock::now();
    ApplyEffectParallel(originalImageData, effectImageData, width, height, blendMode, alpha);
    auto afterTime = std::chrono::steady_clock::now();
    MYFunction::WriteBMPFile("outputBlendedImage.bmp", originalImageData, originalImage.bmp, originalImage.bmpInfo);


    double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
    float_t duration_milliseconds = duration_second * 1000;
    std::cout << duration_milliseconds << "毫秒" << std::endl;

    return 0;
}
