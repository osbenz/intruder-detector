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
	cv::VideoCapture capture(0); // The value 0 is for the default computer webcam 

	// Check if the webcam is opened or not
	if (!capture.isOpened()) {
		std::cerr << "Error! Can't open the webcam" << std::endl;
		return 0;
	}
	Sleep(5000);

	std::vector<cv::Point2f> p0, p1;  
	cv::Mat prevFrame, prevGray; // previous frame, previous gray
	
	// Find good features in the first frame
	capture >> prevFrame;
	cv::cvtColor(prevFrame, prevGray, cv::COLOR_BGR2GRAY);
	goodFeaturesToTrack(prevGray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);

	int numImages(0);
	while (1) {
		cv::Mat currFrame, currGray; // current frame, current gray
		int count(0);
		capture >> currFrame;
		cv::cvtColor(currFrame, currGray, cv::COLOR_BGR2GRAY);

		// Compute optical flow
		std::vector<uchar> status;
		std::vector<float> err;
		cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS), 10, 0.03);
		cv::calcOpticalFlowPyrLK(prevGray, currGray, p0, p1, status, err, cv::Size(15, 15), 2, criteria);
		
		for (uint i = 0; i < p0.size(); i++) {
			if (status[i] == 0) count++;
		}

		// If there are enough differences between the two frames (we can choose other value than 5)
		if (count >= 5) {
			numImages++;
			std::string path = "./images/intruder" + std::to_string(numImages);
			std::string filepath = path + ".jpg";
			cv::imwrite(filepath, currFrame);
			
			// I decided to take 20 photos, that would be enough to identify the intruder 
			if (numImages >= 20) {
				#ifdef _WIN32 // If Windows
				// Activate existing python virtual environment
				system(".\\goole_drive_api\\env\\Scripts\\activate");
				// Execute python script that uploads the photos taken by our intruder detector program
				system(".\\goole_drive_api\\google_drive_api.py"); 
				system("C:\\Windows\\System32\\shutdown /s");
				return 0;
				#else // Linux or Mac
				system("source ./goole_drive_api/env/bin/activate");
				system("./goole_drive_api/google_drive_api");
				system("shutdown -P now");
				return 0;
				#endif		
			}
		}

		// updating the previous frame and previous points
		prevGray = currGray.clone();
		p0 = p1;
	}

	return 0;
}

