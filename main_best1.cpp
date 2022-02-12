#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.141592f

///////////////////////////////////////////////////////
// 누적 배열 원소 최대값에 곱해줄 threshold 비율 설정!!
#define thres_rate 0.55f 
///////////////////////////////////////////////////////

int main()
{
	//이미지 로드
	cv::Mat image1 = cv::imread("input2.jpg"); // for line detection
	cv::Mat image2 = cv::imread("input2.jpg"); // for lane detection

	//캐니 알고리즘을 위해 그레이 스케일 이미지로 변환
	cv::Mat gray;
	cv::cvtColor(image1, gray, CV_BGR2GRAY);

	// 캐니 알고리즘 적용
	cv::Mat contours;
	cv::Canny(image1, contours, 100, 350);	//100, 350은 각각 threshold값.

	cv::namedWindow("contours");
	cv::imshow("contours", contours);

	//이미지 가로, 세로, 대각선
	const int width = image1.cols;
	const int height = image1.rows;
	const int diagonal = sqrt(width * width + height * height);

	int theta = 180; //누적 배열 가로
	int p = 0; // 누적 배열 세로
	int thres_line = -1; // 직선이기 위한 임계 초기값
	int thres_lane = -1; // 차선 검출을 위한 초기값
	
	// 허프공간에서 쓰일 sin, cos 값 초기화
	float* pre_sin = new float[theta + 1];
	float* pre_cos = new float[theta + 1];

	// -90<= theta <= 90
	for (int i = 0; i <= theta; i++)
	{
		pre_sin[i] = sin((i - 90) * PI / theta);
		pre_cos[i] = cos((i - 90) * PI / theta);
	}

	//누적 배열의 크기 = (2 * diagonal) * (2 * 90)
	//누적 배열 초기화
	int h_size = (diagonal * 2) * (90 * 2) + 2;
	int *hough = new int[h_size];

	for (int i = 0; i < h_size; i++)
		hough[i] = 0;

	//누적 배열 구하기
	for (int i = 0; i < height; i++) // i = y좌표
	{
		for (int j = 0; j < width; j++) // j = x좌표
		{
			if (contours.at<uchar>(i, j) == 0)
				continue;
			for (int k = 0; k <= theta; k++)
			{
				p = (int)(i * pre_cos[k] + j * pre_sin[k]);
				if (p >= 0)
					hough[p * theta + k] ++;
			}
		}
	}

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////△△△누적배열 생성완료△△△///////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	//누적배열 원소 중 최대값 검출 ( 직선이 되기 위한 임계값 설정을 위함 )
	for (int i = 0; i < h_size; i++)
	{
		if (hough[i] >= thres_line)
			thres_line = hough[i];
	}

	//원소 최대값에 미리 정해놓은 임계비율을 곱함. 인풋 이미지마다 비율 조정. 
	thres_line *= thres_rate;
	
	/*
	평가1. 임계치 이상의 직선 검출.
	       x절편, y절편의 범위를 이용하여 직선 그리기.
	*/
	for (int i = 0; i < theta; i++)
	{
		for (int j = 0; j < diagonal * 2; j++)
		{
			if (hough[j * theta + i] >= thres_line)
			{
				if (j / pre_cos[i] > 0 && j / pre_sin[i] > 0)
				{
					cv::Point start_point(0, j / pre_cos[i]);		//line의 시작점 
					cv::Point end_point(j / pre_sin[i], 0);	//line의 끝점
					cv::line(image1, start_point, end_point, cv::Scalar(255, 0, 0), 1);

					cv::Point start_point2(0, j / pre_cos[i]);		//line의 시작점
					cv::Point end_point2(width - 1, (j - (width - 1) * pre_sin[i]) / pre_cos[i]);	//line의 끝점
					cv::line(image1, start_point2, end_point2, cv::Scalar(255, 0, 0), 1);

					cv::Point start_point3(j / pre_sin[i], 0);		//line의 시작점
					cv::Point end_point3((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line의 끝점
					cv::line(image1, start_point3, end_point3, cv::Scalar(255, 0, 0), 1);
				}

				else if (j / pre_cos[i] >= 0 && j / pre_sin[i] <= 0)
				{
					cv::Point start_point(0, j / pre_cos[i]);		//line의 시작점 

					if (i < 135)
					{
						cv::Point end_point2((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line의 끝점
						cv::line(image1, start_point, end_point2, cv::Scalar(255, 0, 0), 1);
					}

					else
					{
						cv::Point end_point(width - 1, (j - (width - 1) * pre_sin[i]) / pre_cos[i]);	//line의 끝점
						cv::line(image1, start_point, end_point, cv::Scalar(255, 0, 0), 1);
					}
				}

				else if (j / pre_cos[i] <= 0 && j / pre_sin[i] >= 0)
				{
					
					cv::Point start_point(j / pre_sin[i], 0);		//line의 시작점 

					cv::Point end_point(width - 1, (j - (width - 1) * pre_sin[i]) / pre_cos[i]);	//line의 끝점
					cv::line(image1, start_point, end_point, cv::Scalar(255, 0, 0), 1);

					cv::Point end_point1((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line의 끝점
					cv::line(image1, start_point, end_point1, cv::Scalar(255, 0, 0), 1);
				}

				else
					continue;
			}
		}
	}

	//검출된 직선이 그려진 이미지 = Result1
	cv::namedWindow("Result1");
	cv::imshow("Result1", image1);

	//이미지 저장
	cv::imwrite("Result2_1.jpg", image1);

	/*
	- 차선 검출을 위한 변수 선언
	- 차를 타고있는 '나'를 기준으로 왼쪽 차선과 오른쪽 차선을 각각 검출
	- right_thres : -90도에서 0도 사이에 있는 차선을 검출하기 위한 임계치
	- left_thres : 0도에서 90도 사이에 있는 차선을 검출하기 위한 임계치
	- theta_range : 검출된 좌 우 차선 안 쪽에 있는 차선 검출( 중앙선 )
	*/
	int left_thres = -1;
	int right_thres = -1;
	int theta_range[2] = { 0, };

	// 소실점(좌우차선의 교점)을 구하기 위한 직선의 방정식 변수
	// m과 b는 각각의 기울기, y절편
	// x, y 는 소실점 좌표
	float m[2] = { 0 };
	float b[2] = { 0 };
	float x = 0;
	float y = 0;

	
	for (int i = 0; i < diagonal * 2; i++)
	{
		// 우측 차선 검출 및 직선의 방정식
		for (int j = 0; j < 90; j++)
		{
			if (right_thres <= hough[i * theta + j])
			{
				right_thres = hough[i * theta + j];

				m[0] = -pre_sin[j] / pre_cos[j];
				b[0] = (i / pre_cos[j]);
			}
		}

		// 좌측 차선 검출 및 직선의 방정식
		for (int j = 90; j < theta; j++)
		{
			if (left_thres <= hough[i * theta + j])
			{
				left_thres = hough[i * theta + j];

				m[1] = -pre_sin[j] / pre_cos[j];
				b[1] = (i / pre_cos[j]);
			}
		}
	}

	// 차선 검출을 위한 새로운 임계값 설정
	thres_lane = (right_thres + left_thres) / 5;

	// 소실점 좌표 구하기
	x = (b[1] - b[0]) / (m[0] - m[1]);
	y = m[0] * x + b[0];

	// 좌 우 차선을 각각 대표하는 하나의 직선만 그려줌.
	for (int i = 0; i < theta; i++)
	{
		for (int j = 0; j < diagonal * 2; j++)
		{
			if (hough[j * theta + i] >= left_thres)
			{
				theta_range[0] = i;

				cv::Point start_point3(x, y);		//line의 시작점 = 소실점
				cv::Point end_point3((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line의 끝점
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);

				j = diagonal * 2;
			}

			if (hough[j * theta + i] >= right_thres)
			{
				theta_range[1] = i;

				cv::Point start_point3(x, y);		//line의 시작점 = 소실점
				cv::Point end_point3((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line의 끝점
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);

				j = diagonal * 2;

			}
		}
	}

	//중앙선 검출을 위한 임계치 설정
	thres_lane *= thres_rate;

	int tmp = 0;

	//theta 범위의 올바른 설정을 위해 내림차순
	if (theta_range[0] > theta_range[1])
	{
		tmp = theta_range[0];
		theta_range[0] = theta_range[1];
		theta_range[1] = tmp;
	}

	for (int i = 0; i < diagonal * 2; i++)
	{
		for (int j = 0; j < theta_range[0] - 30; j++)
		{
			if (hough[i * theta + j] >= thres_lane)
			{
				cv::Point start_point3(x, y);		//line의 시작점
				cv::Point end_point3((i - (height - 1) * pre_cos[j]) / pre_sin[j], height - 1);	//line의 끝점
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);
			}
		}

		for (int j = theta_range[1] + 30; j < theta; j++)
		{
			if (hough[i * theta + j] >= thres_lane)
			{
				cv::Point start_point3(x, y);		//line의 시작점
				cv::Point end_point3((i - (height - 1) * pre_cos[j]) / pre_sin[j], height - 1);	//line의 끝점
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);

				j = theta;
				i = diagonal * 2;
			}
		}
	}




	cv::namedWindow("Result2");
	cv::imshow("Result2", image2);

	//이미지 저장
	cv::imwrite("Result2_2.jpg", image2);

	cv::waitKey(0);

	return 0;

}