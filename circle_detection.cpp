#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

void check_program_arguments(int argc) {
	if(argc != 2) {
		std::cout << "Error! Program usage:" << std::endl;
		std::cout << "./circle_detect image_circles_path" << std::endl;
		std::exit(-1);
	}	
}

void check_if_image_exist(const cv::Mat &img, const std::string &path) {
	if(img.empty()) {
		std::cout << "Error! Unable to load image: " << path << std::endl;
		std::exit(-1);
	}	
}

int main(int argc, char **argv) {
	// Usage: ./circle_detect image_circles_path
	//check_program_arguments(argc);
	
	// Load input image
	std::string path_image{argv[1]};
	
	cv::VideoCapture cap(path_image);

	// Check if the image can be loaded
	//check_if_image_exist(bgr_image, path_image);
	
	if (!cap.isOpened())
	{
	  std::cout << "ERROR: Cannot open camera" << std::endl;
	  return -1;
	}
	
	std::cout << "open camera ok" << std::endl;
	
	cv::Mat bgr_image;
	
	while (1)
	{
    cap >> bgr_image;
    
    if (bgr_image.empty()) break;
    
    cv::Mat orig_image = bgr_image.clone();

	  cv::medianBlur(bgr_image, bgr_image, 3);

	  // Convert input image to HSV
	  cv::Mat hsv_image;
	  cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

	  // Threshold the HSV image, keep only the red pixels
	  cv::Mat lower_red_hue_range;
	  cv::Mat upper_red_hue_range;
	  cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
	  cv::inRange(hsv_image, cv::Scalar(156, 100, 100), cv::Scalar(180, 255, 255), upper_red_hue_range);
	
	  // Combine the above two images
	  cv::Mat red_hue_image;
	  cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
	
	  cv::Mat blue_hue_image;
	  cv::inRange(hsv_image, cv::Scalar(100, 100, 100), cv::Scalar(124, 255, 255), blue_hue_image);
	
	  cv::Mat mix_hue_image;
	  cv::addWeighted(red_hue_image, 1.0, blue_hue_image, 1.0, 0.0, mix_hue_image);
	  
	  cv::Mat dilate_image;
	  cv::dilate(mix_hue_image, dilate_image, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9)), cv::Point(-1, -1), 1);

    cv::Mat blur_image;
	  cv::GaussianBlur(mix_hue_image, blur_image, cv::Size(9, 9), 2, 2);

	  // Use the Hough transform to detect circles in the combined threshold image
	  std::vector<cv::Vec3f> circles;
	  cv::HoughCircles(blur_image, circles, CV_HOUGH_GRADIENT, 1, blur_image.rows/8, 100, 60, 0, 0);

	  // Loop over all detected circles and outline them on the original image
	  // if(circles.size() == 0) std::exit(-1);
	  for(size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		  cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		  int radius = std::round(circles[current_circle][2]);

		  cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 0), 5);
	  }

	  // Show images
	  cv::namedWindow("HSV image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("HSV image", hsv_image);
	  cv::namedWindow("Threshold lower image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Threshold lower image", lower_red_hue_range);
	  cv::namedWindow("Threshold upper image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Threshold upper image", upper_red_hue_range);
	  cv::namedWindow("Threshold blue hue image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Threshold blue hue image", blue_hue_image);
	  cv::namedWindow("Combined red hue threshold image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Combined red hue threshold image", red_hue_image);
	  cv::namedWindow("Combined mix hue threshold image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Combined mix hue threshold image", mix_hue_image);
	  cv::namedWindow("Dilate image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Dilate image", dilate_image);
	  cv::namedWindow("Blur image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Blur image", blur_image);
	  cv::namedWindow("Detected red circles on the input image", cv::WINDOW_AUTOSIZE);
	  cv::imshow("Detected red circles on the input image", orig_image);

	  cv::waitKey(30);
  }
	
  return 0;
}
