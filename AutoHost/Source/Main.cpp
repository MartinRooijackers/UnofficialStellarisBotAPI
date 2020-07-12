

//OpenCV EAST test on Stellaris
//Original code from: https://github.com/opencv/opencv/blob/master/samples/dnn/text_detection.cpp
//Changed to test the algorithm on Stellaris screenshots to see how well the algorithm would work in game


//#include <GdiPlus.h>
//#include <memory>

#include <cmath>        // std::abs

#include <iostream>
#include <fstream>
#include <string>
//#include <filesystem>

#include <sys/types.h>
#include <sys/stat.h>

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

#define _CRT_SECURE_NO_WARNINGS

//#include "asprise_ocr_api.h"

#include <cstdlib>

//#include <afxwin.h>
#include <iomanip>

#include <chrono>

#include <OleCtl.h>
//#include "clip/clip.h"

#include<Windows.h>

//#include "EDLines.h"


#ifdef WIN32
#define stat _stat
#endif

using namespace cv;
using namespace cv::dnn;
using namespace std;

//namespace fs = std::experimental::filesystem;
//using namespace cv::line_descriptor;


POINT SessionIDbutton;
POINT TextScreen;
POINT TextChat;
POINT StartButtonGame;
POINT FasterButton;
POINT PauseButton;
POINT MoreButton; //needed to open up chat
POINT ChatButton; //the actual button to open up chat
POINT SwitchToChatButton; //swtich focus to chat window
POINT SwitchToPlayerButton; //switch focus to players window
POINT PutmousetoScrollPlayerList;

std::string sessionID;
std::string ParadoxDocumentsFolder = "";


enum ScreenGrabMethod { SCREENSHOT, GRABBMP, HWNDmethod };


ScreenGrabMethod ScreengrabMethodPicked;

//bool that indicates wether to compensate for the text sclaing setting in windows 10. Setting it to 100% is recomended anyway though
bool UIfix;




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

RgbColor HsvToRgb(HsvColor hsv)
{
	RgbColor rgb;
	unsigned char region, remainder, p, q, t;

	if (hsv.s == 0)
	{
		rgb.r = hsv.v;
		rgb.g = hsv.v;
		rgb.b = hsv.v;
		return rgb;
	}

	region = hsv.h / 43;
	remainder = (hsv.h - (region * 43)) * 6;

	p = (hsv.v * (255 - hsv.s)) >> 8;
	q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
	t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
	case 0:
		rgb.r = hsv.v; rgb.g = t; rgb.b = p;
		break;
	case 1:
		rgb.r = q; rgb.g = hsv.v; rgb.b = p;
		break;
	case 2:
		rgb.r = p; rgb.g = hsv.v; rgb.b = t;
		break;
	case 3:
		rgb.r = p; rgb.g = q; rgb.b = hsv.v;
		break;
	case 4:
		rgb.r = t; rgb.g = p; rgb.b = hsv.v;
		break;
	default:
		rgb.r = hsv.v; rgb.g = p; rgb.b = q;
		break;
	}

	return rgb;
}

HsvColor RgbToHsv(RgbColor rgb)
{
	HsvColor hsv;
	unsigned char rgbMin, rgbMax;

	rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
	rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

	hsv.v = rgbMax;
	if (hsv.v == 0)
	{
		hsv.h = 0;
		hsv.s = 0;
		return hsv;
	}

	hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
	if (hsv.s == 0)
	{
		hsv.h = 0;
		return hsv;
	}

	if (rgbMax == rgb.r)
		hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
	else if (rgbMax == rgb.g)
		hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
	else
		hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

	return hsv;
}




void detectCirle() {




}



void SetMousePosition(int x, int y)
{
	long fScreenWidth = GetSystemMetrics(SM_CXSCREEN) - 1;
	long fScreenHeight = GetSystemMetrics(SM_CYSCREEN) - 1;


	//long fScreenWidth = 1080 - 1;
	//long fScreenHeight = 1920 - 1;




	// http://msdn.microsoft.com/en-us/library/ms646260(VS.85).aspx
	// If MOUSEEVENTF_ABSOLUTE value is specified, dx and dy contain normalized absolute coordinates between 0 and 65,535.
	// The event procedure maps these coordinates onto the display surface.
	// Coordinate (0,0) maps onto the upper-left corner of the display surface, (65535,65535) maps onto the lower-right corner.
	float fx = x * (65535.0f / fScreenWidth);
	float fy = y * (65535.0f / fScreenHeight);

	INPUT Input = { 0 };
	Input.type = INPUT_MOUSE;

	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

	Input.mi.dx = (long)fx;
	Input.mi.dy = (long)fy;

	SendInput(1, &Input, sizeof(INPUT));
}


#define SCROLLUP 120
#define SCROLLDOWN -120

//
// Desc    : Scrolls the mouse down
// Returns : Nothing.
//
void ScrollDown()
{

	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, SCROLLDOWN, 0);

}
//
// Desc    : Scrolls the mouse up
// Returns : Nothing.
//
void ScrollUP()
{

	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, SCROLLUP, 0);

}



//
// Desc    : Clicks the left mouse button down and releases it.
// Returns : Nothing.
//
void LeftClick()
{

	//Sleep(100);

	INPUT    Input = { 0 };													// Create our input.

	Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;							// We are setting left mouse button down.
	SendInput(1, &Input, sizeof(INPUT));								// Send the input.

	//Sleep(100);

	ZeroMemory(&Input, sizeof(INPUT));									// Fills a block of memory with zeros.
	Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;								// We are setting left mouse button up.
	SendInput(1, &Input, sizeof(INPUT));								// Send the input.
}


//
// Desc    : Clicks the right mouse button down and releases it.
// Returns : Nothing.
//
void RightClick()
{
	INPUT    Input = { 0 };													// Create our input.

	Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;							// We are setting left mouse button down.
	SendInput(1, &Input, sizeof(INPUT));								// Send the input.

	ZeroMemory(&Input, sizeof(INPUT));									// Fills a block of memory with zeros.
	Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;								// We are setting left mouse button up.
	SendInput(1, &Input, sizeof(INPUT));								// Send the input.
}


void sendKeyStroke() {

	INPUT input;
	WORD vkey = VK_ESCAPE; // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


}


WORD charToVKey( char Letter ) {


	switch (Letter) {



	case ')': return 0x30;
	case '!': return 0x31;
	case '@': return 0x32;
	case '#': return 0x33;
	case '$': return 0x34;
	case '%': return 0x35;
	case '^': return 0x36;
	case '&': return 0x37;
	case '*': return 0x38;
	case '(': return 0x39;

	case '0': return 0x30;
	case '1': return 0x31;
	case '2': return 0x32;
	case '3': return 0x33;
	case '4': return 0x34;
	case '5': return 0x35;
	case '6': return 0x36;
	case '7': return 0x37;
	case '8': return 0x38;
	case '9': return 0x39;

	case '<': return	0xBC;
	case ',': return	0xBC;


	case ';': return	0xBA;		
	case ':': return	0xBA;

	case '|': return	0xDC;
	case '\\': return	0xDC;

	case '~': return 0xC0;
	case '`': return 0xC0;

	case '\'': return 0xDE;
	case '\"': return 0xDE;


	case '?': return	0xBF;
	case '/': return	0xBF;

	case '>': return	0xBE;
	case '.': return	0xBE;
		
	case '[': return	0xDB;
	case '{': return	0xDB;

	case ']': return	0xDD;
	case '}': return	0xDD;



	case ' ': return	0x20;

		
	case 'A':
	case 'a': return 0x41;

	case 'B':
	case 'b': return 0x42;

	case 'C':
	case 'c': return 0x43;

	case 'D':
	case 'd': return 0x44;


	case 'E':
	case 'e': return 0x45;

	case 'F':
	case 'f': return 0x46;

	case 'G':
	case 'g': return 0x47;



	case 'H':
	case 'h': return 0x48;


	case 'I':
	case 'i': return 0x49;

	case 'J':
	case 'j': return 0x4A;


	case 'K':
	case 'k': return 0x4B;


	case 'L':
	case 'l': return 0x4C;

	case 'M':
	case 'm': return 0x4D;


	case 'N':
	case 'n': return 0x4E;


	case 'O':
	case 'o': return 0x4F;


	case 'P':
	case 'p': return 0x50;


	case 'Q':
	case 'q': return 0x51;


	case 'R':
	case 'r': return 0x52;


	case 'S':
	case 's': return 0x53;

	case 'T':
	case 't': return 0x54;



	case 'U':
	case 'u': return 0x55;



	case 'V':
	case 'v': return 0x56;



	case 'W':
	case 'w': return 0x57;



	case 'X':
	case 'x': return 0x58;



	case 'Y':
	case 'y': return 0x59;



	case 'Z':
	case 'z': return 0x5A;



	}

	return	0xBF;
	//return  0x00;
}



bool needShiftCheck(char letter) {


	if (isupper(letter)) {
		return true;
	}

	switch (letter) {

	case '!': return true;
	case '@': return true;
	case '#': return true;
	case '$': return true;
	case '%': return true;
	case '^': return true;
	case '&': return true;
	case '*': return true;
	case '(': return true;
	case ')': return true;

	case '{': return true;
	case '}': return true;

	case ':': return true;
	case '?': return true;
	case '<': return true;
	case '>': return true;
	case '|': return true;
	case '_': return true;
	case '+': return true;

	case '~': return true;



	case '\"': return true;

	default: return false;
	}

	return false;

}


void sendKeyStrokeChar(char letter) {

	//
	//VK_SHIFT
	//	0x10


	INPUT input;
	INPUT shiftInput;
	WORD vkey = charToVKey(letter); // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	//std::cout << vkey << endl;

	bool needShift = needShiftCheck(letter);

	if (needShift) {

		WORD shiftKey = 0x10;

		shiftInput.type = INPUT_KEYBOARD;
		shiftInput.ki.wScan = MapVirtualKey(shiftKey, MAPVK_VK_TO_VSC);
		shiftInput.ki.time = 0;
		shiftInput.ki.dwExtraInfo = 0;
		shiftInput.ki.wVk = shiftKey;
		shiftInput.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
		SendInput(1, &shiftInput, sizeof(INPUT));


	}

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


	if (needShift) {

		shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &shiftInput, sizeof(INPUT));


	}


}

/*
void sendKeyStrokeE() {

	INPUT input;
	WORD vkey = 0x45; // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


}

*/



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
	height = windowsize.bottom / 1.0;  //change this to whatever size you want to resize to
	width = windowsize.right / 1.0;

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



std::string getSCreenshot() {



	Sleep(4000);

	WIN32_FIND_DATA data;
	//std::string screenshotLocation = "C:\\Users\\Gebruiker\\Documents\\Paradox Interactive\\Stellaris\\screenshots\\";
	std::string screenshotLocation = ParadoxDocumentsFolder + "screenshots/";
	std::string tc = screenshotLocation + "/*";
	HANDLE hFind = FindFirstFile(tc.c_str(), &data);      // DIRECTORY

	int bestTime = 0;
	std::string locationBest = "";

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			//std::cout << data.cFileName << std::endl;
			std::string screenshotName(data.cFileName);
			if (screenshotName.size() < 3) {
				continue;
			}
			std::string getscreenshotLocation = screenshotLocation + screenshotName;
			//std::cout << "location of file: " << getscreenshotLocation << std::endl;

			std::string filename = getscreenshotLocation;
			struct stat result;
			if (stat(filename.c_str(), &result) == 0)
			{
				auto mod_time = result.st_mtime;
				//std::cout << mod_time << endl;
				if (mod_time > bestTime) {
					bestTime = mod_time;
					locationBest = filename;
				}
				//...
			}

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}



	std::cout << "best string: " << locationBest << endl;

	std::cout << "best time: " << bestTime << endl;

	//return 0;
	bool deleteScreens = false;
	//WARNING: only enabled this is need really be. 
	//If the paradox documents folder is set wrong, this part of the function will delete the wrong things
	if ((deleteScreens == true) && (ParadoxDocumentsFolder != "")) {
		// remove all other screenshots
		if (locationBest != "") {


			hFind = FindFirstFile(tc.c_str(), &data);      // DIRECTORY


			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					//std::cout << data.cFileName << std::endl;
					std::string screenshotName(data.cFileName);
					if (screenshotName.size() < 3) {
						continue;
					}
					std::string getscreenshotLocation = screenshotLocation + screenshotName;
					//std::cout << "location of file: " << getscreenshotLocation << std::endl;

					std::string filename = getscreenshotLocation;
					struct stat result;
					if (stat(filename.c_str(), &result) == 0)
					{
						if (filename != locationBest) {
							std::cout << "removing screenshot: " << filename << endl;
							remove(filename.c_str());
						}
					}

				} while (FindNextFile(hFind, &data));
				FindClose(hFind);
			}


		}
	}

	return locationBest;

}

std::string takeScreenshot() {

	//presses F11 for screenshot
	INPUT input;
	WORD vkey = 0x7A; // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));



	return getSCreenshot();

}



void sendKeyStrokeEnter() {

	INPUT input;
	WORD vkey = 0x0D; // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


}



//check if the chat screen is up
//if not, make it reappear
void CheckIfChatIsUp() {

	// x=146 y=1035


	//chat open:  29,45,38

	//players tab open:  33,67,51

	/*
	HDC dc = GetDC(NULL);
	//COLORREF color = GetPixel(dc, 605, 428);
	COLORREF color = GetPixel(dc, 146, 1035);
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);
	std::cout << "Pixel values at location x=146 y=1035: " << red << " " << green << " " << blue << endl;
	*/


	//std::string ChatSCreenPic = takeScreenshot();
	//Mat ChatMat = imread(ChatSCreenPic);
	Mat ChatMat;


	if (ScreengrabMethodPicked == SCREENSHOT) {
		std::string locationBest = "";
		locationBest = takeScreenshot();
		ChatMat = imread(locationBest);
	}

	if (ScreengrabMethodPicked == HWNDmethod) {
		HWND hwndDesktop = GetDesktopWindow();
		ChatMat = hwnd2mat(hwndDesktop);
	}


	//TOCHANGE: do something better than this
	imwrite("chat.bmp", ChatMat);
	ChatMat = imread("chat.bmp");



	Vec3b pixelP = ChatMat.at<Vec3b>(1035, 146); // or img_bgr.at<Vec3b>(r,c)
	uchar blueP = pixelP[0];
	uchar greenP = pixelP[1];
	uchar redP = pixelP[2];
	int blue = (int)blueP;
	int green = (int)greenP;
	int red = (int)redP;


	std::cout << "chat screen pixel test: " << red << " " << green << " " << blue << endl;


	//chat is open, so no need to re-open it
	if ((red == 29) && (green == 45) && (blue == 38)) {
		std::cout << "chat is open, so no need to re-open it" << endl;
		return;
	}

	if ((red == 33) && (green == 67) && (blue == 51)) {
		std::cout << "player window is open, so no need to re-open it" << endl;
		return;
	}


	std::cout << "chat windows is closed. Reopening it" << endl;
	//otherwise, do reopen it
	Sleep(10);
	SetMousePosition(ChatButton.x, ChatButton.y);
	Sleep(10);
	LeftClick();
	Sleep(10);


}


