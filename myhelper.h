#pragma once

#include <FreeImage.h>
#include <wx/image.h>
#include <opencv2/core.hpp>


struct DetectedChart
{
    std::array<cv::Point2f, 4> box;   // TL, TR, BR, BL
    std::vector<cv::Point2f> centers; // 24 patch centers
};

typedef struct
{
    int x1, x2, x3, x4;
    int y1, y2, y3, y4;

}ccheck;


FIBITMAP* ConvertToRGBF(FIBITMAP* src);

cv::Mat freeImageToMat(FIBITMAP* bmp);

FIBITMAP* matToFreeImage(const cv::Mat& mat);

FIBITMAP* ConvertToFloatLinearManual(
    FIBITMAP* src,
    bool assumeSRGBFor8Bit = true,
    bool applyGammaTo16Bit = false,
    float gamma16 = 2.2f
);

wxImage FI_ConverttowxImage(FIBITMAP* mib);
double mclamp(double x, double a, double b);
