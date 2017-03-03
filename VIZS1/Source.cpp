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
	Mat src = imread("../Data/kruhy.bmp",IMREAD_UNCHANGED);
	if (src.empty())
	{
		cout << "can not open ";
		return -1;
	}

	Mat dst, HoughSource, HSV_Image;
	GaussianBlur(src, dst, Size(3, 3), 0, 0); //Rozmazáva hrany
	medianBlur(dst, dst, 25);
	cvtColor(dst, HSV_Image, CV_RGB2HSV);
	//imshow("HSV", HSV_Image);
	
	//imshow("Original", src);
	//imshow("Gauss",dst);
	imshow("Gaus+Median", dst);
	//Canny(dst, Keny, 50, 200, 3);
	//imshow("Canny", Keny);

	vector<Vec3f> circles;
	cvtColor(dst, HoughSource, CV_RGB2GRAY);
	HoughCircles(HoughSource, circles, HOUGH_GRADIENT, 2, HoughSource.rows / 4, 200, 100, 0, 0);

	printf("Pocet kruhov: %u\n", (uint16_t)circles.size());


	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		
		printf("x=%i y=%i\n", center.x, center.y);

		uint64_t meanH = 0;
		uint64_t meanS = 0;
		uint64_t meanV = 0;
		Vec3b hsv;

		for (int x = (center.x - radius / 2); x < (center.x + radius / 2); x++)
		{
			for (int y = (center.y - radius / 2); y < (center.y + radius / 2); y++)
			{
				hsv = HSV_Image.at<Vec3b>(y, x);
				meanH += hsv.val[0];
				meanS += hsv.val[1];
				meanV += hsv.val[2];
				//line(src, Point(x, y), Point(x, y), hsv);
			}
		}

		uint H = (uint)meanH / (radius*radius);
		uint S = (uint)meanS / (radius*radius);
		uint V = (uint)meanV / (radius*radius);
		printf("H=%u S=%u V=%u\n", H, S, V);

		Mat someColor = Mat(1, 1, CV_8UC3, Scalar(H, S, V));
		cvtColor(someColor, someColor, CV_HSV2RGB);
		Scalar rgb = someColor.at<Vec3b>(0, 0);

		String txtColor;
		if (H > 100 && H < 140) txtColor = "RCircle";
		if (H > 30 && H < 99) txtColor = "GCircle";
		if (H < 29 || H > 150) txtColor = "BCircle";

		// draw the circle outline
		circle(src, center, radius, rgb, 3, 8, 0);
		putText(src, txtColor, center, FONT_HERSHEY_PLAIN, 2, Scalar(0, 255, 255), 2);
	}
	//Red H->100:140
	//Green H->30:99
	//Blue H->0:29 & 150:180
	/*for (int x = 0; x <= 180; x++)
	{
		Mat someColor = Mat(1, 1, CV_8UC3, Scalar(x, 255, 255));
		cvtColor(someColor, someColor, CV_HSV2RGB);
		Scalar rgb = someColor.at<Vec3b>(0, 0);
		line(src, Point(x, 0), Point(x, 9), rgb);
	}
	for (int x = 0; x <= 85; x++)
	{
		Mat someColor = Mat(1, 1, CV_8UC3, Scalar(x+180, 255, 255));
		cvtColor(someColor, someColor, CV_HSV2RGB);
		Scalar rgb = someColor.at<Vec3b>(0, 0);
		line(src, Point(x, 10), Point(x, 19), rgb);
	}*/

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