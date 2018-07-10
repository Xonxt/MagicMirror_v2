#pragma once
#include "KinectBody.h"

typedef std::unordered_map< UINT64, KinectBody* > UserList;

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

class CBodyBasics
{
	//kinect 2.0 frame size
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;

public:
	CBodyBasics();
	~CBodyBasics();
	void                    Update();//function to update and process next frame
	HRESULT                 InitializeDefaultSensor();//initialize default kinect sensor

	cv::Mat* getSkeletonImg();
	cv::Mat* getDepthImg();
	cv::Mat* getBgrImg();

	UserList* getUserList();

	void prettify(/*KinectBody * body, */cv::Scalar& color);

private:
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;
	IBodyFrameReader*       m_pBodyFrameReader;
	IDepthFrameReader*      m_pDepthFrameReader;
	IBodyIndexFrameReader*  m_pBodyIndexFrameReader;
	IColorFrameReader*      m_pColorFrameReader;
	RGBQUAD*                m_pColorRGBX;


	void ProcessBody(int nBodyCount, IBody** ppBodies);

	void getColorFrame();

	void DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1);

	void DrawBone(KinectBody* kinectBody, JointType joint0, JointType joint1);

	void DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState);

	void DrawSkeleton(KinectBody* kinectBody);

	cv::Mat skeletonImg;
	cv::Mat depthImg;
	cv::Mat bgrImg;

	// the list of tracked bodies
	UserList kinectBodies;
};



