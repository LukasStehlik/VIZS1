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

void Kruhy(Mat ColMat, Mat *src, String Color);
void Objekty(Mat ColMat, Mat *src, String Color);
void MeanLines(vector<Vec2f> lines, vector<Vec2f> *linesFiltered);
float Sum(vector<float> x);
Point CenterCalc(vector<Point> points);
float Lengths(vector<Point> points, vector<float> *dlzka);

#define PravyUhol 90

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
		Mat src = imread(cesta, IMREAD_UNCHANGED);
		
		//cap >> src;
		if (src.empty())
		{
			cout << "can not open ";
			return -1;
		}

		Mat AFilter, HSV_Image, Rmat, Gmat, Bmat, HoughSource, Temp;
		GaussianBlur(src, AFilter, Size(3, 3), 0, 0); //Rozmazáva hrany
		//medianBlur(AFilter, AFilter, 5);
		cvtColor(AFilter, HSV_Image, CV_BGR2HSV);

		inRange(HSV_Image, Scalar(100, 100, 20),Scalar(140,255,255),Bmat);
		inRange(HSV_Image, Scalar(20, 100, 20), Scalar(95, 255, 255), Gmat);
		inRange(HSV_Image, Scalar(0, 100, 20), Scalar(15, 255, 255), Rmat);
		inRange(HSV_Image, Scalar(160, 100, 20), Scalar(180, 255, 255), Temp);
		addWeighted(Rmat, 1.0, Temp, 1.0, 0.0, Rmat);

		GaussianBlur(Rmat, Rmat, Size(5, 5), 3, 3);
		GaussianBlur(Gmat, Gmat, Size(5, 5), 3, 3);
		GaussianBlur(Bmat, Bmat, Size(5, 5), 3, 3);
		//imshow("Rmat", Rmat);
		//imshow("Gmat", Gmat);
		//imshow("Bmat", Bmat);

		Kruhy(Rmat, &src, "R");
		Kruhy(Gmat, &src, "G");
		Kruhy(Bmat, &src, "B");

		Objekty(Rmat, &src, "R");
		Objekty(Gmat, &src, "G");
		Objekty(Bmat, &src, "B");

		imshow("Detekcia", src);
		printf("\n");
		if (waitKey(30) == 27) break;
	}

	waitKey(10);

	return 0;
}

void Kruhy(Mat ColMat, Mat *src, String Color)
{
	vector<Vec3f> circles;
	HoughCircles(ColMat, circles, HOUGH_GRADIENT, 2, ColMat.rows / 4, 200, 100, 0, 0);

	printf("Pocet kruhov: %u\n", (uint16_t)circles.size());

	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		printf("x=%i y=%i\n", center.x, center.y);

		circle(*src, center, radius, Scalar(50, 50, 50), 3, 8, 0);
		putText(*src, Color + "Circle", center, FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);
	}
}

