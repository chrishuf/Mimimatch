#include "ColMatch.h"


// ===============================================================
// OpenCV operations
// ===============================================================


// Detect the chart in the image
bool ColMatch::detectChart(const cv::Mat& bgr_img, DetectedChart& out)
{
    if (bgr_img.empty())
    {
        wxMessageBox("No Image found");
        return false;
    }


    //For this to work we need to be in sRGB gamma 2.2
    //Create a new image just for detection
    cv::Mat detect_img;

    if (bgr_img.type() == CV_8UC3)
    {
        detect_img = bgr_img;
    }
    else if (bgr_img.type() == CV_32FC3 || bgr_img.type() == CV_64FC3)
    {
        cv::Mat tmp;
        cv::max(bgr_img, 0.0, tmp);
        cv::pow(tmp, 1.0 / 2.2, tmp);
        tmp *= 255.0;
        tmp.convertTo(detect_img, CV_8UC3);
    }
    else
    {
        wxMessageBox("Wrong image format");
        return false;
    }

    //Create an detector and run it
    auto detector = cv::mcc::CCheckerDetector::create();

    if (!detector->process(detect_img, cv::mcc::MCC24, 1))
    {
        wxMessageBox("Could not detect Chart");
        return false;
    }

    // Get the color checker and ouput the box and centers of the patches
    auto checker = detector->getBestColorChecker();
    auto box = checker->getBox();

    out.box[0] = box[0];
    out.box[1] = box[1];
    out.box[2] = box[2];
    out.box[3] = box[3];
    out.centers = checker->getColorCharts();

    return true;
}
//Get the colors of the checker using the center sizes
cv::Mat ColMatch::extractPatchColors(const cv::Mat& bgr_img,const std::vector<cv::Point2f>& centers,int radius)
{
    //no image no fun
    if (bgr_img.empty())
    {
       // wxMessageBox("No Image found");
        return cv::Mat();
    }

    //create the patches mat
    cv::Mat patches((int)centers.size(), 1, CV_64FC3);

    //take the pixel value of the each center and calculate the
    //bounding rectangle for calculating the mean color
    for (int i = 0; i < (int)centers.size(); ++i)
    {

        int cx = cvRound(centers[i].x);
        int cy = cvRound(centers[i].y);

        int x0 = std::max(0, cx - radius);
        int y0 = std::max(0, cy - radius);
        int x1 = std::min(bgr_img.cols, cx + radius + 1);
        int y1 = std::min(bgr_img.rows, cy + radius + 1);

        cv::Rect roi(x0, y0, x1 - x0, y1 - y0);
        if (roi.width <= 0 || roi.height <= 0)
        {
            patches.at<cv::Vec3d>(i, 0) = cv::Vec3d(0, 0, 0);
            continue;
        }

        cv::Scalar meanVal = cv::mean(bgr_img(roi));

        //if it is not a floating point (which it should be)
        // divide the value by 255.
        if (bgr_img.type() == CV_8UC3)
        {
            patches.at<cv::Vec3d>(i, 0) = cv::Vec3d(
                meanVal[2] / 255.0,
                meanVal[1] / 255.0,
                meanVal[0] / 255.0
            );
        }
        else
        {
            patches.at<cv::Vec3d>(i, 0) = cv::Vec3d(
                meanVal[2],
                meanVal[1],
                meanVal[0]
            );
        }
    }

    return patches;
}
// simple function which detects the chart and extracts the colors
//used for debugging
cv::Mat ColMatch::detectAndExtractPatches(const cv::Mat& bgr_img)
{
    DetectedChart chart;

    if (!detectChart(bgr_img, chart))
        return cv::Mat();
    return extractPatchColors(bgr_img, chart.centers, 3);
}

