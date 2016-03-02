#include <cv.h>
#include <highgui.h>

using namespace cv;

int main(int argc, char** argv) {
	enum Space {
		Ycrcb, Lab, HSV, RGB
	};
	VideoCapture cap(0); // open the default camera
	if (cap.isOpened()) {
		// check if we succeeded
		Mat hist1, hist2;
		int histSize = 256;
		int hist_w = 512;
		int hist_h = 400;
		int bin_w = cvRound((double) hist_w / histSize);
		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 };
		const float* histRange = { range };

		bool uniform = true;
		bool accumulate = false;
		String colorSpace = "Ycrcb";
		/// Initialize values
		std::cout << "Contrast enhancement and histogram equalization "
				<< std::endl;

		Space space = Ycrcb;
		for (unsigned int i = 0; i < colorSpace.length(); ++i) {
			colorSpace[i] = tolower(colorSpace[i]);
		}
		if (colorSpace.find("lab") != std::string::npos)
			space = Lab;

		if (colorSpace.find("hsv") != std::string::npos)
			space = HSV;

		if (colorSpace.find("rgb") != std::string::npos)
			space = RGB;

		while (1) {
			Mat frame;
			cap >> frame;
			Mat new_image = Mat::zeros(frame.size(), frame.type());
			if (frame.empty())
				break; // end of video stream

			/* Gives the new value to the new image (every pixel, every channel)
			 * It does the operation new_image(i,j) = alpha*image(i,j) + beta */
			//frame.convertTo(new_image, -1, alpha, beta);
			/* Histogram equalization */

			vector<Mat> channels;
			Mat img_hist_equalized;

			/* Changes the color image from BGR to other format */
			switch (space) {
			case Ycrcb:
				cvtColor(frame, img_hist_equalized, CV_BGR2YCrCb);
				break;
			case HSV:
				cvtColor(frame, img_hist_equalized, CV_BGR2HSV);
				break;
			case Lab:
				cvtColor(frame, img_hist_equalized, CV_BGR2Lab);
				break;
			default:
				img_hist_equalized = new_image;
			}

			split(img_hist_equalized, channels);

			calcHist(&channels[0], 1, 0, Mat(), hist1, 1, &histSize, &histRange,
					uniform, accumulate);
			/* Equalize histogram of the Light channel */
			switch (space) {
			case Ycrcb:
				//channels[0].convertTo(channels[0], -1, alpha, beta);
				equalizeHist(channels[0], channels[0]);
				break;
			case HSV:
				equalizeHist(channels[2], channels[2]);
				break;
			case Lab:
				equalizeHist(channels[0], channels[0]);
				break;
			default:
				;
			}

			/// Compute the histograms:

			calcHist(&channels[0], 1, 0, Mat(), hist2, 1, &histSize, &histRange,
					uniform, accumulate);

			merge(channels, img_hist_equalized); //merge 3 channels including the modified 1st channel into one image

			switch (space) {
			//change the color image from YCrCb to BGR format (to display image properly)
			case Ycrcb:
				cvtColor(img_hist_equalized, img_hist_equalized, CV_YCrCb2BGR);
				break;
			case HSV:
				cvtColor(img_hist_equalized, img_hist_equalized, CV_HSV2BGR);
				break;
			case Lab:
				cvtColor(img_hist_equalized, img_hist_equalized, CV_Lab2BGR);
				break;
			default:
				;
			}

			// Show images
			imshow("Contrast!", frame);
			cv::Mat lab;
			cv::cvtColor(frame, lab, CV_BGR2Lab);

			// Extract the L channel
			std::vector<cv::Mat> lab_planes(3);
			cv::split(lab, lab_planes); // now we have the L image in lab_planes[0]

			Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

			/// Normalize the result to [ 0, histImage.rows ]
			normalize(hist1, hist1, 0, histImage.rows, NORM_MINMAX, -1, Mat());
			normalize(hist2, hist2, 0, histImage.rows, NORM_MINMAX, -1, Mat());
			/// Draw for each channel
			for (int i = 1; i < histSize; i++) {
				line(histImage,
						Point(bin_w * (i - 1),
								hist_h - cvRound(hist2.at<float>(i - 1))),
						Point(bin_w * (i),
								hist_h - cvRound(hist2.at<float>(i))),
						Scalar(0, 255, 0), 2, 8, 0);
				line(histImage,
						Point(bin_w * (i - 1),
								hist_h - cvRound(hist1.at<float>(i - 1))),
						Point(bin_w * (i),
								hist_h - cvRound(hist1.at<float>(i))),
						Scalar(0, 0, 255), 2, 8, 0);
			}

			/// Display
			imshow("Histograma Contraste (Verde ecualizado)", histImage);
			// apply the CLAHE algorithm to the L channel

			calcHist(&lab_planes[0], 1, 0, Mat(), hist1, 1, &histSize,
					&histRange, uniform, accumulate);

			cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
			clahe->setClipLimit(1.7);
			cv::Mat dst;
			clahe->apply(lab_planes[0], dst);

			// Merge the the color planes back into an Lab image
			dst.copyTo(lab_planes[0]);
			calcHist(&lab_planes[0], 1, 0, Mat(), hist2, 1, &histSize,
					&histRange, uniform, accumulate);
			cv::merge(lab_planes, lab);
			Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

			normalize(hist1, hist1, 0, histImage2.rows, NORM_MINMAX, -1, Mat());
			normalize(hist2, hist2, 0, histImage2.rows, NORM_MINMAX, -1, Mat());
			/// Draw for each channel
			for (int i = 1; i < histSize; i++) {
				line(histImage2,
						Point(bin_w * (i - 1),
								hist_h - cvRound(hist2.at<float>(i - 1))),
						Point(bin_w * (i),
								hist_h - cvRound(hist2.at<float>(i))),
						Scalar(0, 255, 0), 2, 8, 0);
				line(histImage2,
						Point(bin_w * (i - 1),
								hist_h - cvRound(hist1.at<float>(i - 1))),
						Point(bin_w * (i),
								hist_h - cvRound(hist1.at<float>(i))),
						Scalar(0, 0, 255), 2, 8, 0);
			}

			/// Display
			imshow("Histograma Clahe (Verde ecualizado)", histImage2);
			// convert back to RGB
			cv::Mat image_clahe;
			cv::cvtColor(lab, image_clahe, CV_Lab2BGR);
			imshow("Clahe", image_clahe);

			imshow("Contrast enhanced", img_hist_equalized);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}

	}

	return 0;
}

