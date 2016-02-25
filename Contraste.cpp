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

		double alpha = 1.75; /* contrast factor */
		double beta = 30; /* brightness bias */
		String colorSpace = "Ycrcb";
		/// Initialize values
		std::cout << "Contrast enhancement and histogram equalization "
				<< std::endl;
		std::cout << "-------------------------" << std::endl;
		std::cout << "Enter the factor (must be a number, for instance 1.25): ";
		std::cin >> alpha;
		std::cout << "Enter the bias (must be a number, for instance 10): ";
		std::cin >> beta;
		std::cout
				<< "Enter the color space Ycrcb, Lab, HSV or RGB (this last one no so good)\n"
						"(Ycrcb or Lab are strongly recommend. Ycrcb by defect)";

		std::cin >> colorSpace;
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

			/* Equalize histogram of the Light channel */
			switch (space) {
			case Ycrcb:
				equalizeHist(channels[0], channels[0]);
				channels[0].convertTo(channels[0], -1, alpha, beta);
				break;
			case HSV:
				equalizeHist(channels[2], channels[2]);
				channels[2].convertTo(channels[2], -1, alpha, beta);
				break;
			case Lab:
				equalizeHist(channels[0], channels[0]);
				channels[0].convertTo(channels[0], -1, alpha, beta);
				break;
			case RGB:
				equalizeHist(channels[0], channels[0]);
				equalizeHist(channels[1], channels[1]);
				equalizeHist(channels[2], channels[2]);
				channels[0].convertTo(channels[0], -1, alpha, beta);
				channels[1].convertTo(channels[1], -1, alpha, beta);
				channels[2].convertTo(channels[2], -1, alpha, beta);
				break;
			default:
				;
			}

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
			imshow(":O I can see you!", frame);


			imshow("Contrast enhanced", img_hist_equalized);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}

	}

	return 0;
}

