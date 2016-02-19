#include <cv.h>
#include <highgui.h>

using namespace cv;


int main4(int argc, char** argv) {

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
			imshow(":O I can see you!", frame);
			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}

	return 0;
}

