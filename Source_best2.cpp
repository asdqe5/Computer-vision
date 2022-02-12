#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <vector>

#define octave 2 //��Ÿ�� 2��

using namespace cv;

typedef struct featurePoint { 
	int y; //y��ǥ
	int x; //x��ǥ
	int o; //��Ÿ���ȣ
	int i; //DOG��ȣ
}featurePoint;



int main()
{
	//�̹��� �ε�
	Mat image = imread("img1.bmp");
	Mat image_gray;
	cvtColor(image, image_gray, CV_BGR2GRAY);

	const int width = image_gray.cols;
	const int height = image_gray.rows;

	//1. �Է� �������κ��� scale-space �����
	Mat** scaleSpace = new Mat*[octave];
	for (int i = 0; i < octave; i++)
	{
		scaleSpace[i] = new Mat[6];
	}

	//2. scale-space���� DoGä�� �����ϱ�
	Mat** DOG = new Mat*[octave];
	for (int i = 0; i < octave; i++)
	{
		DOG[i] = new Mat[5];
	}

	double sigma = 1.6;
	double diff_sigma;
	double newSigma = sigma;
	const double k = pow(2, (1.0f / 3.0f)); // sigma�� 2^(1/3)�辿 Ŀ��

										 /*Ŀ�λ����� ���ϱ�*/
	int ksize = int(ceil(sigma * 6)); //Ŀ�λ������ 6*sigma ���� �ø� ����
	if (ksize % 2 == 0) ksize++; // ¦���� �׺��� ū Ȧ���� ������ش�

	Size mysize(ksize, ksize);

	GaussianBlur(image_gray, scaleSpace[0][0], mysize, sqrt(sigma*sigma - 0.5*0.5), sqrt(sigma*sigma - 0.5*0.5)); //���� ���������� �Է� ���� ��ü�� 0.5�� �������Ǿ��ٰ� ������.
	for (int i = 0; i < octave; i++)
	{
		for (int j = 1; j < 6; j++)
		{
			newSigma = newSigma * k; //�ñ׸� k�� Ű���
			diff_sigma = sqrt(newSigma * newSigma - sigma * sigma); //���� ������ ���Ͽ� �ñ׸�i+1�� �ñ׸�i�� ���� ���ϱ�																	
			ksize = (int)(ceil(newSigma * 6));   //Ŀ�� ������ �ٽ� �����ֱ�
			if (ksize % 2 == 0) ksize++;
			mysize.height = ksize;
			mysize.width = ksize;
			GaussianBlur(scaleSpace[i][j - 1], scaleSpace[i][j], mysize, diff_sigma, diff_sigma); //������ ���� �ñ׸� ���̷� ������
			sigma = newSigma;
			absdiff(scaleSpace[i][j], scaleSpace[i][j - 1], DOG[i][j - 1]);
		}
		if (i == octave - 1) break; //������ octave�̸� for�� Ż��
		resize(scaleSpace[i][3], scaleSpace[i + 1][0], Size(scaleSpace[i][3].cols / 2, scaleSpace[i][3].rows / 2)); //�ٿ���ø�
		sigma = 1.6f; //�ñ׸� �ʱ�ȭ
		newSigma = 1.6f; 
	}

	//3. DoGä���� �̿��ؼ� feature point ã��

	std::vector<featurePoint> F; //Ư¡�� ����
	int n[26]; // neighbor
	for (int i = 0; i < octave; i++) //��� ��Ÿ�꿡 ���� ������ ã�´�
	{
		for (int j = 1; j < 4; j++) //�� �Ʒ��� �� ���� DOG�� ������ �� ���� ���� ������ ã�´�
		{
			for (int x = 1; x < DOG[i][j].cols - 1; x++)
			{
				for (int y = 1; y < DOG[i][j].rows - 1; y++)
				{
					bool isPoint = false; //�����ּ� Ȥ�� �����ִ��� true, �ƴϸ� false
					int pivot = DOG[i][j].at<uchar>(y, x); //pivot(������)

														   //�迭 neighbor �ʱ�ȭ
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
					}//26���� �̿��ʱ�ȭ

					int min = INT_MAX, max = INT_MIN;
					for (int k = 0; k < 26; k++)
					{
						if (n[k] < min) min = n[k]; //�̿��� �ּҰ� Ž��
						if (n[k] > max) max = n[k]; //�̿��� �ִ밪 Ž��
					}
					if (pivot < min) isPoint = true; //�ּҰ����� pivot�� ������ �����ּ�
					else if (pivot > max) isPoint = true; //�ִ밪���� pivot�� ũ�� �����ִ�
					if (isPoint) //�����ּ� Ȥ�� �����ִ��� Ư¡�� ���տ� �߰�
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


	// 4. ��Ÿ�� 0�� �ִ� ������ ��ġ�� �ٲٰ�
	//    ã�� feature point display�ϱ� (���� ���������� scale ����)
	for (int i = 0; i < F.size(); i++) 
	{
		Point2f center(F.at(i).x * pow(2, F.at(i).o), F.at(i).y * pow(2, F.at(i).o));
		float e = (F.at(i).o + F.at(i).i) / 3.0f;
		int r = 1.6 * pow(2, e);//���� ������ = �����ϰ�
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
