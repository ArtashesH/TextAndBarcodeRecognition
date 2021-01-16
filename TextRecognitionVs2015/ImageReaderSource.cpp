/*
*  Copyright 2010-2011 ZXing authors
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ImageReaderSource.h"
#include <zxing/common/IllegalArgumentException.h>

#include "NaturalBinarization.h"

#include <zxing/LuminanceSource.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/oned/OneDReader.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/Exception.h>


#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include "lodepng.h"
#include "jpgd.h"
#include "MatSource.h"


using std::string;
using std::ostringstream;
using zxing::Ref;
using zxing::ArrayRef;
using zxing::LuminanceSource;

ImageReaderSource::ImageReaderSource(ArrayRef<char> image_, int width, int height, int comps_)
	: Super(width, height), image(image_), comps(comps_) {}

Ref<LuminanceSource> ImageReaderSource::create_image(const cv::Mat& inputImg) {
	cv::Mat inputMat = inputImg.clone();
	cv::cvtColor(inputMat, inputMat, CV_BGR2GRAY);
	return zxing::Ref<LuminanceSource>(new MatSource(inputMat));
}

Ref<LuminanceSource> ImageReaderSource::create_image_binary(cv::Mat& inputImg) {
	cv::Mat grayMat;
	cv::cvtColor(inputImg, grayMat, CV_BGR2GRAY);

	NaturalBinarization natBin(40, 40, 0.3);
	grayMat = natBin.run(grayMat, 1.0);

	return zxing::Ref<LuminanceSource>(new MatSource(grayMat));
}