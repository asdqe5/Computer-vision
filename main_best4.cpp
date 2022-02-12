#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define search_thres 40
#define patch_size 31

//1 x 9 sort를 위한 shift값 저장
vector<vector<int> > shift_stack;

// 3x3 median filter
int median(Mat& standard, int y, int x, int w)
{
	int temp = 0;
	vector<int> s;

	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			s.push_back(standard.at<uchar>(y + i, x + j));
		}
	}

	for (int i = 9; i > 0; i--)
	{
		for (int j = 0; j < i - 1; j++)
		{
			if (s[j] > s[j + 1])
			{
				temp = s[j];
				s[j] = s[j + 1];
				s[j + 1] = temp;
			}
		}
	}

	return s[4];
}

//Census transform
int cal_CT(Mat& standard, Mat& compare, int y, int x, int shift)
{
	vector<int> ref; //standard
	vector<int> target; //compare

	for (int i = -patch_size / 2 + 1; i < patch_size / 2 + 1; ++i)
	{
		for (int j = -patch_size / 2 + 1; j < patch_size / 2 + 1; ++j)
		{
			if (y + i >= 0 && x + j >= 0 && x - shift + j >= 0 && y+i <= standard.rows && x - shift + j <= standard.cols)
			{
				printf("y+i, x+j, rows, cols:, %d %d %d %d\n", y + i, x + j, standard.rows, standard.cols);
				if (standard.at<uchar>(y, x) > standard.at<uchar>(y + i, x + j))
					ref.push_back(1);
				else
					ref.push_back(0);

				if (compare.at<uchar>(y, x - shift) > compare.at<uchar>(y + i, x - shift + j))
					target.push_back(1);
				else
					target.push_back(0);
			}
			
		}
	}

	int c = 0;
	for (int i = 0; i < ref.size(); i = i+1)
	{
		if (ref[i] != target[i])
			c++;
	}

	return c;
}

//find minimum value for stereo match
int stereo_match(Mat& standard, Mat& compare, int y, int x)
{
	int min = 1000000;
	int temp = 0;

	int min_pix = 0;
	int shift = 1;
	int count = 0;
	int p = 0;

	while (x - shift > 0 && count < search_thres)
	{
		temp = cal_CT(standard, compare, y, x, shift);

		if (min > temp)
		{
			min = temp;
			min_pix = x - shift;
			p = shift;
		}

		shift++;
		count++;
	}

	shift_stack.push_back(vector<int>());
	shift_stack[y].push_back(p);

	return abs(min_pix- x);
}

//왼쪽영상과 오른쪽 영상은 일치하지 않기 때문에 일정영역에 대한 shift값은 찾을 수 없다. -> 후처리를 위한 함수
int back_sort(int y, int x, int w)
{
	int temp = 0;
	vector<int> s;

	for (int i = 0; i < 9; ++i)
		s.push_back(shift_stack[y][search_thres + i]);

	for (int i = 9; i > 0; i--)
	{
		for (int j = 0; j < i - 1; j++)
		{
			if (s[j] > s[j + 1])
			{
				temp = s[j];
				s[j] = s[j + 1];
				s[j + 1] = temp;
			}
		}
	}

	return s[4];
}

//영상은 x축으로만 shift되므로 노이즈에 대한 임펄스 shift의 처리로 1 x 9 median sort 적용 
int sort(int y, int x, int w)
{
	int temp = 0;
	vector<int> s;

	if (x - search_thres < 0)
		return back_sort(y,x,w);
	
	if (x + 4 > w)
		return shift_stack[y][x];
		

	else
	{
		for (int i = - 4; i < 5; ++i)
			s.push_back(shift_stack[y][x + i]);

		for (int i = 9; i > 0; i--)
		{
			for (int j = 0; j < i - 1; j++)
			{
				if (s[j] > s[j + 1])
				{
					temp = s[j];
					s[j] = s[j + 1];
					s[j + 1] = temp;
				}
			}
		}

		return s[4];
	}
}

void main()
{
	//right가 기준영상. -> left영상에서 현재위치를 기준으로 right 탐색
	Mat left = imread("im_left_test.png");
	Mat left_gry;
	Mat right = imread("im_right_test.png");
	Mat right_gry;

	Mat left_res;
	Mat res;
	
	/*
	최종 결과 저장 : final_res
	*/
	Mat final_res;

	int w = left.cols;
	int h = left.rows;
	
	cvtColor(left, left_gry, CV_RGB2GRAY);
	cvtColor(right, right_gry, CV_RGB2GRAY);

	cvtColor(left, left_res, CV_RGB2GRAY); //정렬 전
	cvtColor(right, res, CV_RGB2GRAY); //정렬 1번 후
	cvtColor(right, final_res, CV_RGB2GRAY); //정렬 2번 후

	for (int i = 0; i < h ; ++i)
	{
		for (int j = 0; j < w ; ++j)
		{
			left_res.at<uchar>(i, j) = stereo_match(left_gry, right_gry, i, j);
			printf("(%d, %d) 완료1\n", i, j);
		}
	}

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			res.at<uchar>(i, j) = sort(i, j, w);
			printf("(%d, %d) 완료2\n", i, j);
		}
	}

	for (int i = 1; i < h - 1; ++i)
	{
		for (int j = 1; j < w - 1; ++j)
		{
			printf("(%d, %d) 완료3\n", i, j);
			final_res.at<uchar>(i, j) = median(res, i, j, w);
		}
	}

	////////////////////////////////////////////////////////////////
	///////////////////////MSE/////////////////////////////////////
	////////////////////////////////////////////////////////////////

	int MSE = 0;
	Mat anw = imread("disp_left.png");
	cvtColor(anw, anw, CV_RGB2GRAY);

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			if (anw.at<uchar>(i, j) != 0)
				MSE += abs(anw.at<uchar>(i, j) / 4 - final_res.at<uchar>(i, j));
		}
	}

	printf("MSE : %d\n", MSE);

	imshow("res", final_res);
	imwrite("res.png", final_res);

	waitKey(0);

	system("pause");
}