//Doing the selection of the CS here to keep all cv operations as much as possible
//inside this class
cv::ccm::COLOR_SPACE ColMatch::setColorSpace(int colspac)
{
    cv::ccm::COLOR_SPACE colorSpace;
    colorSpace = cv::ccm::COLOR_SPACE_sRGB;

    if (colspac == 0)colorSpace = cv::ccm::COLOR_SPACE_sRGB;
    if (colspac == 1)colorSpace = cv::ccm::COLOR_SPACE_sRGBL;
    if (colspac == 2)colorSpace = cv::ccm::COLOR_SPACE_AdobeRGB;
    if (colspac == 3)colorSpace = cv::ccm::COLOR_SPACE_AdobeRGBL;
    if (colspac == 4)colorSpace = cv::ccm::COLOR_SPACE_WideGamutRGB;
    if (colspac == 5)colorSpace = cv::ccm::COLOR_SPACE_WideGamutRGBL;
    if (colspac == 6)colorSpace = cv::ccm::COLOR_SPACE_ProPhotoRGB;
    if (colspac == 7)colorSpace = cv::ccm::COLOR_SPACE_ProPhotoRGBL;
    if (colspac == 8)colorSpace = cv::ccm::COLOR_SPACE_DCI_P3_RGB;
    if (colspac == 9)colorSpace = cv::ccm::COLOR_SPACE_DCI_P3_RGBL;
    if (colspac == 10)colorSpace = cv::ccm::COLOR_SPACE_AppleRGB;
    if (colspac == 11)colorSpace = cv::ccm::COLOR_SPACE_AppleRGBL;
    if (colspac == 12)colorSpace = cv::ccm::COLOR_SPACE_REC_709_RGB;
    if (colspac == 13)colorSpace = cv::ccm::COLOR_SPACE_REC_709_RGBL;
    if (colspac == 14)colorSpace = cv::ccm::COLOR_SPACE_REC_2020_RGB;
    if (colspac == 15)colorSpace = cv::ccm::COLOR_SPACE_REC_2020_RGBL;
    if (colspac == 16)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D65_2;
    if (colspac == 17)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D65_10;
    if (colspac == 18)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D50_2;
    if (colspac == 19)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D50_10;
    if (colspac == 20)colorSpace = cv::ccm::COLOR_SPACE_XYZ_A_2;
    if (colspac == 21)colorSpace = cv::ccm::COLOR_SPACE_XYZ_A_10;
    if (colspac == 22)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D55_2;
    if (colspac == 23)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D55_10;
    if (colspac == 24)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D75_2;
    if (colspac == 25)colorSpace = cv::ccm::COLOR_SPACE_XYZ_D75_10;
    if (colspac == 26)colorSpace = cv::ccm::COLOR_SPACE_XYZ_E_2;
    if (colspac == 27)colorSpace = cv::ccm::COLOR_SPACE_XYZ_E_10;
    if (colspac == 28)colorSpace = cv::ccm::COLOR_SPACE_Lab_D65_2;
    if (colspac == 29)colorSpace = cv::ccm::COLOR_SPACE_Lab_D65_10;
    if (colspac == 30)colorSpace = cv::ccm::COLOR_SPACE_Lab_D50_2;
    if (colspac == 31)colorSpace = cv::ccm::COLOR_SPACE_Lab_D50_10;
    if (colspac == 32)colorSpace = cv::ccm::COLOR_SPACE_Lab_A_2;
    if (colspac == 33)colorSpace = cv::ccm::COLOR_SPACE_Lab_A_10;
    if (colspac == 34)colorSpace = cv::ccm::COLOR_SPACE_Lab_D55_2;
    if (colspac == 35)colorSpace = cv::ccm::COLOR_SPACE_Lab_D55_10;
    if (colspac == 36)colorSpace = cv::ccm::COLOR_SPACE_Lab_D75_2;
    if (colspac == 37)colorSpace = cv::ccm::COLOR_SPACE_Lab_D75_10;
    if (colspac == 38)colorSpace = cv::ccm::COLOR_SPACE_Lab_E_2;
    if (colspac == 39)colorSpace = cv::ccm::COLOR_SPACE_Lab_E_10;

    return colorSpace;
}

