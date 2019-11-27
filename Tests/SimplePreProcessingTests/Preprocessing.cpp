

#include <cmath>        // std::abs

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>

#include <opencv2/opencv_modules.hpp>


//#include <opencv2/line_descriptor.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/highgui/highgui.hpp>
//#include <opencv/highgui.h>


#include<Windows.h>

//#include "EDLines.h"

using namespace cv;
using namespace cv::dnn;
using namespace std;


//using namespace cv::line_descriptor;


void decode(const Mat& scores, const Mat& geometry, float scoreThresh,
	std::vector<RotatedRect>& detections, std::vector<float>& confidences);






typedef struct RgbColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RgbColor;

typedef struct HsvColor
{
	unsigned char h;
	unsigned char s;
	unsigned char v;
} HsvColor;


//TODO: detect star systems with a variant of hough circle detection (Circle Hough Transform)
void detectCirle() {




}



//floodfill stuff
bool Visited[1920][1080] = { 0 };
int BestsoFar = 0;
int Current = 0;
int bestX = 0;
int BestY = 0;

struct position {
	int x;
	int y;
};


int main(int argc, char** argv)
{


	// No need to mess around with Videocapture for this small test, so open screenshot of the game instead
	Mat image;
	string imageLoc = "D:/StellarisAI/StarStstemTest.png"; //replace with location of the image you want to test
	image = imread(imageLoc); //change this to whereever you have a screenshot of the game
	//image = imread("D:/StellarisAI/StarStstemTest.png", CV_LOAD_IMAGE_COLOR);

	Mat HSVimage;
	image.copyTo(HSVimage);

	cv::cvtColor(image, HSVimage, cv::COLOR_BGR2HSV);

	//deteclines();
	//return 0;

	if (!image.data)
	{
		//std::cout << "no image found";
		return -1;
	}


	static const std::string kWinName = "Lane extraction";
	namedWindow(kWinName, WINDOW_AUTOSIZE);

	

	Mat frame, blob;

	//check filtering based on hue and RGB
	for (int r = 1; r < image.rows - 1; r++) {
		for (int c = 1; c < image.cols - 1; c++) {
			//image.at(y,x);
			//img_bgr(r, c)
			Vec3b pixel = image.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;
			//std::cout << image.at<Vec3b>(r, c) << std::endl;
			//std::cout << blueP  << " " << greenP << " " << redP << std::endl;

			//when filtered, replace the original RGB with black
			Vec3b nothing(0, 0, 0);

			//filter based on R: 38  G: 202 B: 202 , OpenCV uses BGR so opposide order
			Vec3b lineColor(202, 202, 38);

			int redDifferece = std::abs(redP - 30);
			int greenDifferece = std::abs(greenP - 220);
			int blueDifferece = std::abs(blueP - 220);

			//int diffNeeded = 40;
			int diffNeeded = 80;

			/*
			if (converted.h == 120) {
				//std::cout << "hue accepted (row,col): " << r << " " <<  c << endl;
			}
			else {
				image.at<Vec3b>(r, c) = nothing;
			}
			*/

			Vec3b pixelHSV = HSVimage.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			//get hue from HSVimage pixel data
			int hue = (int)pixelHSV[0];

			int hueDifference = std::abs(hue - 84);
			if (hueDifference < 10) {				
				//std::cout << "hue accepted (row,col): " << r << " " <<  c << endl;
			}
			else {
				image.at<Vec3b>(r, c) = nothing;
			}


			if (greenDifferece < diffNeeded && blueDifferece < diffNeeded) {
				//std::cout << "RGB accepted (row,col): " << r << " " <<  c << endl;
			}
			else {
				image.at<Vec3b>(r, c) = nothing;
			}

		}
	}

	//Done with first step in preprocessing


	//TODO: floodfill
	/*
	for (int r = 1; r < image.rows - 1; r++) {
		for (int c = 1; c < image.cols - 1; c++) {

			Vec3b pixel = image.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;

			
			if (blueP != 0 && greenP != 0 && redP != 0) {
				if (Visited[r][c] != 1) {
					vector<position> FloodFill;
					position start;
					start.x = c;
					start.y = r;
					FloodFill.push_back(start);
					while (!FloodFill.empty()) {
						int x = FloodFill.back().x;
						int y = FloodFill.back().y;
						Visited[x][y] = 1;
						FloodFill.pop_back();
						//if(   )

					}

				}
			}
			
		}
	}
	*/




	imshow(kWinName, image);
	//imshow(kWinName, ExtraTansform);
	waitKey();


	//check to see if the same can be used to locate start system locations
	Mat ExtraTansform;
	ExtraTansform = imread(imageLoc);
	//image.copyTo(ExtraTansform);
	Mat HSVimage2;
	ExtraTansform.copyTo(HSVimage2);
	cv::cvtColor(ExtraTansform, HSVimage2, cv::COLOR_BGR2HSV);


	for (int r = 1; r < ExtraTansform.rows - 1; r++) {
		for (int c = 1; c < ExtraTansform.cols - 1; c++) {
			//image.at(y,x);
			//img_bgr(r, c)
			Vec3b pixel = ExtraTansform.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;
			//std::cout << image.at<Vec3b>(r, c) << std::endl;
			//std::cout << blueP  << " " << greenP << " " << redP << std::endl;
			Vec3b nothing(0, 0, 0);

			Vec3b lineColor(202, 202, 38);
			//filter based on R: 38  G: 202 B: 202
			//image.at<Vec3b>(r, c) = nothing;

			RgbColor thisRGB;
			thisRGB.b = blueP;
			thisRGB.g = greenP;
			thisRGB.r = redP;
			//HsvColor converted = RgbToHsv(thisRGB);

			int redDifferece = std::abs(redP - 255);
			int greenDifferece = std::abs(greenP - 220);
			int blueDifferece = std::abs(blueP - 220);

			//int diffNeeded = 80;
			int diffNeeded = 80;




			if (greenDifferece < diffNeeded && blueDifferece < diffNeeded) {
				//std::cout << "RGB accepted (row,col): " << r << " " <<  c << endl;
			}
			else {
				ExtraTansform.at<Vec3b>(r, c) = nothing;
			}


			Vec3b pixelHSV = HSVimage2.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)

			int hue = (int)pixelHSV[0];
			int hueDifference = std::abs(hue - 120);

			int Value = (int)pixelHSV[2];
			int valueDifference = std::abs(hue - 200);

			if (redDifferece > 70) {				
				ExtraTansform.at<Vec3b>(r, c) = nothing;
			}
			else {
				//ExtraTansform.at<Vec3b>(r, c) = nothing;
			}

			/*
			// hueDifference > 40
			if (Value < 200) {
				//ExtraTansform.at<Vec3b>(r, c) = nothing;
			}
			else {
				//ExtraTansform.at<Vec3b>(r, c) = nothing;
			}
			*/


		}
	}



	//imshow(kWinName, image);
	imshow(kWinName, ExtraTansform);
	waitKey();
	waitKey(0);

	return 0;

}

