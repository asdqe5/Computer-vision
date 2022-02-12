#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <vector>

#define octave 2 //옥타브 2개

using namespace cv;

typedef struct featurePoint { 
	int y; //y좌표
	int x; //x좌표
	int o; //옥타브번호
	int i; //DOG번호
}featurePoint;



int main()
{
	//이미지 로드
	Mat image = imread("img1.bmp");
	Mat image_gray;
	cvtColor(image, image_gray, CV_BGR2GRAY);

	const int width = image_gray.cols;
	const int height = image_gray.rows;

	//1. 입력 영상으로부터 scale-space 만들기
	Mat** scaleSpace = new Mat*[octave];
	for (int i = 0; i < octave; i++)
	{
		scaleSpace[i] = new Mat[6];
	}

	//2. scale-space에서 DoG채널 생성하기
	Mat** DOG = new Mat*[octave];
	for (int i = 0; i < octave; i++)
	{
		DOG[i] = new Mat[5];
	}

	double sigma = 1.6;
	double diff_sigma;
	double newSigma = sigma;
	const double k = pow(2, (1.0f / 3.0f)); // sigma가 2^(1/3)배씩 커짐

										 /*커널사이즈 정하기*/
	int ksize = int(ceil(sigma * 6)); //커널사이즈는 6*sigma 에서 올림 적용
	if (ksize % 2 == 0) ksize++; // 짝수면 그보다 큰 홀수로 만들어준다

	Size mysize(ksize, ksize);

	GaussianBlur(image_gray, scaleSpace[0][0], mysize, sqrt(sigma*sigma - 0.5*0.5), sqrt(sigma*sigma - 0.5*0.5)); //실제 구현에서는 입력 영상 자체가 0.5로 스무딩되었다고 가정함.
	for (int i = 0; i < octave; i++)
	{
		for (int j = 1; j < 6; j++)
		{
			newSigma = newSigma * k; //시그마 k배 키우기
			diff_sigma = sqrt(newSigma * newSigma - sigma * sigma); //성능 개선을 위하여 시그마i+1과 시그마i의 차이 구하기																	
			ksize = (int)(ceil(newSigma * 6));   //커널 사이즈 다시 구해주기
			if (ksize % 2 == 0) ksize++;
			mysize.height = ksize;
			mysize.width = ksize;
			GaussianBlur(scaleSpace[i][j - 1], scaleSpace[i][j], mysize, diff_sigma, diff_sigma); //위에서 구한 시그마 차이로 스무딩
			sigma = newSigma;
			absdiff(scaleSpace[i][j], scaleSpace[i][j - 1], DOG[i][j - 1]);
		}
		if (i == octave - 1) break; //마지막 octave이면 for문 탈출
		resize(scaleSpace[i][3], scaleSpace[i + 1][0], Size(scaleSpace[i][3].cols / 2, scaleSpace[i][3].rows / 2)); //다운샘플링
		sigma = 1.6f; //시그마 초기화
		newSigma = 1.6f; 
	}

	//3. DoG채널을 이용해서 feature point 찾기

	std::vector<featurePoint> F; //특징점 집합
	int n[26]; // neighbor
	for (int i = 0; i < octave; i++) //모든 옥타브에 대해 극점을 찾는다
	{
		for (int j = 1; j < 4; j++) //맨 아래와 맨 위의 DOG를 제외한 세 영상에 대해 극점을 찾는다
		{
			for (int x = 1; x < DOG[i][j].cols - 1; x++)
			{
				for (int y = 1; y < DOG[i][j].rows - 1; y++)
				{
					bool isPoint = false; //지역최소 혹은 지역최대라면 true, 아니면 false
					int pivot = DOG[i][j].at<uchar>(y, x); //pivot(기준점)

														   //배열 neighbor 초기화
					{
						n[0] = DOG[i][j - 1].at<uchar>(y - 1, x - 1);
						n[1] = DOG[i][j].at<uchar>(y - 1, x - 1);
						n[2] = DOG[i][j + 1].at<uchar>(y - 1, x - 1);
						n[3] = DOG[i][j - 1].at<uchar>(y - 1, x);
						n[4] = DOG[i][j].at<uchar>(y - 1, x);
						n[5] = DOG[i][j + 1].at<uchar>(y - 1, x);
						n[6] = DOG[i][j - 1].at<uchar>(y - 1, x + 1);
						n[7] = DOG[i][j].at<uchar>(y - 1, x + 1);
						n[8] = DOG[i][j + 1].at<uchar>(y - 1, x + 1);
						n[9] = DOG[i][j - 1].at<uchar>(y, x - 1);
						n[10] = DOG[i][j].at<uchar>(y, x - 1);
						n[11] = DOG[i][j + 1].at<uchar>(y, x - 1);
						n[12] = DOG[i][j - 1].at<uchar>(y, x);
						n[13] = DOG[i][j + 1].at<uchar>(y, x);
						n[14] = DOG[i][j - 1].at<uchar>(y + 1, x - 1);
						n[15] = DOG[i][j].at<uchar>(y + 1, x - 1);
						n[16] = DOG[i][j + 1].at<uchar>(y + 1, x - 1);
						n[17] = DOG[i][j - 1].at<uchar>(y + 1, x);
						n[18] = DOG[i][j].at<uchar>(y + 1, x);
						n[19] = DOG[i][j + 1].at<uchar>(y + 1, x);
						n[20] = DOG[i][j - 1].at<uchar>(y + 1, x + 1);
						n[21] = DOG[i][j].at<uchar>(y + 1, x + 1);
						n[22] = DOG[i][j + 1].at<uchar>(y + 1, x + 1);
						n[23] = DOG[i][j - 1].at<uchar>(y, x + 1);
						n[24] = DOG[i][j].at<uchar>(y, x + 1);
						n[25] = DOG[i][j + 1].at<uchar>(y, x + 1);
					}//26개의 이웃초기화

					int min = INT_MAX, max = INT_MIN;
					for (int k = 0; k < 26; k++)
					{
						if (n[k] < min) min = n[k]; //이웃중 최소값 탐색
						if (n[k] > max) max = n[k]; //이웃중 최대값 탐색
					}
					if (pivot < min) isPoint = true; //최소값보다 pivot이 작으면 지역최소
					else if (pivot > max) isPoint = true; //최대값보다 pivot이 크면 지역최대
					if (isPoint) //지역최소 혹은 지역최대라면 특징점 집합에 추가
					{
						featurePoint tmp;
						tmp.x = x;
						tmp.y = y;
						tmp.o = i;
						tmp.i = j;
						F.push_back(tmp);
					}
				}
			}
		}
	}


	// 4. 옥타브 0에 있는 영상의 위치로 바꾸고
	//    찾은 feature point display하기 (원의 반지름으로 scale 구별)
	for (int i = 0; i < F.size(); i++) 
	{
		Point2f center(F.at(i).x * pow(2, F.at(i).o), F.at(i).y * pow(2, F.at(i).o));
		float e = (F.at(i).o + F.at(i).i) / 3.0f;
		int r = 1.6 * pow(2, e);//원의 반지름 = 스케일값
		circle(image, center, r, Scalar(0, 255, 0), 1);
	}

	
	imshow("featurepoint", image);
	waitKey(0);


	imwrite("Result.jpg", image);


	for (int i = 0; i < octave; i++)
	{
		delete[] scaleSpace[i];
	}
	delete[] scaleSpace;

	for (int i = 0; i < octave; i++)
	{
		delete[] DOG[i];
	}
	delete[] DOG;

	return 0;
}
