#include <cv.h>
#include <highgui.h>

using namespace cv;

void SaltPepperFilter(cv::Mat &imagen, int n) {
	// Add noise (SaltPepper noise) to the image
	for (int k = 0; k < n; k++) {
		int i = rand() % imagen.rows;
		int j = rand() % imagen.cols;
		/* Salt or Pepper */
		bool sal = rand() % 2 == 0;
		int nuevoColor = 255;
		if (!sal)
			nuevoColor = 0;

		if (imagen.channels() == 1) { // B/W image
			imagen.at<uchar>(i, j) = nuevoColor;
		} else if (imagen.channels() == 3) { // Color image
			imagen.at<cv::Vec3b>(i, j)[0] = nuevoColor;
			imagen.at<cv::Vec3b>(i, j)[1] = nuevoColor;
			imagen.at<cv::Vec3b>(i, j)[2] = nuevoColor;
		}
	}
}

int main(int argc, char** argv) {

	VideoCapture cap(0); // open the default camera
	if (cap.isOpened()) {
		// check if we succeeded
		Mat edges;
		namedWindow("edges", 1);

		while (1) {
			Mat frame;
			cap >> frame;
			if (frame.empty())
				break; // end of video stream
			SaltPepperFilter(frame, frame.cols * frame.rows / 10);
			imshow(":O I can see you!", frame);
			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}

	return 0;
}

