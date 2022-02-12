#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

int main()
{
	//�̹��� �ε�
	cv::Mat image = cv::imread("input.jpg");

	//ĳ�� �˰����� ���� �׷��� ������ �̹����� ��ȯ
	cv::Mat gray;
	cv::cvtColor(image, gray, CV_BGR2GRAY);

	// ĳ�� �˰��� ����
	cv::Mat contours;
	cv::Canny(gray, contours, 100, 350);	//100, 350�� ���� threshold��.

	//cv::namedWindow("contours");
	//cv::imshow("contours", contours);

    //�̹��� ����, ����, �밢��
	const int width = image.cols;
	const int height = image.rows;
	const int diagonal = sqrt(width * width + height * height);
	
	//���� ��ȯ ����
	int theta = 180;
	double dis;
	int max1 = 0;
	int max1_theta = 0;
	int max2 = 0;
	int max2_theta = 0;
	int max3 = 0;

	vector<vector<int>> A(diagonal * 2, vector<int>(theta, 0));
	
	double DegToRad = CV_PI / 180;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (contours.at<uchar>(i,j) == 255)
			{
				for (int th = 0; th < 180; th += 1)
				{
					dis = round(j * cos((th - 90) * DegToRad) + i * sin((th - 90) * DegToRad)) + diagonal;
					A[dis][th] += 1;
				}
			}
		}
	}
	//������ �����ϱ� ���� ������ ������ �ִ밪�� ���� ���� �� ���� ��Ÿ�� ���Ѵ�
	for (int i = 0; i < A.size(); i++)
	{
		for (int j = 0; j < A[i].size() - 90; j++)
		{
			if (A[i][j] > max1)
			{
				max1 = A[i][j];
				max1_theta = j;
			}
		}
		for (int j = 90; j < A[i].size(); j++)
		{
			if (A[i][j] > max1)
			{
				max2 = A[i][j];
				max2_theta = j;
			}
		}
	}

	//�� ������ �߾ӿ� �ִ� �߾Ӽ��� ���� ��Ÿ���� ���Ѵ�.
	for (int i = 0; i < A.size(); i++)
	{
		for (int j = max1_theta + 10; j < max2_theta - 10; j++)
		{
			if (A[i][j] > max3)
			{
				max3 = A[i][j];
			}
		}
	}
	
	//������Ŀ��� �� �������� �ִ밪�� �͵��� �̾Ƴ��� ��ǥ���� �̾Ƴ���.
	cv::Point start_point1, end_point1, start_point2, end_point2, start_point3, end_point3;
	double th;
	double x0, y0;
	for (int i = 0; i < A.size(); i++)
	{
		for (int j = 0; j < A[i].size(); j++)
		{
			if (A[i][j] == max1)
			{
				dis = i - diagonal;
				th = (j - 90) * DegToRad;
				x0 = dis * cos(th);
				y0 = dis * sin(th);
				start_point1.x = cvRound(x0 + diagonal * (-sin(th)));
				start_point1.y = cvRound(y0 + diagonal * (cos(th)));
				end_point1.x = cvRound(x0 + diagonal * (sin(th))); 
				end_point1.y = cvRound(y0 + diagonal * (-cos(th)));
			}
			else if (A[i][j] == max2)
			{
				dis = i - diagonal;
				th = (j - 90) * DegToRad;
				x0 = dis * cos(th);
				y0 = dis * sin(th);
				start_point2.x = cvRound(x0 + diagonal * (-sin(th)));
				start_point2.y = cvRound(y0 + diagonal * (cos(th)));
				end_point2.x = cvRound(x0 + diagonal * (sin(th)));
				end_point2.y = cvRound(y0 + diagonal * (-cos(th)));
			}
			else if ((j > max1_theta + 10) && (j < max2_theta - 10) && (A[i][j] == max3))
			{
				dis = i - diagonal;
				th = (j - 90) * DegToRad;
				x0 = dis * cos(th);
				y0 = dis * sin(th);
				start_point3.x = cvRound(x0 + diagonal * (-sin(th)));
				start_point3.y = cvRound(y0 + diagonal * (cos(th)));
				end_point3.x = cvRound(x0 + diagonal * (sin(th)));
				end_point3.y = cvRound(y0 + diagonal * (-cos(th)));
			}
		}
	}

	//������ ������ ���� ���ϱ�
	cv::Point cross_point;
	float increase1, increase2, constant1, constant2;
	increase1 = (float)(end_point1.y - start_point1.y) / (end_point1.x - start_point1.x);
	constant1 = start_point1.y - increase1 * start_point1.x;
	increase2 = (float)(end_point2.y - start_point2.y) / (end_point2.x - start_point2.x);
	constant2 = start_point2.y - increase2 * start_point2.x;
	cross_point.x = -(constant1 - constant2) / (increase1 - increase2);
	cross_point.y = increase1 * cross_point.x + constant1;

	//���� �׸���
	cv::line(image, start_point1, cross_point, cv::Scalar(255, 0, 0), 3);
	cv::line(image, start_point2, cross_point, cv::Scalar(255, 0, 0), 3);
	cv::line(image, start_point3, cross_point, cv::Scalar(255, 0, 0), 3);
	cv::namedWindow("Result");
	cv::imshow("Result", image);
	
	//�̹��� ����
	cv::imwrite("Result.jpg", image);
	cv::waitKey(0);
	return 0;
}
