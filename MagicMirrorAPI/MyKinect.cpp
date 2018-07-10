#include "stdafx.h"
#include "MyKinect.h"

/// Initializes the default Kinect sensor
HRESULT CBodyBasics::InitializeDefaultSensor()
{
	//
	HRESULT hr;

	// kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)) {
		return hr;
	}

	//kinect
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//
		IDepthFrameSource* pDepthFrameSource = NULL;//
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//
		IColorFrameSource* pColorFrameSource = NULL;

		//kinect
		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}
		else return(hr);

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}
		else return(hr);


		//coordinatemapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}
		else return(hr);

		//bodyframe
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}
		else return(hr);

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}
		else return(hr);

		//depth frame
		if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}
		else return(hr);

		if (SUCCEEDED(hr)) {
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}
		else return(hr);

		//body index frame
		if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}
		else return(hr);

		if (SUCCEEDED(hr)) {
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}
		else return(hr);


		SafeRelease(pColorFrameSource);
		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		SafeRelease(pBodyIndexFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "Kinect initialization failed!" << std::endl;
		return E_FAIL;
	}

	//skeletonImg
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	// RGB image
	bgrImg.create(cColorHeight, cColorWidth, CV_8UC3);
	bgrImg.setTo(0);

	return hr;
}

cv::Mat* CBodyBasics::getSkeletonImg()
{
	return &skeletonImg;
}

cv::Mat* CBodyBasics::getDepthImg()
{
	//cv::Mat image;
	//cv::cvtColor(this->depthImg, image, cv::COLOR_GRAY2BGR);
	return &depthImg;
}

cv::Mat* CBodyBasics::getBgrImg()
{
	//cv::Mat image;
	//cv::cvtColor(this->depthImg, image, cv::COLOR_GRAY2BGR);

	//cv::imshow("sdfsdf", bgrImg);
	//cv::waitKey(0);

	return &bgrImg;
}

UserList* CBodyBasics::getUserList()
{
	return &this->kinectBodies;
}

