#include "stdafx.h"

#include "PoseRecognizer.h"

extern "C" {

	// constructor
	__declspec(dllexport) BOOL __stdcall PoseRecognizer_Create(PoseRecognizer* &instance) {
		instance = new PoseRecognizer();

		if (instance == NULL)
			return false;

		return TRUE;
	}

	// destructor
	__declspec(dllexport) BOOL __stdcall PoseRecognizer_Destroy(PoseRecognizer* instance) {
		if (instance == NULL)
			return FALSE;

		delete instance;

		return TRUE;
	}

	// initialize the pose recognizer
	__declspec(dllexport) BOOL __stdcall PoseRecognizer_initialize(PoseRecognizer* instance, bool& result) {

		if (instance == NULL) {
			return FALSE;
		}

		try {
			result = instance->initialize();
			return result;
		}
		catch (...) {
			return FALSE;
		}
	}

	// process next frame
	__declspec(dllexport) BOOL __stdcall PoseRecognizer_processNextFrame(PoseRecognizer* instance, bool& result) {

		if (instance == NULL) {
			return FALSE;
		}

		try {
			result = instance->processNextFrame();
			return result;
		}
		catch (...) {
			return FALSE;
		}
	}

	// get RGB image
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getRgbFrame(PoseRecognizer* instance, uchar* outputImage) {
		if (instance == NULL)
			return FALSE;

		try {
			//util::Log("Sending RGB frame...");
			cv::Mat* rgbFrame = instance->getRgbFrame();

			int channels = rgbFrame->channels();
			int width = rgbFrame->cols;
			int height = rgbFrame->rows;

			std::memcpy(outputImage, rgbFrame->data, height * width * channels * sizeof(uchar));

			//util::Log("RGB frame sent!");

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// get DepthMap image
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getDepthMap(PoseRecognizer* instance, uchar* outputImage) {
		if (instance == NULL)
			return FALSE;

		try {
			//util::Log("Depth frame sent!");
			cv::Mat* depthMap = instance->getDepthFrame();

			int channels = depthMap->channels();
			int width = depthMap->cols;
			int height = depthMap->rows;

			std::memcpy(outputImage, depthMap->data, height * width * channels * sizeof(uchar));

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// get skeleton image
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getSkeletonImage(PoseRecognizer* instance, uchar* outputImage) {
		if (instance == NULL)
			return FALSE;

		try {
			cv::Mat* skeletonImg = instance->getSkeletonImage();

			int channels = skeletonImg->channels();
			int width = skeletonImg->cols;
			int height = skeletonImg->rows;

			std::memcpy(outputImage, skeletonImg->data, height * width * channels * sizeof(uchar));

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// is there at least one user present and tracked?
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_userPresent(PoseRecognizer* instance, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {
			result = false;
			UserList* users = instance->getUserList();

			if (users->size() > 0) {
				result = (users->begin()->second->isBodyTracked());
			}

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// give the list of user ID's that are currently being tracked:
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserIdList(PoseRecognizer* instance, int& amount, UINT64** list) {
		if (instance == NULL)
			return FALSE;

		try {
			std::vector<UINT64> idList;
			
			for (auto& it : *instance->getUserList()) {
				idList.push_back(it.first);
			}

			*list = new UINT64[idList.size()];

			for (int i = 0; i < idList.size(); i++) {
				(*list)[i] = idList.at(i);
			}

			amount = idList.size();

			//util::Log("cpp, size: " + amount);

			return TRUE;
		}
		catch (std::exception& e) {	
			util::Log("error while returning the userlist");
			return FALSE;
		}

		return FALSE;
	}

	// get specific user's poseName:
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserPoseName(PoseRecognizer* instance, UINT64 userId, char* poseName, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				result = false;
				return FALSE;
			}
			else {
				
				std::strcpy(poseName, users->at(userId)->getPoseName().c_str());
				result = true;
				return TRUE;
			}
		}
		catch (...) {
			result = false;
			util::Log("exception while giving the poseName of user: " + std::to_string(userId));
			return FALSE;
		}
	}

	// get specific user's poseID:
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserPose(PoseRecognizer* instance, UINT64 userId, int& poseIndex, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				poseIndex = -1;
				result = false;
			}
			else {
				poseIndex = users->at(userId)->getPoseIndex();
				result = true;
			}

			return TRUE;
		}
		catch (...) {
			result = false;
			util::Log("exception while giving the poseIndex of user: " + std::to_string(userId));
			return FALSE;
		}
	}

	// GET THE MOVEMENT DIRECTION OF THE SPECIFIED HAND FOR SPECIFIED USER
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserHandDirection(PoseRecognizer* instance, UINT64 userId, int handNumber, int& direction, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				result = false;
				return TRUE;
			}
			else {
				if (handNumber == 0) { // left hand
					direction = (int) users->at(userId)->getLeftHandTracker()->getDirection();
				}
				else { // right hand
					direction = (int) users->at(userId)->getRightHandTracker()->getDirection();
				}				

				result = true;
				return TRUE;
			}
		}
		catch (...) {
			result = false;			
			return FALSE;
		}
	}

	// CHECK IF SPECIFIED HAND FOR SPECIFIED USER CHANGED DIRECTION
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_userHandChangedDirection(PoseRecognizer* instance, UINT64 userId, int handNumber, bool& isChanged, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				result = false;
				return TRUE;
			}
			else {
				if (handNumber == 0) { // left hand
					isChanged = (int)users->at(userId)->getLeftHandTracker()->directionChanged;
				}
				else { // right hand
					isChanged = (int)users->at(userId)->getRightHandTracker()->directionChanged;
				}

				result = true;
				return TRUE;
			}
		}
		catch (...) {
			result = false;
			return FALSE;
		}
	}

	// RETURN STATE OF SPECIFIED HAND FOR SPECIFIED USER 
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserHandState(PoseRecognizer* instance, UINT64 userId, int handNumber, int& handState, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				result = false;
				return TRUE;
			}
			else {

				if (handNumber == 0) { // left hand
					handState = (int)users->at(userId)->getLeftHandState();
				}
				else { // right hand
					handState = (int)users->at(userId)->getRightHandState();
				}

				result = true;
				return TRUE;
			}
		}
		catch (...) {
			result = false;
			return FALSE;
		}
	}

	// CHECK IF SPECIFIED HAND FOR SPECIFIED USER IS MOVING
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_isUserHandMoving(PoseRecognizer* instance, UINT64 userId, int handNumber, bool& isMoving, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {
			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				result = false;
				return TRUE;
			}
			else {

				if (handNumber == 0) { // left hand
					isMoving = (int)users->at(userId)->getLeftHandTracker()->isMoving();
				}
				else { // right hand
					isMoving = (int)users->at(userId)->getRightHandTracker()->isMoving();
				}

				result = true;
				return TRUE;
			}
		}
		catch (...) {
			result = false;
			return FALSE;
		}
	}

	// RETURN MOVEMENT SPEED OF SPECIFIED HAND FOR SPECIFIED USER 
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserHandSpeed(PoseRecognizer* instance, UINT64 userId, int handNumber, double& handSpeed, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0 || users->find(userId) == users->end()) {
				result = false;
				return TRUE;
			}
			else {

				if (handNumber == 0) { // left hand
					handSpeed = (int)users->at(userId)->getLeftHandTracker()->getAverageAcceleration();
				}
				else { // right hand
					handSpeed = (int)users->at(userId)->getRightHandTracker()->getAverageAcceleration();
				}

				result = true;
				return TRUE;
			}
		}
		catch (...) {
			result = false;
			return FALSE;
		}
	}


	// extract real-world joint coordinates for a specific user
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserJointRealworld(PoseRecognizer* instance, UINT64 userId, int jointNumber, double& x, double& y, double& z, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {
			if (instance->getUserList()->find(userId) == instance->getUserList()->end())
				throw std::exception("...");

			cv::Point3f joint = instance->getUserList()->at(userId)->extractJoint3D((JointType)jointNumber);
			x = joint.x;
			y = joint.y;
			z = joint.z;

			result = true;

			return TRUE;
		}
		catch (std::exception& e) {
			x = 0; y = 0; z = 0;
			result = false;
			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// extract "scene" joint coordinates for a specific user
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getUserJointScene(PoseRecognizer* instance, UINT64 userId, int jointNumber, double& x, double& y, double& z, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {
			if (instance->getUserList()->find(userId) == instance->getUserList()->end())
				throw std::exception("...");

			cv::Mat* depth = instance->getDepthFrame();
			cv::Point2f joint = instance->getUserList()->at(userId)->extractJoint2D((JointType)jointNumber);
			x = joint.x;
			y = joint.y;
			z = depth->at<float>(y, x);
			z = 0;

			result = true;

			return TRUE;
		}
		catch (std::exception& e) {
			x = 0; y = 0; z = 0;
			result = false;
			//util::Log("getting scene coords fail - std::exception caught");
			return TRUE;
		}
		catch (...) {
			//util::Log("getting scene coords fail - other exception caught!");
			return FALSE;
		}
	}

	// extract real-world joint coordinates
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getJointRealworld(PoseRecognizer* instance, int jointNumber, double& x, double& y, double& z, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {
			cv::Point3f joint = instance->getUserList()->begin()->second->extractJoint3D((JointType)jointNumber);
			x = joint.x;
			y = joint.y;
			z = joint.z;

			result = true;

			return TRUE;
		}
		catch (std::exception& e) {
			x = 0; y = 0; z = 0;
			result = false;
			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// extract "scene" joint coordinates
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getJointScene(PoseRecognizer* instance, int jointNumber, double& x, double& y, double& z, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {
			cv::Mat* depth = instance->getDepthFrame();
			cv::Point2f joint = instance->getUserList()->begin()->second->extractJoint2D((JointType)jointNumber);
			x = joint.x;
			y = joint.y;
			z = depth->at<float>(y, x);
			z = 0;

			result = true;

			return TRUE;
		}
		catch (std::exception& e) {
			x = 0; y = 0; z = 0;
			result = false;
			//util::Log("getting scene coords fail - std::exception caught");
			return TRUE;
		}
		catch (...) {
			//util::Log("getting scene coords fail - other exception caught!");
			return FALSE;
		}
	}

	// GET INDEX OF THE POSE OF THE SINGLE (FIRST AVAILABLE) USER!!!!!
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getPose(PoseRecognizer* instance, int& poseIndex, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0) {
				poseIndex = 0;
				result = false;
			}
			else {
				poseIndex = users->begin()->second->getPoseIndex() + 1;
				result = true;
			}

			return TRUE;

		}
		catch (...) {
			return FALSE;
		}
	}

	// GET POSE NAME OF THE FIRST AVAILABLE USER!!!!!
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getPoseName(PoseRecognizer* instance, char* poseName, bool& result) {
		if (instance == NULL)
			return FALSE;

		try {

			UserList* users = instance->getUserList();

			if (users->size() == 0) {
				std::strcpy(poseName, "");
				result = false;
			}
			else {
				std::strcpy(poseName, users->begin()->second->getPoseName().c_str());
				result = true;
			}

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// ADD THE NEW POSE WITH THE SPECIFIED NAME TO THE DATABASE 
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_addNewPose(PoseRecognizer* instance, char* poseName, double dof) {
		if (instance == NULL)
			return FALSE;

		try {

			instance->addNewPose(poseName);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// SAVE A TRAINING SAMPLE FOR THE SPECIFIED POSE
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_saveTrainingDataByName(PoseRecognizer* instance, char* poseName) {
		if (instance == NULL)
			return FALSE;

		try {
			instance->savePoseData(poseName);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	_declspec(dllexport) BOOL __stdcall PoseRecognizer_saveTrainingDataByIdx(PoseRecognizer* instance, int poseIndex) {
		if (instance == NULL)
			return FALSE;

		try {

			instance->savePoseData(poseIndex);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// FOR SPECIFIC USER EXPECT A SPECIFIC POSE AND GIVE TE SCORE FROM 0 TO 1
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_expectUserPoseByName(PoseRecognizer* instance, UINT64 userId, char* poseName, double& result) {
		if (instance == NULL)
			return FALSE;

		try {

			result = instance->expectPose(userId, poseName);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// FOR THE SPECIFIS USER EXPECT THE POSE BY THE INDEX
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_expectUserPoseByIdx(PoseRecognizer* instance, UINT64 userId, int poseIndex, double& result) {
		if (instance == NULL)
			return FALSE;

		try {

			result = instance->expectPose(userId, poseIndex);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// FOR SINGLE (FIRST AVAILABLE) USER EXPECT THE POSE BY NAME 
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_expectPoseByName(PoseRecognizer* instance, char* poseName, double& result) {
		if (instance == NULL)
			return FALSE;

		try {

			result = instance->expectPose(poseName);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// FOR SINGLE (FIRST AVAILABLE) USER EXPECT THE POSE BY INDEX 
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_expectPoseByIdx(PoseRecognizer* instance, int poseIndex, double& result) {
		if (instance == NULL)
			return FALSE;

		try {

			result = instance->expectPose(poseIndex);

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}

	// GIVE A LIST OF POSES CURRENTLY IN THE DATASET
	_declspec(dllexport) BOOL __stdcall PoseRecognizer_getPoseList(PoseRecognizer* instance, char** poseList, int size) {
		if (instance == NULL)
			return FALSE;

		try {
			std::vector<KinectPose>* poseVector = instance->getPoseVector();
			
			poseList = new char*[poseVector->size()];

			for (size_t i = 0; i < poseVector->size(); i++) {
				poseList[i] = new char[poseVector->at(i).getPoseName().size() + 1];
				strcpy(poseList[i], poseVector->at(i).getPoseName().c_str());
			}

			size = poseVector->size();

			return TRUE;
		}
		catch (...) {
			return FALSE;
		}
	}
}

void foo();

//int main(){
//	//foo();
//	return 0;
//}

void foo() {
	// Create a new recognizer object
	PoseRecognizer pr = PoseRecognizer();

	if (!pr.initialize()) {
		std::cerr << "init error!" << std::endl;
	}

	int key;

	cv::namedWindow("Video", cv::WINDOW_FULLSCREEN);

	while (true) {
		pr.processNextFrame();

		//cv::Mat img = *pr.getRgbFrame();
		cv::Mat img = *pr.getSkeletonImage();

		if (img.empty())
			continue;

		if (pr.getUserList()->size() > 0) {
			for (auto& user : *pr.getUserList()) {
				//std::string pose = user.second->getPoseName();

				//cv::putText(img, pose, cv::Point(40, 50), cv::FONT_HERSHEY_PLAIN, 5, CV_RGB(0, 255, 0), 7);
			}
		}

		cv::imshow("Video", img);

		key = cv::waitKey(1);
		if (key == 27)
			break;
		else if (key == 'b') {
			pr.savePoseData("Conductor");
		}
	}

	cv::destroyAllWindows();
}


/*
// TEST for testing the libSVM
#include "svm.h"
#include <random>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;
struct svm_node *x_space;

int main() {

	// load pose data
	std::vector<std::string> fileList = util::get_all_files_names_within_folder(POSE_DATA_DIR);
	std::vector<KinectPose> poseVector;
	int i = 0, j = 0;
	for (std::string fileName : fileList) {
		poseVector.push_back(KinectPose(i++, fileName));
	}

	// load the data into arrays
	int numberOfFeatures;			// number of features (12)
	int avgNumOfSamples = 0;		// number of samples (1600+)

	int numberOfSamples = 0;
	for (auto& fPose : poseVector) {
		numberOfFeatures = fPose.getFeatureVector()->at(0).size();
		numberOfSamples += fPose.getFeatureVector()->size();
		avgNumOfSamples += fPose.getFeatureVector()->size();
	}

	i = 0; j = 0;
	int* labels = new int[numberOfSamples];
	float** trainingData = new float*[numberOfSamples];

	for (auto& pose : poseVector) {
		auto* featureVector = pose.getFeatureVector();

		for (auto& vector : *featureVector) {
			trainingData[i] = new float[numberOfFeatures];
			j = 0;
			for (double& f : vector) {
				trainingData[i][j] = f;
				j++;
			}
			labels[i] = pose.getPoseIndex();
			i++;
		}
	}

	//model = svm_load_model("svm.dat");

	std::cout << "array created" << std::endl;

	// loading the SVM
	int sizeOfProblem = numberOfSamples;	//number of lines with labels
	int elements = numberOfFeatures;		//number of features for each data vector

	//initialize the size of the problem with just an int
	prob.l = sizeOfProblem;

	//here we need to give some memory to our structures
	// @param prob.l = number of labels
	// @param elements = number of features for each label
	prob.y = Malloc(double, prob.l); //space for prob.l doubles
	prob.x = Malloc(struct svm_node *, prob.l); //space for prob.l pointers to struct svm_node
	x_space = Malloc(struct svm_node, (elements + 1) * prob.l); //memory for pairs of index/value

	i = 0; j = 0; int k = 0;
	for (auto& pose : poseVector) {
		auto* featureVector = pose.getFeatureVector();

		for (auto& vector : *featureVector) {
			//trainingData[i] = new float[numberOfFeatures];
			//j = 0;
			prob.x[i] = &x_space[j];
			k = 0;
			for (double& f : vector) {
				//trainingData[i][j] = f;
				x_space[j].index = k + 1; //index of value
				x_space[j].value = f; //value
				k++;
				j++;
			}
			x_space[j].index = -1; //state the end of data vector
			x_space[j].value = 0;
			j++;
			//labels[i] = pose.getPoseIndex();

			prob.y[i] = pose.getPoseIndex();
			i++;
		}
	}

	std::cout << "data converted to libSVM format" << std::endl;

	//set all default parameters for param struct
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 1.0 / 12;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 1000;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	std::cout << "parameters set!" << std::endl;

	//try to actually execute it
	model = svm_train(&prob, &param);

	std::cout << "Training finished!" << std::endl;

	svm_save_model("libSVM_model.dat", model);

	std::cout << "model saved!" << std::endl;

	std::cout << "\nTesting the model:" << std::endl;

	// test the classifier:
	std::srand(unsigned(std::time(NULL)));

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(-1.0, 1.0);

	float error = 0;
	for (i = 0; i < 5; i++) {
		int randSampleNum = std::rand() % numberOfSamples;

		//std::vector<float> featureArray(numberOfFeatures);
		float* featureArray = new float[numberOfFeatures];

		std::cout << "Initial vector: ";
		for (j = 0; j < numberOfFeatures; j++) {
			featureArray[j] = trainingData[randSampleNum][j];
			std::cout << featureArray[j] << " ";
		}
		std::cout << "\nNoised vector: ";

		// add noise:
		for (j = 0; j < numberOfFeatures; j++) {
			featureArray[j] += dis(gen);
			std::cout << featureArray[j] << " ";
		}

		std::cout << "\n";

		struct svm_node* sample = Malloc(struct svm_node, (numberOfFeatures + 1));
		for (j = 0; j < numberOfFeatures; ++j) {
			sample[j].index = j + 1; //index of value
			sample[j].value = featureArray[j]; //value
		}
		sample[j].index = -1; //state the end of data vector
		sample[j].value = 0;
		j++;

		double result = svm_predict(model, sample);

		double* probabilities = new double[poseVector.size()];
		double result2 = svm_predict_probability(model, sample, probabilities);

		std::cout << "Sample " << randSampleNum << 
			"\tTruth: " << labels[randSampleNum] << "(" << poseVector.at(labels[randSampleNum]).getPoseName() << ")" <<
			"\tPredicted: " << result2 << "(" << poseVector.at((int)result).getPoseName() << ")" << std::endl;
		std::cout << "Probabilities: ";

		for (size_t i = 0; i < poseVector.size(); i++) {
			std::cout << i<< ": " << probabilities[i] << "; ";
		}
		std::cout << "\n\n";

		error += ((labels[randSampleNum] == result) ? 1 : 0);

		//delete[] featureArray;
	}

	error /= i;

	std::cout << "Accuracy: " << error << std::endl;

	return 0;
}

*/