//Calculate the Color Correction Matrix
bool ColMatch::calculateCCM(const cv::Mat& target_patches,const cv::Mat& ref_patches,cv::Mat& out_ccm,int colspac)
{

    //The Default Color Space
    cv::ccm::COLOR_SPACE colorSpace;
    colorSpace = setColorSpace(colspac);



    //Set the model
    cv::ccm::ColorCorrectionModel model(
        target_patches,
        ref_patches,
        colorSpace
    );

    //Set the paramters
    model.setCCM_TYPE(cv::ccm::CCM_3x3);
    //This is the color model used, I got the best results with DISTANCE_RGB
    model.setDistance(cv::ccm::DISTANCE_RGB);
    model.setLinear(cv::ccm::LINEARIZATION_GAMMA);
    model.setMaxCount(10000);
    model.setEpsilon(1e-7);

    model.run();
    m_loss = model.getLoss();
    out_ccm = model.getCCM();

    if (out_ccm.empty())
    {
        wxMessageBox("Could not generate the Color Correction Matrix");
        return false;
    }


    //Adjust the brightness to match the target patches


    cv::Mat ccm64;
    out_ccm.convertTo(ccm64, CV_64F);

    double yTarget = 0.0;
    double yRef = 0.0;

    for (int i = 0; i < target_patches.rows; ++i)
    {
        cv::Vec3d t = target_patches.at<cv::Vec3d>(i, 0);
        cv::Vec3d r = ref_patches.at<cv::Vec3d>(i, 0);

        cv::Vec3d tc;
        tc[0] = ccm64.at<double>(0, 0) * t[0] + ccm64.at<double>(0, 1) * t[1] + ccm64.at<double>(0, 2) * t[2];
        tc[1] = ccm64.at<double>(1, 0) * t[0] + ccm64.at<double>(1, 1) * t[1] + ccm64.at<double>(1, 2) * t[2];
        tc[2] = ccm64.at<double>(2, 0) * t[0] + ccm64.at<double>(2, 1) * t[1] + ccm64.at<double>(2, 2) * t[2];

        tc[0] = std::max(0.0, tc[0]);
        tc[1] = std::max(0.0, tc[1]);
        tc[2] = std::max(0.0, tc[2]);

        double yt = 0.2126 * tc[0] + 0.7152 * tc[1] + 0.0722 * tc[2];
        double yr = 0.2126 * r[0] + 0.7152 * r[1] + 0.0722 * r[2];

        yTarget += yt;
        yRef += yr;
    }

    if (yTarget <= 1e-12)
        return false;

    double out_gain = yRef / yTarget;

    ccm64 *= out_gain;
    ccm64.convertTo(out_ccm, out_ccm.type());

    return true;
}


//Normalisation functions, not used keeping it, becasue it might become handy
bool ColMatch::normalizeCCMRowsToOne(cv::Mat& ccm)
{
    if (ccm.empty() || ccm.rows != 3 || ccm.cols != 3)
        return false;

    cv::Mat ccm64;
    ccm.convertTo(ccm64, CV_64F);

    for (int r = 0; r < 3; ++r)
    {
        double s =
            ccm64.at<double>(r, 0) +
            ccm64.at<double>(r, 1) +
            ccm64.at<double>(r, 2);

        if (std::abs(s) < 1e-12)
            return false;

        ccm64.at<double>(r, 0) /= s;
        ccm64.at<double>(r, 1) /= s;
        ccm64.at<double>(r, 2) /= s;
    }

    ccm64.convertTo(ccm, ccm.type());
    return true;
}

//Normalisation functions, not used keeping it, becasue it might become handy
bool ColMatch::normalizeCCMColsToOne(cv::Mat& ccm)
{
    if (ccm.empty() || ccm.rows != 3 || ccm.cols != 3)
        return false;

    cv::Mat ccm64;
    ccm.convertTo(ccm64, CV_64F);

    for (int c = 0; c < 3; ++c)
    {
        double sum =
            ccm64.at<double>(0, c) +
            ccm64.at<double>(1, c) +
            ccm64.at<double>(2, c);

        if (std::abs(sum) < 1e-12)
            return false;

        ccm64.at<double>(0, c) /= sum;
        ccm64.at<double>(1, c) /= sum;
        ccm64.at<double>(2, c) /= sum;
    }

    ccm64.convertTo(ccm, ccm.type());
    return true;
}