void CopyPastaGameChat(const std::string& s) {

	std::cout << "copy pasta clipboard" << endl;

	HWND hwnd = GetDesktopWindow();
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		std::cout << "copy pasta failed";
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);


	//Sleep(1000);
	SetMousePosition(TextChat.x, TextChat.y);
	//SetMousePosition(TextScreen.x, TextScreen.y);
	Sleep(10);
	LeftClick();
	Sleep(10);
	//now copy paste it
	std::cout << "now pase it" << endl;

	//sendKeyStrokeChar('s');

	INPUT input;
	INPUT shiftInput;
	WORD vkey = charToVKey('v'); // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	//std::cout << vkey << endl;

	//sendKeyStrokeChar('s');

	INPUT MultiInput[2];

	//left control

	//WORD shiftKey = 0x44; // D
	WORD shiftKey = 0xA2; //left control
	//WORD shiftKey = 0x11; //control
	//WORD shiftKey = 0x10;

	/*
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
	*/

	/*
	MultiInput[0].type = INPUT_KEYBOARD;
	MultiInput[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
	MultiInput[0].ki.time = 0;
	MultiInput[0].ki.dwExtraInfo = 0;
	MultiInput[0].ki.wVk = VK_CONTROL;
	MultiInput[0].ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN


	MultiInput[1].type = INPUT_KEYBOARD;
	MultiInput[1].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	MultiInput[1].ki.time = 0;
	MultiInput[1].ki.dwExtraInfo = 0;
	MultiInput[1].ki.wVk = vkey;
	MultiInput[1].ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN


	SendInput(2, MultiInput, sizeof(MultiInput));

	MultiInput[0].ki.dwFlags = KEYEVENTF_KEYUP;
	MultiInput[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, MultiInput, sizeof(MultiInput));
	*/


	bool useCtl = true;
	if (useCtl) {
		shiftInput.type = INPUT_KEYBOARD;
		shiftInput.ki.wScan = MapVirtualKey(shiftKey, MAPVK_VK_TO_VSC);
		shiftInput.ki.time = 0;
		shiftInput.ki.dwExtraInfo = 0;
		shiftInput.ki.wVk = shiftKey;
		shiftInput.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
		SendInput(1, &shiftInput, sizeof(INPUT));
	}
	Sleep(100);
	//end of left control

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


	//if (isupper(letter)) {
	if (useCtl) {
		shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &shiftInput, sizeof(INPUT));
	}

	//}


}




void CopyPastaLobby(const std::string& s) {

	std::cout << "copy pasta clipboard";

	HWND hwnd = GetDesktopWindow();
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		std::cout << "copy pasta failed";
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);




	SetMousePosition(TextScreen.x, TextScreen.y);
	Sleep(10);
	LeftClick();
	Sleep(10);
	//now copy paste it
	std::cout << "now pase it" << endl;

	//sendKeyStrokeChar('s');

	INPUT input;
	INPUT shiftInput;
	WORD vkey = charToVKey('v'); // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	//std::cout << vkey << endl;

	//sendKeyStrokeChar('s');

	INPUT MultiInput[2];

	//left control

	//WORD shiftKey = 0x44; // D
	WORD shiftKey = 0xA2; //left control
	//WORD shiftKey = 0x11; //control
	//WORD shiftKey = 0x10;

	/*
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
	*/

	/*
	MultiInput[0].type = INPUT_KEYBOARD;
	MultiInput[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
	MultiInput[0].ki.time = 0;
	MultiInput[0].ki.dwExtraInfo = 0;
	MultiInput[0].ki.wVk = VK_CONTROL;
	MultiInput[0].ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN


	MultiInput[1].type = INPUT_KEYBOARD;
	MultiInput[1].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	MultiInput[1].ki.time = 0;
	MultiInput[1].ki.dwExtraInfo = 0;
	MultiInput[1].ki.wVk = vkey;
	MultiInput[1].ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN


	SendInput(2, MultiInput, sizeof(MultiInput));

	MultiInput[0].ki.dwFlags = KEYEVENTF_KEYUP;
	MultiInput[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, MultiInput, sizeof(MultiInput));
	*/


	bool useCtl = true;
	if (useCtl) {
		shiftInput.type = INPUT_KEYBOARD;
		shiftInput.ki.wScan = MapVirtualKey(shiftKey, MAPVK_VK_TO_VSC);
		shiftInput.ki.time = 0;
		shiftInput.ki.dwExtraInfo = 0;
		shiftInput.ki.wVk = shiftKey;
		shiftInput.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
		SendInput(1, &shiftInput, sizeof(INPUT));
	}
	Sleep(100);
	//end of left control

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


	//if (isupper(letter)) {
	if (useCtl) {
		shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &shiftInput, sizeof(INPUT));
	}

	//}


}


void writeTextChatGame(string text) {

	std::cout << "trying to put the following text on chat:" << text << endl;
	//std::cout << text << endl;

	//first make sure the chat window is up:
	CheckIfChatIsUp();


	std::cout << text << endl;
	CopyPastaGameChat(text);
	Sleep(50);
	sendKeyStrokeEnter();
	return;

	//string message = "https://discord.gg/CsmCF3";
	string message = text;

	//std::cout << "textscreen x y: " << TextScreen.x << " " << TextScreen.y << endl;

	//setup to the chat thing
	SetMousePosition(TextChat.x, TextChat.y);
	Sleep(10);
	LeftClick();
	Sleep(10);
	for (int i = 0; i < message.length(); i++) {
		sendKeyStrokeChar(message[i]);
		Sleep(10);

	}
	sendKeyStrokeEnter();



}


void writeTextLobby(string text) {

	std::cout << text << endl;
	CopyPastaLobby(text);
	Sleep(50);
	sendKeyStrokeEnter();
	return;

	//string message = "https://discord.gg/CsmCF3";
	string message = text;

	//std::cout << "textscreen x y: " << TextScreen.x << " " << TextScreen.y << endl;

	//setup to the chat thing
	SetMousePosition(TextScreen.x, TextScreen.y);
	Sleep(10);
	LeftClick();
	Sleep(10);
	for (int i = 0; i < message.length(); i++) {
		sendKeyStrokeChar(message[i]);
		Sleep(10);

	}
	sendKeyStrokeEnter();



}

void copyPasteSessionID() {

	//
	//VK_SHIFT
	//	0x10


	if (UIfix == false) {
		SetMousePosition(TextScreen.x + 350, TextScreen.y - 10);
	}
	else {
		SetMousePosition(1178, 797);
	}


	//SetMousePosition(TextScreen.x + 350, TextScreen.y-10);
	Sleep(10);
	LeftClick();
	//Sleep(1000);
	//SetMousePosition(TextScreen.x, TextScreen.y);
	Sleep(10);

	if (!OpenClipboard(nullptr)) {
		std::cout << "error opening clipboard" << endl;
		return;
	}

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr) {
		std::cout << "error hand;e" << endl;
		return;
	}

	char* pszText = static_cast<char*>(GlobalLock(hData));

	if (pszText == nullptr) {
		std::cout << "error text" << endl;
		return;
	}

	std::string text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	writeTextLobby(text);

	//sessionID = text;

	return;
	/*
	IDataObject ClipData = System.Windows.Forms.Clipboard.GetDataObject();
	string s;
	if (ClipData.GetDataPresent(DataFormats.Text))
	{
		string s = System.Windows.Forms.Clipboard.GetData(DataFormats.Text).ToString();
		labelClip.Text = s;
	}
	*/

	INPUT input;
	INPUT shiftInput;
	WORD vkey = charToVKey('v'); // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	//std::cout << vkey << endl;


	//left control

		WORD shiftKey = 0xA2; //left control

		shiftInput.type = INPUT_KEYBOARD;
		shiftInput.ki.wScan = MapVirtualKey(shiftKey, MAPVK_VK_TO_VSC);
		shiftInput.ki.time = 0;
		shiftInput.ki.dwExtraInfo = 0;
		shiftInput.ki.wVk = shiftKey;
		shiftInput.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
		SendInput(1, &shiftInput, sizeof(INPUT));

		Sleep(10);
	//end of left control

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


	//if (isupper(letter)) {

		shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &shiftInput, sizeof(INPUT));


	//}


}



void writeSessionID() {

	//
	//VK_SHIFT
	//	0x10


	//1178,797


	//VPS fix, in general make sure your have focus on this game
	POINT testLeftClick;
	testLeftClick.x = 10;
	testLeftClick.y = 10;
	if (UIfix == false) {
		testLeftClick.x = (int)(testLeftClick.x / 1.25);
		testLeftClick.y = (int)(testLeftClick.y / 1.25);
	}

	SetMousePosition(testLeftClick.x, testLeftClick.y);


	Sleep(10);
	LeftClick();

	std::cout << "writing session ID to file" << endl;

	SetMousePosition(SessionIDbutton.x, SessionIDbutton.y);
	

	/*
	if (UIfix == false) {
		SetMousePosition(TextScreen.x + 350, TextScreen.y - 10);
	}
	else {
		SetMousePosition(1178, 797);
	}
	*/

	//SetMousePosition(TextScreen.x + 350, TextScreen.y - 10);
	Sleep(1000);
	LeftClick();
	//Sleep(1000);
	//SetMousePosition(TextScreen.x, TextScreen.y);
	Sleep(100);

	if (!OpenClipboard(nullptr)) {
		std::cout << "error opening clipboard" << endl;
		return;
	}

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr) {
		std::cout << "error handle" << endl;
		return;
	}

	char* pszText = static_cast<char*>(GlobalLock(hData));

	if (pszText == nullptr) {
		std::cout << "error getting clipboard text" << endl;
		return;
	}

	std::string text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();


	sessionID = text;

	
	std::cout << "sesison ID to store: " << text << endl;

	ofstream fout("sessionID.txt");
	fout << text << endl;
	fout.close();

	//}


}


POINT GetMousePosition()
{
	static POINT m;
	POINT mouse;
	GetCursorPos(&mouse);
	m.x = mouse.x;
	m.y = mouse.y;
	return m;
}


/*
bool Visited[1920][1080] = { 0 };
int BestsoFar = 0;
int Current = 0;
int bestX = 0;
int BestY = 0;

struct position {
	int x;
	int y;
};
*/



int ScreenX = GetDeviceCaps(GetDC(0), HORZRES);
int ScreenY = GetDeviceCaps(GetDC(0), VERTRES);
//int ScreenX = 1920;
//int ScreenY = 1080;

BYTE* ScreenData;

void ScreenCap()
{
	HDC hdc = GetDC(NULL), hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, ScreenX, ScreenY);
	BITMAPINFOHEADER bmi = {0};
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biWidth = ScreenX;
	bmi.biHeight = -ScreenY;
	bmi.biCompression = BI_RGB;
	SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hdc, 0, 0, SRCCOPY);

	GetDIBits(hdc, hBitmap, 0, ScreenY, ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdc);
}


inline int PosR(int x, int y)
{
	return ScreenData[3 * ((y * ScreenX) + x) + 2];
}

inline int PosG(int x, int y)
{
	return ScreenData[3 * ((y * ScreenX) + x) + 1];
}

inline int PosB(int x, int y)
{
	return ScreenData[3 * ((y * ScreenX) + x)];
}









Mat hwnd2mat2(int x, int y, int w, int h)
{



	HDC hdcSource = GetDC(NULL);
	HDC hdcMemory = CreateCompatibleDC(hdcSource);

	int capX = GetDeviceCaps(hdcSource, HORZRES);
	int capY = GetDeviceCaps(hdcSource, VERTRES);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

	BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);




	//HDC hwindowDC, hwindowCompatibleDC;

	//int height, width, srcheight, srcwidth;
	//HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;


	src.create(h, w, CV_8UC4);

	// create a bitmap
	//hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = w;
	bi.biHeight = -h;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	//SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hdcMemory, 0, 0, w, h, hdcSource, 0, 0, w, h, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hdcMemory, hBitmap, 0, h, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	//DeleteObject(hbwindow);
	//DeleteDC(hwindowCompatibleDC);
	//ReleaseDC(hwnd, hwindowDC);


	DeleteDC(hdcSource);
	DeleteDC(hdcMemory);


	return src;
}



bool saveBitmap(LPCSTR filename, HBITMAP bmp, HPALETTE pal)
{
	bool result = false;
	PICTDESC pd;

	pd.cbSizeofstruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_BITMAP;
	pd.bmp.hbitmap = bmp;
	pd.bmp.hpal = pal;

	LPPICTURE picture;
	HRESULT res = OleCreatePictureIndirect(&pd, IID_IPicture, false,
		reinterpret_cast<void**>(&picture));

	if (!SUCCEEDED(res))
		return false;

	LPSTREAM stream;
	res = CreateStreamOnHGlobal(0, true, &stream);

	if (!SUCCEEDED(res))
	{
		picture->Release();
		return false;
	}

	LONG bytes_streamed;
	res = picture->SaveAsFile(stream, true, &bytes_streamed);

	HANDLE file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (!SUCCEEDED(res) || !file)
	{
		stream->Release();
		picture->Release();
		return false;
	}

	HGLOBAL mem = 0;
	GetHGlobalFromStream(stream, &mem);
	LPVOID data = GlobalLock(mem);

	DWORD bytes_written;

	result = !!WriteFile(file, data, bytes_streamed, &bytes_written, 0);
	result &= (bytes_written == static_cast<DWORD>(bytes_streamed));

	GlobalUnlock(mem);
	CloseHandle(file);

	stream->Release();
	picture->Release();

	return result;
}


//  Anton from stackoverflow
bool screenCapturePart(int x, int y, int w, int h, LPCSTR fname) {
	HDC hdcSource = GetDC(NULL);
	HDC hdcMemory = CreateCompatibleDC(hdcSource);

	int capX = GetDeviceCaps(hdcSource, HORZRES);
	int capY = GetDeviceCaps(hdcSource, VERTRES);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

	BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

	DeleteDC(hdcSource);
	DeleteDC(hdcMemory);

	HPALETTE hpal = NULL;
	if (saveBitmap(fname, hBitmap, hpal)) return true;
	return false;
}






void toClipboard(const std::string& s) {

	std::cout << "copy pasta clipboard";

	HWND hwnd = GetDesktopWindow();
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);



	
	SetMousePosition(TextScreen.x, TextScreen.y);
	Sleep(10);
	LeftClick();
	Sleep(10);
	//now copy paste it
	std::cout << "now pase it" << endl;

	//sendKeyStrokeChar('s');

	INPUT input;
	INPUT shiftInput;
	WORD vkey = charToVKey('v'); // see link below  https://docs.microsoft.com/nl-nl/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	//std::cout << vkey << endl;
	
	sendKeyStrokeChar('s');

	INPUT MultiInput[2];

	//left control

	//WORD shiftKey = 0x44; // D
	WORD shiftKey = 0xA2; //left control
	//WORD shiftKey = 0x11; //control
	//WORD shiftKey = 0x10;


	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


	/*
	MultiInput[0].type = INPUT_KEYBOARD;
	MultiInput[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
	MultiInput[0].ki.time = 0;
	MultiInput[0].ki.dwExtraInfo = 0;
	MultiInput[0].ki.wVk = VK_CONTROL;
	MultiInput[0].ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN


	MultiInput[1].type = INPUT_KEYBOARD;
	MultiInput[1].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	MultiInput[1].ki.time = 0;
	MultiInput[1].ki.dwExtraInfo = 0;
	MultiInput[1].ki.wVk = vkey;
	MultiInput[1].ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN


	SendInput(2, MultiInput, sizeof(MultiInput));

	MultiInput[0].ki.dwFlags = KEYEVENTF_KEYUP;
	MultiInput[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, MultiInput, sizeof(MultiInput));
	*/

	
	bool useCtl = true;
	if (useCtl) {
		shiftInput.type = INPUT_KEYBOARD;
		shiftInput.ki.wScan = MapVirtualKey(shiftKey, MAPVK_VK_TO_VSC);
		shiftInput.ki.time = 0;
		shiftInput.ki.dwExtraInfo = 0;
		shiftInput.ki.wVk = shiftKey;
		shiftInput.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
		SendInput(1, &shiftInput, sizeof(INPUT));
	}
	Sleep(100);
	//end of left control

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));



	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));


	//if (isupper(letter)) {
	if (useCtl) {
		shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &shiftInput, sizeof(INPUT));
	}
	
	//}


}



