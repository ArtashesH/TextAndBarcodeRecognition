#ifndef READBARCODE_HPP
#define READBARCODE_HPP

#include <fstream>
#include <string>
#include "ImageReaderSource.h"
#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/ReaderException.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <exception>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/multi/qrcode/QRCodeMultiReader.h>
#include <zxing/multi/ByQuadrantReader.h>
#include <zxing/multi/MultipleBarcodeReader.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>

#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>

#include "NaturalBinarization.h"

using namespace zxing;
using namespace zxing::multi;
using namespace zxing::qrcode;

enum class MODEOFCODE { BARCODE, QRCODE };

class Code_Reader {
public:
	bool setImage(const cv::Mat, const MODEOFCODE, const std::string = std::string());
	std::string showCode();
	inline int get_angle() const;
private:
	std::vector<Ref<Result>> decode(const Ref<BinaryBitmap>, const DecodeHints);
	void rotateImageWithAngle(const int, const char);
	bool read_image(Ref<LuminanceSource>, const bool, const char);
	
private:
	std::string				 m_image_path;
	cv::Mat					 m_clone;
	cv::Mat					 m_image;
	int						 m_angle;
	std::vector<bool>		 m_areCodsDetected;
	MODEOFCODE				 m_kindOfDecode;
	std::vector<std::string> m_results;
};

inline int Code_Reader::get_angle() const{
	return m_angle;
}

#endif

