#include <cv.h>
#include <highgui.h>

using namespace cv;
int findBiggestContour(vector<vector<Point> >);

/*
 * Apply some filters in order to  smoothen the image.
 */
int applyFilters(Mat p1, int n) {
	Scalar white = Scalar(110, 110, 100);
	Scalar whitemax = Scalar(255, 255, 255);

	// Filters: Blur and dilate
	Mat element = getStructuringElement(MORPH_RECT, Size(n,n));
	blur(p1, p1, Size(2 * n, 2 * n));
	inRange(p1, white, whitemax, p1);
	dilate(p1, p1, element);
	inRange(p1, white, whitemax, p1);
}

int main2(int argc, char** argv) {
	enum Space {
		blue, green, red
	};
	String color;
	std::cout
			<< "Enter the color you want to see your skin like: red, blue or green:\n"
					"(Green by defect)\n";

	std::cin >> color;
	Space option = green;
	for (unsigned int i = 0; i < color.length(); ++i) {
		color[i] = tolower(color[i]);
	}
	if (color.find("blue") != std::string::npos)
		option = blue;

	if (color.find("red") != std::string::npos)
		option = red;

	vector<Mat> channels, channels2;

	VideoCapture cap(0); // open the default camera
	if (cap.isOpened()) {
		// check if we succeeded
		Mat p2, p3, p4;

		while (1) {
			Mat frame, frame2;
			cap >> frame;
			cap >> frame2;
			if (frame.empty())
				break; // end of video stream
			Mat img_hist, hsv_image;
			Mat canny_output;
			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;
			Scalar hsv_min2 = Scalar(3, 12, 30);
			Scalar hsv_max2 = Scalar(20, 150, 255);

			/* Change the color space */
			cvtColor(frame, img_hist, CV_BGR2HSV);
			/*Apply a range, the skin range */

			inRange(img_hist, hsv_min2, hsv_max2, p4);
			//Aply some filters
			applyFilters(p4, 3.5);
			p4.copyTo(p3);

			/*Find the smallest contours */
			Mat pequenos = Mat::zeros(p3.size(), CV_8UC1);
			findContours(p3, contours, hierarchy, CV_RETR_TREE,
					CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

			for (int i = 0; i < contours.size(); i++) {

				int area = cv::contourArea(contours.at(i));
				if (area < 1200)
					drawContours(pequenos, contours, i, Scalar(255), -1, 8,
							hierarchy, 0, Point());
			}

			split(frame2, channels);
			/* Use the color Alien filter only if, p4, the pixels are likely to be skin and
			 * if they are not small (pequenos)
			 */
			for (int i = 0; i < frame.rows; ++i)
				for (int j = 0; j < frame.cols; ++j)
					if (p4.at<uchar>(i, j) >= 254
							&& pequenos.at<uchar>(i, j) < 200) {
						channels[red].at<uchar>(i, j) = channels[red].at<uchar>(
								i, j) / 1.5;
						channels[green].at<uchar>(i, j) = channels[green].at<
								uchar>(i, j) / 1.5;
						;
						channels[blue].at<uchar>(i, j) =
								channels[blue].at<uchar>(i, j) / 1.5;
						channels[option].at<uchar>(i, j) = channels[option].at<
								uchar>(i, j) + 75;
					}

			merge(channels, frame2); //merge 3 channels including the modified 1st channel into one image
			imshow("Alien!", frame2);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}

	return 0;
}

int findBiggestContour(vector<vector<Point> > contours) {
	int indexOfBiggestContour = -1;
	int sizeOfBiggestContour = 0;
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > sizeOfBiggestContour) {
			sizeOfBiggestContour = contours[i].size();
			indexOfBiggestContour = i;
		}
	}
	return indexOfBiggestContour;
}
