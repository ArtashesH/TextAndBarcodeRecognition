#include "NaturalBinarization.h"

// C++ 
#include <iostream>

// From the BINARIZATION module
#include "BinarisationUtilities.h"

//OpenCV modules
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cassert>



#define BINARIZEWOLF_VERSION	"2.1 (Avril 2nd, 2009)"

NaturalBinarization::NaturalBinarization(int winx /* = 40 */, int winy /* = 40 */, float k /* = 0.5f */, NaturalBinarization::version v /* = Wolf */)
	:	m_winx(winx)
	,	m_winy(winy)
	,	m_optk(k)
	,	m_version(v)
{}



cv::Mat NaturalBinarization::run(const cv::Mat& m1, double r/* = 1.0*/)
{
	cv::Mat m;
	cv::Mat m2 = m1.clone();
	assert(r > 0.0 && r <= 1.0);

    if(r != 1){
        cv::resize(m2.clone(), m, cv::Size(), r, r);

    }else {
         m = m2.clone();
    }

	cv::Mat surface;
	cv::Mat dummy1, dummy2;
	cv::Mat input;
	BinarisationUtilities::NiblackVersion versionCode;	

	switch (m_version)
	{
	case Niblack:
		versionCode = BinarisationUtilities::NIBLACK_CLASSIC;
		std::cerr << "Niblack (1986)\n";
		break;

	case Sauvola:
		versionCode = BinarisationUtilities::NIBLACK_SAUVOLA;
		std::cerr << "Sauvola et al. (1997)\n";
		break;

	case Wolf:
		versionCode = BinarisationUtilities::NIBLACK_WOLF2;
		break;

	case Wolf_2007:
		versionCode = BinarisationUtilities::NIBLACK_WOLF_2007;
		std::cerr << "Wolf (2007) - pyramidal, experimental!!!\n";
		break;

	default:
		assert(0);	
	}
	// Main routine
	cv::Mat outputCV;
	// Load the image                            
	if(m.type() == CV_8UC3) {
		cv::cvtColor(m, input, cv::COLOR_BGR2GRAY);
	} else if(m.type() == CV_8UC1) {
		input = m.clone();
	}

	// Treat the window size
	if (m_winx == 0 || m_winy==0) {
		m_winy = (int) (2.0 * input.rows - 1 )  / 3;
		m_winx = (int) input.cols-1 < m_winy ? input.cols-1 : m_winy;
		// if the window is too big, than we assume that the image
		// is not a single text box, but a document page: set
		// the window size to a fixed constant.
        if (m_winx > 150)
            m_winx = m_winy = 50;
	}

	// Calculate the surface	
    surface = BinarisationUtilities::surfaceNiblackImproved (input, versionCode, m_winx, m_winy, m_optk, 128, dummy1, dummy2);

	// Threshold
	BinarisationUtilities::thresholdWithSurface (input, surface);

	if(int(r) != 1)
	cv::resize(input.clone(), input, m1.size(), r, r, cv::INTER_NEAREST);
	return input;
}