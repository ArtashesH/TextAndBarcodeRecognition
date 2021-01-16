// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __IMAGE_READER_SOURCE_H_
#define __IMAGE_READER_SOURCE_H_
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

#include <zxing/LuminanceSource.h>
#include <opencv2\core\core.hpp>


class ImageReaderSource : public zxing::LuminanceSource {
private:
	typedef LuminanceSource Super;
	const zxing::ArrayRef<char> image;
	const int comps;

public:
	static zxing::Ref<LuminanceSource> create_image(const cv::Mat& inputImg);
	static zxing::Ref<LuminanceSource> create_image_binary(cv::Mat& inputImg);

	ImageReaderSource(zxing::ArrayRef<char> image, int width, int height, int comps);
};

#endif