#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <unordered_map>
#include <omp.h>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#define PI M_PI

typedef std::vector<double> FeatureString;

namespace util {

	static cv::Scalar Color[] = {
		cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 0), cv::Scalar(255, 0, 0),
		cv::Scalar(255, 0, 255), cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 255),
		cv::Scalar(255, 69, 0), cv::Scalar(255, 128, 128), cv::Scalar(255, 128, 255),
		 cv::Scalar(0,0,0) };

	// access colors with these names
	enum Colors {
		COLOR_WHITE = 0,
		COLOR_RED = 1,
		COLOR_GREEN = 2,
		COLOR_BLUE = 3,
		COLOR_PURPLE,
		COLOR_CYAN,
		COLOR_YELLOW,
		COLOR_ORANGE,
		COLOR_LIGHTBLUE,
		COLOR_PINK,		
		COLOR_BLACK
	};

	/// <summary>
	/// Check if the a value lies in the specified range
	/// </summary>
	/// <param name="value">The value in question</param>
	/// <param name="A">The lower limit</param>
	/// <param name="B">The upper limit</param>
	/// <returns>Returns "true" if the value lies in the range A &lt;= V &lt;= B</returns>
	bool isInRange(const double value, const double A, const double B);

	/// <summary>
	/// Correct the angle to a proper Cartesian (0..360) orientation
	/// </summary>
	/// <param name="alpha">The input angle</param>
	/// <returns>The angle in the 0..360 range</returns>
	double correctAngle(double alpha);

	/// <summary>
	/// Get the angle of a vector between two specified points
	/// </summary>
	/// <param name="pt1">The starting point of the vector</param>
	/// <param name="pt2">The ending poing of the vector</param>
	/// <returns>The value of the angle in degrees (not radians!)</returns>
	double getAngle(const cv::Point2f& pt1, const cv::Point2f& pt2);

	/// <summary>
	/// Calculate difference between two angles in the Cardesian coordinate system (0..360)
	/// </summary>
	/// <param name="alpha">The first angle</param>
	/// <param name="beta">The second angle</param>
	/// <returns>The correct difference between two angles</returns>
	double angleDifference(const double alpha, const double beta);

	/// <summary>
	/// Calculate the Euclidian distance between two points in the 3D space
	/// using formula: D = sqrt((x1 - x2) ^ 2 + (y1 - y2) ^ 2 + (z1 - z2) ^ 2)
	/// </summary>
	/// <param name="a">The first point</param>
	/// <param name="b">The second point</param>
	/// <returns>The distance between points</returns>
	double calcDistance(const cv::Point3f& a, const cv::Point3f& b);

	/// <summary>
	/// Calculate the Euclidian distance between two points in the 3D space
	/// using formula: D = sqrt( (x1-x2)^2 + (y1-y2)^2 )
	/// </summary>
	/// <param name="a">The first point</param>
	/// <param name="b">The second point</param>
	/// <returns>The distance between points</returns>
	double calcDistance(const cv::Point2f& a, const cv::Point2f& b);

	/// <summary>
	/// Calculates the sigmoid function of x
	/// </summary>
	/// <param name="x">x</param>
	/// <returns>sigmoid function</returns>
	double sigmoidFunction(const double x);

	/// <summary>
	/// Calculate the angle, formed by three points in 3D space. 	
	/// Used to calculate the elbow angle between the upper arm and lower arm,
	/// where the three points are : shoulder - elbow - wrist.
	/// </summary>
	/// <param name="a">The first point</param>
	/// <param name="b">The second point</param>
	/// <param name="c">The third point</param>
	/// <param name="direction">Do we need to take into account the direction</param>
	/// <returns>The angle in radians</returns>
	double getThreePointAngle(const cv::Point3f& a, const cv::Point3f& b, const cv::Point3f& c, bool direction = false);

	/// <summary>
	/// Decrements the value, but doesn't allow it to go lower than the provided limit
	/// </summary>
	/// <param name="value">The variable that needs to be decremented</param>
	/// <param name="lowerLimit">The minimal value beyond which the variable will not decrease</param>
	void declim(int& value, const int lowerLimit);

	/// <summary>
	/// Increments the value, but doesn't allow it to go higher than the provided limit
	/// </summary>
	/// <param name="value">The variable that needs to be incremented</param>
	/// <param name="upperLimit">The maximum value beyond which the variable will not increase</param>
	void inclim(int& value, const int upperLimit);

	/// <summary>
	/// Overlays a transparent image over another image
	/// </summary>
	/// <param name="src">The original soure image, upon which another image will be overlaid</param>
	/// <param name="overlay">The image that will be overlaid on the source image</param>
	/// <param name="location">The upper left corner position of the overlay image on the destination image</param>
	void overlayImage(cv::Mat& src, cv::Mat& overlay, const cv::Point & location);

	/// <summary>
	/// Correctly crops a specified rectangle from the image, taking into account the image boundaries
	/// </summary>
	/// <param name="inputImage">Input image</param>
	/// <param name="outputCrop">Output cropped region</param>
	/// <param name="roiRectangle">The Rectange that specifies the cropped region</param>
	void cropRoi(const cv::Mat inputImage, cv::Mat& outputCrop, cv::Rect& roiRectangle);

	/// <summary>
	/// Return current time as string
	/// </summary>
	/// <returns></returns>
	std::string getTimeAsString();

	/// <summary>
	/// Returns all file names in a specified folder. Warning: Uses Windows API!
	/// </summary>
	/// <param name="folder">The path to the folder</param>
	/// <returns>Returns a list of file names</returns>
	std::vector<std::string> get_all_files_names_within_folder(std::string folder);

	/// <summary>
	/// Log an event (a string) in a 'log.txt' file
	/// </summary>
	/// <param name="message">Message to be logged in the file</param>
	void Log(std::string message);

	/// <summary>
	/// Generate a rundom integer number in the specified range
	/// </summary>
	/// <param name="a">Tye minimal value</param>
	/// <param name="b">The maximum value</param>
	/// <returns>A random integer in the range [a; b]</returns>
	int randBetween(int a, int b);
}
