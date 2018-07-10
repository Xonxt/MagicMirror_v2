#include "stdafx.h"
#include "PoseRecognizer.h"

PoseRecognizer::PoseRecognizer()
{
	util::Log("-------------------------");
}


PoseRecognizer::~PoseRecognizer()
{

}

bool PoseRecognizer::initialize()
{
	// read the pose data from the files
	poseVector = initPoseData(POSE_DATA_DIR);

	if (poseVector.size() == 0) {
		std::cerr << "Failed to load the pose data!" << std::endl;
		util::Log("Failed to load the pose training data!");
		return false;
	}

	hr = myKinect.InitializeDefaultSensor();

	if (FAILED(hr)) {
		std::cerr << "Failed to initialize the Kinect device" << std::endl;
		util::Log("Failed to connect to a Kinect device");
		return false;
	}

	retrainClassifier();

	return true;
}

std::vector<KinectPose> PoseRecognizer::initPoseData(const std::string path) {
	std::vector<std::string> fileList = util::get_all_files_names_within_folder(path);
	std::vector<KinectPose> poseVector;
	int i = 0;
	for (std::string fileName : fileList) {
		poseVector.push_back(KinectPose(i++, fileName));
	}	

	return poseVector;
}

bool PoseRecognizer::reloadPoseData(const std::string folder) {

	poseVector = this->initPoseData(folder);

	return (poseVector.size() > 0);
}

void PoseRecognizer::retrainClassifier()
{
	util::Log("Training the classifier");

	svm = SVMClassifier(1.0f, 1.0f);

	if (!svm.tryLoadFromFile("svm.dat")) {
		svm.loadTrainingData(poseVector);
		svm.train();
	}
}

void PoseRecognizer::prettify(/*KinectBody * body, */cv::Scalar & color)
{
	myKinect.prettify(/*body, */color);
}


bool PoseRecognizer::processNextFrame()
{
	// process the next frame, acquire frames, extract skeletons
	myKinect.Update();

	/*skeletonImage = myKinect.getSkeletonImg();
	depthMap = myKinect.getDepthImg();
	bgrImage = myKinect.getBgrImg();*/

	// get te list of users from the current frame	
	this->userList = myKinect.getUserList();

	for (auto& iterator : *userList) { // !!!!!

		FeatureString featureString = iterator.second->extractUserFeatures();  // !!!

		if (featureString.size() > 0) {
			estimatePose(iterator.second, featureString);

			if (rememberCurrentPose == true) {
				if (featureString.size() > 0) {
					poseVector[currentPoseId].addNewTrainingSample(featureString);
					//util::Log("sample saved");
					std::cout << "sample " << poseVector[currentPoseId].getFeatureVector()->size() << " for pose " << poseVector[currentPoseId].getPoseName() << " saved!" << std::endl;
				}

				rememberCurrentPose = false;
			}
		}
				
		// prettify(/*iterator.second, */util::Color[iterator.second->getPoseIndex()+1]);
		
	}

	return true;
}

cv::Mat* PoseRecognizer::getRgbFrame()
{
	//return bgrImage;
	return myKinect.getBgrImg();
}

cv::Mat* PoseRecognizer::getDepthFrame()
{
	//return depthMap;
	return myKinect.getDepthImg();
}

cv::Mat* PoseRecognizer::getSkeletonImage()
{
	return myKinect.getSkeletonImg();
}


void PoseRecognizer::addNewPose(const std::string poseName)
{
	int newNumber = poseVector.size();

	std::ostringstream ost;
	ost << POSE_DATA_DIR << "pose" << newNumber + 1 << ".txt";
	std::ofstream ofs(ost.str(), std::ios::out);

	ofs << poseName << std::endl;
	
	ofs.close();

	poseVector.push_back(KinectPose(newNumber, ost.str()));
}

void PoseRecognizer::savePoseData(const std::string poseName)
{
	for (KinectPose pose : poseVector) {
		if (pose.getPoseName() == poseName) {
			savePoseData(pose.getPoseIndex());
		}
	}
}

void PoseRecognizer::savePoseData(const int poseIndex)
{
	rememberCurrentPose = true;

	currentPoseId = poseIndex;
}

std::vector<KinectPose>* PoseRecognizer::getPoseVector()
{
	return &poseVector;
}

double PoseRecognizer::expectPose(const int poseIndex)
{
	//KinectUser user = userList[0];
	KinectBody* user = (*userList).begin()->second;

	if (userList->size() == 0)
		std::cerr << "No users present!" << std::endl;

	FeatureString featureString = user->extractUserFeatures();

	if (featureString.size() == 0)
		return 0.0;
	
	double prob = 0.0f;
	int poseIdx = (int)svm.predict_probability(featureString, &prob);

	if (poseIdx == poseIndex) {
		return prob;
	}
	else
		return 0.0f;
}

double PoseRecognizer::expectPose(UINT64& userId, const int poseIndex)
{
	//KinectUser user = userList[0];
	if (userList->find(userId) == userList->end())
		return -1;

	KinectBody* user = userList->at(userId);

	if (user == NULL)
		std::cerr << "No users present!" << std::endl;

	FeatureString featureString = user->extractUserFeatures();

	if (featureString.size() == 0)
		return 0.0;

	double prob = 0.0f;
	int poseIdx = (int)svm.predict_probability(featureString, &prob);

	if (poseIdx == poseIndex) {
		return prob;
	}
	else
		return 0.0f;
}

double PoseRecognizer::expectPose(const std::string poseName)
{
	for (KinectPose pose : poseVector) {
		//if (std::)
		if (pose.getPoseName() == poseName) {
			return expectPose(pose.getPoseIndex());
		}
	}

	return 0.0;
}

double PoseRecognizer::expectPose(UINT64& userId, const std::string poseName)
{
	for (KinectPose pose : poseVector) {
		if (pose.getPoseName() == poseName) {
			return expectPose(userId, pose.getPoseIndex());
		}
	}

	return 0.0;
}

/**
	Recogize the current pose from the array of possible poses
*/
int PoseRecognizer::estimatePose(KinectBody* user, const FeatureString & featureString) {

	int poseIndex = -1;	
	
	//double result = svm.predict(featureString);
	double prob = 0.0f;
	double result = svm.predict_probability(featureString, &prob);

	if (prob < 0.65)
		result = -1.0f;

	user->addPoseTumbler((int)result);

	// now check if this pose is held for at least N consecuitive instances
	if (user->getPoseTumbler().size() == (HOLD_POSE % 2) ? HOLD_POSE : HOLD_POSE + 1) {

		int mid = (0 + user->getPoseTumbler().size()) / 2;

		if (std::abs(user->getPoseTumblerAvg() - user->getPoseTumbler()[mid]) < 0.001) {

			poseIndex = user->getPoseTumbler()[mid];

			user->setPoseName((poseIndex >= 0) ? poseVector[poseIndex].getPoseName() : "");
			user->setPoseIndex(poseIndex);
		}
	}

	return poseIndex;
}

UserList* PoseRecognizer::getUserList() {
	return this->userList;
}



