#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

int main()
{
	//이미지 로드
	cv::Mat image = cv::imread("input.jpg");

	//캐니 알고리즘을 위해 그레이 스케일 이미지로 변환
	cv::Mat gray;
	cv::cvtColor(image, gray, CV_BGR2GRAY);

	// 캐니 알고리즘 적용
	cv::Mat contours;
	cv::Canny(gray, contours, 100, 350);	//100, 350은 각각 threshold값.

	//cv::namedWindow("contours");
	//cv::imshow("contours", contours);

    //이미지 가로, 세로, 대각선
	const int width = image.cols;
	const int height = image.rows;
	const int diagonal = sqrt(width * width + height * height);
	
	//허프 변환 구현
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
	//차선을 인지하기 위해 구역을 나누어 최대값을 갖는 곳의 그 값과 세타를 구한다
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

	//두 차선의 중앙에 있는 중앙선의 값과 세타값을 구한다.
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
	
	//누적행렬에서 각 구역에서 최대값인 것들을 뽑아내어 좌표값을 뽑아낸다.
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

	//차선이 만나는 교점 구하기
	cv::Point cross_point;
	float increase1, increase2, constant1, constant2;
	increase1 = (float)(end_point1.y - start_point1.y) / (end_point1.x - start_point1.x);
	constant1 = start_point1.y - increase1 * start_point1.x;
	increase2 = (float)(end_point2.y - start_point2.y) / (end_point2.x - start_point2.x);
	constant2 = start_point2.y - increase2 * start_point2.x;
	cross_point.x = -(constant1 - constant2) / (increase1 - increase2);
	cross_point.y = increase1 * cross_point.x + constant1;

	//차선 그리기
	cv::line(image, start_point1, cross_point, cv::Scalar(255, 0, 0), 3);
	cv::line(image, start_point2, cross_point, cv::Scalar(255, 0, 0), 3);
	cv::line(image, start_point3, cross_point, cv::Scalar(255, 0, 0), 3);
	cv::namedWindow("Result");
	cv::imshow("Result", image);
	
	//이미지 저장
	cv::imwrite("Result.jpg", image);
	cv::waitKey(0);
	return 0;
}