struct Vote {
	std::string Username;
	int choice;
};


enum StateLobby { TEXTING,  VOTING_START , VOTING_KICK, START , INTO_GAME   };

StateLobby LobbyState;


enum StateGame { START_LOADING , INITIALIZE, CHAT , KICK, VOTE_KICK };

StateGame GameState;



//variables loading in with the txt
std::string TesseractLocation = "C:\\Tesseract-OCR\\";
//ParadoxDocumentsFolder = "C:/Users/Gebruiker/Documents/Paradox Interactive/Stellaris/";
std::string MessageLobby = "test message";
int PlayersNeededBeforeStart = 15;

//

//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


std::chrono::steady_clock::time_point beginSessionID = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point endSessionID = std::chrono::steady_clock::now();

std::chrono::steady_clock::time_point beginMessage = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point endMessage = std::chrono::steady_clock::now();



//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;


bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

int main(int argc, char** argv)
{
	// Parse command line arguments.
	//CommandLineParser parser(argc, argv, keys);
	//parser.about("Use this script to run TensorFlow implementation (https://github.com/argman/EAST) of "
	//	"EAST: An Efficient and Accurate Scene Text Detector (https://arxiv.org/abs/1704.03155v2)");

	
	/*
	std::string somet;
	std::cout << "type something then tab into Stellaris. Tab back in 10 seconds to see the result" << endl;
	std::cin >> somet;

	Sleep(5000);

	// Get the window handle of the Stellaris application.
	LPCTSTR WindowName = "Stellaris";
	//HWND StellarisWindow = ::FindWindow(0, WindowName);
	HWND hwndDesktop = GetDesktopWindow();

		//HWND hwndDesktop = GetDesktopWindow();
	//Mat src = hwnd2mat(StellarisWindow);
    Mat src = hwnd2mat(hwndDesktop);
	imshow("output", src);



	waitKey(0);

	return 0;
	*/



	/*
	WIN32_FIND_DATA data;
	std::string screenshotLocation = "C:\\Users\\Gebruiker\\Documents\\Paradox Interactive\\Stellaris\\screenshots\\";
	HANDLE hFind = FindFirstFile("C:\\Users\\Gebruiker\\Documents\\Paradox Interactive\\Stellaris\\screenshots\\*", &data);      // DIRECTORY

	int bestTime = 0;
	std::string locationBest = "";

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			std::cout << data.cFileName << std::endl;
			std::string screenshotName(data.cFileName);
			if (screenshotName.size() < 3) {
				continue;
			}
			std::string getscreenshotLocation = screenshotLocation + screenshotName;
			std::cout << "location of file: " << getscreenshotLocation << std::endl;

			std::string filename = getscreenshotLocation;
			struct stat result;
			if (stat(filename.c_str(), &result) == 0)
			{
				auto mod_time = result.st_mtime;
				std::cout << mod_time << endl;
				if (mod_time > bestTime) {
					bestTime = mod_time;
					locationBest = filename;
				}
				//...
			}

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}



	std::cout << "best string: " << locationBest << endl;

	std::cout << "best time: " << bestTime << endl;

	return 0;
	*/

	ifstream f("message.txt"); //taking file as inputstream
	string str;
	if (f) {
		ostringstream ss;
		ss << f.rdbuf(); // reading data
		str = ss.str();
	}
	std::cout << str;
	MessageLobby = str;

	//return 0;

	std::string rulesetcreate;


	ifstream f2("rulesetcreate.txt"); //taking file as inputstream
	string str2;
	if (f2) {
		ostringstream ss;
		ss << f2.rdbuf(); // reading data
		str2 = ss.str();
	}
	std::cout << "ruleset create:" << endl << str2;
	rulesetcreate = str2;



	std::string rulesetingame;


	ifstream f3("rulesetingame.txt"); //taking file as inputstream
	string str3;
	if (f3) {
		ostringstream ss;
		ss << f3.rdbuf(); // reading data
		str3 = ss.str();
	}
	std::cout << "ruleset in game:" << endl << str3;
	rulesetingame = str3;




	std::cout << "starting up, reading settings file:" << endl;



	std::vector<std::string> WhiteList;

	std::ifstream whiteListFile("whitelist.txt");
	std::string WhitelistNames;
	if (whiteListFile.is_open())
	{
		std::cout << "managed to open settings file " << endl;
		while (getline(whiteListFile, WhitelistNames))
		{
			if (WhitelistNames.size() > 1) {
				WhiteList.push_back(WhitelistNames);
				std::cout << "whitelist name added: " << WhitelistNames << endl;
			}
		}
		whiteListFile.close();
	}


	//default values

	UIfix = true;
	int secondsBetweenMessage = 60;
	beginMessage = std::chrono::steady_clock::now();
	std::string StartingVoteText = "Starting vote to start the game. Vote 1 for yes, 2 for no";
	std::string AutoStartEnoughPeopleMessage = "Got enough people, starting game";
	std::string MessageChatInGame = "Test message for chat in game";


	std::string VoteStartPassedMessage = "vote passed, starting game";
	std::string VoteStartFailedMessage = "vote failed, game not starting atm";

	std::string AutoUnpauseActivatedMessage = "auto unpause now activated";
	std::string HelpCommandMessageLobby = "The following commands are availabe, !sessionID , !votestart";
	std::string NicknameToIgnore = "LetaBot";

	std::string DiscordLink = "no link set";
	std::string StartTime = "no start time set";



	int SecondsToreadyUp = 5;
	int PlayersNeededToStartStartVote = 10;
	int SecondsForVote = 30;
	int SecondsTillAutoUnpause = 30;
	int PercentageNeededForVote = 30; //at least this % of people need to vote for it to count


	ScreengrabMethodPicked = HWNDmethod;


	std::ifstream settingsFile("settings.txt");
	std::string settingLine;
	if (settingsFile.is_open())
	{
		std::cout << "managed to open settings file " << endl;
		while (getline(settingsFile, settingLine))
		{
			//std::cout << settingLine << '\n';

			if (settingLine.find("Start Time:") != std::string::npos) {
				StartTime = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						StartTime += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}



			if (settingLine.find("Discord link:") != std::string::npos) {
				DiscordLink = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						DiscordLink += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}




			if (settingLine.find("Username to ignore:") != std::string::npos) {
				NicknameToIgnore = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						NicknameToIgnore += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}



			if (settingLine.find("Help Command message lobby:") != std::string::npos) {
				HelpCommandMessageLobby = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						HelpCommandMessageLobby += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}



			if (settingLine.find("Auto unpause activated message:") != std::string::npos) {
				AutoUnpauseActivatedMessage = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						AutoUnpauseActivatedMessage += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}


			if (settingLine.find("Votestart failed message:") != std::string::npos) {
				VoteStartFailedMessage = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						VoteStartFailedMessage += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}


			if (settingLine.find("Votestart passed message:") != std::string::npos) {
				VoteStartPassedMessage = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						VoteStartPassedMessage += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
			}


			if (settingLine.find("Automessage in-game chat:") != std::string::npos) {
				MessageChatInGame = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						MessageChatInGame += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//currentMembers = line;
			}



			if (settingLine.find("Autostart lobby message:") != std::string::npos) {
				AutoStartEnoughPeopleMessage = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						AutoStartEnoughPeopleMessage += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//currentMembers = line;
			}



			
			if (settingLine.find("Starting vote message:") != std::string::npos) {
				StartingVoteText = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						StartingVoteText += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//currentMembers = line;
			}


			//Tesseract location
			if (settingLine.find("Tesseract Location:") != std::string::npos) {
				TesseractLocation = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						TesseractLocation += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//currentMembers = line;
			}


			//ParadoxDocumentsFolder
			if (settingLine.find("Paradox Document Folder:") != std::string::npos) {
				ParadoxDocumentsFolder = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						ParadoxDocumentsFolder += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//currentMembers = line;
			}


			/*
			//message in the lobby
			if (settingLine.find("Message Lobby:") != std::string::npos) {
				MessageLobby = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						MessageLobby += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//currentMembers = line;
			}

			*/


			if (settingLine.find("Percentage of players that need to vote:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					PercentageNeededForVote = std::stoi(playersS);
					//make sure it is within range
					if (PercentageNeededForVote > 100) {
						std::cout << "PercentageNeededForVote above 100, setting to 100" << endl;
						PercentageNeededForVote = 100;
					}
					if (PercentageNeededForVote < 0) {
						std::cout << "PercentageNeededForVote below 0, setting to 0" << endl;
						PercentageNeededForVote = 0;
					}
				}
				else {
					std::cout << "amount of seconds till auto unpause is not a number" << endl;
				}

			}



			if (settingLine.find("Amount of seconds till auto unpause:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					SecondsTillAutoUnpause = std::stoi(playersS);
				}
				else {
					std::cout << "amount of seconds till auto unpause is not a number" << endl;
				}

			}



			

			if (settingLine.find("Amount of seconds a vote will last:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					SecondsForVote = std::stoi(playersS);
				}
				else {
					std::cout << "player number in settings is not a number" << endl;
				}

			}



			//total number of players in lobby
			if (settingLine.find("Players Needed:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS) ) {
					PlayersNeededBeforeStart = std::stoi(playersS);
				}
				else {
					std::cout << "player number in settings is not a number" << endl;
				}

			}


			//total number of players needed in lobby to start vote
			if (settingLine.find("Players needed to start vote:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					PlayersNeededToStartStartVote = std::stoi(playersS);
				}
				else {
					std::cout << "player number in settings is not a number" << endl;
				}

			}

			//PlayersNeededToStartStartVote



			//seconds between message
			if (settingLine.find("Seconds between message:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					secondsBetweenMessage = std::stoi(playersS);
				}
				else {
					std::cout << "second needed before message settings is not a number" << endl;
				}
				//currentMembers = line;
			}

			if (settingLine.find("Second to ready up:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					SecondsToreadyUp = std::stoi(playersS);
				}
				else {
					std::cout << "second needed before ready up is not a number" << endl;
				}
				//currentMembers = line;
			}




			//total number of players in lobby
			if (settingLine.find("UIfix:") != std::string::npos) {
				std::string uiFIXs = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						uiFIXs += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (uiFIXs == "false") {
					UIfix = false;
				}
				else {
					UIfix = true;
				}
				//currentMembers = line;
			}





		}
		settingsFile.close();
	}









	//begin test grabbing usernames ---------------------------------------------------------------------------------------------------
	/*
	//RGB:  180,202,195
	std::string TesseractCommand2 = TesseractLocation + "tesseract " + TesseractLocation + "test.jpg " + TesseractLocation + "test";
	std::string testSCreen = "C:/Users/Gebruiker/Documents/Paradox Interactive/Stellaris/screenshots/2020_04_30_1.png";

	//Rect RectangleToSelect(440, 245, 140, 20);
	//Mat dst5 = src(RectangleToSelect);


		//		Rect RectangleToSelect(440, 245, 140, 20);
	//screenCapturePart(440, 245, 140, 20, "a.bmp");
	Mat GetPlayersChat = imread(testSCreen);
	//cv::imshow("test", GetPlayersChat);

	//cv::waitKey(0);

	//invert colors
	for (int r = 1; r < GetPlayersChat.rows - 1; r++) {
		int c = 479;
		//for (int c = 1; c < GetPlayersChat.cols - 1; c++) {

			Vec3b pixel = GetPlayersChat.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;
			if (redP == 180 && greenP == 202 && blueP == 195) {
				std::cout << "first pixel found:" << r << endl;


				//check if this pixel thing is also the case a bit further ahead.
				//cause if not, then you wont be able to click the kick button
				Vec3b pixel2 = GetPlayersChat.at<Vec3b>(r+99, c); // or img_bgr.at<Vec3b>(r,c)
				uchar blue2 = pixel2[0];
				uchar green2 = pixel2[1];
				uchar red2 = pixel2[2];
				int blueP2 = (int)blue2;
				int greenP2 = (int)green2;
				int redP2 = (int)red2;
				if (redP2 == 180 && greenP2 == 202 && blueP2 == 195) {
					std::cout << "second pixel found" << endl;
					Rect RectangleToSelect(102, r+5, 410-102, 20);
					Mat screenName = GetPlayersChat(RectangleToSelect);
					cv::imshow("test", screenName);
					cv::waitKey(0);


					//invert colors
					for (int r3 = 1; r3 < screenName.rows - 1; r3++) {
						for (int c3 = 1; c3 < screenName.cols - 1; c3++) {
							Vec3b pixel3 = screenName.at<Vec3b>(r3, c3); // or img_bgr.at<Vec3b>(r,c)
							uchar blue3 = pixel3[0];
							uchar green3 = pixel3[1];
							uchar red3 = pixel3[2];
							int blueP3 = (int)blue3;
							int greenP3 = (int)green3;
							int redP3 = (int)red3;
							Vec3b invertColor3(255 - blueP3, 255 - greenP3, 255 - redP3);
							screenName.at<Vec3b>(r3, c3) = invertColor3;
						}
					}

					cv::imshow("test", screenName);
					cv::waitKey(0);

					imwrite(TesseractLocation + "test.jpg", screenName);
					Sleep(10);
					std::system(TesseractCommand2.c_str());


					string line;
					ifstream myfile(TesseractLocation + "test.txt");
					if (myfile.is_open())
					{
						while (getline(myfile, line))
						{
							std::cout << line << '\n';
							if (line.size()>1) {
								std::cout << "found a player: " << line << endl;
							}
						}
						myfile.close();
					}


				}


			}
			Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);

			//GetPlayersChat.at<Vec3b>(r, c) = invertColor;
		//}
	}

	return 0;


	*/
	//end grabbing usernames ingame ---------------------------------------------------------------------------------------------------
 




	//TODO: main function
	//int something;
    std::string startingState; //indicating where to start in the Stellaris auto-host

	std::cout << "Current Settings:" << endl;
	std::cout << "Messages that gets auto send in lobby: " << MessageLobby << endl;
	std::cout << "Total amount of people before starting: " << PlayersNeededBeforeStart << endl;
	std::cout << "Total amount of people before vote start is allowed: " << PlayersNeededToStartStartVote << endl;
	std::cout << "Location of Stellaris document folder: " << ParadoxDocumentsFolder << endl;
	std::cout << "Location of Tesseract: " << TesseractLocation << endl;
	std::cout << "Second between auto message: " << secondsBetweenMessage << endl;
	std::cout << "Second to ready up: " << SecondsToreadyUp << endl;
	std::cout << "UIfix: " << UIfix << endl;



	//std::cout << endl << "Program initiated. Type something and press enter to start. When it starts, tab into Stellaris" << endl;
	std::cout << endl << "Program initiated. Where is the auto-host currently used?" << endl;
	std::cout <<  "[1] (Default) Lobby " << endl;
	std::cout << "[2] Loading game , Waiting for players... " << endl;
	std::cout << "[3] In game, auto-sending text messages and auto-unpausing " << endl;


	std::cin >> startingState;
	//testOcr();
	//return 0;
	std::cout << "starting" << endl;

	LobbyState = TEXTING;
	GameState = START_LOADING;


	std::chrono::steady_clock::time_point beginVoting = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point endVoting = std::chrono::steady_clock::now();


	/*
	Mat test5 = hwnd2mat2(0, 0, 1920, 1080);
	imshow("Title", test5);
	//hwnd2mat2
	waitKey(0);
	//ScreenData = new BYTE[3 * ScreenX * ScreenY];
	Sleep(5000);

	screenCapturePart(0, 0, 1920, 1080, "D:/a.bmp");
	return 0;
	*/

	/*
	//beign copying clipboard

	//HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);;

	HBITMAP hBitmap2 = (HBITMAP)GetClipboardData(CF_BITMAP);;


	int x_size = 1920, y_size = 1080; // <-- Your res for the image





	//HBITMAP hBitmap; // <-- The image represented by hBitmap

	Mat matBitmap; // <-- The image represented by mat





	// Initialize DCs

	HDC hdcSys = GetDC(NULL); // Get DC of the target capture..
	HDC hdcMem = CreateCompatibleDC(hdcSys); // Create compatible DC 






	void* ptrBitmapPixels; // <-- Pointer variable that will contain the potinter for the pixels









	// Create hBitmap with Pointer to the pixels of the Bitmap
	BITMAPINFO bi; HDC hdc;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = x_size;
	bi.bmiHeader.biHeight = -y_size;  //negative so (0,0) is at top left

	bi.bmiHeader.biPlanes = 1;

	bi.bmiHeader.biBitCount = 32;
	hdc = GetDC(NULL);
	//hBitmap = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &ptrBitmapPixels, NULL, 0);
	// ^^ The output: hBitmap & ptrBitmapPixels


	// Set hBitmap in the hdcMem 
	SelectObject(hdcMem, hBitmap2);



	// Set matBitmap to point to the pixels of the hBitmap
	matBitmap = Mat(y_size, x_size, CV_8UC4, ptrBitmapPixels, 0);
	//                ^^ note: first it is y, then it is x. very confusing

	// * SETUP DONE *




	// Now update the pixels using BitBlt
	BitBlt(hdcMem, 0, 0, x_size, y_size, hdcSys, 0, 0, SRCCOPY);


	// Just to do some image processing on the pixels.. (Dont have to to this)
	Mat matRef = matBitmap(Range(100, 200), Range(100, 200));
	//                              y1    y2            x1     x2
	bitwise_not(matRef, matRef); // Invert the colors in this x1,x2,y1,y2




	// Display the results through Mat
	imshow("Title", matBitmap);

	// Wait until some key is pressed

	waitKey(0);













	//end copying clipbord
	*/

	//getImageClipBoard();
	//return 0;

	//string imageLoc = "D:/StellarisAI/StarStstemTest2.png";
	//image = imread(imageLoc); //change this to whereever you have a screenshot of the game

	//HWND hwndDesktop = GetDesktopWindow();
	//Mat test = imread("D:/StellarisAI/Backup/test.png");

	//Mat test = imread("D:/StellarisAI/Backup/2020_04_20_2.png");


	//imshow("output", test);
	//imshow("output", src);

	//cout << "image size: " << test.cols << " " << test.rows;

	//waitKey(0);


	//  X = 606  for the kick button
	//  X = 650  for the line

	//Rect RectangleToSelect(1685, 97, 1897-1685, 293-97);  // username thingy
	//Rect RectangleToSelect(683, 271, 1143 - 683, 788 - 271); //text box in the game lobby



	//Mat dst6 = test(RectangleToSelect);
	//Mat dst6Invert;
	//cvNot(dst5, dst5);




	//cv::threshold(dst6, dst6Invert, 70, 255, THRESH_BINARY_INV);

	//imshow("output2", dst5Invert);


	//select based on color

    /*
	//invert colors
	for (int r = 1; r < test.rows - 1; r++) {
		for (int c = 1; c < test.cols - 1; c++) {

			Vec3b pixel = test.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;


			Vec3b colorBoxNeeded(157, 255, 108);
			Vec3b colorNeeded(180, 222, 116);
			Vec3b nothing(0, 0, 0);

			if (redP == 116 && greenP == 222 && blueP == 180 && c == 605) {
			//if (redP == 108 && greenP == 255 && blueP == 157 && c == 605  ) {
				//test.at<Vec3b>(r, c) = colorNeeded;
			}
			else {
				test.at<Vec3b>(r, c) = nothing;
			}
			//Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);

			//dst6.at<Vec3b>(r, c) = invertColor;

		}
	}
	*/


	/*
	//invert colors
	for (int r = 1; r < dst6.rows - 1; r++) {
		for (int c = 1; c < dst6.cols - 1; c++) {

			Vec3b pixel = dst6.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;

			Vec3b invertColor(255- blueP, 255- greenP, 255- redP);

			dst6.at<Vec3b>(r, c) = invertColor;
		}
	}
	*/


	//imwrite("D:/Tesseract-OCR/test.jpg", dst6Invert); // 
	//imwrite("D:/Tesseract-OCR/test.jpg", test); // 

	//return 0;



	//POINT TextScreen;


	if (UIfix == false) {
		TextScreen.x = 593;
		TextScreen.y = 652;
	}
	else {
		TextScreen.x = 593*1.25;
		TextScreen.y = 652* 1.25;
	}

	//1680x1050
	//TextScreen.x = 637;
	//TextScreen.y = 802;

	TextChat.x = 77;
	TextChat.y = 980;
	if (UIfix == false) {

		TextChat.x = (int)(TextChat.x / 1.25);
		TextChat.y = (int)(TextChat.y / 1.25);

	}

	//TextScreen.x = 895;
	//TextScreen.y = 871;



	POINT StartButton;
	StartButton.x = 1167;
	StartButton.y = 690;
	if (UIfix == true) {

		StartButton.x = (int)(StartButton.x * 1.25);
		StartButton.y = (int)(StartButton.y * 1.25);

	}


	POINT ScrollPosition;
	ScrollPosition.x = 388;
	ScrollPosition.y = 309;
	if (UIfix == false) {

		ScrollPosition.x = (int)(ScrollPosition.x / 1.25);
		ScrollPosition.y = (int)(ScrollPosition.y / 1.25);

	}





	//POINT StartButtonGame;
	StartButtonGame.x = 1322;
	StartButtonGame.y = 753;
	if (UIfix == false) {
		StartButtonGame.x = (int)(1322 / 1.25);
		StartButtonGame.y = (int)(753 / 1.25);
	}


	//POINT FasterButton;
	FasterButton.x = 1883;
	FasterButton.y = 14;
	if (UIfix == false) {
		FasterButton.x = (int)(FasterButton.x / 1.25);
		FasterButton.y = (int)(FasterButton.y / 1.25);
	}

	//POINT PauseButton;
	PauseButton.x = 1814;
	PauseButton.y = 14;
	if (UIfix == false) {
		PauseButton.x = (int)(PauseButton.x / 1.25);
		PauseButton.y = (int)(PauseButton.y / 1.25);
	}


	//POINT MoreButton; //needed to open up chat
	MoreButton.x = 1807;
	MoreButton.y = 1060;
	if (UIfix == false) {
		MoreButton.x = (int)(MoreButton.x / 1.25);
		MoreButton.y = (int)(MoreButton.y / 1.25);
	}

	/*
	2.6.3
	//POINT ChatButton; //needed to open up chat
	ChatButton.x = 1807;
	ChatButton.y = 1024;
	if (UIfix == false) {
		ChatButton.x = (int)(ChatButton.x / 1.25);
		ChatButton.y = (int)(ChatButton.y / 1.25);
	}
	*/

	//POINT ChatButton; //needed to open up chat
	ChatButton.x = 1740;
	ChatButton.y = 1060;
	if (UIfix == false) {
		ChatButton.x = (int)(ChatButton.x / 1.25);
		ChatButton.y = (int)(ChatButton.y / 1.25);
	}



	//POINT SwitchToChatButton; //needed to open up chat
	SwitchToChatButton.x = 104;
	SwitchToChatButton.y = 1020;
	if (UIfix == false) {
		SwitchToChatButton.x = (int)(SwitchToChatButton.x / 1.25);
		SwitchToChatButton.y = (int)(SwitchToChatButton.y / 1.25);
	}
	//POINT SwitchToPlayerButton; //needed to open up chat
	SwitchToPlayerButton.x = 212;
	SwitchToPlayerButton.y = 1020;
	if (UIfix == false) {
		SwitchToPlayerButton.x = (int)(SwitchToPlayerButton.x / 1.25);
		SwitchToPlayerButton.y = (int)(SwitchToPlayerButton.y / 1.25);
	}

	// POINT PutmousetoScrollPlayerList; //needed to open up chat
	//PutmousetoScrollPlayerList.x = 46;
	//PutmousetoScrollPlayerList.y = 533;
	PutmousetoScrollPlayerList.x = 357;
	PutmousetoScrollPlayerList.y = 498;
	if (UIfix == false) {
		PutmousetoScrollPlayerList.x = (int)(PutmousetoScrollPlayerList.x / 1.25);
		PutmousetoScrollPlayerList.y = (int)(PutmousetoScrollPlayerList.y / 1.25);
	}

	/*
		if (UIfix == false) {
		SetMousePosition(TextScreen.x + 350, TextScreen.y - 10);
	}
    */

	SessionIDbutton.x = 1178;
	SessionIDbutton.y = 797;

	//std::system("D:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");
	//return 0;



	std::ifstream UIsettingsFile("UIsettings.txt");
	std::string UILine;
	if (UIsettingsFile.is_open())
	{

		std::cout << "UI settings opened" << endl;
		while (getline(UIsettingsFile, UILine))
		{

			if (UILine.find("Lobby text box X:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < UILine.size(); i++) {
					if (startAdding == true) {
						playersS += UILine[i];
					}
					if (UILine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					TextScreen.x = std::stoi(playersS);
					std::cout << "text screen X changed to: " << TextScreen.x << endl;
				}
				else {
					std::cout << "Lobby text X not a number" << endl;
				}
			}


			if (UILine.find("Lobby text box Y:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < UILine.size(); i++) {
					if (startAdding == true) {
						playersS += UILine[i];
					}
					if (UILine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					TextScreen.y = std::stoi(playersS);
					std::cout << "text screen Y changed to: " << TextScreen.y << endl;
				}
				else {
					std::cout << "Lobby text Y not a number" << endl;
				}
			}




			if (UILine.find("Sesion ID box X:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < UILine.size(); i++) {
					if (startAdding == true) {
						playersS += UILine[i];
					}
					if (UILine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					SessionIDbutton.x = std::stoi(playersS);
				}
				else {
					std::cout << "sessionID box X not a number" << endl;
				}
			}


			if (settingLine.find("Sesion ID box Y:") != std::string::npos) {
				std::string playersS = "";
				bool startAdding = false;
				for (int i = 0; i < settingLine.size(); i++) {
					if (startAdding == true) {
						playersS += settingLine[i];
					}
					if (settingLine[i] == ':') {
						startAdding = true;
					}
				}
				//std::cout << playersS << endl;
				if (is_number(playersS)) {
					SessionIDbutton.y = std::stoi(playersS);
				}
				else {
					std::cout << "sessionID box X not a number" << endl;
				}
			}

		}

	}


	std::cout << "ready up" << endl;
	Sleep(5000);

	//test scrolling
	//ScrollDown();

	/*
	ifstream finTest("trycolor.txt");
	std::string lineTest;
	if (finTest.is_open())
	{
		while (getline(finTest, lineTest))
		{
			cout << lineTest << '\n';
			toClipboard(lineTest);
		}
		finTest.close();
	}

	//toClipboard("test3");
	*/

	//return 0;
	

	//StartButton = GetMousePosition();
	POINT kickButton;
	//POINT kickButton = GetMousePosition();
	kickButton.x = 500;
	kickButton.y = 346;


	//cout << kickButton.x << " " << kickButton.y << endl;
	//return 0;


	/*

	HWND hwndDesktop = GetDesktopWindow();
	Mat src = hwnd2mat(hwndDesktop);
	imshow("output", src);

	//waitKey(0);

	Rect RectangleToSelect(440, 245, 140, 20);
	Mat dst5 = src(RectangleToSelect);
	Mat dst5Invert;
	//cvNot(dst5, dst5);

	cv::threshold(dst5, dst5Invert, 70, 255, THRESH_BINARY_INV);

	imshow("output2", dst5Invert);


	imwrite("D:/Tesseract-OCR/test.jpg", dst5Invert); // 

	Sleep(10);

	std::system("D:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");

	string line;
	ifstream myfile("D:/Tesseract-OCR/test.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			cout << line << '\n';
			if (line == "1 Players, 1 Ready") {
				cout << "done\n";
			}
		}
		myfile.close();
	}

	*/

	//waitKey(0);

	// D:\Tesseract-OCR\tesseract test.jpg test



	//return 0;


	//httpstd::cout << "starting while loop" << endl;

	std::vector<Vote> countingVotes;


	//std::string Tesseract;
	//	std::string TesseractCommand = TesseractLocation + "tesseract " + TesseractLocation + "test.jpg " + TesseractLocation + "test" + " --dpi 200";
	std::string TesseractCommand = TesseractLocation + "tesseract " + TesseractLocation + "test.jpg " + TesseractLocation + "test";
	std::string TesseractCommandDPI200 = TesseractLocation + "tesseract " + TesseractLocation + "test.jpg " + TesseractLocation + "test" + " --dpi 200";

	std::cout << TesseractCommand << endl;

	int TotalRead = 0;//how many lines to skip


	//return 0;


	//LobbyState = START; //TODO: remove this , for testing purpose only


	//GameState = CHAT;
	//goto LabelIngame;
	bool MoreThanLobby = false;


	if (startingState == "2") {

		GameState = START_LOADING;
		goto LabelIngame;

	}


	if (startingState == "3") {

		GameState = CHAT;
		goto LabelIngame;

	}


	writeSessionID();




	//loop for lobby
	while (1) {


		if (GetKeyState(VK_RCONTROL) & 0x8000)
		{
			// right control down
			return 0;
		}

		//prevent spam
		Sleep(5000);


		endMessage = std::chrono::steady_clock::now();

		//int secondsMessage = 5l;

		endMessage = std::chrono::steady_clock::now();
		int timeElapsedMessage = std::chrono::duration_cast<std::chrono::seconds> (endMessage - beginMessage).count();

		if (timeElapsedMessage > secondsBetweenMessage) {
			beginMessage = std::chrono::steady_clock::now();
			writeTextLobby(MessageLobby);

		}



		//writeTextLobby("https://discord.gg/CsmCF3");

		Sleep(10);

		//		Rect RectangleToSelect(440, 245, 140, 20);
		screenCapturePart(440, 245, 140, 20, "a.bmp");
		//Mat PlayersReady = imread("a.bmp");




		//std::string locationBest = "";

		//locationBest = takeScreenshot();
		
		
		/*
		Sleep(5000);

		WIN32_FIND_DATA data;
		//std::string screenshotLocation = "C:\\Users\\Gebruiker\\Documents\\Paradox Interactive\\Stellaris\\screenshots\\";
		std::string screenshotLocation = ParadoxDocumentsFolder + "screenshots/";
		std::string tc = screenshotLocation + "/*";
		HANDLE hFind = FindFirstFile(tc.c_str(), &data);      // DIRECTORY

		int bestTime = 0;
		std::string locationBest = "";

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				std::cout << data.cFileName << std::endl;
				std::string screenshotName(data.cFileName);
				if (screenshotName.size() < 3) {
					continue;
				}
				std::string getscreenshotLocation = screenshotLocation + screenshotName;
				std::cout << "location of file: " << getscreenshotLocation << std::endl;

				std::string filename = getscreenshotLocation;
				struct stat result;
				if (stat(filename.c_str(), &result) == 0)
				{
					auto mod_time = result.st_mtime;
					std::cout << mod_time << endl;
					if (mod_time > bestTime) {
						bestTime = mod_time;
						locationBest = filename;
					}
					//...
				}

			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}



		std::cout << "best string: " << locationBest << endl;

		std::cout << "best time: " << bestTime << endl;

		//return 0;
		*/

		//Mat src = imread(locationBest);

		Mat src;

		if (ScreengrabMethodPicked == SCREENSHOT) {
			std::string locationBest = "";
			locationBest = takeScreenshot();
			src = imread(locationBest);
		}

		if (ScreengrabMethodPicked == HWNDmethod) {
			HWND hwndDesktop = GetDesktopWindow();
			src = hwnd2mat(hwndDesktop);
		}


		Rect RectangleToSelect(440, 245, 140, 20);
		Mat PlayersReady = src(RectangleToSelect);

		//TOCHANGE: do something better than this
		cv::imwrite("c.bmp", PlayersReady);
		PlayersReady = cv::imread("c.bmp");

		//cv::imshow("playredu", PlayersReady);



		//cv::waitKey(0);


		std::cout << "rows and cols: " << PlayersReady.rows << " " << PlayersReady.cols << endl;

		//imshow("screenstest", PlayersReady);
		//waitKey(0);

		//invert colors
		for (int r = 0; r < PlayersReady.rows; r++) {
			for (int c = 0; c < PlayersReady.cols ; c++) {

				//std::cout << c << endl;

				Vec3b pixel = PlayersReady.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
				//std::cout << "before " << c << ": " << pixel << endl;
				uchar blue = pixel[0];
				uchar green = pixel[1];
				uchar red = pixel[2];
				int blueP = (int)blue;
				int greenP = (int)green;
				int redP = (int)red;


				if (c == 1) {
					///std::cout << redP << " " << greenP << " " << blueP << endl;
				}


				if (c == 109) {
					//std::cout << redP << " " << greenP << " " << blueP << endl;
				}

				Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);

				PlayersReady.at<Vec3b>(r, c) = invertColor;
				Vec3b pixelC = PlayersReady.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
				//std::cout << pixelC << endl;


			}
			//cv::imshow("playredu", PlayersReady);

			//cv::waitKey(0);

		}


		//cv::imshow("playredu", PlayersReady);

		//cv::waitKey(0);

		cv::imwrite(TesseractLocation + "test.jpg", PlayersReady);
		


		Sleep(10);

		//std::string TesseractCommand = TesseractLocation + "tesseract " + 
		std::system(TesseractCommand.c_str() );
		//std::system(TesseractCommandDPI200.c_str());
		//TesseractLocation
		//std::system("C:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");


		bool done = false;
		int numberofPlayers = 0;
		int nomberofPlayersReady = 0;
		string line;
		ifstream myfile(TesseractLocation + "test.txt");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::cout << line << '\n';
				/*
				if (line.find("4 Players, 4s Ready") != std::string::npos) {
					//std::cout << "found!" << '\n';
					std::cout << "done\n";
					done = true;
				}
				*/

				if (line.find("Players") != std::string::npos) {

					//writeTextLobby("OCR output: " + line);

					//grab number of players
					string numberString = "";
					for (int i = 0; i <line.size(); i++) {
						if (isdigit(line[i]) ) {
							numberString += line[i];
						}
						if (line[i] == 'P') {
							break;
						}
					}
					std::cout << "numberString:  " << numberString << endl;
					if (is_number(numberString)) {
						numberofPlayers = std::stoi(numberString);
					}
					else {
						std::cout << "getting number of players from OCR went wrong" << endl;
					}

					string readyString = "";
					bool addToString = false;
					for (int i = 0; i < line.size(); i++) {
						if (isdigit(line[i]) && addToString == true) {
							readyString += line[i];
						}
						if (line[i] == ',') {
							addToString = true;
						}
						if (line[i] == 'R' && addToString == true) {
							break;
						}
					}
					//grab the ones that are ready
					//std::cout << "numberReadyString:  " << readyString << endl;
					if (is_number(readyString)) {
						nomberofPlayersReady = std::stoi(readyString);
					}
					else {
						std::cout << "getting number of ready players from OCR went wrong: " << readyString << endl;
					}

					//return 0;

					//if we got enough people and they are all readied up, then start
					// 					if (numberofPlayers == PlayersNeededBeforeStart && numberofPlayers == nomberofPlayersReady) {
					if (numberofPlayers >= PlayersNeededBeforeStart && numberofPlayers == nomberofPlayersReady) {
						writeTextLobby(AutoStartEnoughPeopleMessage);
						LobbyState = START;
					}

				}

			}
			myfile.close();
		}


		std::cout << "number of players:  " << numberofPlayers << endl;

		if (done == true) {
			//StartButton
			//SetMousePosition(StartButton.x, StartButton.y);
			//Sleep(10);
			//LeftClick();
			//break;
		}


		//check if a vote is up
		if (LobbyState == TEXTING) {
			//grab text in channel

			std::cout << "checking for vote " << endl;

			//write out all members
			//writeTextLobby("/members");
			//writeTextLobby("/save test");

			//Sleep(20);


			/*
			string currentMembers;
			//myfile.open("C:/Users/Gebruiker/Documents/Paradox Interactive/Stellaris/chat logs/test.txt");
			myfile.open(ParadoxDocumentsFolder + "chat logs/test.txt");
			if (myfile.is_open())
			{
				std::cout << "managed to open chat log file " << endl;
				while (getline(myfile, line))
				{
					std::cout << line << '\n';

					if (line.find("Members:") != std::string::npos) {
						
						currentMembers = line;
					}
				}
				myfile.close();
			}
			else {
				std::cout << "didnt managed to open chat log file " << endl;
			}
			std::cout << currentMembers << endl;
			*/


			//writeTextLobby(currentMembers);

			/*

			Sleep(10);
			screenCapturePart(680, 272, 1143 - 680, 788 - 272, "./a.bmp");
			Mat LobbyChat = imread("./a.bmp");


			Mat HSVimage;
			LobbyChat.copyTo(HSVimage);

			cv::cvtColor(LobbyChat, HSVimage, cv::COLOR_BGR2HSV);



			//invert colors
			for (int r = 1; r < LobbyChat.rows - 1; r++) {
				for (int c = 1; c < LobbyChat.cols - 1; c++) {

					Vec3b pixel = LobbyChat.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					uchar blue = pixel[0];
					uchar green = pixel[1];
					uchar red = pixel[2];
					int blueP = (int)blue;
					int greenP = (int)green;
					int redP = (int)red;


					//Vec3b pixelHSV = HSVimage.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					//int hue = (int)pixelHSV[0];
					//int hueDifference = std::abs(hue - 84);


					Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
					Vec3b white(255, 255, 255);



					//cut out all the light colors
					if (redP < 50 && blueP < 50 && greenP < 50) {
						LobbyChat.at<Vec3b>(r, c) = white;
					}
					else {
						LobbyChat.at<Vec3b>(r, c) = invertColor;
					}
				}
			}
			
			imwrite("C:/Tesseract-OCR/test.jpg", LobbyChat);

			Sleep(10);
			//std::system("C:\\Tesseract-OCR\\tesseract C:\\Tesseract-OCR\\test.jpg C:\\Tesseract-OCR\\test");
			std::system(TesseractCommand.c_str());

			*/

			Sleep(10);
			writeTextLobby("/save test");
			Sleep(2000);

			bool AlreadyPostedHelp = false;
			bool AlreadyPostedSessionID = false;
			bool AlreadyPostedVoteFailed = false;
			bool AlreadypostedRuleset = false;
			bool AlreadypostedRulesetingame = false;
			bool AlreadypostedDiscordLink = false;
			bool AlreadypostedStartTime = false;


			//string line;
			//ifstream myfile("D:/Tesseract-OCR/test.txt");
			//myfile.open(TesseractLocation + "test.txt");
			int CurrentRead = 0;
			myfile.open(ParadoxDocumentsFolder + "chat logs/test.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					CurrentRead++;
					if (CurrentRead < TotalRead) {
						continue;
					}
					
					std::cout << line << '\n';


					if (TotalRead <= CurrentRead) {
						TotalRead++;
					}


					//check if this isnt just the host/username to ignore posting
					string userName = "";
					bool adduserName = false;
					for (int i = 0; i < line.size(); i++) {
						if (line[i] == ')') {
							break;
						}
						if (adduserName == true) {
							userName += line[i];
						}
						if (line[i] == '(') {
							adduserName = true;
						}

					}

					//skip if it is the uername set to ignore
					if (userName == NicknameToIgnore) {
						continue;
					}

					//&& line.find(":") != std::string::npos

					if (line.find("!votestart") != std::string::npos
						&& line.find(":") != std::string::npos
						 && LobbyState != VOTING_START
						 && LobbyState != VOTING_KICK
						 && MoreThanLobby
						) {
						if (numberofPlayers >= PlayersNeededToStartStartVote) {
							std::cout << "Starting vote to start game" << endl;
							writeTextLobby(StartingVoteText);
							beginVoting = std::chrono::steady_clock::now();
							LobbyState = VOTING_START;
							countingVotes.clear();
						}
						else {
							if (AlreadyPostedVoteFailed == false) {
								AlreadyPostedVoteFailed = true;
								std::string NotEnoughforVote = "Need at least " + std::to_string(PlayersNeededToStartStartVote) + " players for a votestart";
								writeTextLobby(NotEnoughforVote);
							}
						}
					}


					if (line.find("!sessionID") != std::string::npos
						&& line.find(":") != std::string::npos) {
						endSessionID = std::chrono::steady_clock::now();
						//int secondsbwtween = std::chrono::duration_cast<std::chrono::seconds> (endSessionID - beginSessionID).count();
						//if (secondsbwtween > 5) {
						if (AlreadyPostedSessionID == false) {
							AlreadyPostedSessionID = true;
							std::cout << "posting session ID  " << endl;
							copyPasteSessionID();
							beginSessionID = std::chrono::steady_clock::now();
						}
						//}
					}
					
					if (line.find("!help") != std::string::npos
						&& line.find(":") != std::string::npos) {
						if (AlreadyPostedHelp == false) {
							std::cout << "posting !help" << endl;
							AlreadyPostedHelp = true;
							writeTextLobby(HelpCommandMessageLobby);
						}
					}

					if (line.find("!rulesetcreate") != std::string::npos
						&& line.find(":") != std::string::npos) {
						if (AlreadypostedRuleset == false) {
							std::cout << "posting !rulesetcreate" << endl;
							AlreadypostedRuleset = true;
							writeTextLobby(rulesetcreate);
						}
					}


					if (line.find("!rulesetingame") != std::string::npos
						&& line.find(":") != std::string::npos) {
						if (AlreadypostedRulesetingame == false) {
							std::cout << "posting !rulesetingame" << endl;
							AlreadypostedRulesetingame = true;
							writeTextLobby(rulesetingame);
						}
					}

					if (line.find("!discord") != std::string::npos
						&& line.find(":") != std::string::npos) {
						if (AlreadypostedDiscordLink == false) {
							std::cout << "posting !discord" << endl;
							AlreadypostedDiscordLink = true;
							writeTextLobby(DiscordLink);
						}
					}
					

					if (line.find("!starttime") != std::string::npos
						&& line.find(":") != std::string::npos) {
						if (AlreadypostedStartTime == false) {
							std::cout << "posting !starttime" << endl;
							AlreadypostedStartTime = true;
							writeTextLobby(StartTime);
						}
					}


					//WhiteList;

					if (line.find("!forcestart") != std::string::npos
						&& line.find(":") != std::string::npos
						&& MoreThanLobby
						) {
						bool whiteListedName = false;
						for (int w = 0; w < WhiteList.size(); w++) {
							if (WhiteList[w] == userName) {
								whiteListedName = true;
								std::cout << "force start by whitelisted username" << endl;
							}
						}
						if (whiteListedName == true) {
							std::cout << "force starting" << endl;
							LobbyState = START;
							writeTextLobby("Force starting");
						}
					}


				}
				myfile.close();
			}

		}

		if (LobbyState == VOTING_START
			   ) {




			std::cout << " vote counting" << endl;
			/*
			Sleep(10);
			screenCapturePart(680, 272, 1143 - 680, 788 - 272, "a.bmp");
			Mat LobbyChatVote = imread("a.bmp");



			//invert colors
			for (int r = 1; r < LobbyChatVote.rows - 1; r++) {
				for (int c = 1; c < LobbyChatVote.cols - 1; c++) {

					Vec3b pixel = LobbyChatVote.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					uchar blue = pixel[0];
					uchar green = pixel[1];
					uchar red = pixel[2];
					int blueP = (int)blue;
					int greenP = (int)green;
					int redP = (int)red;

					//Vec3b pixelHSV = HSVimage.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					//int hue = (int)pixelHSV[0];
					//int hueDifference = std::abs(hue - 84);

					Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
					Vec3b white(255, 255, 255);


					//cut out all the light colors
					if (redP < 50 && blueP < 50 && greenP < 50) {
						LobbyChatVote.at<Vec3b>(r, c) = white;
					}
					else {
						LobbyChatVote.at<Vec3b>(r, c) = invertColor;
					}
				}
			}
			imwrite(TesseractLocation + "test.jpg", LobbyChatVote);

			Sleep(10);
			//std::system("C:\\Tesseract-OCR\\tesseract C:\\Tesseract-OCR\\test.jpg C:\\Tesseract-OCR\\test");
			std::system(TesseractCommand.c_str());
			*/

			Sleep(10);
			writeTextLobby("/save test");
			Sleep(2000);

			//check the votes
			//myfile.open(TesseractLocation + "test.txt");
			int CurrentRead = 0;
			myfile.open(ParadoxDocumentsFolder + "chat logs/test.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					/*
					if (line == "") {
						continue;
					}
					*/
					CurrentRead++;
					if (CurrentRead < TotalRead) {
						continue;
					}

					std::cout << line << '\n';

					if (TotalRead <= CurrentRead) {
						TotalRead++;
					}

					string userName = "";
					string voteNumber = "";
					int vote = 0;
					if (line.find("[") != std::string::npos) {
						//chat message
						int i = 0;
						bool adduserName = false;
							for (i = 0; i < line.size(); i++) {
								if (line[i] == ')') {
									break;
								}
								if (adduserName == true) {
									userName += line[i];
								}
								if (line[i] == '(') {
									adduserName = true;
								}

							}

							int StartMessagePlace = 0; 
							//int StartMessagePlace = line.find(")");
							//make sure this is the last one of it
							while (line.find(")", StartMessagePlace + 1) != std::string::npos) {
								StartMessagePlace = line.find(")", StartMessagePlace + 1);
							}
							std::cout << adduserName << " place " << StartMessagePlace << " line size: " << line.size() << endl;
							//a vote 
							if (line.size() == StartMessagePlace + 4 ){
								if (std::isdigit(line[StartMessagePlace + 3]) ){
									voteNumber += line[StartMessagePlace + 3];
								}
							}

							/*
							for (i = i; i < line.size(); i++) {
								if ( std::isdigit( line[i] )
									 && ((i+1) == line.size()) ) {
									voteNumber += line[i];
									//break;
								}

							}
							*/


							if (voteNumber != "" && voteNumber.size() < 2) {
								std::cout << "vote to int: " << voteNumber << endl;
								vote = std::stoi(voteNumber);
							}

							//check if it is a valid vote and username
							//and that  has voted  isn't included (otherwise the host will vote)
							// && line.find("has voted") == std::string::npos
							if ((vote == 1 || vote == 2) && userName != "") {
								bool alreadyVoted = false;
								for (int j = 0; j < countingVotes.size(); j++) {
									if (countingVotes[j].Username == userName) {
										alreadyVoted = true;
									}
								}
								
								if (userName == NicknameToIgnore) {
									alreadyVoted = true;
								}
								

								if (alreadyVoted == false) {
									std::cout << "vote added: " << userName << "  voting for:" << vote << endl;
									Vote newVote;
									newVote.Username = userName;
									newVote.choice = vote;
									countingVotes.push_back(newVote);
									string confirmText = userName + " has voted " + std::to_string(vote);
									writeTextLobby(confirmText);
								}
							}
					}

				}
				myfile.close();
			}


			endVoting = std::chrono::steady_clock::now();
			int timeElapsed = std::chrono::duration_cast<std::chrono::seconds> (endVoting - beginVoting).count();

			//voting for "SecondsForVote" seconds
			if (timeElapsed > SecondsForVote) {
				std::cout << "counting votes" << endl;
				int vote1 = 0;
				int vote2 = 0;
				for (int i = 0; i < countingVotes.size(); i++) {

					if (countingVotes[i].choice == 1) {
						vote1++;
					}
					if (countingVotes[i].choice == 2) {
						vote2++;
					}
				}

				string postResults = std::to_string(vote1) + " people have voted option 1. " + std::to_string(vote2) + " have voted option 2";
				writeTextLobby(postResults);

				int MinimumVotersNeeded = (int)((PercentageNeededForVote / 100.0) * (numberofPlayers-1));
				//extra check
				if (MinimumVotersNeeded > (numberofPlayers - 1)) {
					std::cout << "somehow the amount of voters needed was higher than the amount of people in the lobby";
					MinimumVotersNeeded = (numberofPlayers - 1);
				}
				//numberofPlayers;
				if ((vote1 + vote2) < MinimumVotersNeeded) {
					std::string NotEnoughVoteMessage = "At least " + std::to_string(MinimumVotersNeeded) + " players need to vote";
					writeTextLobby(NotEnoughVoteMessage);
					beginVoting = std::chrono::steady_clock::now();
					LobbyState = TEXTING;
				}else if (vote1 > vote2 && (vote1 + vote2) > 0) {
					writeTextLobby(VoteStartPassedMessage);
					LobbyState = START;
				}
				else {
					writeTextLobby(VoteStartFailedMessage);
					beginVoting = std::chrono::steady_clock::now();
					LobbyState = TEXTING;
				}

				Sleep(1000);
			}

		} // if (LobbyState == VOTING) {





		if (LobbyState == START) {

			std::string readyupMEssage = "Game starting. Everybody ready up. Autokicking players who aren't ready will happen in " + std::to_string(SecondsToreadyUp) + " seconds";
			//writeTextLobby("Game starting. Everybody ready up. Autokicking players who aren't ready will happen in 5 seconds");
			writeTextLobby(readyupMEssage);
			Sleep(SecondsToreadyUp*1000);

			std::cout << "Trying to start player" << endl;

			int totalScroll = 50;

			//scroll back up first to reset the scrolling we did
			while (totalScroll > 0) {
				totalScroll--;
				//now scroll down to find others
				Sleep(10);
				SetMousePosition(ScrollPosition.x, ScrollPosition.y);
				//LeftClick();
				ScrollUP();
				Sleep(10);


			}

			totalScroll = 50;

			while (totalScroll > 0) {



				//first determine if we have the right number of people

				//stop this if right control is hold down
				if (GetKeyState(VK_RCONTROL) & 0x8000)
				{
					// right control down
					return 0;
				}


				totalScroll--;

				std::cout << "While loop start" << endl;



				//check number of players

				Sleep(10);

				//		Rect RectangleToSelect(440, 245, 140, 20);


				//screenCapturePart(440, 245, 140, 20, "a.bmp");
				//Mat PlayersReady = imread("a.bmp");

				//std::string locationBest = takeScreenshot();


				Mat src;

				if (ScreengrabMethodPicked == SCREENSHOT) {
					std::string locationBest = "";
					locationBest = takeScreenshot();
					src = imread(locationBest);
				}

				if (ScreengrabMethodPicked == HWNDmethod) {
					HWND hwndDesktop = GetDesktopWindow();
					src = hwnd2mat(hwndDesktop);
				}


				//TOCHANGE: do something better than this
				imwrite("checkkick.bmp", src);
				src = imread("checkkick.bmp");



				//imshow("playersthing", src);

				//waitKey(0);

				//Mat src = imread(locationBest);
				Rect RectangleToSelect(440, 245, 140, 20);
				Mat PlayersReady = src(RectangleToSelect);

				//invert colors
				for (int r = 1; r < PlayersReady.rows - 1; r++) {
					for (int c = 1; c < PlayersReady.cols - 1; c++) {

						Vec3b pixel = PlayersReady.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
						uchar blue = pixel[0];
						uchar green = pixel[1];
						uchar red = pixel[2];
						int blueP = (int)blue;
						int greenP = (int)green;
						int redP = (int)red;

						Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);

						PlayersReady.at<Vec3b>(r, c) = invertColor;
					}
				}

				cv::imwrite(TesseractLocation + "test.jpg", PlayersReady);



				Sleep(10);

				//std::string TesseractCommand = TesseractLocation + "tesseract " + 
				std::system(TesseractCommand.c_str());
				//TesseractLocation
				//std::system("C:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");


				bool done = false;
				int numberofPlayersStart = 0;
				int nomberofPlayersReadyStart = 0;
				string line;
				ifstream myfile(TesseractLocation + "test.txt");
				if (myfile.is_open())
				{
					while (getline(myfile, line))
					{
						std::cout << line << '\n';
						/*
						if (line.find("4 Players, 4s Ready") != std::string::npos) {
							//std::cout << "found!" << '\n';
							std::cout << "done\n";
							done = true;
						}
						*/

						if (line.find("Players") != std::string::npos) {

							//grab number of players
							string numberString = "";
							for (int i = 0; i < line.size(); i++) {
								if (isdigit(line[i])) {
									numberString += line[i];
								}
								if (line[i] == 'P') {
									break;
								}
							}
							std::cout << "numberString:  " << numberString << endl;
							if (is_number(numberString)) {
								numberofPlayersStart = std::stoi(numberString);
							}
							else {
								std::cout << "getting number of players from OCR went wrong" << endl;
							}

							string readyString = "";
							bool addToString = false;
							for (int i = 0; i < line.size(); i++) {
								if (isdigit(line[i]) && addToString == true) {
									readyString += line[i];
								}
								if (line[i] == ',') {
									addToString = true;
								}
								if (line[i] == 'R' && addToString == true) {
									break;
								}
							}
							//grab the ones that are ready
							std::cout << "numberReadyString:  " << readyString << endl;
							if (is_number(readyString)) {
								nomberofPlayersReadyStart = std::stoi(readyString);
							}
							else {
								std::cout << "getting number of ready players from OCR went wrong: " << readyString << endl;
							}
							/*
							//if we got enough people and they are all readied up, then start
							if (numberofPlayers == PlayersNeededBeforeStart && numberofPlayers == nomberofPlayersReady) {
								writeTextLobby("Got enough people, starting game");
								LobbyState = START;
							}
							*/

						}

					}
					myfile.close();
				}




				//end check number of players


				//all ready, so start the game
				// && numberofPlayers >= PlayersNeededBeforeStart
				if (numberofPlayersStart == nomberofPlayersReadyStart
					 ) {
					
					
					std::cout << "All ready, lets go" << endl;
					SetMousePosition(StartButton.x, StartButton.y);
					Sleep(10);
					LeftClick();
					LobbyState = INTO_GAME;//starting game
					break;
					


				}

				//otherwise, kick people who arent ready
				Sleep(10);


				SetMousePosition(0, 0);//keep mouse at 0,0 so as to not block the kick button

				//screenCapturePart(0, 0, 1920, 1080, "b.bmp");
				//Mat PlayersToBeKicked = imread("b.bmp");

				//std::string locationBest2 = takeScreenshot();

				//Mat PlayersToBeKicked = imread(locationBest2);

				Mat PlayersToBeKicked;

				if (ScreengrabMethodPicked == SCREENSHOT) {
					std::string locationBest2 = "";
					locationBest2 = takeScreenshot();
					PlayersToBeKicked = imread(locationBest2);
				}

				if (ScreengrabMethodPicked == HWNDmethod) {
					HWND hwndDesktop = GetDesktopWindow();
					PlayersToBeKicked = hwnd2mat(hwndDesktop);
				}



				//TOCHANGE: do something better than this
				imwrite("checkkick2.bmp", PlayersToBeKicked);
				PlayersToBeKicked = imread("checkkick2.bmp");


				//Rect RectangleToSelect(440, 245, 140, 20);
				//Mat PlayersReady = src(RectangleToSelect);


				for (int r = 1; r < PlayersToBeKicked.rows - 1; r++) {
					//for (int c = 1; c < src.cols - 1; c++) {
					int c = 606;
					Vec3b pixel = PlayersToBeKicked.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					uchar blue = pixel[0];
					uchar green = pixel[1];
					uchar red = pixel[2];
					int blueP = (int)blue;
					int greenP = (int)green;
					int redP = (int)red;





					//redP == 108 && greenP == 255 && blueP == 157  //ready color
					//redP == 116 && greenP == 222 && blueP == 180  //kick button color
					if (redP == 116 && greenP == 222 && blueP == 180) {
						//test.at<Vec3b>(r, c) = colorNeeded;
						//MouseY = r;
						int readyC = 650; //column in which the ready color should be apparent.
						Vec3b pixelR = PlayersToBeKicked.at<Vec3b>(r, readyC); // or img_bgr.at<Vec3b>(r,c)
						uchar blueR = pixelR[0];
						uchar greenR = pixelR[1];
						uchar redR = pixelR[2];
						int bluePr = (int)blueR;
						int greenPr = (int)greenR;
						int redPr = (int)redR;



						//ready color
						if (redPr == 108 && greenPr == 255 && bluePr == 157) {
							//if this person is ready, do nothing
						}
						else {
							std::cout << "kicking player" << r << endl;
							std::cout << redPr << " " << greenPr << " " << bluePr << endl;
							//otherwise, kick him
							int kickX = (int)(606.0);
							int kickY = (int)(r);
							if (UIfix == false) {
								kickX = (int)(606.0 / 1.25);
								kickY = (int)(r / 1.25);
							}
							//int kickX = (int)(606.0 / 1.25);
							//int kickY = (int)(r / 1.25);
							SetMousePosition(kickX, kickY);
							Sleep(10);
							LeftClick();
							break;
						}


					}

					//Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
					//src.at<Vec3b>(r, c) = invertColor;

					//}
				} // for (int r = 1; r < PlayersToBeKicked.rows - 1; r++) {


				//now scroll down to find others
				Sleep(10);
				SetMousePosition(ScrollPosition.x, ScrollPosition.y);
				//LeftClick();
				ScrollDown();
				Sleep(1000); //100


				/*
				//StartButton
				SetMousePosition(StartButton.x, StartButton.y);
				Sleep(10);
				LeftClick();
				break;
				*/
			} // while (totalScroll > 0) {
			//return 0;

		} // 		if (LobbyState == START) {


		if (LobbyState == INTO_GAME) {
			break;
		}







		//return 0;

		//Sleep(5000);

		/*
		//string message = "https://discord.gg/CsmCF3";
		string message = "https://discord.gg/CsmCF3";

		//setup to the chat thing
		SetMousePosition(TextScreen.x, TextScreen.y);
		Sleep(10);
		LeftClick();
		Sleep(10);
		for (int i = 0; i < message.length(); i++) {
			sendKeyStrokeChar(message[i]);
			Sleep(10);

		}
		sendKeyStrokeEnter();
		*/



		/*
		SetMousePosition(TextScreen.x, TextScreen.y);
		Sleep(10);
		LeftClick();
		Sleep(10);
		sendKeyStrokeChar('T');
		Sleep(10);
		sendKeyStrokeChar('e');
		Sleep(10);
		sendKeyStrokeChar('s');
		Sleep(10);
		sendKeyStrokeChar('t');
		Sleep(10);
		sendKeyStrokeEnter();
		*/

		//check if 3 players are in and ready


		/*
		HDC dc = GetDC(NULL);
		//COLORREF color = GetPixel(dc, 605, 428);
		COLORREF color = GetPixel(dc, 1900, 1000);
		int red = GetRValue(color);
		int green = GetGValue(color);
		int blue = GetBValue(color);
		std::cout << "screen size thing: " << red << " " << green << " " << blue  << endl;
		ReleaseDC(NULL, dc);
		*/



		//HWND hwndDesktop = GetDesktopWindow();
		//Mat src = hwnd2mat(hwndDesktop);


		//building up the screenshot from scratch
		//HDC dc = GetDC(NULL);


		//HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);;
		
		

		//if (IsClipboardFormatAvailable(CF_BITMAP))
			//handle = (HBITMAP)GetClipboardData(CF_BITMAP);
	    //}
		//Mat src2 = bitmap2mat(handle);
		//Mat src = hwnd2mat(hwndDesktop);
		//imshow("output", src2);
		//CBitmap* bm = CBitmap::FromHandle(handle);

		//waitKey(0);

	   // Get the window handle of the Stellaris application.
		//LPCTSTR WindowName = "Stellaris";
		//HWND StellarisWindow = ::FindWindow(0, WindowName);


		//HWND hwndDesktop = GetDesktopWindow();
		//Mat src = hwnd2mat(StellarisWindow);
		//Mat src = hwnd2mat(hwndDesktop);
		//imshow("output", src);

		//cout << "image size: " << src.cols << " " << src.rows;

		//waitKey(0);

		/*
		Mat ScreenShot(1080, 1920, CV_8UC3);


		ScreenCap();
		std::cout << "starting pixel grab"  << endl;
		std::cout << ScreenX << " " << ScreenY << endl;

		return 0;
		//int ScreenX = GetDeviceCaps(GetDC(0), HORZRES);
		//int ScreenY = GetDeviceCaps(GetDC(0), VERTRES);


		for (int r = 1; r < ScreenShot.rows - 1; r++) {
			for (int c = 1; c < ScreenShot.cols - 1; c++) {

				//cout << " getting pixel at: " << r << " " << c << endl;

				//COLORREF color = GetPixel(dc, c, r);

				int red = PosR(c, r);
				int green = PosG(c, r);
				int blue = PosB(c, r);

				Vec3b BGR(blue, green, red);
				ScreenShot.at<Vec3b>(r, c) = BGR;

		  }

	    }

		//ReleaseDC(NULL, dc);

		std::cout << "finished pixel grab" << endl;



		imshow("output", ScreenShot);


		std::cout << "screen size thing: " << ScreenShot.cols << " " << ScreenShot.rows << endl;



		waitKey(0);
		*/

		//  X = 606  for the kick button
	    //  X = 650  for the line


		int MouseX = 605;
		int MouseY = 0;
		/*
		for (int r = 1; r < src.rows - 1; r++) {
			//for (int c = 1; c < src.cols - 1; c++) {
			    int c = 700;
				Vec3b pixel = src.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
				uchar blue = pixel[0];
				uchar green = pixel[1];
				uchar red = pixel[2];
				int blueP = (int)blue;
				int greenP = (int)green;
				int redP = (int)red;



				//redP == 108 && greenP == 255 && blueP == 157
				//redP == 116 && greenP == 222 && blueP == 180
				if (redP == 116 && greenP == 222 && blueP == 180) {
					//test.at<Vec3b>(r, c) = colorNeeded;
					MouseY = r;
				}

				Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
				src.at<Vec3b>(r, c) = invertColor;

			//}
		}
		*/


		//std::cout << "Mouse position: " << MouseX << " " << MouseY << endl;

		//SetMousePosition(MouseX, MouseY);

		//imshow("output2", src);


		//waitKey(0);
		//Sleep(500000);


		/*

		Rect RectangleToSelect(440, 245, 140, 20);
		Mat dst5 = src(RectangleToSelect);
		Mat dst5Invert;
		//cvNot(dst5, dst5);

		cv::threshold(dst5, dst5Invert, 70, 255, THRESH_BINARY_INV);

		//imshow("output2", dst5Invert);


		imwrite("D:/Tesseract-OCR/test.jpg", dst5Invert); // 

		Sleep(10);

		std::system("D:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");

		bool done = false;
		string line;
		ifstream myfile("D:/Tesseract-OCR/test.txt");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				cout << line << '\n';

				if (line.find("3 Players, 3 Ready") != std::string::npos) {
					//std::cout << "found!" << '\n';
					cout << "done\n";
					done = true;
				}

		

			}
			myfile.close();
		}


		if (done == true) {
			//StartButton
			SetMousePosition(StartButton.x, StartButton.y);
			Sleep(10);
			LeftClick();
			break;
		}

		*/

	}//end loop for lobby



    LabelIngame: ;





	bool InGame = false;

	int TotalReadInGame = 0;//how many lines to skip in the in-game chat

	std::string toKick = "";

	//now do the things while in game
	while (1) {

		std::cout << "currently in game" << endl;



		if (GetKeyState(VK_RCONTROL) & 0x8000)
		{
			// right control down
			return 0;
		}


		Sleep(5000);

		if (GameState == START_LOADING) {

			std::cout << "Start loading" << endl;

			bool GameStarted = true;
			//screenCapturePart(715, 417, 885- 715, 441- 417, "a.bmp");//location of "waiting for players"
			//Mat GameReady = imread("a.bmp");



			// Get the window handle of the Stellaris application.
			 //LPCTSTR WindowName = "Stellaris";
			 //HWND StellarisWindow = ::FindWindow(0, WindowName);
			 //Mat GameReady2 = hwnd2mat(StellarisWindow);


			 /*
			 takeScreenshot();

			 Sleep(4000);

			 WIN32_FIND_DATA data;
			 //std::string screenshotLocation = "C:\\Users\\Gebruiker\\Documents\\Paradox Interactive\\Stellaris\\screenshots\\";
			 std::string screenshotLocation = ParadoxDocumentsFolder + "screenshots/";
			 std::string tc = screenshotLocation + "/*";
			 HANDLE hFind = FindFirstFile(tc.c_str(), &data);      // DIRECTORY

			 int bestTime = 0;
			 std::string locationBest = "";

			 if (hFind != INVALID_HANDLE_VALUE) {
				 do {
					 //std::cout << data.cFileName << std::endl;
					 std::string screenshotName(data.cFileName);
					 if (screenshotName.size() < 3) {
						 continue;
					 }
					 std::string getscreenshotLocation = screenshotLocation + screenshotName;
					 //std::cout << "location of file: " << getscreenshotLocation << std::endl;

					 std::string filename = getscreenshotLocation;
					 struct stat result;
					 if (stat(filename.c_str(), &result) == 0)
					 {
						 auto mod_time = result.st_mtime;
						 std::cout << mod_time << endl;
						 if (mod_time > bestTime) {
							 bestTime = mod_time;
							 locationBest = filename;
						 }
						 //...
					 }

				 } while (FindNextFile(hFind, &data));
				 FindClose(hFind);
			 }



			 std::cout << "best string: " << locationBest << endl;

			 std::cout << "best time: " << bestTime << endl;

			 //return 0;
			 */

			 //std::string locationBest = takeScreenshot();

			 //Mat src = imread(locationBest);

			 //Rect RectangleToSelect(715, 417, 885 - 715, 445 - 417);
			 //Mat GameReady2 = src(RectangleToSelect);

			 //Mat GameReady2 = imread(locationBest);


			 Mat src;

			 if (ScreengrabMethodPicked == SCREENSHOT) {
				 std::string locationBest = "";
				 locationBest = takeScreenshot();
				 src = imread(locationBest);
			 }

			 if (ScreengrabMethodPicked == HWNDmethod) {
				 HWND hwndDesktop = GetDesktopWindow();
				 src = hwnd2mat(hwndDesktop);
			 }


			 //TOCHANE: ? somehow works fine here


			 Rect RectangleToSelect2(715, 417, 885 - 715, 445 - 417);
			 Mat GameReady = src(RectangleToSelect2);
			 cv::imwrite("LoadingScreen.jpg", GameReady);
			 //Mat dst5 = src(RectangleToSelect);

			 //HWND hwndDesktop = GetDesktopWindow();
			 //Mat src = hwnd2mat(StellarisWindow);
			 //Mat src = hwnd2mat(hwndDesktop);
			 //imshow("output", src);


			//invert colors
			for (int r = 1; r < GameReady.rows - 1; r++) {
				for (int c = 1; c < GameReady.cols - 1; c++) {

					Vec3b pixel = GameReady.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					uchar blue = pixel[0];
					uchar green = pixel[1];
					uchar red = pixel[2];
					int blueP = (int)blue;
					int greenP = (int)green;
					int redP = (int)red;

					Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
					Vec3b white(255, 255,255);
					//Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
					if ( (redP < 80) && (blueP < 80) && (greenP < 80) ) {
						GameReady.at<Vec3b>(r, c) = white;
					}
					else {
						GameReady.at<Vec3b>(r, c) = invertColor;
					}
				}
			}

			cv::imwrite(TesseractLocation + "test.jpg", GameReady);



			Sleep(10);

			//std::string TesseractCommand = TesseractLocation + "tesseract " + 
			//std::string TesseractCommandPNG = TesseractLocation + "tesseract " + TesseractLocation + "test.png " + TesseractLocation + "test";
			std::string DPIcommand = TesseractCommand + " --dpi 200";

			//std::system(TesseractCommand.c_str());
			std::system(DPIcommand.c_str());
			//TesseractLocation
			//std::system("C:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");
			Sleep(100);

			string line;
			ifstream myfile(TesseractLocation + "test.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					std::cout << line << '\n';

					if (line.find("Waiting") != std::string::npos
						&& InGame == false) {
						InGame = true;
						//GameStarted = false;
					}

					if (line.find("Waiting") != std::string::npos
						&& InGame == true) {
						GameStarted = false;
					}


				}
				myfile.close();
			}

			if (InGame == true && GameStarted == true) {
				GameState = INITIALIZE;//now that the game has loaded, unpause and bring up chat
			}

			if (InGame == true && GameStarted == false){

				SetMousePosition(StartButtonGame.x, StartButtonGame.y);
				Sleep(10);
				LeftClick();
			}


		} // 		if (GameState == START_LOADING) {


		if (GameState == INITIALIZE) {


			//press enter to get rid of the initial screen. Shouldn't matter in observer mode, but does if you pick an empire
			Sleep(1000);
			sendKeyStrokeEnter();

			//set game speed to fast
			Sleep(10);
			SetMousePosition(FasterButton.x, FasterButton.y);
			Sleep(10);
			LeftClick();

			/*
			//unpause the game
			Sleep(10);
			SetMousePosition(PauseButton.x, PauseButton.y);
			Sleep(10);
			LeftClick();
			*/

			/*
			2.6.3
			//more button  to open chat
			Sleep(10);
			SetMousePosition(MoreButton.x, MoreButton.y);
			Sleep(10);
			LeftClick();
			*/

			//open chat
			Sleep(100);
			SetMousePosition(ChatButton.x, ChatButton.y);
			Sleep(10);
			LeftClick();
			Sleep(2000);

			GameState = CHAT;

			//SecondsTillAutoUnpause;
			
			std::string messageUnpause = "Game will auto unpause in " + std::to_string(SecondsTillAutoUnpause) + " seconds";

			writeTextChatGame(messageUnpause);

			writeTextChatGame("/members");
			writeTextChatGame("/save members");

			//wait for the program to be done writing
			Sleep(100);

			string members = "";
			string line;
			ifstream myfile(ParadoxDocumentsFolder + "chat logs/members.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					std::cout << line << '\n';

					if (line.find("Members:") != std::string::npos) {
						std::cout << "found members "<< endl;
						ofstream fout("members.txt");
						fout << line;
						fout.close();
					}


				}
				myfile.close();
			}

			//10 seconds
			//Sleep(10000);
			Sleep(SecondsTillAutoUnpause * 1000);

			writeTextChatGame(AutoUnpauseActivatedMessage);

		}

		if (GameState == CHAT) {






			endMessage = std::chrono::steady_clock::now();
			int timeElapsedMessage = std::chrono::duration_cast<std::chrono::seconds> (endMessage - beginMessage).count();

			if (timeElapsedMessage > secondsBetweenMessage) {
				beginMessage = std::chrono::steady_clock::now();
				writeTextChatGame(MessageChatInGame);

			}


			//writeTextChatGame(MessageLobby);



			writeTextChatGame("/save ingame");

			Sleep(200);

			int CurrentRead = 0;
			std::string line;
			ifstream myfile;
			myfile.open(ParadoxDocumentsFolder + "chat logs/ingame.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					/*
					if (line == "") {
						continue;
					}
					*/
					CurrentRead++;
					if (CurrentRead < TotalReadInGame) {
						continue;
					}

					std::cout << line << '\n';

					if (TotalRead <= CurrentRead) {
						TotalRead++;
					}

					//string userName = "";
					string voteName = "";
					if (line.find("!sessionID") != std::string::npos) {
						writeTextChatGame(sessionID);

					}

					if (line.find("!votekick") != std::string::npos
						&& GameState != VOTE_KICK) {
						std::cout << "starting a votekick";

						toKick = "";
						int getPlayerToKick = line.find("!votekick") + 10;
						for (int i = getPlayerToKick; i < line.size(); i++) {
							toKick += line[i];
						}
						std::string messageVoteKickStarted = "Starting a vote to kick player: " + toKick + ". Vote 1 for yes, 2 for no";
						writeTextChatGame(messageVoteKickStarted);
						countingVotes.clear();
						beginVoting = std::chrono::steady_clock::now();
						GameState = VOTE_KICK;
					}

				}
				myfile.close();
			}






			//check if the game is paused


			//1780 , 25

			/*

			//screenCapturePart(1780, 15, 1850 - 1780, 35 - 15, "a.bmp");//location of "waiting for players"
			//Mat GameSpeed = imread("a.bmp");
			screenCapturePart(0, 0, 1920, 1080, "a.bmp");//location of "waiting for players"
			Mat GameSpeed = imread("a.bmp");


			Vec3b pixel = GameSpeed.at<Vec3b>(25, 1780); // or img_bgr.at<Vec3b>(r,c)
			uchar blue = pixel[0];
			uchar green = pixel[1];
			uchar red = pixel[2];
			int blueP = (int)blue;
			int greenP = (int)green;
			int redP = (int)red;

			//123 , 73 , 51
			
			std::cout << "colors pixel" << redP << " " << greenP << " " << blueP << endl;




			if ( (redP == 123) && (greenP == 73) && (blueP == 51) ) {
				//if the game is paused, then unpause it
				std::cout << "unpausing game" << endl;
				Sleep(10);
				SetMousePosition(PauseButton.x, PauseButton.y);
				Sleep(10);
				LeftClick();
				Sleep(10);
				//move the mouse back to the chat to prevent OCR errors
				SetMousePosition(TextChat.x, TextChat.y);
			}

			*/

			/*
			HDC dc = GetDC(NULL);
			//COLORREF color = GetPixel(dc, 605, 428);
			COLORREF color = GetPixel(dc, 1780, 25);
			int red = GetRValue(color);
			int green = GetGValue(color);
			int blue = GetBValue(color);
			std::cout << "Unpause Pixel values at location x=1780 y=25: " << red << " " << green << " " << blue << endl;
			*/

			//std::string PauseSCreen = takeScreenshot();
			//Mat PauseMat = imread(PauseSCreen);

			Mat PauseMat;

			if (ScreengrabMethodPicked == SCREENSHOT) {
				std::string locationBest = "";
				locationBest = takeScreenshot();
				PauseMat = imread(locationBest);
			}

			if (ScreengrabMethodPicked == HWNDmethod) {
				HWND hwndDesktop = GetDesktopWindow();
				PauseMat = hwnd2mat(hwndDesktop);
			}


			//TOCHANGE: do something better than this
			imwrite("pause.bmp", PauseMat);
			PauseMat = imread("pause.bmp");


			Vec3b pixelP = PauseMat.at<Vec3b>(25, 1780); // or img_bgr.at<Vec3b>(r,c)
			uchar blueP = pixelP[0];
			uchar greenP = pixelP[1];
			uchar redP = pixelP[2];
			int bluePP = (int)blueP;
			int greenPP = (int)greenP;
			int redPP = (int)redP;

			std::cout << "pause pixel test: " << redPP << " " << greenPP << " " << bluePP << endl;

			if ((redPP == 123) && (greenPP == 73) && (bluePP == 51)) {
				//if the game is paused, then unpause it
				std::cout << "unpausing game" << endl;
				Sleep(10);
				SetMousePosition(PauseButton.x, PauseButton.y);
				Sleep(10);
				LeftClick();
				Sleep(10);
				//move the mouse back to the chat to prevent OCR errors
				SetMousePosition(TextChat.x, TextChat.y);
			}



			/*
			//Begin check for laggers




			//set mouse to the date to get all users and those who lag
			SetMousePosition(PauseButton.x, PauseButton.y);
			Sleep(100);

			screenCapturePart(0, 0, 1920, 1080, "a.bmp");//location of "waiting for players"
			Mat LaggerCheck = imread("a.bmp");

			//1662, 53
			Vec3b pixelL = LaggerCheck.at<Vec3b>(1662, 53); // or img_bgr.at<Vec3b>(r,c)
			uchar blueL = pixelL[0];
			uchar greenL = pixelL[1];
			uchar redL = pixelL[2];
			int bluePL = (int)blueL;
			int greenPL = (int)greenL;
			int redPL = (int)redL;


			std::cout << "colors pixel" << redPL << " " << greenPL << " " << bluePL << endl;

			int length = 114 - 54;
			//first check if the username is even open
			// 119 , 221 , 177
			if ( (redPL == 119) && (greenPL == 221) && (bluePL == 177)) {
				int c = 1662;
				//start at 60 (61), and find the bottom pixel
				int r = 60;
				while (r < LaggerCheck.rows - 1) {
					r++;
					Vec3b pixelL2 = LaggerCheck.at<Vec3b>(c, r); // or img_bgr.at<Vec3b>(r,c)
					uchar blueL2 = pixelL2[0];
					uchar greenL2 = pixelL2[1];
					uchar redL2 = pixelL2[2];
					int bluePL2 = (int)blueL2;
					int greenPL2 = (int)greenL2;
					int redPL2 = (int)redL2;
					if ((redPL2 == 119) && (greenPL2 == 221) && (bluePL2 == 177)) {
						length = r = 54;

					}
			
				}

			}
			else {
				std::cout << "username lag not showing" << endl;
			}



			//end check for laggers
			*/


			/*
			//invert colors
			for (int r = 1; r < GameSpeed.rows - 1; r++) {
				for (int c = 1; c < GameSpeed.cols - 1; c++) {

					Vec3b pixel = GameSpeed.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					uchar blue = pixel[0];
					uchar green = pixel[1];
					uchar red = pixel[2];
					int blueP = (int)blue;
					int greenP = (int)green;
					int redP = (int)red;

					Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);
					Vec3b black(255 - blueP, 255 - greenP, 255 - redP);



					GameSpeed.at<Vec3b>(r, c) = invertColor;
				}
			}

			imwrite(TesseractLocation + "test.jpg", GameSpeed);



			Sleep(10);

			//std::string TesseractCommand = TesseractLocation + "tesseract " + 
			std::system(TesseractCommand.c_str());
			//TesseractLocation
			//std::system("C:\\Tesseract-OCR\\tesseract D:\\Tesseract-OCR\\test.jpg D:\\Tesseract-OCR\\test");


			string line;
			ifstream myfile(TesseractLocation + "test.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					std::cout << line << '\n';

					if (line.find("Paused") != std::string::npos) {
						//if the game is paused, then unpause it

						Sleep(10);
						SetMousePosition(PauseButton.x, PauseButton.y);
						Sleep(10);
						LeftClick();
						Sleep(10);
						//move the mouse back to the chat to prevent OCR errors
						SetMousePosition(TextChat.x, TextChat.y);
					}

					

				}
				myfile.close();
			}

			*/

			//return 0;
		}


		if (GameState == VOTE_KICK) {





			std::cout << " vote counting" << endl;

			//writeTextLobby("/save ingame");
			writeTextChatGame("/save ingame");
			Sleep(100); //give it some time to write

			//check the votes
			//myfile.open(TesseractLocation + "test.txt");
			int CurrentRead = 0;
			ifstream myfile;
			std::string line;
			myfile.open(ParadoxDocumentsFolder + "chat logs/ingame.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					/*
					if (line == "") {
						continue;
					}
					*/
					CurrentRead++;
					if (CurrentRead < TotalReadInGame) {
						continue;
					}

					std::cout << line << '\n';

					if (TotalReadInGame <= CurrentRead) {
						TotalReadInGame++;
					}

					string userName = "";
					string voteNumber = "";
					int vote = 0;
					if (line.find("[") != std::string::npos) {
						//chat message
						int i = 0;
						bool adduserName = false;
						for (i = 0; i < line.size(); i++) {
							if (line[i] == ')') {
								break;
							}
							if (adduserName == true) {
								userName += line[i];
							}
							if (line[i] == '(') {
								adduserName = true;
							}

						}


						int StartMessagePlace = 0;
						//int StartMessagePlace = line.find(")");
						//make sure this is the last one of it
						while (line.find(")", StartMessagePlace + 1) != std::string::npos) {
							StartMessagePlace = line.find(")", StartMessagePlace + 1);
						}
						std::cout << adduserName << " place " << StartMessagePlace << " line size: " << line.size() << endl;
						//a vote 
						if (line.size() == StartMessagePlace + 4) {
							if (std::isdigit(line[StartMessagePlace + 3])) {
								voteNumber += line[StartMessagePlace + 3];
							}
						}


						/*
						for (i = i; i < line.size(); i++) {
							if (std::isdigit(line[i])
								&& ((i + 1) == line.size())) {
								voteNumber += line[i];
								//break;
							}

						}
						*/



						if (voteNumber != "" && voteNumber.size() < 2) {
							std::cout << "vote to int: " << voteNumber << endl;
							vote = std::stoi(voteNumber);
						}

						//check if it is a valid vote and username
						//&& line.find("has voted") == std::string::npos
						if ((vote == 1 || vote == 2) && userName != "") {
							bool alreadyVoted = false;
							for (int j = 0; j < countingVotes.size(); j++) {
								if (countingVotes[j].Username == userName) {
									alreadyVoted = true;
								}
							}
							/*
							if (userName == "LetaBot") {
								alreadyVoted = true;
							}
							*/

							if (alreadyVoted == false) {
								std::cout << "vote added: " << userName << "  voting for:" << vote << endl;
								Vote newVote;
								newVote.Username = userName;
								newVote.choice = vote;
								countingVotes.push_back(newVote);
								string confirmText = userName + " has voted " + std::to_string(vote);
								writeTextChatGame(confirmText);
								//writeTextLobby(confirmText);
							}
						}
					}

				}
				myfile.close();
			}


			endVoting = std::chrono::steady_clock::now();
			int timeElapsed = std::chrono::duration_cast<std::chrono::seconds> (endVoting - beginVoting).count();

			//voting for "SecondsForVote" seconds
			if (timeElapsed > SecondsForVote) {
				std::cout << "counting votes" << endl;
				int vote1 = 0;
				int vote2 = 0;
				for (int i = 0; i < countingVotes.size(); i++) {

					if (countingVotes[i].choice == 1) {
						vote1++;
					}
					if (countingVotes[i].choice == 2) {
						vote2++;
					}
				}

				string postResults = std::to_string(vote1) + " people have voted option 1. " + std::to_string(vote2) + " have voted option 2";
				//writeTextLobby(postResults);
				writeTextChatGame(postResults);

				if (vote1 > vote2 && (vote1 + vote2) > 0 ) {
					std::string voteKickPassed = "Vote kick passed. Kicking player: " + toKick;
					writeTextChatGame(voteKickPassed);
					GameState = KICK;
				}
				else {
					std::string voteKickFailed = "Vote kick failed. Not kicking player: " + toKick;
					writeTextChatGame(voteKickFailed);
					//writeTextLobby(VoteStartFailedMessage);
					beginVoting = std::chrono::steady_clock::now();
					GameState = CHAT;
				}

				Sleep(1000);
			}




		} // if( gamestate votekick)


		if (GameState == KICK) {

			//first, get in the player windows


			//PutmousetoScrollPlayerList


	        //first make sure the chat/player window is up:
			CheckIfChatIsUp();

			bool KickedPlayer = false;

			Sleep(10);
			SetMousePosition(SwitchToPlayerButton.x, SwitchToPlayerButton.y);
			Sleep(10);
			LeftClick();
			Sleep(100);

			//begin test grabbing usernames ---------------------------------------------------------------------------------------------------

			//RGB:  180,202,195
			//std::string TesseractCommand2 = TesseractLocation + "tesseract " + TesseractLocation + "test.jpg " + TesseractLocation + "test";
			//std::string testSCreen = "C:/Users/Gebruiker/Documents/Paradox Interactive/Stellaris/screenshots/2020_04_30_1.png";

			//Rect RectangleToSelect(440, 245, 140, 20);
			//Mat dst5 = src(RectangleToSelect);


				//		Rect RectangleToSelect(440, 245, 140, 20);
			//screenCapturePart(440, 245, 140, 20, "a.bmp");
			//Mat GetPlayersChat = imread(testSCreen);
			//cv::imshow("test", GetPlayersChat);

			int TotalScroll = 10; // 50
			while (TotalScroll > 0) {
				TotalScroll--;


				if (GetKeyState(VK_RCONTROL) & 0x8000)
				{
					// right control down
					return 0;
				}

				//first make sure the chat/player window is up:
				CheckIfChatIsUp();


				//screenCapturePart(0, 0, 1920, 1080, "a.bmp");//location of "waiting for players"
				//Mat GetPlayersChat = imread("a.bmp");
				//cv::waitKey(0);


				//std::string locationBest = takeScreenshot();

				//Mat GetPlayersChat = imread(locationBest);

				Mat GetPlayersChat;

				if (ScreengrabMethodPicked == SCREENSHOT) {
					std::string locationBest = "";
					locationBest = takeScreenshot();
					GetPlayersChat = imread(locationBest);
				}

				if (ScreengrabMethodPicked == HWNDmethod) {
					HWND hwndDesktop = GetDesktopWindow();
					GetPlayersChat = hwnd2mat(hwndDesktop);
				}


				//TOCHANGE: do something better than this
				imwrite("getplayersChat.bmp", GetPlayersChat);
				GetPlayersChat = imread("getplayersChat.bmp");

				//Rect RectangleToSelect(440, 245, 140, 20);
				//Mat PlayersReady = src(RectangleToSelect);

				//check username
				for (int r = 1; r < GetPlayersChat.rows - 1; r++) {
					int c = 479;
					//for (int c = 1; c < GetPlayersChat.cols - 1; c++) {

					Vec3b pixel = GetPlayersChat.at<Vec3b>(r, c); // or img_bgr.at<Vec3b>(r,c)
					uchar blue = pixel[0];
					uchar green = pixel[1];
					uchar red = pixel[2];
					int blueP = (int)blue;
					int greenP = (int)green;
					int redP = (int)red;
					if (redP == 180 && greenP == 202 && blueP == 195) {
						std::cout << "first pixel found:" << r << endl;

						//out of bounds check
						if (r + 99 >= GetPlayersChat.rows) {
							std::cout << "out of bounds";
							continue;
						}

						//check if this pixel thing is also the case a bit further ahead.
						//cause if not, then you wont be able to click the kick button
						Vec3b pixel2 = GetPlayersChat.at<Vec3b>(r + 99, c); // or img_bgr.at<Vec3b>(r,c)
						uchar blue2 = pixel2[0];
						uchar green2 = pixel2[1];
						uchar red2 = pixel2[2];
						int blueP2 = (int)blue2;
						int greenP2 = (int)green2;
						int redP2 = (int)red2;
						if (redP2 == 180 && greenP2 == 202 && blueP2 == 195) {
							std::cout << "second pixel found" << endl;
							Rect RectangleToSelect(102, r + 5, 410 - 102, 20);
							Mat screenName = GetPlayersChat(RectangleToSelect);
							//cv::imshow("test", screenName);
							//cv::waitKey(0);


							//invert colors
							for (int r3 = 1; r3 < screenName.rows - 1; r3++) {
								for (int c3 = 1; c3 < screenName.cols - 1; c3++) {
									Vec3b pixel3 = screenName.at<Vec3b>(r3, c3); // or img_bgr.at<Vec3b>(r,c)
									uchar blue3 = pixel3[0];
									uchar green3 = pixel3[1];
									uchar red3 = pixel3[2];
									int blueP3 = (int)blue3;
									int greenP3 = (int)green3;
									int redP3 = (int)red3;
									Vec3b invertColor3(255 - blueP3, 255 - greenP3, 255 - redP3);
									screenName.at<Vec3b>(r3, c3) = invertColor3;
								}
							}

							//cv::imshow("test", screenName);
							//cv::waitKey(0);

							cv::imwrite(TesseractLocation + "test.jpg", screenName);
							Sleep(10);
							std::system(TesseractCommand.c_str());


							string line;
							ifstream myfile(TesseractLocation + "test.txt");
							if (myfile.is_open())
							{
								while (getline(myfile, line))
								{
									std::cout << line << '\n';
									if (line.size() > 1) {
										std::cout << "found a player: " << line << endl;
										if (line.find(toKick) != std::string::npos) { //found the player we wanted to kick
											std::cout << "kicking player: " << toKick << endl;
											int kickButtonX = 456;
											int kickButtonY = r + 70;
											if (UIfix == false) {
												kickButtonX = (int)(kickButtonX / 1.25);
												kickButtonY = (int)(kickButtonY / 1.25);
											}
											SetMousePosition(kickButtonX, kickButtonY);
											Sleep(10);
											LeftClick();


											KickedPlayer = true;
											break;
										}
									}
								}
								myfile.close();
							}


						}


					}

					if (KickedPlayer == true) {
						break;
					}

					//Vec3b invertColor(255 - blueP, 255 - greenP, 255 - redP);

					//GetPlayersChat.at<Vec3b>(r, c) = invertColor;
				//}
				} //for loop

				//return 0;

				if (KickedPlayer == true) {
					break;
				}

				//scroll down the list
				Sleep(10);
				SetMousePosition(PutmousetoScrollPlayerList.x, PutmousetoScrollPlayerList.y);
				Sleep(100);
				ScrollDown();
				Sleep(10);
				ScrollDown();
				Sleep(10);
				ScrollDown();
				Sleep(10);
				ScrollDown();
				Sleep(10);
				ScrollDown();
				Sleep(2000);


			}//while scroll

			//regardless of succes, switch back to chat

			//first make sure the chat/player window is up:
			CheckIfChatIsUp();


			Sleep(10);
			SetMousePosition(SwitchToChatButton.x, SwitchToChatButton.y);
			Sleep(10);
			LeftClick();
			Sleep(10);
			SetMousePosition(10, 10);//move the mouse cursor out of the way
			Sleep(1000);

			if (KickedPlayer == true) {
				std::string succesString = "Managed to kick " + toKick;
				writeTextChatGame(succesString);
			}
			else {
				std::string failedString = "Failed to kick " + toKick;
				writeTextChatGame(failedString);

			}


			GameState = CHAT;

			//end grabbing usernames ingame ---------------------------------------------------------------------------------------------------




		} //if state == kick


	}// 		while (1) {  game playing







	waitKey(0);

	return 0;

}
