#include "BinarisationUtilities.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <cassert>

namespace BinarisationUtilities {
	void thresholdWithSurface (cv::Mat& im, cv::Mat& surface)
	{
		for	(int i = 0 ; i < im.cols ; i++) {
			for	(int j = 0 ; j < im.rows ; j++) {
                im.at<uchar>(j, i) = (im.at<uchar>(j, i) >= surface.at<float>(j, i) ? 255 : 0);
			}
		}
	}

	double calcLocalStatsOptimized(cv::Mat &im, int winx, int winy, cv::Mat & map_m, cv::Mat& map_s)
	{
		assert(im.type() == CV_8UC1);
		double m,s,max_s, sum, sum_sq, foo;
		int wxh	= winx/2;
		int wyh	= winy/2;
		int x_firstth= wxh;
		int y_lastth = im.rows-wyh-1;
		int y_firstth= wyh;

		double winarea = winx*winy;
		map_m = cv::Mat(im.size(), CV_32FC1);
		map_s = cv::Mat(im.size(), CV_32FC1);

		max_s = 0;

		cv::Mat sumImg, sumSqImg;
		cv::integral(im, sumImg, sumSqImg, CV_64F);
		
		for	(int i = y_firstth ; i <= y_lastth; i++) {
			for(int j = x_firstth; j < im.cols - wxh - 1; ++j) {
				const cv::Point tl (j - wxh, i - wyh);
				const cv::Point br (j + wxh, i + wyh);

				const cv::Point tr (j + wxh, i - wyh);
				const cv::Point bl (j - wxh, i + wyh);

				sum = sumImg.at<double>(br) + sumImg.at<double>(tl) - sumImg.at<double>(bl) - sumImg.at<double>(tr);
				sum_sq = sumSqImg.at<double>(br) + sumSqImg.at<double>(tl) - sumSqImg.at<double>(bl) - sumSqImg.at<double>(tr);
				
				m  = sum / winarea;
				s  = sqrt ((sum_sq - (sum*sum)/winarea)/winarea);
				if (s > max_s) {
					max_s = s;
				}
				map_m.at<float>(i, j) = m;
				map_s.at<float>(i, j) = s;
			}
		}
		return max_s;
	}

	double calcLocalStats (cv::Mat &im, int winx, int winy, cv::Mat & map_m, cv::Mat& map_s) 
	{
			assert(im.type() == CV_8UC1);
			double m,s,max_s, sum, sum_sq, foo;
			int wxh	= winx/2;
			int wyh	= winy/2;
			int x_firstth= wxh;
			int y_lastth = im.rows-wyh-1;
			int y_firstth= wyh;
			double winarea = winx*winy;
		    map_m = cv::Mat(im.size(), CV_32FC1);
			map_s = cv::Mat(im.size(), CV_32FC1);

			max_s = 0;
			for	(int j = y_firstth ; j <= y_lastth; j++) {

				// Calculate the initial window at the beginning of the line
				sum = sum_sq = 0;
				for	(int wy=0 ; wy<winy; wy++)
					for	(int wx=0 ; wx<winx; wx++) {
						foo = im.at<uchar>(j-wyh+wy, wx);//][j-wyh+wy];
						sum    += foo;
						sum_sq += foo * foo;
					}

					m  = sum / winarea;
					s  = sqrt ((sum_sq - (sum*sum)/winarea)/winarea);
					if (s > max_s)
						max_s = s;
					map_m.at<float>(j, x_firstth) = m;
					map_s.at<float>(j, x_firstth) = s;

					// Shift the window, add and remove	new/old values to the histogram
					for	(int i=1 ; i <= im.cols - winx; i++) {

						// Remove the left old column and add the right new column
						for (int wy=0; wy<winy; ++wy) {
							foo = im.at<uchar>(j-wyh+wy, i-1);//][j-wyh+wy];
							sum    -= foo;
							sum_sq -= foo*foo;
							foo = im.at<uchar>(j-wyh+wy, i+winx-1);//][];
							sum    += foo;
							sum_sq += foo*foo;
						}
						m  = sum / winarea;
						s  = sqrt ((sum_sq - (sum*sum)/winarea)/winarea);
						if (s > max_s)
							max_s = s;
						map_m.at<float>(j, i+wxh) = m;
						map_s.at<float>(j, i+wxh) = s;
					}
			}		
			return max_s;
	}