void CBodyBasics::prettify(/*KinectBody * body, */cv::Scalar & color)
{
	cv::Mat image = cv::Mat(bgrImg);
	cv::Mat imageP = cv::Mat(bgrImg);
	image = cv::Mat::zeros(bgrImg.size(), CV_8U);
	imageP = cv::Mat::zeros(bgrImg.size(), CV_8U);
	cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);
	cv::cvtColor(imageP, imageP, cv::COLOR_GRAY2BGR);

	for (auto& it : kinectBodies) {

		KinectBody* body = it.second;
		
		cv::Point2f handLeft = body->extractJoint2DColor(JointType_HandLeft);
		cv::Point2f handRight = body->extractJoint2DColor(JointType_HandRight);

		body->getLeftHandTracker()->add(handLeft);
		body->getRightHandTracker()->add(handRight);

		cv::Point2f wristLeft = body->extractJoint2DColor(JointType_WristLeft);
		cv::Point2f wristRight = body->extractJoint2DColor(JointType_WristRight);

		cv::Point2f handTipLeft = body->extractJoint2DColor(JointType_HandTipLeft);
		cv::Point2f handTipRight = body->extractJoint2DColor(JointType_HandTipRight);

		cv::Point2f thumbLeft = body->extractJoint2DColor(JointType_ThumbLeft);
		cv::Point2f thumbRight = body->extractJoint2DColor(JointType_ThumbRight);

		// draw path:
		int N = std::max<int>(body->getLeftHandTracker()->path.size(), body->getRightHandTracker()->path.size());

		for (int i = 1; i < N; i++) {

			int rad = i * util::randBetween(10, 15);

			if (i < body->getLeftHandTracker()->path.size() && body->getLeftHandTracker()->path.size() > 1) {
				cv::line(imageP, body->getLeftHandTracker()->path.at(i), body->getLeftHandTracker()->path.at(i - 1), color, rad);
			}

			if (i < body->getRightHandTracker()->path.size() && body->getRightHandTracker()->path.size() > 1) {
				cv::line(imageP, body->getRightHandTracker()->path.at(i), body->getRightHandTracker()->path.at(i - 1), color, rad);
			}
		}

		if (body->getLeftHandTracker()->isMoving()) {
			int radiusX = util::randBetween(55, 65);
			cv::circle(image, handLeft, radiusX, color, -1);
		}
		else {
			int M = 4;
			for (int i = 0; i < M; i++) {
				int radiusX = (i + 1) * util::randBetween(15, 20);
				cv::Point2f shift = cv::Point2f(util::randBetween(-15, 15), (M - i - 1) * 30);
				cv::circle(image, handLeft - shift, radiusX, color, -1);
			}
		}

		if (body->getRightHandTracker()->isMoving()) {
			int radiusX = util::randBetween(55, 65);
			cv::circle(image, handRight, radiusX, color, -1);
		}
		else {
			int M = 4;
			for (int i = 0; i < M; i++) {
				int radiusX = (i + 1) * util::randBetween(15, 20);
				cv::Point2f shift = cv::Point2f(util::randBetween(-15, 15), (M - i - 1) * 30);
				cv::circle(image, handRight - shift, radiusX, color, -1);
			}
		}

		// tips
		int radiusX = util::randBetween(15, 25);
		cv::circle(image, handTipLeft, radiusX, color, -1);
		cv::circle(image, thumbLeft, radiusX, color, -1);
		cv::circle(image, handTipRight, radiusX, color, -1);
		cv::circle(image, handTipRight, radiusX, color, -1);

		/*cv::circle(image, handLeft, 10, color, -1);
		cv::circle(image, handRight, 10, color, -1);*/
	}

	// blur the paths
	int ksizeX = util::randBetween(35, 45),
		ksizeY = util::randBetween(35, 45);
	if (ksizeX % 2 == 0) ksizeX++;
	if (ksizeY % 2 == 0) ksizeY++;

	cv::blur(imageP, imageP, cv::Size(ksizeX, ksizeY));

	// blur the fire effect on the hands
	ksizeX = util::randBetween(65, 75);
	ksizeY = util::randBetween(65, 75);
	if (ksizeX % 2 == 0) ksizeX++;
	if (ksizeY % 2 == 0) ksizeY++;

	cv::blur(image, image, cv::Size(ksizeX, ksizeY));

	cv::addWeighted(bgrImg, 1.0, imageP, 0.55, 0, bgrImg);
	cv::addWeighted(bgrImg, 1.0, image, 0.55, 0, bgrImg);
}

/// Main processing function
void CBodyBasics::Update()
{
	//skeletonImg.setTo(0);

	//kinect
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//
	IDepthFrame* pDepthFrame = NULL;//
	IBodyIndexFrame* pBodyIndexFrame = NULL;//

											//
	HRESULT hr = S_OK;

	//-----------------------------------------------------------------------
	if (SUCCEEDED(hr)) {
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//
	}
	if (SUCCEEDED(hr)) {
		BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		// update BGR image
		uchar* skeletonData = (uchar*)skeletonImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j) {
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
		}

		delete[] bodyIndexArray;
	}
	SafeRelease(pBodyIndexFrame);//	

	if (SUCCEEDED(hr)) {
		hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//
	}
	if (SUCCEEDED(hr)) {
		UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//
		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);
		// update Depth image

		uchar* depthData = (uchar*)depthImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j) {
			*depthData = depthArray[j];
			++depthData;
		}

		delete[] depthArray;
	}
	SafeRelease(pDepthFrame);

	// get the RGB image
	getColorFrame();

	//---------------------------------------------------------
	if (SUCCEEDED(hr)) {
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//

		if (SUCCEEDED(hr))
		{
			//
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			//
			ProcessBody(BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//
		}
	}
	SafeRelease(pBodyFrame);//
}

