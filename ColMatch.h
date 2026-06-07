#pragma once
#include "wx/wx.h"
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/mcc.hpp>
#include <opencv2/mcc/ccm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "myhelper.h"

//Main class for color correction, keeping most of the opencv functions together


class ColMatch
{
public:



    bool detectChart(const cv::Mat& bgr_img, DetectedChart& out);

    cv::Mat extractPatchColors(const cv::Mat& bgr_img,const std::vector<cv::Point2f>& centers, int radius = 3);

    cv::Mat detectAndExtractPatches(const cv::Mat& bgr_img);

    bool fillChartPointsFromBox(DetectedChart& chart);

    bool calculateCCM(const cv::Mat& target_patches, const cv::Mat& ref_patches, cv::Mat& out_ccm, int colspac);
    cv::ccm::COLOR_SPACE setColorSpace(int colspac);

    cv::Mat applyCCM(const cv::Mat& bgr_img, const cv::Mat& ccm) const;
    cv::Mat invertCCM(const cv::Mat& ccm) const;
    bool normalizeCCMRowsToOne(cv::Mat& ccm);
    bool normalizeCCMColsToOne(cv::Mat& ccm);
    cv::Mat reorderPatchesDatacolor(const cv::Mat& patches) const;

    double getLoss() const { return m_loss; }
    cv::Mat getCCM() const { return m_ccm; }


    void setColorSpace(cv::ccm::COLOR_SPACE cs);

    cv::ccm::COLOR_SPACE getColorSpace() const;

    bool applyCCMToPatches(const cv::Mat& inPatches,
        const cv::Mat& ccm,
        cv::Mat& outPatches);



    bool calculateCCMAndSaturation(const cv::Mat& targetPatches,
        const cv::Mat& refPatches,
        cv::Mat& outCCM,
        double& outSatScale,
        cv::Mat* outSatMatchedPatches,
        int colspac);


    bool calcPatchSaturationScaleFirst3Rows(const cv::Mat& patchesIn,const cv::Mat& patchesOut, double& satScale);


    cv::ccm::COLOR_SPACE m_colorSpace;



private:
    cv::Mat m_ccm;
    double m_loss = 0.0;
};
