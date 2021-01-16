#ifndef BINARISATION_UTILITIES_HPP_
#define BINARISATION_UTILITIES_HPP_
namespace cv {
	class Mat;
}
namespace BinarisationUtilities {
	enum NiblackVersion 
	{
		NIBLACK_CLASSIC=0,
		NIBLACK_SAUVOLA,
		NIBLACK_WOLF1,
		NIBLACK_WOLF2,
		NIBLACK_WOLF_2007
	};
	void thresholdWithSurface (cv::Mat& im, cv::Mat& surface) ;
	double calcLocalStats (cv::Mat &im, int winx, int winy, cv::Mat & out_m, cv::Mat& out_s);
	cv::Mat surfaceNiblackImproved (cv::Mat &i, NiblackVersion version, int win_x, int win_y, double k, double R, cv::Mat& out_map_m, cv::Mat& out_map_s);
}

#endif
