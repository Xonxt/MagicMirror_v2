#include "stdafx.h"
#include "SVMClassifier.h"

SVMClassifier::SVMClassifier(double C, double gamma)
{
	//set all default parameters for param struct
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = gamma;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 1000;
	param.C = C;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
}

bool SVMClassifier::tryLoadFromFile(const std::string & fileName)
{
	std::ifstream ifs(fileName, std::ios::in);

	if (ifs.good()) {
		model = svm_load_model(fileName.c_str());
		util::Log("model loaded!");

		trained = true;

		return true;
	}

	return false;
}

void SVMClassifier::loadTrainingData( std::vector<KinectPose>& poseData)
{
	// load the data into arrays
	int numberOfFeatures;			// number of features (12)
	int avgNumOfSamples = 0;		// number of samples (1600+)

	int numberOfSamples = 0;
	for (auto& fPose : poseData) {		
		numberOfFeatures = fPose.getFeatureVector()->at(0).size();
		numberOfSamples += fPose.getFeatureVector()->size();
		avgNumOfSamples += fPose.getFeatureVector()->size();
	}

	// if the gamma was not set, set it to (1 / n of features)
	if (param.gamma == 0) {
		param.gamma = numberOfFeatures / 12.0f;
	}	

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

	//initialize the data and the labels
	int i = 0, j = 0, k = 0;
	for (auto& pose : poseData) {
		auto* featureVector = pose.getFeatureVector();

		for (auto& vector : *featureVector) {
			
			prob.x[i] = &x_space[j];
			k = 0;
			for (double& f : vector) {				
				x_space[j].index = k + 1; //index of value
				x_space[j].value = f; //value	
				k++;
				j++;
			}
			x_space[j].index = -1; //state the end of data vector
			x_space[j].value = 0;
			j++;
			
			// set the label
			prob.y[i] = pose.getPoseIndex();
			i++;
		}
	}
}

bool SVMClassifier::train()
{
	//try to actually execute it
	model = svm_train(&prob, &param);

	svm_save_model("svm.dat", model);

	util::Log("Training finished!");

	trained = true;

	return true;
}

double SVMClassifier::predict(FeatureString featureString)
{	
	if (trained) {
		int j = 0;
		struct svm_node* sample = Malloc(struct svm_node, (featureString.size() + 1));
		for (j = 0; j < featureString.size(); ++j) {
			sample[j].index = j + 1; //index of value
			sample[j].value = featureString.at(j); //value			
		}
		sample[j].index = -1; //state the end of data vector
		sample[j].value = 0;
		j++;

		//double* probs = new double[featureString.size()];


		return svm_predict(model, sample);


	}

	throw std::exception("model is not loaded or not trained");
	
	return -1;
}

double SVMClassifier::predict_probability(FeatureString featureString, double* probability)
{
	if (trained) {
		int j = 0;
		struct svm_node* sample = Malloc(struct svm_node, (featureString.size() + 1));
		for (j = 0; j < featureString.size(); ++j) {
			sample[j].index = j + 1; //index of value
			sample[j].value = featureString.at(j); //value			
		}
		sample[j].index = -1; //state the end of data vector
		sample[j].value = 0;
		j++;

		double* probs = new double[featureString.size()];

		int result = svm_predict_probability(model, sample, probs);

		*probability = probs[(int)result];

		return result;
	}

	throw std::exception("model is not loaded or not trained");

	return -1;
}


SVMClassifier::~SVMClassifier()
{
}
