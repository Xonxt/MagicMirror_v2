#include "stdafx.h"
#include "KinectBody.h"

KinectBody::KinectBody(ICoordinateMapper* m_pCoordinateMapper)
{
	this->m_pCoordinateMapper = m_pCoordinateMapper;
	handLeft = Hand(4);
	handRight = Hand(4);
}

KinectBody::~KinectBody()
{
}

CameraSpacePoint KinectBody::getCameraSpaceJointPosition(const JointType & joint)
{
	return this->joints[joint].Position;
}

DepthSpacePoint KinectBody::getDepthSpaceJointPosition(const JointType & joint)
{
	DepthSpacePoint depthSpacePosition;
	this->m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[joint].Position, &depthSpacePosition);
	return depthSpacePosition;
}

ColorSpacePoint KinectBody::getColorSpaceJointPosition(const JointType & joint)
{
	ColorSpacePoint point;
	this->m_pCoordinateMapper->MapCameraPointToColorSpace(joints[joint].Position, &point);
	return point;
}

cv::Point3f KinectBody::extractJoint3D(const JointType & joint)
{
	return cv::Point3f(joints[joint].Position.X, joints[joint].Position.Y, joints[joint].Position.Z);
}

cv::Point2f KinectBody::extractJoint2D(const JointType & joint)
{
	DepthSpacePoint point = getDepthSpaceJointPosition(joint);
	return cv::Point2f(point.X, point.Y);
}

cv::Point2f KinectBody::extractJoint2DColor(const JointType & joint)
{
	ColorSpacePoint point = getColorSpaceJointPosition(joint);
	return cv::Point2f(point.X, point.Y);
}

void KinectBody::setJoint(JointType joint, const Joint & position)
{
	this->joints[joint] = position;
}

TrackingState KinectBody::getJointTrackingState(JointType joint)
{
	return joints[joint].TrackingState;
}

BOOLEAN KinectBody::isBodyTracked()
{
	return this->isTracked;;
}

void KinectBody::setTrackedState(BOOLEAN state)
{
	this->isTracked = state;
}

HandState KinectBody::getLeftHandState()
{
	return this->leftHandState;
}

HandState KinectBody::getRightHandState()
{
	return this->rightHandState;
}

void KinectBody::setLeftHandState(const HandState & handState)
{
	this->leftHandState = handState;
}

void KinectBody::setRightHandState(const HandState & handState)
{
	this->rightHandState = handState;
}

void KinectBody::setDistanceToUser(const double distance)
{
	distanceToUser = distance;
}

double KinectBody::getDistanceToUser()
{
	return distanceToUser;
}

int KinectBody::getPoseIndex()
{
	return this->poseIndex;
}

void KinectBody::setPoseIndex(const int index)
{
	this->poseIndex = index;
}

std::string KinectBody::getPoseName()
{
	return this->poseName;
}

void KinectBody::setPoseName(const std::string & poseName)
{
	this->poseName = poseName;
}

void KinectBody::setPose(KinectPose * pose)
{
	this->userPose = pose;
	this->poseName = pose->getPoseName();
	this->poseIndex = pose->getPoseIndex();
}

KinectPose * KinectBody::getPose()
{
	return this->userPose;
}

FeatureString KinectBody::extractUserFeatures()
{
	FeatureString featureString;

	// [False False  True False  True  True False  True  True  True False False]
	// [False  True False False  True  True  True False  True  True False False]

	/*
		Angles formed by three points in space: Shoulder - Elbow - Hand
	*/
	double angle_LeftElbow =
		util::getThreePointAngle(
			extractJoint3D(JointType_ShoulderLeft), 
			extractJoint3D(JointType_ElbowLeft), 
			extractJoint3D(JointType_HandLeft));
	featureString.push_back(angle_LeftElbow);

	double angle_RightElbow =
		util::getThreePointAngle(
			extractJoint3D(JointType_ShoulderRight), 
			extractJoint3D(JointType_ElbowRight), 
			extractJoint3D(JointType_HandRight));			
	featureString.push_back(angle_RightElbow);

	/*
		Angles between shoulder-line and elbow
	*/
	double angle_LeftShoudler =
		util::getThreePointAngle(
			extractJoint3D(JointType_SpineShoulder),
			extractJoint3D(JointType_ShoulderLeft),
			extractJoint3D(JointType_ElbowLeft));
	featureString.push_back(angle_LeftShoudler);

	double angle_RightShoulder =
		util::getThreePointAngle(
			extractJoint3D(JointType_SpineShoulder),
			extractJoint3D(JointType_ShoulderRight),
			extractJoint3D(JointType_ElbowRight));
	featureString.push_back(angle_RightShoulder);

	/*
		Angle (direction) from center of body to hands and elbows:
	*/
	double direction_leftHand =
		util::getThreePointAngle(
			extractJoint3D(JointType_Head),
			extractJoint3D(JointType_SpineMid),
			extractJoint3D(JointType_HandLeft), true);
	featureString.push_back(direction_leftHand);

	double direction_rightHand =
		util::getThreePointAngle(
			extractJoint3D(JointType_Head),
			extractJoint3D(JointType_SpineMid),
			extractJoint3D(JointType_HandRight), true);
	featureString.push_back(direction_rightHand);

	double direction_leftElbow =
		util::getThreePointAngle(
			extractJoint3D(JointType_Head),
			extractJoint3D(JointType_SpineMid),
			extractJoint3D(JointType_ElbowLeft), true);
	featureString.push_back(direction_leftElbow);

	double direction_rightElbow =
		util::getThreePointAngle(
			extractJoint3D(JointType_Head),
			extractJoint3D(JointType_SpineMid),
			extractJoint3D(JointType_ElbowRight), true);
	featureString.push_back(direction_rightElbow);

	/*
		Distance from body center to both hands
	*/
	double dist_leftHand = 
		util::calcDistance(extractJoint3D(JointType_SpineMid), extractJoint3D(JointType_HandLeft));
	featureString.push_back(dist_leftHand);

	double dist_rightHand =
		util::calcDistance(extractJoint3D(JointType_SpineMid), extractJoint3D(JointType_HandRight));
	featureString.push_back(dist_rightHand);	

	// finally, the average acceleration of both hands
	featureString.push_back(handLeft.getAverageAcceleration());
	featureString.push_back(handRight.getAverageAcceleration());

	return featureString;
}

std::vector<int> KinectBody::getPoseTumbler()
{
	return poseTumbler;
}

void KinectBody::addPoseTumbler(const int tumbler, const int max)
{
	poseTumbler.push_back(tumbler);

	if (poseTumbler.size() > max)
		poseTumbler.erase(std::begin(poseTumbler));
}

float KinectBody::getPoseTumblerAvg()
{
	float avg = 0;
	std::for_each(std::begin(poseTumbler), std::end(poseTumbler), [&avg](int& x) { avg += x; });
	avg /= poseTumbler.size();
	return avg;
}

void KinectBody::addHandLeftPositionHostory(cv::Point3f point)
{
	handLeft.add(point);
}

void KinectBody::addHandRightPositionHostory(cv::Point3f point)
{
	handRight.add(point);
}

void KinectBody::addHandLeftPositionHostory(cv::Point2f point)
{
	handLeft.add(point);
}

void KinectBody::addHandRightPositionHostory(cv::Point2f point)
{
	handRight.add(point);
}

Hand* KinectBody::getLeftHandTracker()
{
	return &handLeft;
}

Hand* KinectBody::getRightHandTracker()
{
	return &handRight;
}


