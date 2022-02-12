#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdlib.h>

using namespace cv;
using namespace std;

cv::Mat left_gray;
cv::Mat right_gray;
cv::Mat result;
bool descriptor_left[64];
bool descriptor_right[64];
int cols, rows;

double SSD(Mat& a, Mat& b, int y, int x, int k)
{
	double d_sum = 0;

	for (int i = -10; i < 11; ++i)
	{
		for (int j = -10; j < 11; ++j)
		{
			if (y + i >= 0 && x + j >= 0 && x - k + j)
				d_sum += pow(((double)a.at<uchar>(y + i, x + j) - (double)b.at<uchar>(y + i, x - k + j)), 2);
		}
	}
	return d_sum;
}

int start(Mat& a, Mat& b, int y, int x)
{

	int pix = 0;
	int ct = 0;
	int u = 0;
	int k = 1;

	double min1 = 10000000.0f;
	double temp1 = 0;

	while (x - k > 0 && ct < 50)
	{
		temp1 = SSD(a, b, y, x, k);

		if (min1 > temp1)
		{
			min1 = temp1;
			pix = x - k;
			u = k;
		}
		ct++;
		k++;
	}
	return abs(pix - x);
}

int main()
{
	cv::Mat left = cv::imread("im_left_test.png"); //왼쪽 이미지
	cv::Mat right = cv::imread("im_right_test.png"); //오른쪽 이미지

													 //그레이 이미지로 변환
	cv::cvtColor(left, left_gray, CV_BGR2GRAY);
	cv::cvtColor(right, right_gray, CV_BGR2GRAY);
	cv::cvtColor(left, result, CV_BGR2GRAY);
	cols = left_gray.cols;
	rows = left_gray.rows;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			result.at<uchar>(i, j) = start(left_gray, right_gray, i, j) * 4;
			printf("(%d, %d)\n", i, j);
		}
	}
}