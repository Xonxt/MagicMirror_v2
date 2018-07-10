#pragma once

#include "Utils.h"

struct Hand {
	enum Direction {
		NONE = 0,
		LEFT, RIGHT, 
		DOWN, UP,
		FORWARDS, BACKWARDS
	};

	std::vector<cv::Point3f> positionHistory;
	std::vector<cv::Point2f> path;
	int maxSize;
	bool directionChanged = true;

	std::string temp = "";

	Direction movementDirection = Direction::NONE;
	Direction previousDirection = Direction::NONE;

	Hand(int mxSize = 5) {
		maxSize = mxSize;
		positionHistory.clear();
	}

	void add(cv::Point3f point) {
		positionHistory.push_back(point);

		if (positionHistory.size() > maxSize) {
			positionHistory.erase(positionHistory.begin());
		}

		previousDirection = movementDirection;
		movementDirection = getDirection();

		if (movementDirection == previousDirection)
			directionChanged = false;
		else
			directionChanged = true;
	}

	void add(cv::Point2f point) {
		path.push_back(point);

		if (path.size() > (maxSize + 1)) {
			path.erase(path.begin());
		}
	}

	double getAverageAcceleration() {
		if (positionHistory.size() == 1)
			return 0.0f;
		else {
			double acc = 0.0f;
			for (int i = 0; i < positionHistory.size() - 1; i++) {
				acc += util::calcDistance(positionHistory[i], positionHistory[i + 1]);
			}
			return (acc / (positionHistory.size() - 1)) * 10.0f;
		}
	}

	/// <summary>
	/// Check if the hand is moving or not
	/// </summary>
	/// <returns>TRUE if the hand is moving, FALSE if it doesn't</returns>
	bool isMoving() {
		if (positionHistory.size() == 0)
			return false;

		double mx = 0, my = 0, mz = 0, dx = 0, dy = 0, dz = 0;

		// calculate means
		for (size_t i = 0; i < positionHistory.size(); i++) {
			mx += positionHistory.at(i).x;
			my += positionHistory.at(i).y;
			mz += positionHistory.at(i).z;
		}
		mx /= positionHistory.size();
		my /= positionHistory.size();
		mz /= positionHistory.size();

		// calculate standard deviations
		for (size_t i = 0; i < positionHistory.size(); i++) {
			dx += std::pow(positionHistory.at(i).x - mx, 2);
			dy += std::pow(positionHistory.at(i).y - my, 2);
			dz += std::pow(positionHistory.at(i).z - mz, 2);
		}
		dx = std::sqrt(dx / positionHistory.size());
		dy = std::sqrt(dy / positionHistory.size());
		dz = std::sqrt(dz / positionHistory.size());

		// if the standard deviation on all axes is small, the hand is not moving
		if (dx <= 0.01 && dy <= 0.05 && dz <= 0.01)
			return false;
		else
			return true;
	}

	/// <summary>
	/// Return the movement direction
	/// </summary>
	/// <returns></returns>
	Direction getDirection() {
		if (positionHistory.size() <= 1 || !isMoving())
			return Direction::NONE;

		// vector:
		cv::Vec3f vec = positionHistory.at(positionHistory.size() - 1) - positionHistory.at(0);
		float x = vec.val[0] * 1.5f, y = vec.val[1], z = vec.val[2];

		// angles to axes
		float ax, ay, az;
		ax = std::roundf(std::fabsf(std::acos(x / (std::sqrt(x*x + y*y + z*z)))) * (180.0 / 3.14159265359));
		ay = std::roundf(std::fabsf(std::acos(y / (std::sqrt(x*x + y*y + z*z)))) * (180.0 / 3.14159265359));
		az = std::roundf(std::fabsf(std::acos(z / (std::sqrt(x*x + y*y + z*z)))) * (180.0 / 3.14159265359));

		// convert from radians to degrees

		// check directions:
		int dx = 0, dy = 0, dz = 0;
		int angle = 30;

		if (ax <= (angle + 15))
			dx = 1;
		if (ax >= (180 - (angle + 15)))
			dx = -1;
		
		if (ay <= angle)
			dy = 1;
		if (ay >= (180 - angle))
			dy = -1;

		if (az <= angle)
			dz = 1;
		if (az >= (180 - angle))
			dz = -1;

		//temp = std::to_string((int)dx) + " : " + std::to_string((int)dy) + " : " + std::to_string((int)dz);

		// return directions:
		if (dx > 0 && dy == 0 && dz == 0)
			return Direction::RIGHT;

		if (dx < 0 && dy == 0 && dz == 0)
			return Direction::LEFT;

		if (dx == 0 && dy > 0 && dz == 0)
			return Direction::UP;

		if (dx == 0 && dy < 0 && dz == 0)
			return Direction::DOWN;

		if (dx == 0 && dy == 0 && dz > 0)
			return Direction::BACKWARDS;

		if (dx == 0 && dy == 0 && dz < 0)
			return Direction::FORWARDS;
	}
};


/**
The class, incapsulating the information about a position, that needs to be recognized
*/
class KinectPose {
public:

	/**
	Create a new pose object by parsing the specified data file
	*/
	KinectPose(const int index, const std::string& fileName);

	/**
	Create a new pose object with the specified name, using a vector of feature vectors
	*/
	KinectPose(const int index, const std::string& poseName, const std::vector<FeatureString>& featureVector);

	/**
	These two shouldn't be used
	*/
	KinectPose();
	KinectPose(const int index);

	/**
	Parse the provided file and read all the data about the pose (training samples)

	@param fileName The full or relative path to the file containing pose info.
	*/
	void parsePoseDataFile(const std::string& fileName);

	/**
	Get the entire training data of the pose.
	*/
	std::vector<FeatureString>* getFeatureVector();

	/**
	Get the name of the current pose
	*/
	std::string getPoseName();

	/**
	Set the new name for the current pose

	@param poseName The name of the position.
	*/
	void setPoseName(const std::string& poseName);

	/**
	Get the name of the file that contains the tranining info for this pose
	*/
	std::string getFileName();

	/**
	Set a new file name for this pose

	@param fileName The path to the file containing the training info for this pose
	*/
	void setFileName(const std::string& fileName);

	/**
	Get the id of the pose.
	*/
	int getPoseIndex();

	/**
	This will add the provided feature vector as a new training sample for the current pose

	@param featureString The feature vector from the current instance for the first user
	*/
	void addNewTrainingSample(const FeatureString& featureString);

	~KinectPose();

	/// <summary>
	/// Get the confidence of the pose recognition using Gaussian kernel
	/// </summary>
	/// <returns>Confidcence</returns>
	double getConfidence(const FeatureString& featureString);

private:
	// a vector of all traning samples
	std::vector<FeatureString> featureVector;

	// name of the pose
	std::string poseName;

	// name of the file, that contains the training data
	std::string fileName;

	// hte index (ID) of the pose
	int poseIndex;

	/**
	Get the difference between two angles in the Descartes (Cartesian) coordinate system (0..360)
	@param alpha First angle
	@param beta Second angle
	*/
	double angleDifference(const double alpha, const double beta);

	/**
	Determines if the provided value lies in the specified range

	@param value The value in question
	@param A The lower limit
	@param B The upper limit
	*/
	bool isInRange(const double value, const double A, const double B);

	void recalculateStdev();
	std::vector<float> meansVector;
	std::vector<float> stdevVector;
};

