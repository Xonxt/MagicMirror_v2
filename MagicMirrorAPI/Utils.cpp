#include "stdafx.h"
#include "Utils.h"

namespace util {
	
	bool isInRange(const double value, const double A, const double B) {
		if (value >= std::fmin(A, B) && value <= std::fmax(A, B))
			return true;
		else
			return false;
	}

	// correct the angle to a proper Descartes (0..360) orientation
	double correctAngle(double alpha) {
		double angle = alpha;

		if (isInRange(alpha, 90, 180)) {				// 1st quadrant
			angle -= 90;
		}
		else if (isInRange(alpha, -90, -180)) {	// 2nd quadrant
			angle = (180 - abs(alpha)) + 90;
		}
		else if (isInRange(alpha, -0, -90)) {		// 3rd quadrant
			angle = 180 + (90 - abs(alpha));
		}
		else if (isInRange(alpha, 0, 90)) {			// 4th quadrant
			angle = 270 + abs(alpha);
			if (angle >= 360)
				angle = 0;
		}

		return (angle);
	}

	// get the angle of a vector, from pt1 to pt2
	double getAngle(const cv::Point2f& pt1, const cv::Point2f& pt2) {
		double alpha; // result

					  // calculate vector coordinates
		double
			y = pt2.y - pt1.y,
			x = pt2.x - pt1.x;

		// hypotenuse
		double sqrtResult = 0;
		sqrtResult = std::sqrt(x * x + y * y);
		double angle;
		angle = std::asin(abs(x) / sqrtResult);

		alpha = angle * 180 / M_PI; // angle from North in degrees

								  // correction
		if (x > 0) {
			// I or IV quadrant
			if (y < 0) {
				// IV quadrant
				alpha = 180 - alpha;
			}
		}
		else {
			// II or III quadrant
			if (y > 0) {
				// II quadrant
				alpha = -alpha;
			}
			else
				alpha = alpha - 180;
		}

		return correctAngle(alpha);
	}

	double angleDifference(const double alpha, const double beta) {
		if (isInRange(alpha, 270, 360) && isInRange(beta, 0, 90)) {
			return (360 - alpha) + beta;
		}
		else if (isInRange(beta, 270, 360) && isInRange(alpha, 0, 90)) {
			return (360 - beta) + alpha;
		}
		else {
			return std::abs(alpha - beta);
		}
	}

	double calcDistance(const cv::Point3f& a, const cv::Point3f& b) {
		return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2));
	}

	double calcDistance(const cv::Point2f& a, const cv::Point2f& b) {
		return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
	}

	double sigmoidFunction(const double x)
	{
		return (1 / (1 + std::exp(-x)));
	}

	double getThreePointAngle(const cv::Point3f& a, const cv::Point3f& b, const cv::Point3f& c, bool direction) {

		cv::Point3f upperArm = a - b;
		cv::Point3f lowerArm = b - c;

		double angle = (upperArm.ddot(lowerArm)) /
			(std::sqrt(std::pow(upperArm.x, 2) + std::pow(upperArm.y, 2) + std::pow(upperArm.z, 2)) *
				std::sqrt(std::pow(lowerArm.x, 2) + std::pow(lowerArm.y, 2) + std::pow(lowerArm.z, 2)));

		double sign = 1.0f;

		if (direction)
			if (c.x < b.x)
				sign = -1.0f;

		return sign * std::acos(angle);
	}

	/**
	Decrements the value, but doesn't allow it to go lower than the provided limit

	@param value The variable that needs to be decremented
	@param lowerLimit The minimal value beyond which the variable will not decrease
	*/
	void declim(int& value, const int lowerLimit) {
		value = (--value < lowerLimit) ? lowerLimit : value;
	}

	/**
	Increments the value, but doesn't allow it to go higher than the provided limit

	@param value The variable that needs to be incremented
	@param upperLimit The maximum value beyond which the variable will not increase
	*/
	void inclim(int& value, const int upperLimit) {
		value = (++value >= upperLimit) ? upperLimit : value;
	}


	/**
	Overlays a transparent image over another image
	@param src The original soure image, upon which another image will be overlaid
	@param overlat The image that will be overlaid on the source image
	@param The upper left corner position of the overlay image on the destination image
	*/
	void overlayImage(cv::Mat& src, cv::Mat& overlay, const cv::Point& location) {
		for (int y = std::max(location.y, 0); y < src.rows; ++y) {
			int fY = y - location.y;

			if (fY >= overlay.rows)
				break;

			for (int x = std::max(location.x, 0); x < src.cols; ++x) {
				int fX = x - location.x;

				if (fX >= overlay.cols)
					break;

				double opacity = ((double)overlay.data[fY * overlay.step + fX * overlay.channels() + 3]) / 255;

				for (int c = 0; opacity > 0 && c < src.channels(); ++c) {
					unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
					unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
					src.data[y * src.step + src.channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity * 0.75;
				}
			}
		}
	}

	/**
		Correctly crops a specified rectangle from the image, taking into account
		the image boundaries
	*/
	void cropRoi(const cv::Mat inputImage, cv::Mat& outputCrop, cv::Rect& roiRectangle) {
		cv::Rect roiRect;

		roiRect.x = (roiRectangle.x < 0) ? 0 : roiRectangle.x;
		roiRect.y = (roiRectangle.y < 0) ? 0 : roiRectangle.y;

		if ((roiRect.x - 1 + roiRectangle.width) >= inputImage.cols) {
			roiRect.width = inputImage.cols - 1 - roiRect.x;
		}
		else
			roiRect.width = roiRectangle.width - 1;

		if ((roiRect.y - 1 + roiRectangle.height) >= inputImage.rows) {
			roiRect.height = inputImage.rows - 1 - roiRect.y;
		}
		else
			roiRect.height = roiRectangle.height - 1;

		roiRect.x = abs(roiRect.x);
		roiRect.y = abs(roiRect.y);
		roiRect.width = abs(roiRect.width);
		roiRect.height = abs(roiRect.height);

		outputCrop = cv::Mat(inputImage, roiRect);

		roiRectangle = roiRect;
	}

	/**
		Return current time as string
	*/
	std::string getTimeAsString()
	{
		char name[12];
		time_t now = time(0);

		strftime(name, sizeof(name), "%H-%M-%S", localtime(&now));

		std::ostringstream ost;
		ost << name;

		return ost.str();
	}

	/**
		Gets the list of files in the folder. Uses Windows API, so it needs to be reworked
		for other operating systems

		@param folder The folder that contains files

		@return A string vector with the names of files in the folder.
	*/
	
	std::vector<std::string> get_all_files_names_within_folder(std::string folder) {
		std::vector<std::string> names;
		std::string s = folder + "/*.txt";
		std::wstring search_path = std::wstring(s.begin(), s.end());

		WIN32_FIND_DATA fd;
		HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

					std::string fullPath = folder + "/";

					std::wstring fname(std::wstring(fullPath.begin(), fullPath.end()) + fd.cFileName);

					fullPath = std::string(fname.begin(), fname.end());

					names.push_back(fullPath);
				}
			} while (::FindNextFile(hFind, &fd));
			::FindClose(hFind);
		}
		return names;
	}

	void Log(std::string message) {
		std::ofstream ofs("log.txt", std::ios::out | std::ios::app);
		ofs << util::getTimeAsString() << " :: " << message << std::endl;
		ofs.close();

	}
	int randBetween(int a, int b)
	{
		return std::rand() % (b - a) + a;
	}
}