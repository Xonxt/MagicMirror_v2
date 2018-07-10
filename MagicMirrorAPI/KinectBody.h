#pragma once
#include <Kinect.h>
#include <string>
#include "KinectPose.h"

class KinectBody
{
public:
	KinectBody(ICoordinateMapper* m_pCoordinateMapper);
	~KinectBody();

	cv::Point3f extractJoint3D(const JointType& joint);
	cv::Point2f extractJoint2D(const JointType& joint);
	cv::Point2f extractJoint2DColor(const JointType& joint);

	CameraSpacePoint getCameraSpaceJointPosition(const JointType& joint);
	DepthSpacePoint  getDepthSpaceJointPosition(const JointType& joint);
	ColorSpacePoint getColorSpaceJointPosition(const JointType& joint);

	void setJoint(JointType joint, const Joint& position);
	TrackingState getJointTrackingState(JointType joint);

	BOOLEAN isBodyTracked();
	void setTrackedState(BOOLEAN state);

	HandState getLeftHandState();
	HandState getRightHandState();

	void setLeftHandState(const HandState& handState);
	void setRightHandState(const HandState& handState);

	// get/set distance
	void setDistanceToUser(const double distance);
	double getDistanceToUser();

	/**
	Get the id of the current user's pose.
	@return Returns the id (index) of the current recognized pose or -1 if no pose is detected
	*/
	int getPoseIndex();

	/**
	Set the new pose index
	*/
	void setPoseIndex(const int index);

	/**
	Get the name of the current pose (instrument)
	*/
	std::string getPoseName();

	/**
	Change the name of the current pose, assumed by the user
	*/
	void setPoseName(const std::string& poseName);

	/**
		Set the new pose

		@param pose Pointer to the pose
	*/
	void setPose(KinectPose* pose);

	/**
		Get the pointer to the current pose of the user
	*/
	KinectPose* getPose();

	/**
		Extract the user's features from the current frame.
		Refer to the KinectPose.h for details.
	*/
	FeatureString extractUserFeatures();

	/**
		Get the tumbler array for the current pose recognition attempt.
		See the "poseTumbler" attribute for details.
	*/
	std::vector<int> getPoseTumbler();

	/**
		Add a new number to the tumbler.
		@param tumbler ID of the pose
		@param max Maximum number of tumblers (i.e. for how many consecutive frames does
		the user need to hold the pose for it ti be recognized). Default: 3.
	*/
	void addPoseTumbler(const int tumbler, const int max = 3);

	/**
		Get the average value from the tumbler
	*/
	float getPoseTumblerAvg();

	void addHandLeftPositionHostory(cv::Point3f point);
	void addHandRightPositionHostory(cv::Point3f point);
	void addHandLeftPositionHostory(cv::Point2f point);
	void addHandRightPositionHostory(cv::Point2f point);
	
	/* Get the hand tracks */
	Hand* getLeftHandTracker();
	Hand* getRightHandTracker();

private:
	BOOLEAN isTracked = false;

	Joint joints[JointType_Count];
	HandState leftHandState = HandState_Unknown;
	HandState rightHandState = HandState_Unknown;

	// two hands for calculating acerage speed:
	Hand handLeft, handRight;

	// coordinate mapper to convert 3D to Depth
	ICoordinateMapper* m_pCoordinateMapper;

	// The pointer to the current pose of the user
	KinectPose* userPose;

	// The current pose name
	std::string poseName = "";

	// current pose id
	int poseIndex = -1;

	// vector of salien points (most important joint coordinates)
	std::vector<cv::Point3f> salientPoints;

	// distance between user's shoulders
	double shoulderDist;
	
	// distance to user in meters
	double distanceToUser = 1.0f;

	/**
	The pose tumbler. This is basiaclly a queue of pose id's, which is kept at certain length.
	This is used to check, for how many concesutive frames does the user hold a certain pose, before
	concluding that this is indeed a newly recognized pose.

	For example, let's say a user is first standing still (-1). And then at frame 6 he assumes
	a "Violin playing position" (2). The the tumbler will look like this:

	Frame:  Tumbler:
	0                  <---- Nothing
	1       -1
	2       -1 -1
	3       -1 -1 -1   <---- Still nothing
	4       -1 -1 -1
	5       -1 -1 -1
	6        2 -1 -1   <---- User assumes position...
	7        2  2 -1
	8        2  2  2   <---- "Violin" pose recognized!!
	*/
	std::vector<int> poseTumbler;
};


