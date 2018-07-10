#pragma once

//#include "KinectPose.h"
#include "MyKinect.h"
#include "SVMClassifier.h"

#define MAX_USERS 2
#define NEAREST_NEIGHBOURS 5
#define HOLD_POSE 6

#define POSE_DATA_DIR "./instruments/"

class PoseRecognizer {

public:

	PoseRecognizer();
	~PoseRecognizer();

	bool initialize();

	/**
	Acquire and process the next frame from the Kinect
	*/
	bool processNextFrame();

	/**
	Return the frame from the Kinect's RGB camera
	*/
	cv::Mat* getRgbFrame();

	/**
	Return the depth frame from Kinect's ToF sensor
	@param equalize Is it necessary to equalize the histogram in the grayscale image?
	*/
	cv::Mat* getDepthFrame();

	/**
	Modify the input frame
	*/
	cv::Mat* getSkeletonImage();

	/**
	Adds a new posr with the specified name
	*/
	void addNewPose(const std::string poseName);

	/**
	Remember current pose by the first user
	*/
	void savePoseData(const std::string poseName);
	void savePoseData(const int poseIndex);

	/**
	Get a list of currently available poses
	*/
	std::vector<KinectPose>* getPoseVector();

	/**
		Compare the current joint positions to only one specific pose
	*/
	double expectPose(const int poseIndex);
	double expectPose(const std::string poseName);

	/**
	Compare the current joint positions to only one specific pose for a specific user
	*/
	double expectPose(UINT64& userId, const int poseIndex);
	double expectPose(UINT64& userId, const std::string poseName);

	/**
	Get a list of currently tracked users
	*/
	UserList* getUserList();


private:

	// user list
	UserList* userList;

	cv::Mat* depthMap;
	cv::Mat* bgrImage;
	cv::Mat* skeletonImage;

	/**
		Reload the pose information from the files in a different folder.
		See the description of the initPoseData() method for details on how
		the files should be formatted.

		@param folder The name of the folder that contains the pose data.

		@return Returns "true" if the reinitialization of the pose data was successful.
	*/
	bool reloadPoseData(const std::string folder);

	// pose data from the files
	std::vector<KinectPose> poseVector;

	// number of nearest neighbours
	int nearestNeighbours = 7;

	/**
		Read the position data from the files in the provided folder. Every files has to have
		the .txt file extension and needs to have the following contents:

		Pose_name
		Degree_of_freedom
		Left_elbow_angle,Right_elbow_angle;
		Left_hand_direction,Right_hand_direction;
		Left_elbow_position;
		Left_wrist_position;
		Right_elbow_position;
		Right_wrist_position;

		here, "Pose_name" is just the name of the pose, like "Flute", "Violin" or "Conducting"
		"Degree_of_freedom" is the value that determines how much freedom is given to the recognition
		algorithm when trying to recognize this particular pose. The bigger the value, the more precise
		the user needs to be when emulating the pose. Values from 15 to 20 are optimal.

		You can easily add new positions to the system, by adding a new file with only
		the first two lines present. To record completely new data for existing pose, remove
		everything except the first two lines.

		@param path The name of the folder that contains the pose data.

		@return Returns the vector of new poses, that were parsed from the files in the folder.
	*/
	std::vector<KinectPose> initPoseData(const std::string path);

	/**
		Estimate the pose of the current user. The method calculates the 'distance' from the
		current user's feature vector to every training sample of evey pose, then uses the
		K Nearest Neighbours algorithm to recognize the position.

		If a position was recognized it will automatically update the info about the current pose
		in the user list.

		@param user The reference to the current user, for whom the recognition is performed
		@param featureString A vector of features, extracted for the curren user
		@param nearestNeighbours The parameter for the Nearest Neighbours algorithm. Default: 5.

		@return The number of the recognized pose in the pose vector or -1 if no pose was recognized.
	*/
	int estimatePose(KinectBody* user, const FeatureString& featureString);

	/**
		Toggles remembering current pose
	*/
	bool rememberCurrentPose = false;
	int currentPoseId = 0;

	// Kinect Adapter:
	CBodyBasics myKinect;
	HRESULT hr;

	/* Machine Learning algorithm */
	SVMClassifier svm;	

	void retrainClassifier();

	/* Prettify the RGB frame */
	void prettify(/*KinectBody* body, */cv::Scalar& color);
};

