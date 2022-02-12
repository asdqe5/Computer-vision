#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.141592f

///////////////////////////////////////////////////////
// ���� �迭 ���� �ִ밪�� ������ threshold ���� ����!!
#define thres_rate 0.55f 
///////////////////////////////////////////////////////

int main()
{
	//�̹��� �ε�
	cv::Mat image1 = cv::imread("input2.jpg"); // for line detection
	cv::Mat image2 = cv::imread("input2.jpg"); // for lane detection

	//ĳ�� �˰����� ���� �׷��� ������ �̹����� ��ȯ
	cv::Mat gray;
	cv::cvtColor(image1, gray, CV_BGR2GRAY);

	// ĳ�� �˰��� ����
	cv::Mat contours;
	cv::Canny(image1, contours, 100, 350);	//100, 350�� ���� threshold��.

	cv::namedWindow("contours");
	cv::imshow("contours", contours);

	//�̹��� ����, ����, �밢��
	const int width = image1.cols;
	const int height = image1.rows;
	const int diagonal = sqrt(width * width + height * height);

	int theta = 180; //���� �迭 ����
	int p = 0; // ���� �迭 ����
	int thres_line = -1; // �����̱� ���� �Ӱ� �ʱⰪ
	int thres_lane = -1; // ���� ������ ���� �ʱⰪ
	
	// ������������ ���� sin, cos �� �ʱ�ȭ
	float* pre_sin = new float[theta + 1];
	float* pre_cos = new float[theta + 1];

	// -90<= theta <= 90
	for (int i = 0; i <= theta; i++)
	{
		pre_sin[i] = sin((i - 90) * PI / theta);
		pre_cos[i] = cos((i - 90) * PI / theta);
	}

	//���� �迭�� ũ�� = (2 * diagonal) * (2 * 90)
	//���� �迭 �ʱ�ȭ
	int h_size = (diagonal * 2) * (90 * 2) + 2;
	int *hough = new int[h_size];

	for (int i = 0; i < h_size; i++)
		hough[i] = 0;

	//���� �迭 ���ϱ�
	for (int i = 0; i < height; i++) // i = y��ǥ
	{
		for (int j = 0; j < width; j++) // j = x��ǥ
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
	////////////////���⴩���迭 �����Ϸ����///////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	//�����迭 ���� �� �ִ밪 ���� ( ������ �Ǳ� ���� �Ӱ谪 ������ ���� )
	for (int i = 0; i < h_size; i++)
	{
		if (hough[i] >= thres_line)
			thres_line = hough[i];
	}

	//���� �ִ밪�� �̸� ���س��� �Ӱ������ ����. ��ǲ �̹������� ���� ����. 
	thres_line *= thres_rate;
	
	/*
	��1. �Ӱ�ġ �̻��� ���� ����.
	       x����, y������ ������ �̿��Ͽ� ���� �׸���.
	*/
	for (int i = 0; i < theta; i++)
	{
		for (int j = 0; j < diagonal * 2; j++)
		{
			if (hough[j * theta + i] >= thres_line)
			{
				if (j / pre_cos[i] > 0 && j / pre_sin[i] > 0)
				{
					cv::Point start_point(0, j / pre_cos[i]);		//line�� ������ 
					cv::Point end_point(j / pre_sin[i], 0);	//line�� ����
					cv::line(image1, start_point, end_point, cv::Scalar(255, 0, 0), 1);

					cv::Point start_point2(0, j / pre_cos[i]);		//line�� ������
					cv::Point end_point2(width - 1, (j - (width - 1) * pre_sin[i]) / pre_cos[i]);	//line�� ����
					cv::line(image1, start_point2, end_point2, cv::Scalar(255, 0, 0), 1);

					cv::Point start_point3(j / pre_sin[i], 0);		//line�� ������
					cv::Point end_point3((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line�� ����
					cv::line(image1, start_point3, end_point3, cv::Scalar(255, 0, 0), 1);
				}

				else if (j / pre_cos[i] >= 0 && j / pre_sin[i] <= 0)
				{
					cv::Point start_point(0, j / pre_cos[i]);		//line�� ������ 

					if (i < 135)
					{
						cv::Point end_point2((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line�� ����
						cv::line(image1, start_point, end_point2, cv::Scalar(255, 0, 0), 1);
					}

					else
					{
						cv::Point end_point(width - 1, (j - (width - 1) * pre_sin[i]) / pre_cos[i]);	//line�� ����
						cv::line(image1, start_point, end_point, cv::Scalar(255, 0, 0), 1);
					}
				}

				else if (j / pre_cos[i] <= 0 && j / pre_sin[i] >= 0)
				{
					
					cv::Point start_point(j / pre_sin[i], 0);		//line�� ������ 

					cv::Point end_point(width - 1, (j - (width - 1) * pre_sin[i]) / pre_cos[i]);	//line�� ����
					cv::line(image1, start_point, end_point, cv::Scalar(255, 0, 0), 1);

					cv::Point end_point1((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line�� ����
					cv::line(image1, start_point, end_point1, cv::Scalar(255, 0, 0), 1);
				}

				else
					continue;
			}
		}
	}

	//����� ������ �׷��� �̹��� = Result1
	cv::namedWindow("Result1");
	cv::imshow("Result1", image1);

	//�̹��� ����
	cv::imwrite("Result2_1.jpg", image1);

	/*
	- ���� ������ ���� ���� ����
	- ���� Ÿ���ִ� '��'�� �������� ���� ������ ������ ������ ���� ����
	- right_thres : -90������ 0�� ���̿� �ִ� ������ �����ϱ� ���� �Ӱ�ġ
	- left_thres : 0������ 90�� ���̿� �ִ� ������ �����ϱ� ���� �Ӱ�ġ
	- theta_range : ����� �� �� ���� �� �ʿ� �ִ� ���� ����( �߾Ӽ� )
	*/
	int left_thres = -1;
	int right_thres = -1;
	int theta_range[2] = { 0, };

	// �ҽ���(�¿������� ����)�� ���ϱ� ���� ������ ������ ����
	// m�� b�� ������ ����, y����
	// x, y �� �ҽ��� ��ǥ
	float m[2] = { 0 };
	float b[2] = { 0 };
	float x = 0;
	float y = 0;

	
	for (int i = 0; i < diagonal * 2; i++)
	{
		// ���� ���� ���� �� ������ ������
		for (int j = 0; j < 90; j++)
		{
			if (right_thres <= hough[i * theta + j])
			{
				right_thres = hough[i * theta + j];

				m[0] = -pre_sin[j] / pre_cos[j];
				b[0] = (i / pre_cos[j]);
			}
		}

		// ���� ���� ���� �� ������ ������
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

	// ���� ������ ���� ���ο� �Ӱ谪 ����
	thres_lane = (right_thres + left_thres) / 5;

	// �ҽ��� ��ǥ ���ϱ�
	x = (b[1] - b[0]) / (m[0] - m[1]);
	y = m[0] * x + b[0];

	// �� �� ������ ���� ��ǥ�ϴ� �ϳ��� ������ �׷���.
	for (int i = 0; i < theta; i++)
	{
		for (int j = 0; j < diagonal * 2; j++)
		{
			if (hough[j * theta + i] >= left_thres)
			{
				theta_range[0] = i;

				cv::Point start_point3(x, y);		//line�� ������ = �ҽ���
				cv::Point end_point3((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line�� ����
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);

				j = diagonal * 2;
			}

			if (hough[j * theta + i] >= right_thres)
			{
				theta_range[1] = i;

				cv::Point start_point3(x, y);		//line�� ������ = �ҽ���
				cv::Point end_point3((j - (height - 1) * pre_cos[i]) / pre_sin[i], height - 1);	//line�� ����
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);

				j = diagonal * 2;

			}
		}
	}

	//�߾Ӽ� ������ ���� �Ӱ�ġ ����
	thres_lane *= thres_rate;

	int tmp = 0;

	//theta ������ �ùٸ� ������ ���� ��������
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
				cv::Point start_point3(x, y);		//line�� ������
				cv::Point end_point3((i - (height - 1) * pre_cos[j]) / pre_sin[j], height - 1);	//line�� ����
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);
			}
		}

		for (int j = theta_range[1] + 30; j < theta; j++)
		{
			if (hough[i * theta + j] >= thres_lane)
			{
				cv::Point start_point3(x, y);		//line�� ������
				cv::Point end_point3((i - (height - 1) * pre_cos[j]) / pre_sin[j], height - 1);	//line�� ����
				cv::line(image2, start_point3, end_point3, cv::Scalar(255, 0, 0), 3);

				j = theta;
				i = diagonal * 2;
			}
		}
	}




	cv::namedWindow("Result2");
	cv::imshow("Result2", image2);

	//�̹��� ����
	cv::imwrite("Result2_2.jpg", image2);

	cv::waitKey(0);

	return 0;

}