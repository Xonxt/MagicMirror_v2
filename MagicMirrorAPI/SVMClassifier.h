#pragma once
#include "svm.h"
#include "KinectPose.h"
#include <fstream>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

class SVMClassifier {
public:	
	SVMClassifier(double C = 1.0f, double gamma = 0.0f);

	bool tryLoadFromFile(const std::string& fileName);

	void loadTrainingData(std::vector<KinectPose>& poseData);

	bool train();

	double predict(FeatureString featureString);
	double predict_probability(FeatureString featureString, double* probability);

	~SVMClassifier();
private:
	struct svm_parameter param;		// set by parse_command_line
	struct svm_problem prob;		// set by read_problem
	struct svm_model *model;
	struct svm_node *x_space;

	bool trained = false;
};