void decode(const Mat& scores, const Mat& geometry, float scoreThresh,
	std::vector<RotatedRect>& detections, std::vector<float>& confidences)
{
	detections.clear();
	CV_Assert(scores.dims == 4); CV_Assert(geometry.dims == 4); CV_Assert(scores.size[0] == 1);
	CV_Assert(geometry.size[0] == 1); CV_Assert(scores.size[1] == 1); CV_Assert(geometry.size[1] == 5);
	CV_Assert(scores.size[2] == geometry.size[2]); CV_Assert(scores.size[3] == geometry.size[3]);

	const int height = scores.size[2];
	const int width = scores.size[3];
	for (int y = 0; y < height; ++y)
	{
		const float* scoresData = scores.ptr<float>(0, 0, y);
		const float* x0_data = geometry.ptr<float>(0, 0, y);
		const float* x1_data = geometry.ptr<float>(0, 1, y);
		const float* x2_data = geometry.ptr<float>(0, 2, y);
		const float* x3_data = geometry.ptr<float>(0, 3, y);
		const float* anglesData = geometry.ptr<float>(0, 4, y);
		for (int x = 0; x < width; ++x)
		{
			float score = scoresData[x];
			if (score < scoreThresh)
				continue;

			// Decode a prediction.
			// Multiple by 4 because feature maps are 4 time less than input image.
			float offsetX = x * 4.0f, offsetY = y * 4.0f;
			float angle = anglesData[x];
			float cosA = std::cos(angle);
			float sinA = std::sin(angle);
			float h = x0_data[x] + x2_data[x];
			float w = x1_data[x] + x3_data[x];

			Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
				offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
			Point2f p1 = Point2f(-sinA * h, -cosA * h) + offset;
			Point2f p3 = Point2f(-cosA * w, sinA * w) + offset;
			RotatedRect r(0.5f * (p1 + p3), Size2f(w, h), -angle * 180.0f / (float)CV_PI);
			detections.push_back(r);
			confidences.push_back(score);
		}
	}
}

