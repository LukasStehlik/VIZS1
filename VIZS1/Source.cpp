/**
* @file Morphology_1.cpp
* @brief Erosion and Dilation sample code
* @author OpenCV team
*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat src = imread("../Data/kruh2.bmp",IMREAD_UNCHANGED);
	if (src.empty())
	{
		cout << "can not open ";
		return -1;
	}

	Mat dst, HoughSource, HSV_Image;
	GaussianBlur(src, dst, Size(7, 7), 0, 0); //Rozmazáva hrany
	medianBlur(dst, dst, 29);
	cvtColor(dst, HSV_Image, CV_RGB2HSV);

	//imshow("Original", src);
	//imshow("Gauss",dst);
	//imshow("Gaus+Median", dst);
	//Canny(dst, Keny, 50, 200, 3);
	//imshow("Canny", Keny);

	vector<Vec3f> circles;
	cvtColor(dst, HoughSource, CV_RGB2GRAY);
	cout << "Bullshit" << endl;
	HoughCircles(HoughSource, circles, HOUGH_GRADIENT, 1, HoughSource.rows / 4, 200, 100);

	printf("Pocet kruhov: %u\n", (uint16_t)circles.size());


	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		
		printf("x=%i y=%i\n", center.x, center.y);

		uint meanH = 0;
		uint meanS = 0;
		uint meanV = 0;
		Vec3b hsv;

		for (int j = (center.x - radius / 2); j < (center.x + radius / 2); j++)
		{
			hsv = HSV_Image.at<Vec3b>(j, center.y);
			meanH += hsv.val[0];
			meanS += hsv.val[1];
			meanV += hsv.val[2];
		}

		uint H = meanH / radius;
		uint S = meanS / radius;
		uint V = meanV / radius;
		printf("H=%u S=%u V=%u\n", H * 2, S, V);

		Mat someColor = Mat(1, 1, CV_8UC3, Scalar(H , S, V));
		cvtColor(someColor, someColor, COLOR_HSV2RGB);
		Scalar rgb = someColor.at<Vec3b>(0, 0);

		// draw the circle outline
		circle(src, center, radius, rgb, 3, 8, 0);
		putText(src, "Kruh", center, FONT_HERSHEY_PLAIN, 2, Scalar(0, 255, 255), 2);
	}
	imshow("Circles", src);
	waitKey(0);
	return 0;
	

#if 0
	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
#else
	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
#endif
	imshow("source", src);
	imshow("detected lines", dst);

	waitKey();

	return 0;
}