//Helper function to fill the Color pick postions, when the box changes
bool ColMatch::fillChartPointsFromBox(DetectedChart& chart)
{
    const std::vector<cv::Point2f> srcChartCorners =
    {
        cv::Point2f(0.0f, 0.0f), // TL
        cv::Point2f(6.0f, 0.0f), // TR
        cv::Point2f(6.0f, 4.0f), // BR
        cv::Point2f(0.0f, 4.0f)  // BL
    };

    const std::vector<cv::Point2f> dstImageCorners =
    {
        chart.box[0],
        chart.box[1],
        chart.box[2],
        chart.box[3]
    };

    cv::Mat H = cv::getPerspectiveTransform(srcChartCorners, dstImageCorners);
    if (H.empty())
        return false;

    std::vector<cv::Point2f> idealCenters;
    idealCenters.reserve(24);

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 6; ++col)
        {
            idealCenters.emplace_back(col + 0.5f, row + 0.5f);
        }
    }

    cv::perspectiveTransform(idealCenters, chart.centers, H);
    return chart.centers.size() == 24;
}



// Apply the ccm to an image
cv::Mat ColMatch::applyCCM(const cv::Mat& bgr_img, const cv::Mat& ccm) const
{
    cv::Mat rgb;
    cv::cvtColor(bgr_img, rgb, cv::COLOR_BGR2RGB);
    rgb.convertTo(rgb, CV_64F);
    rgb /= 255.0;

    cv::Mat pixels = rgb.reshape(1, rgb.rows * rgb.cols);
    cv::Mat corrected = pixels * ccm;
    corrected = corrected.reshape(3, rgb.rows);

    cv::min(corrected, 1.0, corrected);
    cv::max(corrected, 0.0, corrected);
    corrected *= 255.0;
    corrected.convertTo(corrected, CV_8UC3);
    cv::cvtColor(corrected, corrected, cv::COLOR_RGB2BGR);

    return corrected;
}

// Get the inverse of the CCM
cv::Mat ColMatch::invertCCM(const cv::Mat& ccm) const
{
    cv::Mat ccm_inv;
    cv::invert(ccm, ccm_inv, cv::DECOMP_SVD);

    return ccm_inv;
}

//Sets the working color space
void ColMatch::setColorSpace(cv::ccm::COLOR_SPACE cs)
{
    m_colorSpace = cs;
}

//Gets the working color space
cv::ccm::COLOR_SPACE ColMatch::getColorSpace() const
{
    return m_colorSpace;
}

//Reorder the patches to fit the datacolor checker.
cv::Mat ColMatch::reorderPatchesDatacolor(const cv::Mat& patches) const
{
    // Datacolor SpyderCheckr 24 mirrors columns left<->right within each row
    // Standard layout: 6 columns x 4 rows
    cv::Mat reordered = patches.clone();
    const int rows = 4, cols = 6;

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            reordered.at<cv::Vec3d>(r * cols + c, 0) =
            patches.at<cv::Vec3d>(r * cols + (cols - 1 - c), 0);

    return reordered;
}

//apply the ccm to the patches, not in use yet, might become in future
bool ColMatch::applyCCMToPatches(const cv::Mat& inPatches,
    const cv::Mat& ccm,
    cv::Mat& outPatches) 
{
    if (inPatches.empty() || ccm.empty())
        return false;

    if (ccm.rows != 3 || ccm.cols != 3)
        return false;

    cv::Mat src64, ccm64;
    inPatches.convertTo(src64, CV_64F);
    ccm.convertTo(ccm64, CV_64F);

    cv::Mat patches2D;
    const int origRows = src64.rows;
    const int origCols = src64.cols;
    const int origCh = src64.channels();

    if (origCh == 3)
    {
        patches2D = src64.reshape(1, origRows * origCols);
    }
    else if (origCh == 1 && origCols == 3)
    {
        // Example: 24x3 CV_64F
        patches2D = src64;
    }
    else
    {
        return false;
    }

    cv::Mat corrected2D = patches2D * ccm64;

    if (origCh == 3)
    {
        outPatches = corrected2D.reshape(3, origRows).clone();
    }
    else
    {
        outPatches = corrected2D.clone();
    }

    return true;
}

