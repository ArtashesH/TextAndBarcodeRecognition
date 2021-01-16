#include "TextRegionExtrator.h"
#include "NaturalBinarization.h"

#include <iostream>
#include <ostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <QDebug>
#include <fstream>
#include <stdio.h>

TextRegionExtractor::TextRegionExtractor(const std::string& dataPath, const std::string& lang)
	{
		m_language = lang;
		tess.Init(dataPath.c_str(), m_language.c_str(), tesseract::OEM_DEFAULT);
		tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
		//tess.SetVariable("tessedit_char_blacklist", "`~#$%^&_\|:;{}[]'‘’”");
		//tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz<>=/*+-0123456789#,&:·;“‘’”_.?!@()%");
	}

bool TextRegionExtractor::setFrame(const cv::Mat& frame)
{
	if (frame.empty()) {
		return false;
	}
	cv::Mat grayMat;
	cv::cvtColor(frame, grayMat, CV_BGR2GRAY);
	NaturalBinarization natBin(18, 18, 0.49);
	grayMat = natBin.run(grayMat, 1.0);    
	m_image = grayMat.clone();
	//m_image = frame.clone();
	return true;
}

std::vector<std::pair<cv::Rect, std::string>> TextRegionExtractor::run(const TextRegionExtractor& opt, const tesseract::PageIteratorLevel mode) {
		return TesseractExecution(opt, mode);
	}

std::vector<std::pair<cv::Rect, std::string>> TextRegionExtractor::TesseractExecution(const TextRegionExtractor& opt, const tesseract::PageIteratorLevel level)
{
	if (m_image.empty()) {
		return TextRegionExtractor::RectTextInfo();
	}
	TextRegionExtractor::RectTextInfo  RectTextInfoReturn;
	tess.SetImage(m_image.data, m_image.cols, m_image.rows, m_image.channels(), m_image.step1());
	tess.Recognize(NULL);
	tesseract::ResultIterator* ri = tess.GetIterator();
	unsigned countOfInaneSymbols{};
	bool b{ true };
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			if (word) {
				float conf = ri->Confidence(level);
				int x1, y1, x2, y2;
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);

				/*for (int i = 0; i < strlen(word); ++i) {
				if (word[i] == ',' || word[i] == ':' || word[i] == '·' || word[i] == '’'
				|| word[i] == ';' || word[i] == '“' || word[i] == '‘' || word[i] == '”'
				|| word[i] == '_' || word[i] == '.' || word[i] == '?' || word[i] == '!'
				|| word[i] == '-' || word[i] == '1' || word[i] == '2' || word[i] == '3')
				++countOfInaneSymbols;
				}
				if (countOfInaneSymbols > 15) {
				b = false;
				delete[] word;
				continue;
				}*/

				cv::Rect rec = { x1, y1, x2 - x1, y2 - y1 };
				cv::Mat cutImage = m_image(rec);
				double totalNumberOfPixels = (x2 - x1) * (y2 - y1);
				double countOfNonZeroPixels = cv::countNonZero(cutImage);
				double countOfZeroPixels = totalNumberOfPixels - countOfNonZeroPixels;
				if (totalNumberOfPixels) {
					if (countOfNonZeroPixels / totalNumberOfPixels > 0.93
						|| countOfZeroPixels / totalNumberOfPixels > 0.93) {
						delete[] word;
						continue;
					}
				}

				std::string wtext(word);
				RectTextInfoReturn.push_back(std::make_pair(cv::Rect(x1, y1, x2 - x1, y2 - y1), wtext));
				delete[] word;
			}

		} while (ri->Next(level));
	}

	return RectTextInfoReturn;
}
