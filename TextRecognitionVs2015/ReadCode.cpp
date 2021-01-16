#include "ReadCode.h"

#include <thread>

bool Code_Reader::setImage(const cv::Mat img, const MODEOFCODE kindOfDecode, const std::string img_path) {
	if (img_path.empty()) {
		m_image = img.clone();
		m_clone = m_image.clone();
	}
	else {
		m_image_path = img_path;
		m_image = cv::imread(img_path);
		m_clone = m_image.clone();
	}

	if (m_image.cols == 0 || m_image.rows == 0) {
		return false;
	}
	m_kindOfDecode = kindOfDecode;
	return true;
}

std::string Code_Reader::showCode() {
	m_angle = 0;
	m_areCodsDetected.resize(3, false);
	m_results.resize(3, "");
	Ref<LuminanceSource> source;

	try {
		source = ImageReaderSource::create_image(m_clone);
	}
	catch (const zxing::IllegalArgumentException &e) {
		std::cerr << e.what() << " (ignoring)" << std::endl;
		return NULL;
	}

	m_areCodsDetected[0] = read_image(source, true, 0);
	if (m_areCodsDetected[0]) {
		return std::string(m_results[0].begin(), m_results[0].end());
	}
	else {
		m_areCodsDetected[0] = read_image(source, false, 0);
		if (m_areCodsDetected[0]) {
			return std::string(m_results[0].begin(), m_results[0].end());
		}
	}
	if (!m_areCodsDetected[0]) {
		cv::Mat testMat = m_clone.clone();
		if (testMat.channels() == 3) {
			cv::cvtColor(testMat, testMat, CV_BGR2GRAY);
		}
		NaturalBinarization natBin(40, 40, 0.3);
		testMat = natBin.run(testMat, 1.0);

		cv::cvtColor(testMat, testMat, CV_GRAY2BGR);
		m_clone = testMat.clone();

		try {
			source = ImageReaderSource::create_image(testMat);
		}
		catch (const zxing::IllegalArgumentException &e) {
			std::cerr << e.what() << " (ignoring)" << std::endl;
			return NULL;
		}

		m_areCodsDetected[0] = read_image(source, true, 0);
		if (m_areCodsDetected[0]) {
			return std::string(m_results[0].begin(), m_results[0].end());
		}
		else {
			m_areCodsDetected[0] = read_image(source, false, 0);
			if (m_areCodsDetected[0]) {
				return std::string(m_results[0].begin(), m_results[0].end());
			}
		}
	}
	if (!m_areCodsDetected[0]) {
		try {
			source = ImageReaderSource::create_image_binary(m_clone);
			m_areCodsDetected[0] = read_image(source, true, 0);

			if (!m_areCodsDetected[0]) {
				m_areCodsDetected[0] = read_image(source, false, 0);
				if (m_areCodsDetected[0]) {
					return std::string(m_results[0].begin(), m_results[0].end());
				}

			}
		}
		catch (const zxing::IllegalArgumentException &e) {
			std::cerr << e.what() << " (ignoring)" << std::endl;
			return "";
		}
	}
	if (m_areCodsDetected[0]) {
		return std::string(m_results[0].begin(), m_results[0].end());
	}

	if (!m_areCodsDetected[0]) {
		int angle{ 0 };
		std::vector<int> angs = { 0, 0, 0 };
		while (angle < 345) {
			angs[0] = angs[2] + 7;
			angs[1] = angs[0] + 7;
			angs[2] = angs[1] + 7;
			angle	= angs[2];
			std::thread t1(&Code_Reader::rotateImageWithAngle, this, angs[0], 0);
			std::thread t2(&Code_Reader::rotateImageWithAngle, this, angs[1], 1);
			std::thread t3(&Code_Reader::rotateImageWithAngle, this, angs[2], 2);

			t1.join();
			t2.join();
			t3.join();

			if (m_areCodsDetected[0]) {
				m_angle = angle;
				return std::string(m_results[0].begin(), m_results[0].end());
			}
			if (m_areCodsDetected[1]) {
				m_angle = angle;
				return std::string(m_results[1].begin(), m_results[1].end());
			}
			if(m_areCodsDetected[2]) {
				m_angle = angle;
				return std::string(m_results[2].begin(), m_results[2].end());
			}
		}
	}
	return "";
}

std::vector<Ref<Result> > Code_Reader::decode(const Ref<BinaryBitmap> image, const DecodeHints hints) {
	Ref<Reader> reader(new MultiFormatReader);
	return std::vector<Ref<Result> >(1, reader->decode(image, hints));
}

void Code_Reader::rotateImageWithAngle(const int angle, const char index)
{
	cv::Mat forRot = m_image.clone();
	cv::Point2f center(forRot.cols / 2.0, forRot.rows / 2.0);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
	// determine bounding rectangle
	cv::Rect bbox = cv::RotatedRect(center, forRot.size(), angle).boundingRect();

	// adjust transformation matrix
	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

	cv::warpAffine(forRot, forRot, rot, bbox.size());

	Ref<LuminanceSource> source;
	source = ImageReaderSource::create_image(forRot.clone());
	m_areCodsDetected[index] = read_image(source, true, index);

	if(!m_areCodsDetected[index]){
		source = ImageReaderSource::create_image_binary(forRot.clone());
		m_areCodsDetected[index] = read_image(source, false, index);
	}
}

bool Code_Reader::read_image(Ref<LuminanceSource> source, const bool hybrid, const char index) {
	std::vector<Ref<Result> > results;
	std::string cell_result{};

	try {
		Ref<Binarizer> binarizer;
		if (hybrid) {
			binarizer = new HybridBinarizer(source);
		}
		else {
			binarizer = new GlobalHistogramBinarizer(source);
		}
		DecodeHints	hint;
		if (m_kindOfDecode == MODEOFCODE::BARCODE) {
			DecodeHints hints(DecodeHints::DEFAULT_HINT);
			hint = hints;
		}
		else if (m_kindOfDecode == MODEOFCODE::QRCODE) {
			DecodeHints hints(DecodeHints::QR_CODE_HINT);
			hint = hints;
		}
		Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
		results = decode(binary, hint);
	}
	catch (const ReaderException& e) {
		cell_result = "zxing::ReaderException: " + std::string(e.what());
	}

	if (results.size() == 1) {
		m_results[index] = results[0]->getText()->getText();
		m_areCodsDetected[index] = true;
		return true;
	}

	m_areCodsDetected[index] = false;
	return false;
}