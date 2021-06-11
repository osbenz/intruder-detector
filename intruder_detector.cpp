#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include<iostream>
#include<stdlib.h>
#include <string>

int main()
{	
	cv::VideoCapture capture(0); // the 0 is for the default computer webcam try other integers if it doesn't work for 0 

	// check if the webcam is opened or not
	if (!capture.isOpened()) {
		std::cerr << "Error! Can't open the webcam" << std::endl;
		return -1;
	}
	Sleep(5000);

	std::vector<cv::Point2f> p0, p1;  
	cv::Mat prevFrame, prevGray; // previous frame, previous gray
	
	// find good features in the first frame
	capture >> prevFrame;
	cv::cvtColor(prevFrame, prevGray, cv::COLOR_BGR2GRAY);
	goodFeaturesToTrack(prevGray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);

	int numImages(0);
	while (1) {
		cv::Mat currFrame, currGray; // current frame, current gray
		int count(0);
		capture >> currFrame;
		cv::cvtColor(currFrame, currGray, cv::COLOR_BGR2GRAY);

		// calculate optical flow
		std::vector<uchar> status;
		std::vector<float> err;
		cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS), 10, 0.03);
		cv::calcOpticalFlowPyrLK(prevGray, currGray, p0, p1, status, err, cv::Size(15, 15), 2, criteria);
		
		for (uint i = 0; i < p0.size(); i++) {
			if (status[i] == 0) count++;
		}

		std::cout << p1.size() << " , " << p1.size() << " , " << status.size() << " , " << count << std::endl;
		if (count >= 5) {
			numImages++;
			std::string path = "./images/intruder" + std::to_string(numImages);
			std::string filepath = path + ".jpg";
			cv::imwrite(filepath, currFrame);
			if (numImages >= 20) {
				system("C:\\Windows\\System32\\shutdown /s /t 0");
				return 0;
			}
		}

		// updating the previous frame and previous points
		prevGray = currGray.clone();
		p0 = p1;
	}

	return 0;
}

