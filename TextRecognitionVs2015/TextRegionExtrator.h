#ifndef TEXTREGIONEXTRATOR_H
#define TEXTREGIONEXTRATOR_H

#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>

#include <opencv2/core/core.hpp>
#include <vector>
#include <string>


/// This class created for Text Region extraction purpose
class TextRegionExtractor {

public:
    typedef std::pair <cv::Rect ,std::string> RectTextPair;
    typedef std::vector <RectTextPair>  RectTextInfo;

public:
    TextRegionExtractor(const std::string& = ".", const std::string& = "eng");
	
public:
    bool setFrame (const cv::Mat&);
    std::vector<std::pair<cv::Rect, std::string>> run ( const TextRegionExtractor&, const tesseract::PageIteratorLevel);
	std::vector<std::pair<cv::Rect, std::string>> TesseractExecution(const TextRegionExtractor&, const tesseract::PageIteratorLevel);

private:
	int m_freamcount = 0;
	cv::Mat m_image;
	std::string m_language;
	const char* filename;
	char* text;
	tesseract::TessBaseAPI tess;
};

#endif // TEXTREGIONEXTRATOR_H
