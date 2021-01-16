#ifndef NATURAL_BINARIZATION_H_
#define NATURAL_BINARIZATION_H_

namespace cv{
	class Mat;
}
/// Natural Binarization is utility for text rectification
/// This uses  
/// 
class NaturalBinarization {
	
public:
	enum version {
		Niblack,// (1986)         needs white text on black background\n"
		Sauvola,// et al. (1997)  needs black text on white background\n"
		Wolf,   // et al. (2001)     needs black text on white background\n"
		Wolf_2007 
	};

	/// Constructor for initializing the filtering window
	/// \param winx number of columns
	/// \param winy number of rows
	/// \param k filtering threshold: lower value means more black 
	/// \param v algorithm for binarization
    NaturalBinarization(int winx = 40, int winy = 40, float k = 0.1f, version v = Wolf);

	/// Returns binarized image
	/// \param input the input image
	/// \param input resizing coeff
	cv::Mat run(const cv::Mat& input, double r = 1.0);

private:
	int m_winx;
	int m_winy;
	float m_optk;
	version m_version;
};
#endif //NATURAL_BINARIZATION_H_
