#pragma once
#include "minimal.h"





class ColorMatcher
{
public:
    // ---------------------------------------------------------------
    // Main operations
    // ---------------------------------------------------------------

    // Detect Macbeth chart patches in an image (returns empty Mat on failure)
    cv::Mat detectAndExtractPatches(const cv::Mat& bgr_img,
        const std::string& label) const;

    // Fit CCM from target patches to reference patches, apply to full image.
    // Automatically saves: ccm.txt, ccm_inverse.txt,
    //                      ccm_ocio.txt, ccm_ocio_aces.txt,
    //                      ccm_fusion.txt, ccm_match.dctl,
    //                      ccm_forward.cube, ccm_inverse.cube
    // Also copies the Fusion ColorMatrix node to the clipboard.
    cv::Mat matchToReference(const cv::Mat& target_bgr,
        const cv::Mat& target_patches,
        const cv::Mat& ref_patches,
        const std::string& ccm_out_path,
        cv::Mat& out_ccm);

    // Apply any raw 3x3 CCM matrix directly to an image
    cv::Mat applyCCM(const cv::Mat& bgr_img, const cv::Mat& ccm) const;

    // Invert a CCM and save it to a text file
    cv::Mat invertCCM(const cv::Mat& ccm, const std::string& filepath) const;

    // Reorder patches for Datacolor SpyderCheckr (mirrored column layout)
    cv::Mat reorderPatchesDatacolor(const cv::Mat& patches) const;

    // Debug: visualize detected patch order and print patch 0 / 23 colors
    void debugPatchOrder(const cv::Mat& bgr_img, const std::string& label) const;

    // ---------------------------------------------------------------
    // Export functions (also called automatically by matchToReference)
    // ---------------------------------------------------------------

    void saveCCMToFile(const cv::Mat& ccm, const std::string& filepath) const;
    void saveCCMAsOCIO(const cv::Mat& ccm, const std::string& filepath) const;
    void saveCCMAsOCIO_ACES(const cv::Mat& ccm, const std::string& filepath) const;
    void saveCCMAsFusionText(const cv::Mat& ccm, const std::string& filepath) const;
    void saveCCMAsDCTL(const cv::Mat& ccm, const cv::Mat& ccm_inv,
        const std::string& filepath) const;
    void saveCCMAsCubeLUT(const cv::Mat& ccm, const std::string& filepath,
        int lut_size = 33) const;

    // ---------------------------------------------------------------
    // Clipboard
    // ---------------------------------------------------------------

    // Copy Fusion ColorMatrix node to system clipboard
    void copyFusionColorMatrixToClipboard(const cv::Mat& ccm,
        const cv::Mat& ccm_inv,
        bool use_inverse = false) const;

    static void copyToClipboard(const std::string& text);

    // ---------------------------------------------------------------
    // Getters
    // ---------------------------------------------------------------

    double   getLoss() const { return m_loss; }
    cv::Mat  getCCM()  const { return m_ccm; }

private:
    cv::Mat  m_ccm;
    double   m_loss = 0.0;
};
