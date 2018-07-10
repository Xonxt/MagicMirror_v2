#include "stdafx.h"
#include "KinectPose.h"

KinectPose::KinectPose() {
	this->poseIndex = 0;
	featureVector = std::vector<FeatureString>(6);
	poseName = "None";
}

KinectPose::KinectPose(const int index) {
	this->poseIndex = index;
	featureVector = std::vector<FeatureString>(6);
	poseName = "None";
}

KinectPose::KinectPose(const int index, const std::string & fileName) {
	this->poseIndex = index;
	parsePoseDataFile(fileName);

	if (featureVector.size() > 0)
		recalculateStdev();
}

KinectPose::KinectPose(const int index, const std::string & poseName, const std::vector<FeatureString> & featureVector) {
	this->poseIndex = index;
	this->poseName = poseName;
	this->featureVector = featureVector;

	if (featureVector.size() > 0)
		recalculateStdev();
}

void KinectPose::parsePoseDataFile(const std::string & fileName) {
	
	this->fileName = fileName;

	this->featureVector.clear();


	std::ifstream ifs(fileName, std::ios::in);

	std::string line;

	// Get the first line: the pose name
	std::getline(ifs, line);

	if (line.find(';') == std::string::npos && line.find(',') == std::string::npos) {
		setPoseName(line);
		this->poseName;
	}
	
	// now get the actual features
	while (std::getline(ifs, line)) {
		FeatureString featureString;
		featureString.clear();

		std::stringstream sstream(line);
		std::string substr;
		
		while (std::getline(sstream, substr, ';')) {
			std::stringstream ss2stream(substr);
			std::string val;

			double feature = std::stof(substr);

			featureString.push_back(feature);
		}
		this->featureVector.push_back(featureString);
	}

	ifs.close();
}

std::vector<FeatureString>* KinectPose::getFeatureVector() {
	return &this->featureVector;
}

std::string KinectPose::getPoseName() {
	return this->poseName;
}

void KinectPose::setPoseName(const std::string & poseName) {
	this->poseName = poseName;
}

std::string KinectPose::getFileName() {
	return this->fileName;
}

void KinectPose::setFileName(const std::string & fileName) {
	this->fileName = fileName;
}

int KinectPose::getPoseIndex() {
	return this->poseIndex;
}

void KinectPose::addNewTrainingSample(const FeatureString & featureString) {
	
	// add the feature string to the dataset
	this->featureVector.push_back(featureString);	

	// add it also to the file
	std::ofstream ofs(this->fileName, std::ios::out | std::ios::app);
	
	//ofs << poseName << std::endl;
	//ofs << getReferenceEstimate() << std::endl;
	ofs << std::endl;
	std::copy(std::begin(featureString), std::end(featureString), std::ostream_iterator<double>(ofs, ";"));		
	
	ofs.close();
}


KinectPose::~KinectPose() {
	poseName.clear();
	featureVector.clear();
}

double KinectPose::getConfidence(const FeatureString& featureString)
{
	double product = 0;

	/*std::copy(meansVector.begin(), meansVector.end(), std::ostream_iterator<float>(std::cout, ", "));
	std::cout << std::endl;
	std::copy(stdevVector.begin(), stdevVector.end(), std::ostream_iterator<float>(std::cout, ", "));*/

	/*#pragma omp parallel for reduction(*:product) 
	for (int i = 0; i < featureString.size(); i++)
	{
		product *= ((1/(std::sqrt(2 * M_PI * stdevVector[i]))) * 
			std::exp(-(std::pow(featureString[i] - meansVector[i],2))/(2*std::pow(stdevVector[i],2))));
	}*/

	#pragma omp parallel for reduction(+:product)
	for (int i = 0; i < featureString.size(); i++)
	{
		//product += std::exp(-(std::pow(featureString[i] - meansVector[i], 2)) / (2 * std::pow(stdevVector[i], 2)));
		product += (std::pow(featureString[i] - meansVector[i], 2));
	}

	product /= featureString.size();

	product = util::sigmoidFunction(1/product);

	return product;
}

double KinectPose::angleDifference(const double alpha, const double beta) {
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

// check if the a value lies between two constrains
bool KinectPose::isInRange(const double value, const double A, const double B) {
	if (value >= fmin(A, B) && value <= fmax(A, B))
		return true;
	else
		return false;
}

void KinectPose::recalculateStdev()
{	
	meansVector = std::vector<float>(featureVector.at(0).size(), 0);
	stdevVector = std::vector<float>(featureVector.at(0).size(), 0);

	#pragma omp parallel for
	for (int i = 0; i < featureVector.size(); i++) {
		for (int j = 0; j < featureVector[i].size(); j++) {
			meansVector[j] += featureVector[i][j];
		}
	}

	#pragma omp parallel for
	for (int i = 0; i < meansVector.size(); i++) {
		meansVector[i] /= featureVector.size();
	}

	#pragma omp parallel for
	for (int i = 0; i < featureVector.size(); i++) {
		for (int j = 0; j < featureVector[i].size(); j++) {
			stdevVector[j] += std::pow(featureVector[i][j] - meansVector[j], 2);
		}
	}

	#pragma omp parallel for
	for (int i = 0; i < stdevVector.size(); i++) {
		stdevVector[i] /= featureVector.size() * 10.0;
	}
}