	cv::Mat surfaceNiblackImproved (cv::Mat &im, NiblackVersion version, int winx, int winy, double k, double dR, cv::Mat& map_m, cv::Mat& map_s)
	{
		double m, s, max_s;
		double th=0;
		uchar min_I;

		int wxh	= winx/2;
		int wyh	= winy/2;
		int x_firstth= wxh;
        int x_lastth = im.cols - wxh - 1;
        int y_lastth = im.rows - wyh - 1;
		int y_firstth= wyh;
		///Should be corrected in case of wolf2007
		//FloatPyramid *pyr=NULL;
		//int pyr_access_level=0;
		//float pyr_access_xfactor=0;
	//	float pyr_access_yfactor=0;
		//FloatMatrix *pyr_access_top=NULL;
		//int mx, my;

		cv::Mat ret_im(im.size(), CV_32FC1);



		// Create the local stats and store them in a map
		//max_s = calcLocalStats (im, winx, winy, map_m, map_s);
		max_s = calcLocalStatsOptimized (im, winx, winy, map_m, map_s);

		//std::cout << "----------  Maximum variation is " << max_s<<std::endl;
	
		/*if(max_s > 50) {
			max_s = 50;
		}*/
		
		double minVal = 256, maxVal = 0;

		//min_I = imageMin(im, PLANE_RED);
		
		cv::minMaxLoc(im, &minVal, &maxVal);
		min_I = minVal;
		
		// Create a pyramid which subsamples the standard dev
		
		if (version==NIBLACK_WOLF_2007)
		{
			///Should be corrected in case of wolf2007
		/*	int bigwin = (int) 2*(winx+winy);
			if (bigwin>im.xsize)
				bigwin=im.xsize;
			if (bigwin>im.ysize)
				bigwin=im.ysize;
			std::cerr << "Setting maximum(s) windows size to " << bigwin << endl;
#ifdef iOS
			pyr = new FloatPyramid (map_s, (int) floor(log10(bigwin +0.0)/log10(2.0)), PYRT_MAX_RED22);
#else
			pyr = new FloatPyramid (map_s, (int) floor(std::log10(bigwin +0.0)/std::log10(2.0)), PYRT_MAX_RED22);
#endif
			pyr->build();

			pyr_access_level = pyr->levels()-1;
			pyr_access_xfactor = 1./pow(2.,pyr_access_level);
			pyr_access_yfactor = 1./pow(2.,pyr_access_level);
			pyr_access_top = (*pyr)[pyr_access_level];		*/
		}

		// In a second step, create the surface.
		// ----------------------------------------------------

		for	(int j = y_firstth ; j<=y_lastth; j++) {

			// NORMAL, NON-BORDER AREA IN THE MIDDLE OF THE WINDOW:
			for	(int i=0 ; i <= im.cols - winx; i++) {

				m  = map_m.at<float>(j, i+wxh);//, j);
				s  = map_s.at<float>(j, i+wxh);//, j);

				// Calculate the threshold
				switch (version) {

				case NIBLACK_CLASSIC:
					th = m + k*s;
					break;

				case NIBLACK_SAUVOLA:
					th = m * (1 + k*(s/dR-1));
					break;

				case NIBLACK_WOLF1:
					th = m * (1 + k*(s/max_s-1));
					break;

				case NIBLACK_WOLF2:
					th = m + k * (s/max_s-1) * (m-min_I);
					break;

				case NIBLACK_WOLF_2007:
					/*mx = (int) rint(i*pyr_access_xfactor);
					my = (int) rint(j*pyr_access_yfactor);
					if (mx>=pyr_access_top->xsize) mx=pyr_access_top->xsize-1;
					if (my>=pyr_access_top->ysize) my=pyr_access_top->ysize-1;
					max_s = pyr_access_top->get(mx,my);     					
					th = m + k * (s/max_s-1) * (m-min_I);
					break;*/

				default:
					std::cerr << "Unknown threshold type in \n";
					exit (1);
				}

				ret_im.at<float>(j , i+wxh) = th;

				if (i==0) {
					// LEFT BORDER
					for (int i=0; i<=x_firstth; ++i)
						ret_im.at<float>(j, i) = th;

					// LEFT-UPPER CORNER
					if (j==y_firstth)
						for (int u=0; u<y_firstth; ++u)
							for (int i=0; i<=x_firstth; ++i)
								ret_im.at<float>(u, i) = th;

					// LEFT-LOWER CORNER
					if (j == y_lastth)
						for (int u = y_lastth + 1; u < im.rows; ++u)
							for (int i = 0; i <= x_firstth; ++i)
								ret_im.at<float>(u, i) = th;
				}

				// UPPER BORDER
				if (j == y_firstth)
					for (int u = 0; u < y_firstth; ++u)
						ret_im.at<float>(u, i + wxh) = th;

				// LOWER BORDER
				if (j==y_lastth)
					for (int u=y_lastth + 1; u < im.rows; ++u)
						ret_im.at<float>(u, i+wxh) = th;
			}

			// RIGHT BORDER
			for (int i=x_lastth; i<im.cols; ++i)
				ret_im.at<float>(j, i) = th;

			// RIGHT-UPPER CORNER
			if (j==y_firstth)
				for (int u = 0; u < y_firstth; ++u)
					for (int i = x_lastth; i < im.cols; ++i)
						ret_im.at<float>(u, i) = th;

			// RIGHT-LOWER CORNER
			if (j == y_lastth)
				for (int u = y_lastth + 1; u < im.rows; ++u)
					for (int i = x_lastth; i < im.cols; ++i)
						ret_im.at<float>(u, i) = th;
		}

		
		return ret_im;
	}
}
