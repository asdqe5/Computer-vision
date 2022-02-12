#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <math.h>
#include <iostream>
#include<ctime> //srand
#include<cstdlib> //rand
#include<Windows.h> // RAND_MAX

#define DESCRIPTOR_SPACE 4

using namespace std;

int cols, rows;
cv::Mat gray;
cv::Mat result;
bool descriptor[256];

struct Pos {
	int y, x;
};

vector<Pos> pvec;

double Gaussian_Random() {
	double v1, v2, s;

	do {
		v1 = 2 * ((double)rand() / RAND_MAX) - 1;
		v2 = 2 * ((double)rand() / RAND_MAX) - 1;
		s = v1 * v1 + v2 * v2;
	} while (s >= 1 || s == 0);

	s = sqrt((-2 * log(s)) / s);

	return v1 * s * DESCRIPTOR_SPACE;
}

void Find_Descriptor(int y, int x) {
	for (int i = 0; i < 256; i++) {
		int y1 = y + pvec[i * 2].y;
		int x1 = x + pvec[i * 2].x;
		int y2 = y + pvec[i * 2 + 1].y;
		int x2 = x + pvec[i * 2 + 1].x;

		if (y1 < 0 || y2 < 0 || x1 < 0 || x2 < 0) {
			descriptor[i] = 0; //예외처리
			return;
		}
		if (y1 > rows - 1 || y2 > rows - 1 || x1 > cols - 1 || x2 > cols - 1) {
			descriptor[i] = 0;
			return;
		}

		int p1 = gray.at<uchar>(y1, x1);
		int p2 = gray.at<uchar>(y2, x2);

		if (y1 > y2) {
			if (p1 >= p2) {
				descriptor[i] = 1;
			}
			else {
				descriptor[i] = 0;
			}
		}
		else if (y1 == y2) {
			if (x1 >= x2) {
				if (p1 >= p2) {
					descriptor[i] = 1;
				}
				else {
					descriptor[i] = 0;
				}
			}
			else
			{
				if (p1 > p2) {
					descriptor[i] = 0;
				}
				else
				{
					descriptor[i] = 1;
				}
			}
		}
		else {
			if (p1 > p2) {
				descriptor[i] = 0;
			}
			else
			{
				descriptor[i] = 1;
			}
		}
	}
}

int main()
{
	//이미지 로드
	cv::Mat image = cv::imread("img1.bmp");
	
	//그레이 이미지로 변환
	cv::cvtColor(image, gray, CV_BGR2GRAY);
	result = gray.clone();
	
	cols = gray.cols;
	rows = gray.rows;

	// 라인 패턴
	srand((unsigned int)time(NULL));
	
	for (int i = 0; i < 256; i++) {
		int tmpy1, tmpx1, tmpy2, tmpx2;
		tmpy1 = round(Gaussian_Random());
		tmpx1 = round(Gaussian_Random());
		tmpy2 = round(Gaussian_Random());
		tmpx2 = round(Gaussian_Random());

		if (tmpy1 == tmpy2 && tmpx1 == tmpx2) { // 두 점의 좌표가 같은 경우
			i--;
			cout << "좌표 중복" << i << '\n';
		}
		else// 두점의 좌표가 다른 경우
		{
			for (int j = 0; j < pvec.size()/2; j++) {
				if (pvec[j*2].y == tmpy1) {
					if (pvec[j*2].x == tmpx1) {
						if (pvec[j * 2 + 1].y == tmpy2) {
							if (pvec[j * 2 + 1].x == tmpx2) {
								i--; // 라인이 중복되는 경우
								cout << "라인 중복" << i << ", " << j << '\n';
							}
						}
					}
				}
				else if (pvec[j * 2 + 1].y == tmpy1) {
					if (pvec[j * 2 + 1].x == tmpx1) {
						if (pvec[j * 2].y == tmpy2) {
							if (pvec[j * 2].x == tmpx2) {
								i--; // 라인이 중복되는 경우
								cout << "라인 중복" << i << ", " << j << '\n';
							}
						}
					}
				}
			}
			
			pvec.push_back({ tmpy1, tmpx1 });
			pvec.push_back({ tmpy2, tmpx2 });
		}
	}

	//-------------라인 패턴 완료---------------

	
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			Find_Descriptor(i, j);
			result.at<uchar>(i, j) = descriptor[0] * 128 + descriptor[1] * 64 + descriptor[2] * 32 + descriptor[3] * 16 + descriptor[4] * 8 + descriptor[5] * 4 + descriptor[6] * 2 + descriptor[7];
		}
	}


	cv::namedWindow("Result");
	cv::imshow("Result", result);
	cv::imwrite("Result.bmp", result);


	cv::waitKey(0);

	return 0;
}
