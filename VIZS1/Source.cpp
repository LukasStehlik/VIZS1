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

#define PravyUhol (CV_PI/2)

int main(int argc, char** argv)
{
	/*VideoCapture cap;
	if (!cap.open(0))
	{
		return 0;
	}*/
	for (int im = 0; im < 500; im++)
	{
		char cesta[50];
		sprintf(cesta, "../../OpenCVTest/DataJPG/frame%d.jpg", im);
		//cout << cesta << endl;
		Mat src= imread(cesta, IMREAD_UNCHANGED);
		
		//cap >> src;
		//Mat src = imread("../Data/kruhy.bmp", IMREAD_UNCHANGED);
		if (src.empty())
		{
			cout << "can not open ";
			return -1;
		}

		Mat dst, HoughSource, HSV_Image;
		//dst = src;
		GaussianBlur(src, dst, Size(5, 5), 0, 0); //Rozmazáva hrany
		//medianBlur(src, dst, 5);
		cvtColor(dst, HSV_Image, CV_RGB2HSV);
		//imshow("HSV", HSV_Image);

		//imshow("Original", src);
		//imshow("Gauss",dst);

		//imshow("Gaus+Median", dst);

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

			uint sumH = 0;
			uint sumS = 0;
			uint sumV = 0;
			uint N = 0;
			Vec3b hsv;

			for (int x = (center.x - radius / 2); x < (center.x + radius / 2); x++)
			{
				if (x >= src.cols) break;
				if (x < 0) break;

				for (int y = (center.y - radius / 2); y < (center.y + radius / 2); y++)
				{
					if (y >= src.rows) break;
					if (y < 0) break;

					hsv = HSV_Image.at<Vec3b>(y, x);
					sumH += hsv.val[0];
					sumS += hsv.val[1];
					sumV += hsv.val[2];
					N++;
					//line(src, Point(x, y), Point(x, y), hsv);
				}
			}

			if (N > 0)
			{
				uint H = sumH / N;
				uint S = sumS / N;
				uint V = sumV / N;
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
				putText(src, txtColor, center, FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 255), 2);
			}
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

		Mat Keny;
		Canny(dst, Keny, 50, 200, 3);

		vector<Vec2f> lines;
		HoughLines(Keny, lines, 1, CV_PI / 180, 107, 0, 0);
		printf("Pocet ciar: %u\n", (uint16_t)lines.size());

		/*for (size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(src, pt1, pt2, Scalar(0, 0, 255), 1, CV_AA);
		}*/
		vector<Point> priesecniky;
		vector<float> uhly;
		
		for (uint index = 0, i = 0; i < lines.size(); i++)
		{
			for (uint j = i + 1; j < lines.size(); j++, index++)
			{
				float rho1 = lines[i][0], rho2 = lines[j][0];
				float theta1 = lines[i][1], theta2 = lines[j][1];
				if (abs(theta2 - theta1) > 0.1)
				{
					int x = (int)((rho1*sin(theta2) - rho2*sin(theta1)) / sin(theta2 - theta1));
					int y = (int)((rho2*cos(theta1) - rho1*cos(theta2)) / sin(theta2 - theta1));
					//circle(src, Point(x, y), 5, Scalar(0, 0, 255), 3, 8, 0);
					priesecniky.push_back(Point(x, y));
					uhly.push_back(abs(theta2-theta1));
					printf("x=%d\ty=%d\n", x, y);
				}
				else printf("Nema presecnik\n");
			}
		}

		if (priesecniky.size() <= 5 && priesecniky.size() > 2)
		{
			for (uint i = 0; i < priesecniky.size(); i++)
			{
				for (uint j = i + 1; j < priesecniky.size(); j++)
				{
					printf("uhol=%f\t", uhly[i]);
					if ((uhly[i] > (PravyUhol - 0.1)) && (uhly[i] < (PravyUhol + 0.1)) && (uhly[j] > (PravyUhol - 0.1)) && (uhly[j] < (PravyUhol + 0.1)))
					{
						printf("i=%d\tj=%d\n", i, j);
						line(src, priesecniky[i], priesecniky[j], Scalar(0, 0, 255), 2);
					}
				}
				
			}
		}
			

		imshow("Lines", src);
		if (waitKey(30) == 27) break;
	}

	waitKey(10);

	return 0;
}