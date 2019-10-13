

/*
#include <limits>
#include <vector>
#include <string>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


#include<opencv2/opencv.hpp>
#include<iostream>

#include <Windows.h>

using namespace std;
using namespace cv;

*/

/*
Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

*/



#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>

using namespace cv;
using namespace cv::dnn;

const char* keys =
"{ help  h     | | Print help message. }"
"{ input i     | | Path to input image or video file. Skip this argument to capture frames from a camera.}"
"{ model m     | | Path to a binary .pb file contains trained network.}"
"{ width       | 320 | Preprocess input image by resizing to a specific width. It should be multiple by 32. }"
"{ height      | 320 | Preprocess input image by resizing to a specific height. It should be multiple by 32. }"
"{ thr         | 0.5 | Confidence threshold. }"
"{ nms         | 0.4 | Non-maximum suppression threshold. }";

void decode(const Mat& scores, const Mat& geometry, float scoreThresh,
	std::vector<RotatedRect>& detections, std::vector<float>& confidences);

int main(int argc, char** argv)
{
	// Parse command line arguments.
	CommandLineParser parser(argc, argv, keys);
	parser.about("Use this script to run TensorFlow implementation (https://github.com/argman/EAST) of "
		"EAST: An Efficient and Accurate Scene Text Detector (https://arxiv.org/abs/1704.03155v2)");

	/*
	if (argc == 1 || parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}
	*/


	/*
	float confThreshold = parser.get<float>("thr");
	float nmsThreshold = parser.get<float>("nms");
	int inpWidth = parser.get<int>("width");
	int inpHeight = parser.get<int>("height");
	String model = parser.get<String>("model");
	*/
	float confThreshold = 0.3;
	float nmsThreshold = 0.2;
	int inpWidth = 1920 - 320;
	int inpHeight = 1056 + 32;
	//int inpWidth = 320*2;
	//int inpHeight = 320*2;
	String model = "D:/StellarisAI/frozen_east_text_detection.pb";


	/*
	if (!parser.check())
	{
		parser.printErrors();
		return 1;
	}
	*/


	CV_Assert(!model.empty());

	// Load network.
	Net net = readNet(model);

	// Open a video file or an image file or a camera stream.
	Mat image;
	image = imread("D:/StellarisAI/StellarisTest.png");

	VideoCapture cap;
	cap.open("D:/StellarisAI/StellarisTest.jpg", cv::CAP_IMAGES);
	//cap.open(image);


	/*
	if (parser.has("input"))
		cap.open(parser.get<String>("input"));
	else
		cap.open(0);
	*/


	static const std::string kWinName = "EAST: An Efficient and Accurate Scene Text Detector";
	namedWindow(kWinName, WINDOW_NORMAL);

	std::vector<Mat> outs;
	std::vector<String> outNames(2);
	outNames[0] = "feature_fusion/Conv_7/Sigmoid";
	outNames[1] = "feature_fusion/concat_3";

	Mat frame, blob;
	while (waitKey(1) < 0)
	{
		image.copyTo(frame);
		//image >> frame;
		/*
		cap >> frame;
		if (frame.empty())
		{
			waitKey();
			break;
		}
		*/

		blobFromImage(frame, blob, 1.0, Size(inpWidth, inpHeight), Scalar(123.68, 116.78, 103.94), true, false);
		//blobFromImage(frame, blob, 1.0, Size(inpWidth, inpHeight), Scalar(123.68, 116.78, 103.94), true, false);
		net.setInput(blob);
		net.forward(outs, outNames);

		Mat scores = outs[0];
		Mat geometry = outs[1];

		// Decode predicted bounding boxes.
		std::vector<RotatedRect> boxes;
		std::vector<float> confidences;
		decode(scores, geometry, confThreshold, boxes, confidences);

		// Apply non-maximum suppression procedure.
		std::vector<int> indices;
		NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

		// Render detections.
		Point2f ratio((float)frame.cols / inpWidth, (float)frame.rows / inpHeight);
		for (size_t i = 0; i < indices.size(); ++i)
		{
			RotatedRect& box = boxes[indices[i]];

			Point2f vertices[4];
			box.points(vertices);
			for (int j = 0; j < 4; ++j)
			{
				vertices[j].x *= ratio.x;
				vertices[j].y *= ratio.y;
			}
			for (int j = 0; j < 4; ++j)
				line(frame, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 1);
		}

		// Put efficiency information.
		std::vector<double> layersTimes;
		double freq = getTickFrequency() / 1000;
		double t = net.getPerfProfile(layersTimes) / freq;
		//std::string label = format("Inference time: %.2f ms", t);
		//putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));

		imshow(kWinName, frame);
	}
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


//int main()
//{

	/*
	Mat image;

	// LOAD image
	image = imread("D:/StellarisAI/stellaris.jpg");   // Read the file "image.jpg".
		   //This file "image.jpg" should be in the project folder.
		   //Else provide full address : "D:/images/image.jpg"

	if (!image.data)  // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	//DISPLAY image
	namedWindow("window"); // Create a window for display.
	imshow("window", image); // Show our image inside it.

	//SAVE image
	imwrite("result.jpg", image);// it will store the image in name "result.jpg"

	waitKey(0);                       // Wait for a keystroke in the window
	return 0;

	*/


	/*
	HWND hwndDesktop = GetDesktopWindow();
	Mat src = hwnd2mat(hwndDesktop);
	imshow("output", src);
	waitKey(0);

	*/


	/*

	Mat image = Mat::zeros(300, 600, CV_8UC3);
	circle(image, Point(250, 150), 100, Scalar(0, 255, 128), -100);
	circle(image, Point(350, 150), 100, Scalar(255, 255, 255), -100);
	imshow("Display Window", image);
	waitKey(0);
	return 0;

	*/

	/*
	Mat img = imread("D:\StellarisAI\stellaris.jpg");
	namedWindow("image", WINDOW_NORMAL);
	imshow("image", img);
	waitKey(0);
	return 0;
	*/


	//}



	/*


	#include <iostream>
	using namespace std;
	int main()
	{
		cout << "Hello, World!";
		return 0;





	}

	*/


