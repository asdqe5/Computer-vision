#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <vector>
#include <time.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

int randomRange(int n1, int n2);

int main(void)
{
	//이미지 로드
	Mat image = imread("img1.bmp");
	Mat image_gray;
	cvtColor(image, image_gray, CV_BGR2GRAY);
	int range = 50;
	int *brief = new int[256];
	//각 픽셀에서 range에 맞는 두 점을 뽑아내기 위한 random point 배열을 -range부터 range까지의 랜덤값으로 초기화한다. 
	int **rand_point = new int *[256];
	for (int i = 0; i < 256; i++)
	{
		rand_point[i] = new int[4];
	}
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			rand_point[i][j] = randomRange(-range, range);
		}
	}
	const int width = image_gray.cols;
	const int height = image_gray.rows;

	//descriptor를 영상으로 표현하기 위해 Mat을 선언한다.
	Mat BRIEF(height, width, CV_8UC1);

	//각 픽셀마다 random_point의 값을 통해서 256개의 라인, 즉 각 픽셀을 중심으로 두개의 점을 구하고 그 두개의 점을 비교하여 첫번째 점이 크면 1로, 그렇지 않으면 0으로 하는 256비트의 descriptor를 만든다.
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int a = 0; a < 256; a++)
			{
				//각 픽셀을 중심으로 두 개의 점을 구하고 이미지를 벗어났을 시 예외처리를 해준다.
				int i1 = i + rand_point[a][0];
				if (i1 < 0)
					i1 = 0;
				else if (i1 > height - 1)
					i1 = height -1;
				int i2 = i + rand_point[a][1];
				if (i2 < 0)
					i2 = 0;
				else if (i2 > height - 1)
					i2 = height - 1;
				int j1 = j + rand_point[a][2];
				if (j1 < 0)
					j1 = 0;
				else if (j1 > width-1)
					j1 = width-1;
				int j2 = j + rand_point[a][3];
				if (j2 < 0)
					j2 = 0;
				else if (j2 > width-1)
					j2 = width-1;
				//descriptor인 brief 배열을 1과 0으로 초기화해준다.
				if (image_gray.at<uchar>(i1, j1) > image_gray.at<uchar>(i2, j2))
					brief[a] = 1;
				else
					brief[a] = 0;
			}
			//256비트의 descriptor에서 상위 8개의 비트만 꺼내어 이미지로 만들기 위해 0~255 사이의 수로 바꿔준다.
			BRIEF.at<uchar>(i,j) = (int)(brief[0] * pow(2.0, 7.0) + brief[1] * pow(2.0, 6.0) + brief[2] * pow(2.0, 5.0) + brief[3] * pow(2.0, 4.0) + brief[4] * pow(2.0, 3.0) + brief[5] * pow(2.0, 2.0) + brief[6] * pow(2.0, 1.0) + brief[7] * pow(2.0, 0.0));
		}
	}
	imshow("BRIEF", BRIEF);
	cv::waitKey(0);

	cv::imwrite("Result1.bmp", BRIEF);
	return 0;
}
int randomRange(int n1, int n2)
{
	return (int)(rand() % (n2 - n1 + 1)) + n1;
}