#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*) {
	/// Reduce noise with a kernel 3x3
	blur(src_gray, detected_edges, Size(3, 3));

	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio,
			kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);
}

/* CLAHE + Laplacian filter */
int main4(int argc, char** argv) {

	VideoCapture cap(0); // open the default camera
	if (cap.isOpened()) {
		// check if we succeeded
		while (1) {
			Mat frame;
			cap >> frame;
			if (frame.empty())
				break; // end of video stream
			frame.copyTo(src);
			dst.create(src.size(), src.type());

			/// Convert the image to grayscale
			cvtColor(src, src_gray, CV_BGR2GRAY);

			/// Create a window
			namedWindow(window_name, CV_WINDOW_AUTOSIZE);

			/// Create a Trackbar for user to enter threshold
			createTrackbar("Min Threshold:", window_name, &lowThreshold,
					max_lowThreshold, CannyThreshold);

			/// Show the image
			CannyThreshold(0, 0);


			Mat img, imgLaplacian, imgResult;

			//------------------------------------------------------------------------------------------- real stuffs now
			img = frame;

			/* We define a kernel */
			Mat kernel =
					(Mat_<float>(3, 3) << 0.5, 1, 0.5, 1, -6, 1, 0.5, 1, 0.5);

			filter2D(img, imgLaplacian, CV_32F, kernel);
			img.convertTo(img, CV_32F);
			imgResult = img - imgLaplacian;

			// convert back to 8bits gray scale
			imgResult.convertTo(imgResult, CV_8U);
			imgLaplacian.convertTo(imgLaplacian, CV_8U);
			Mat nueva, nueva2;
			imgResult.copyTo(nueva);
			imgResult.copyTo(nueva2);
			vector<Mat> channels, channels2, channels3;
			split(nueva, channels);
			split(nueva2, channels3);
			split(imgResult, channels2);

			for (int i = 0; i < imgResult.rows; ++i) {
				for (int j = 0; j < imgResult.cols; ++j) {
					channels[0].at<uchar>(i, j) = channels2[0].at<uchar>(
							imgResult.rows - i - 1, imgResult.cols - j - 1);
					channels[1].at<uchar>(i, j) = channels2[1].at<uchar>(
							imgResult.rows - i - 1, imgResult.cols - j - 1);
					channels[2].at<uchar>(i, j) = channels2[2].at<uchar>(
							imgResult.rows - i - 1, imgResult.cols - j - 1);
				}

			}
			for (int i = 0; i < imgResult.rows; ++i) {
				for (int j = 0; j < imgResult.cols; ++j) {
					channels3[0].at<uchar>(i, j) = channels2[0].at<uchar>(i, j)
							* i / imgResult.rows
							+ channels[0].at<uchar>(i, j) * (imgResult.rows - i)
									/ imgResult.rows;
					channels3[1].at<uchar>(i, j) = channels2[1].at<uchar>(i, j)
							* i / imgResult.rows
							+ channels[1].at<uchar>(i, j) * (imgResult.rows - i)
									/ imgResult.rows;
					channels3[2].at<uchar>(i, j) = channels2[2].at<uchar>(i, j)
							* i / imgResult.rows
							+ channels[2].at<uchar>(i, j) * (imgResult.rows - i)
									/ imgResult.rows;
				}

			}
			merge(channels2, imgResult);
			merge(channels3, nueva2);
			merge(channels, nueva);

			imshow("original", frame);
			imshow("composition", nueva2);
			imshow("sharpen", imgResult);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}

	return 0;
}

