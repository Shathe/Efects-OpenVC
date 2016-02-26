#include <cv.h>
#include <highgui.h>

using namespace cv;

enum Space {
	blue, green, red
};

Mat posterize(Mat src, int NeachChannel, int offset) {
	if (NeachChannel + offset > 0) {
		int minR = 255;
		int minB = 255;
		int minG = 255;
		int maxR = 0;
		int maxB = 0;
		int maxG = 0;

		vector<Mat> channels;
		split(src, channels);

		NeachChannel = NeachChannel + (offset / 3);
		int extrasDivisions = offset % 3;

		for (int y = 0; y < src.rows; y++) {
			for (int x = 0; x < src.cols; x++) {
				if (channels[blue].at<uchar>(x, y) < minB)
					minB = channels[blue].at<uchar>(x, y);
				if (channels[blue].at<uchar>(x, y) > maxB)
					maxB = channels[blue].at<uchar>(x, y);
				if (channels[green].at<uchar>(x, y) < minG)
					minG = channels[green].at<uchar>(x, y);
				if (channels[green].at<uchar>(x, y) > maxG)
					maxG = channels[green].at<uchar>(x, y);
				if (channels[red].at<uchar>(x, y) < minR)
					minR = channels[red].at<uchar>(x, y);
				if (channels[red].at<uchar>(x, y) > maxR)
					maxR = channels[red].at<uchar>(x, y);
			}

		}

		/* Now you have the ranges colours of the picture */

		/* As the humam being detects better variations of the channel green > red > blue
		 * if you have to choose between them, for giving more range to those channels */

		if (NeachChannel == 0)
			NeachChannel++;
		int rangeSectionB = (maxB - minB) / (NeachChannel);
		int rangeSectionG = (maxG - minG) / (NeachChannel);
		int rangeSectionR = (maxR - minR) / (NeachChannel);

		if (extrasDivisions > 1) {
			rangeSectionR = (maxG - minG) / (NeachChannel + 1);
		}
		/*Calculate the mean of each channel */
		cv: Scalar meanB = mean(channels[blue]);
		float meanBF = meanB.val[0];
		uchar meanBC = meanBF;
		Scalar meanG = mean(channels[green]);
		float meanGF = meanG.val[0];
		uchar meanGC = meanGF;
		Scalar meanR = mean(channels[red]);
		float meanRF = meanR.val[0];
		uchar meanRC = meanRF;
		/* Calculate the variation along the histogram */
		Mat prueba;
		equalizeHist(channels[blue], prueba);
		prueba = abs(channels[blue] - prueba);
		float variationBlue = (sum(prueba)[0] / 150000000)-0.03;
		equalizeHist(channels[green], prueba);
		prueba = abs(channels[green] - prueba);
		float variationGreen = (sum(prueba)[0] / 150000000) -0.03;
		equalizeHist(channels[red], prueba);
		prueba = abs(channels[red] - prueba);
		float variationRed = (sum(prueba)[0] / 150000000)-0.03;

		for (int i = 0; i < src.rows; ++i) {
			for (int j = 0; j < src.cols; ++j) {
				uchar numSectionBlue = (channels[blue].at<uchar>(i, j)
						/ rangeSectionB);
				uchar result = numSectionBlue * rangeSectionB;
				if (NeachChannel == 1) {
					/*if NeachChannel == 0, 1 or 2 colours */
					channels[blue].at<uchar>(i, j) = meanBC;
				} else {
					channels[blue].at<uchar>(i, j) = result
							* (1 - variationBlue) + meanBC * variationBlue;

				}

				uchar numSectionGreen = (channels[green].at<uchar>(i, j)
						/ rangeSectionG);
				result = numSectionGreen * rangeSectionG;
				if (result < 0) {
					/*if NeachChannel == 0, 1 or 2 colours */
					channels[green].at<uchar>(i, j) = meanGC;
				} else {
					channels[green].at<uchar>(i, j) = result
							* (1 - variationGreen) + meanGC * variationGreen;

				}

				uchar numSectionRed = (channels[red].at<uchar>(i, j)
						/ rangeSectionR);
				result = numSectionRed * rangeSectionR;
				if (result < 0) {
					/*if NeachChannel == 0, 1 or 2 colours */
					channels[red].at<uchar>(i, j) = meanRC;
				} else {
					channels[red].at<uchar>(i, j) = result * (1 - variationRed)
							+ meanRC * variationRed;

				}

			}
		}

		merge(channels, src); //merge 3 channels including the modified 1st channel into one image

	}
	return src;
}
int main11(int argc, char** argv) {

	VideoCapture cap(0); // open the default camera
	if (cap.isOpened()) {
		int numberC=2;
		std::cout << "Enter the number of divisions of each channel you want to have: ";
				std::cin >> numberC;

		while (1) {
			Mat frame;
			cap >> frame;
			if (frame.empty())
				break; // end of video stream

			frame = posterize(frame, numberC, 0);
			imshow(":O I can see you!", frame);

			if (waitKey(1) == 27)
				break; // stop capturing by pressing ESC
		}
	}

	return 0;
}