void Objekty(Mat ColMat, Mat *src, String Color)
{
	Mat Keny;
	Canny(ColMat, Keny, 50, 200, 3);
	//imshow("Kenny", Keny);

	vector<Vec2f> lines;
	HoughLines(Keny, lines, 1, CV_PI / 180, 50, 0, 0);
	printf("Pocet ciar: %u\n", (uint16_t)lines.size());

	//Vypísanie lines
	for (uint i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		printf("rho=%.2f\ttheta=%.2f\n", rho, theta);
	}
	printf("\n");

	vector<Vec2f> linesFiltered;
	MeanLines(lines, &linesFiltered);

	printf("Pocet ciar filtrovanych: %u\n", (uint16_t)linesFiltered.size());
	
	//Vypísanie lines filtrovaných
	for (uint i = 0; i < linesFiltered.size(); i++)
	{
		float rho = linesFiltered[i][0], theta = linesFiltered[i][1];
		printf("rho=%.2f\ttheta=%.2f\n", rho, theta);
	}
	printf("\n");

	vector<Point> priesecniky;
	vector<float> uhly;

	for (uint i = 0; i < linesFiltered.size(); i++)
	{
		for (uint j = i + 1; j < linesFiltered.size(); j++)
		{
			float rho1 = linesFiltered[i][0], rho2 = linesFiltered[j][0];
			float theta1 = linesFiltered[i][1], theta2 = linesFiltered[j][1];
			if (abs(theta2 - theta1) > 0.1)
			{
				int x = (int)((rho1*sin(theta2) - rho2*sin(theta1)) / sin(theta2 - theta1));
				int y = (int)((rho2*cos(theta1) - rho1*cos(theta2)) / sin(theta2 - theta1));
				//circle(*src, Point(x, y), 5, Scalar(0, 0, 255), 3, 8, 0);
				priesecniky.push_back(Point(x, y));
				float uhol = (float)(abs(theta2 - theta1) * 180 / CV_PI);
				uhol = uhol > 90 ? 180 - uhol : uhol;
				uhly.push_back(uhol);
				//printf("x=%d\ty=%d\n", x, y);
			}
			else printf("Nema presecnik\n");
		}
	}

	float uhlySum = Sum(uhly);
	if (priesecniky.size() == 4 && uhlySum > 345 && uhlySum < 375)
	{
		vector<float> dlzky;
		float minDlzka = Lengths(priesecniky, &dlzky);
		float meanDlzky = Sum(dlzky)/6.82;

		for (uint i = 0, index = 0; i < priesecniky.size(); i++)
		{
			for (uint j = i + 1; j < priesecniky.size(); j++, index++)
			{
				//printf("uhol=%f\t", uhly[i]);
				//printf("i=%d\tj=%d\n", i, j);
				if (meanDlzky < minDlzka*1.1) //Štvorec
				{
					if (dlzky[index]<minDlzka*1.2) line(*src, priesecniky[i], priesecniky[j], Scalar(50, 50, 50), 2);
				}
				else //Obdåžnik
				{
					if (dlzky[index]<minDlzka*1.6) line(*src, priesecniky[i], priesecniky[j], Scalar(50, 50, 50), 2);
				}
			}
		}
		if (meanDlzky < minDlzka*1.1) putText(*src, Color + "Square", CenterCalc(priesecniky), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);
		else putText(*src, Color + "Rectangle", CenterCalc(priesecniky), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);
	}

	if (priesecniky.size() == 3 && uhlySum > 165 && uhlySum < 195)
	{
		for (uint i = 0; i < priesecniky.size(); i++)
		{
			for (uint j = i + 1; j < priesecniky.size(); j++)
			{
				//printf("uhol=%f\t", uhly[i]);
				//printf("i=%d\tj=%d\n", i, j);
				line(*src, priesecniky[i], priesecniky[j], Scalar(50, 50, 50), 2);
			}
		}
		putText(*src, Color + "Triangle", CenterCalc(priesecniky), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);
	}
}

void MeanLines(vector<Vec2f> lines, vector<Vec2f> *linesFiltered)
{
	vector<uint> pocetnost;

	for (uint i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		if (rho < 0)
		{
			rho = abs(rho);
			theta -= (float)CV_PI;
		}
		//if (theta < 0) theta += (float)CV_PI;

		for (uint j = 0; j <= (*linesFiltered).size(); j++)
		{
			if (j < (*linesFiltered).size())
			{
				float rhoM = (*linesFiltered)[j][0] / pocetnost[j];
				float thetaM = (*linesFiltered)[j][1] / pocetnost[j];
				if (abs(rho - rhoM) < 30 && abs(theta - thetaM) < (15 * CV_PI / 180))
				{
					(*linesFiltered)[j][0] += rho;
					(*linesFiltered)[j][1] += theta;
					pocetnost[j]++;
					break;
				}
			}
			else
			{
				(*linesFiltered).push_back(Vec2f(rho, theta));
				pocetnost.push_back(1);
				break;
			}
		}
	}
	for (uint i = 0; i < (*linesFiltered).size(); i++)
	{
		(*linesFiltered)[i][0] /= pocetnost[i];
		(*linesFiltered)[i][1] /= pocetnost[i];
	}
}

float Sum(vector<float> x)
{
	float sum = 0;
	for (uint i = 0; i < x.size(); i++) sum += abs(x[i]);
	return sum;
}

Point CenterCalc(vector<Point> points)
{
	Point center = Point(0, 0);

	for (uint i = 0; i < points.size(); i++)
	{
		center.x += points[i].x;
		center.y += points[i].y;
	}
	center.x /= (int)points.size();
	center.y /= (int)points.size();
	return center;
}

float Lengths(vector<Point> points, vector<float> *dlzka)
{
	for (uint i = 0; i < points.size(); i++)
	{
		for (uint j = i + 1; j < points.size(); j++)
		{
			float l = (float)sqrt(pow(points[i].x - points[j].x, 2) + pow(points[i].y - points[j].y, 2));
			(*dlzka).push_back(l);
		}
	}
	float minDlzka = (*dlzka)[0];
	for (uint i = 1; i < (*dlzka).size(); i++)
	{
		if ((*dlzka)[i] < minDlzka) minDlzka = (*dlzka)[i];
	}
	return minDlzka;
}