//calculate the saturation not used yet, but returns a color corrected patches
//(Maybe usefull in future)
bool ColMatch::calculateCCMAndSaturation(const cv::Mat& targetPatches,
    const cv::Mat& refPatches,
    cv::Mat& outCCM,
    double& outSatScale,
    cv::Mat* outSatMatchedPatches,
    int colspac)
{

    setColorSpace(setColorSpace(colspac));
    

    if (!calculateCCM(targetPatches, refPatches, outCCM, colspac))
    {

        return false;
    }

    cv::Mat correctedPatches;
    if (!applyCCMToPatches(targetPatches, outCCM, correctedPatches))
    {

        return false;
    }

    cv::Mat satMatched;
    cv::Mat* perPatchSatDiff;
  
    if (!calcPatchSaturationScaleFirst3Rows(correctedPatches, refPatches, outSatScale))
    {

        return false;
    }
    if (outSatMatchedPatches)
        *outSatMatchedPatches = satMatched.clone();

    return true;
}


//calculate the saturation factor from the checkers
bool ColMatch::calcPatchSaturationScaleFirst3Rows(const cv::Mat& patchesIn,
    const cv::Mat& patchesOut,
    double& satScale) 
{
    satScale = 1.0;
    //check if patches are valid
    if (patchesIn.empty() || patchesOut.empty())
        return false;


    //convert to 64bit float
    cv::Mat in64, out64;
    patchesIn.convertTo(in64, CV_64F);
    patchesOut.convertTo(out64, CV_64F);

    cv::Mat in2D, out2D;
    int patchCount = 0;
    //check if channel amount is valid, and set the amount of patches
    if (in64.channels() == 3 && out64.channels() == 3)
    {
        if (in64.rows != out64.rows || in64.cols != out64.cols)
            return false;

        in2D = in64.reshape(1, in64.rows * in64.cols);
        out2D = out64.reshape(1, out64.rows * out64.cols);

        patchCount = std::min(in64.rows, 3) * in64.cols;
    }
    else if (in64.channels() == 1 && out64.channels() == 1 &&
        in64.cols == 3 && out64.cols == 3)
    {
        if (in64.rows != out64.rows)
            return false;

        in2D = in64;
        out2D = out64;

        patchCount = std::min(in64.rows, 18); // first 3 rows of 6x4 checker
    }
    else
    {
        return false;
    }

    if (patchCount <= 0)
        return false;

    double num = 0.0;
    double den = 0.0;


    //calculate the saturation for each patch
    for (int i = 0; i < patchCount; ++i)
    {
        cv::Vec3d rgbIn(in2D.at<double>(i, 0),
            in2D.at<double>(i, 1),
            in2D.at<double>(i, 2));

        cv::Vec3d rgbOut(out2D.at<double>(i, 0),
            out2D.at<double>(i, 1),
            out2D.at<double>(i, 2));

        cv::Mat inPix(1, 1, CV_32FC3);
        cv::Mat outPix(1, 1, CV_32FC3);

        inPix.at<cv::Vec3f>(0, 0) = cv::Vec3f(
            static_cast<float>(rgbIn[0]),
            static_cast<float>(rgbIn[1]),
            static_cast<float>(rgbIn[2])
        );

        outPix.at<cv::Vec3f>(0, 0) = cv::Vec3f(
            static_cast<float>(rgbOut[0]),
            static_cast<float>(rgbOut[1]),
            static_cast<float>(rgbOut[2])
        );

        //convert the rgb values to hsv
        cv::Mat inHls, outHls;
        cv::cvtColor(inPix, inHls, cv::COLOR_RGB2HLS);
        cv::cvtColor(outPix, outHls, cv::COLOR_RGB2HLS);

        const double sIn = static_cast<double>(inHls.at<cv::Vec3f>(0, 0)[2]);
        const double sOut = static_cast<double>(outHls.at<cv::Vec3f>(0, 0)[2]);


        num += sIn * sOut;
        den += sIn * sIn;
    }
    //calculate the colors.
    satScale = (den > 1e-12) ? (num / den) : 1.0;
    return true;
}
