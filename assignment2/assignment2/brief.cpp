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
	//�̹��� �ε�
	Mat image = imread("img1.bmp");
	Mat image_gray;
	cvtColor(image, image_gray, CV_BGR2GRAY);
	int range = 50;
	int *brief = new int[256];
	//�� �ȼ����� range�� �´� �� ���� �̾Ƴ��� ���� random point �迭�� -range���� range������ ���������� �ʱ�ȭ�Ѵ�. 
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

	//descriptor�� �������� ǥ���ϱ� ���� Mat�� �����Ѵ�.
	Mat BRIEF(height, width, CV_8UC1);

	//�� �ȼ����� random_point�� ���� ���ؼ� 256���� ����, �� �� �ȼ��� �߽����� �ΰ��� ���� ���ϰ� �� �ΰ��� ���� ���Ͽ� ù��° ���� ũ�� 1��, �׷��� ������ 0���� �ϴ� 256��Ʈ�� descriptor�� �����.
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int a = 0; a < 256; a++)
			{
				//�� �ȼ��� �߽����� �� ���� ���� ���ϰ� �̹����� ����� �� ����ó���� ���ش�.
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
				//descriptor�� brief �迭�� 1�� 0���� �ʱ�ȭ���ش�.
				if (image_gray.at<uchar>(i1, j1) > image_gray.at<uchar>(i2, j2))
					brief[a] = 1;
				else
					brief[a] = 0;
			}
			//256��Ʈ�� descriptor���� ���� 8���� ��Ʈ�� ������ �̹����� ����� ���� 0~255 ������ ���� �ٲ��ش�.
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