// Handle new body data
void CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	//
	HRESULT hr;

	//
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			UINT64 trackingId;
			hr = pBody->get_TrackingId(&trackingId);

			//auto itBody = kinectBodies.find(trackingId);

			//if (itBody == kinectBodies.end()) {
			//	kinectBodies.emplace(trackingId, new KinectBody(m_pCoordinateMapper));
			//}

			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			//// update tracked state:
			//kinectBodies.at(trackingId)->setTrackedState(bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				//UINT64 trackingId;
				//hr = pBody->get_TrackingId(&trackingId);

				auto itBody = kinectBodies.find(trackingId);

				if (itBody == kinectBodies.end()) {
					kinectBodies.emplace(trackingId, new KinectBody(m_pCoordinateMapper));
				}

				// update tracked state:
				kinectBodies.at(trackingId)->setTrackedState(bTracked);

				Joint joints[JointType_Count];//
				HandState leftHandState = HandState_Unknown;//
				HandState rightHandState = HandState_Unknown;//
															 //
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				kinectBodies.at(trackingId)->setLeftHandState(leftHandState);
				kinectBodies.at(trackingId)->setRightHandState(rightHandState);

				//
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					int numOfGoodJoints = 0;
					for (int j = 0; j < _countof(joints); ++j)
					{
						// update joint positions
						kinectBodies.at(trackingId)->setJoint((JointType)j, joints[j]);

						if (joints[j].TrackingState == TrackingState::TrackingState_Tracked)
							numOfGoodJoints++;

						if (j == (int)JointType_HandLeft)
							kinectBodies.at(trackingId)->addHandLeftPositionHostory(
								cv::Point3f(joints[j].Position.X, joints[j].Position.Y, joints[j].Position.Z));

						if (j == (int)JointType_HandRight)
							kinectBodies.at(trackingId)->addHandRightPositionHostory(
								cv::Point3f(joints[j].Position.X, joints[j].Position.Y, joints[j].Position.Z));			
					}

					if (numOfGoodJoints <= (_countof(joints) / 2))
						kinectBodies.erase(trackingId);
				}
			}
			else {
				if (kinectBodies.find(trackingId) != kinectBodies.end())
					kinectBodies.erase(trackingId);
			}
		}
	}

	for (auto& it : kinectBodies) {
		DrawSkeleton(it.second);

		//double speed = it.second->getRightHandTracker()->getAverageAcceleration();

		//cv::putText(skeletonImg, std::to_string(speed), cv::Point(40, 50), cv::FONT_HERSHEY_PLAIN, 3, CV_RGB(255, 0, 0), 3);
	}
}

void CBodyBasics::getColorFrame()
{
	if (!m_pColorFrameReader)
	{
		return;
	}

	IColorFrame* pColorFrame = NULL;

	HRESULT hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		UINT nBufferSize = 0;
		RGBQUAD *pBuffer = NULL;

		hr = pColorFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
			}
			else if (m_pColorRGBX)
			{
				pBuffer = m_pColorRGBX;

				nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);

				hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);

				//bgrImg.setTo(0);
				uchar* argbData = (uchar*)bgrImg.data;

				for (int j = 0; j < cColorHeight * cColorWidth * sizeof(RGBQUAD); j += sizeof(RGBQUAD)) {
					*argbData = reinterpret_cast<BYTE*>(pBuffer)[j]; ++argbData;
					*argbData = reinterpret_cast<BYTE*>(pBuffer)[j + 1]; ++argbData;
					*argbData = reinterpret_cast<BYTE*>(pBuffer)[j + 2]; ++argbData;
					//*argbData = reinterpret_cast<BYTE*>(pBuffer)[j + 3]; ++argbData;
				}
			}
			else
			{
				std::cout << "nope " << std::endl;
				hr = E_FAIL;
			}
		}

		SafeRelease(pFrameDescription);
	}

	SafeRelease(pColorFrame);
}

/// Constructor
CBodyBasics::CBodyBasics() :
	m_pKinectSensor(NULL),
	m_pCoordinateMapper(NULL),
	m_pBodyFrameReader(NULL),
	m_pColorFrameReader(NULL),
	m_pColorRGBX(NULL) {

	// create heap storage for color pixel data in RGBX format
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
}

/// Destructor
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper);

	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}

	// done with color frame reader
	SafeRelease(m_pColorFrameReader);

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}


