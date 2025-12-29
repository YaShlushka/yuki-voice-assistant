#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <vector>

using namespace boost::numeric;

// Randomize as ublas vector
void Randomize(ublas::vector<double>& vec);

// Randomize as ublas matrix
void Randomize(ublas::matrix<double>& m);

// The sigmoid function.
void Sigmoid(ublas::vector<double>& v);

// Derivative of the sigmoid function.
void SigmoidDerivative(ublas::vector<double>& v);

class NeuralNetwork {
 public:
	using InputElem = ublas::vector<double>;
	using OutputElem = ublas::vector<double>;
	using TrainingData = std::pair<InputElem, OutputElem>;
	using TrainingDataIterator = typename std::vector<TrainingData>::iterator;
	using BiasesVector = std::vector<ublas::vector<double>>;
	using WeightsVector = std::vector<ublas::matrix<double>>;

	NeuralNetwork(const std::vector<int>& sizes);

	// Initialise the vector of Biases and Matrix of weights
	void PopulateZeroWeightsAndBiases(WeightsVector& w, BiasesVector& b);

	// Returns the output of the network if the input is a
	OutputElem FeedForward(InputElem a) const;

	//	Train the neural network using mini-batch stochastic
	//	gradient descent.The training_data is a vector of pairs
	// representing the training inputs and the desired
	//	outputs.The other non - optional parameters are
	//	self - explanatory.If test_data is provided then the
	//	network will be evaluated against the test data after each
	//	epoch, and partial progress printed out.This is useful for
	//	tracking progress, but slows things down substantially.
	void SGD(std::vector<TrainingData> training_data, int epochs, int mini_batch_size, double eta,
				std::vector<TrainingData> test_data);

	// Populates the gradient for the cost function for the biases in the vector nabla_b
	// and the weights in nabla_w
	void BackProp(const InputElem& td_elem, const OutputElem& td_res, BiasesVector& nabla_b,
					  WeightsVector& nabla_w);

	// Update the network's weights and biases by applying
	//	gradient descent using backpropagation to a single mini batch.
	//	The "mini_batch" is a list of tuples "(x, y)", and "eta"
	//	is the learning rate."""
	void UpdateMiniBatch(TrainingDataIterator td, int mini_batch_size, double eta);

	// Return the number of test inputs for which the neural
	//	network outputs the correct result. Note that the neural
	//	network's output is assumed to be the index of whichever
	//	neuron in the final layer has the highest activation.
	int evaluate(const std::vector<TrainingData>& td) const;

	// Return the vector of partial derivatives \partial C_x /
	//	\partial a for the output activations.
	ublas::vector<double> cost_derivative(const ublas::vector<double>& output_activations,
													  const ublas::vector<double>& y) const;

 private:
	std::vector<int> m_sizes_;
	BiasesVector biases_;
	WeightsVector weights_;
};

// int main() {
// 	std::random_device rd;
// 	std::mt19937 gen(rd());
// 	std::uniform_real_distribution<double> dist(-0.5, 0.5);
// 	std::vector<NeuralNetwork::TrainingData> training_data, test_data;
// 	for (int i = 0; i < 5000; ++i) {
// 		double rand_val = dist(gen);
// 		double double_val = rand_val * 2;
// 		NeuralNetwork::InputElem input(1);
// 		NeuralNetwork::OutputElem output(1);
// 		input[0] = rand_val;
// 		output[0] = double_val;
// 		training_data.push_back({input, output});
// 	}
//
// 	for (int i = 0; i < 1000; ++i) {
// 		double rand_val = dist(gen);
// 		double double_val = rand_val * 2;
// 		NeuralNetwork::InputElem input(1);
// 		NeuralNetwork::OutputElem output(1);
// 		input[0] = rand_val;
// 		output[0] = double_val;
// 		test_data.push_back({input, output});
// 	}
//
// 	NeuralNetwork net({1, 1, 1});
// 	net.SGD(training_data, 100, 50, 0.01, test_data);
// }