//
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//
	cv::Scalar color;

	switch (handState) {
	case HandState_Open:
		color = CV_RGB(255, 0, 0);
		break;
	case HandState_Closed:
		color = CV_RGB(0, 255, 0);
		break;
	case HandState_Lasso:
		color = CV_RGB(0, 0, 255);
		break;
	default://
		return;
	}

	cv::circle(skeletonImg,
		cv::Point2f(depthSpacePosition.X, depthSpacePosition.Y),
		20, color, -1);
}

void CBodyBasics::DrawSkeleton(KinectBody* body)
{
	//-----------------------hand states----------------------------
	DrawHandState(body->getDepthSpaceJointPosition(JointType_HandLeft), body->getLeftHandState());
	DrawHandState(body->getDepthSpaceJointPosition(JointType_HandRight), body->getRightHandState());

	//---------------------------body-------------------------------
	DrawBone(body, JointType_Head, JointType_Neck);
	DrawBone(body, JointType_Neck, JointType_SpineShoulder);
	DrawBone(body, JointType_SpineShoulder, JointType_SpineMid);
	DrawBone(body, JointType_SpineMid, JointType_SpineBase);
	DrawBone(body, JointType_SpineShoulder, JointType_ShoulderRight);
	DrawBone(body, JointType_SpineShoulder, JointType_ShoulderLeft);
	DrawBone(body, JointType_SpineBase, JointType_HipRight);
	DrawBone(body, JointType_SpineBase, JointType_HipLeft);

	// -----------------------Right Arm ------------------------------------ 
	DrawBone(body, JointType_ShoulderRight, JointType_ElbowRight);
	DrawBone(body, JointType_ElbowRight, JointType_WristRight);
	DrawBone(body, JointType_WristRight, JointType_HandRight);
	DrawBone(body, JointType_HandRight, JointType_HandTipRight);
	DrawBone(body, JointType_WristRight, JointType_ThumbRight);

	//----------------------------------- Left Arm--------------------------
	DrawBone(body, JointType_ShoulderLeft, JointType_ElbowLeft);
	DrawBone(body, JointType_ElbowLeft, JointType_WristLeft);
	DrawBone(body, JointType_WristLeft, JointType_HandLeft);
	DrawBone(body, JointType_HandLeft, JointType_HandTipLeft);
	DrawBone(body, JointType_WristLeft, JointType_ThumbLeft);

	// ----------------------------------Right Leg--------------------------------
	DrawBone(body, JointType_HipRight, JointType_KneeRight);
	DrawBone(body, JointType_KneeRight, JointType_AnkleRight);
	DrawBone(body, JointType_AnkleRight, JointType_FootRight);

	// -----------------------------------Left Leg---------------------------------
	DrawBone(body, JointType_HipLeft, JointType_KneeLeft);
	DrawBone(body, JointType_KneeLeft, JointType_AnkleLeft);
	DrawBone(body, JointType_AnkleLeft, JointType_FootLeft);

}

// Draws one bone of a body (joint to joint)
void CBodyBasics::DrawBone(KinectBody* kinectBody, JointType joint0, JointType joint1)
{
	TrackingState joint0State = kinectBody->getJointTrackingState(joint0);
	TrackingState joint1State = kinectBody->getJointTrackingState(joint1);

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	cv::Point2f
		p1 = cv::Point2f(kinectBody->getDepthSpaceJointPosition(joint0).X,
			kinectBody->getDepthSpaceJointPosition(joint0).Y),

		p2 = cv::Point2f(kinectBody->getDepthSpaceJointPosition(joint1).X,
			kinectBody->getDepthSpaceJointPosition(joint1).Y);

	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		cv::line(skeletonImg, p1, p2, cv::Scalar(255, 255, 255), 2);
		cv::circle(skeletonImg, p1, 4, cv::Scalar(255, 0, 255), -1);
		cv::circle(skeletonImg, p2, 4, cv::Scalar(255, 0, 255), -1);
	}
	else
	{
		cv::line(skeletonImg, p1, p2, cv::Scalar(0, 0, 255), 2);
	}
}

