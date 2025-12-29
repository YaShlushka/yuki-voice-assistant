#include "neural-network.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

// Randomize as ublas vector
void Randomize(ublas::vector<double>& vec) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::normal_distribution<> d(0, 1);
	for (auto& e : vec) {
		e = d(gen);
	}
}

// Randomize as ublas matrix
void Randomize(ublas::matrix<double>& m) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::normal_distribution<> d(0, 1);
	for (auto& e : m.data()) {
		e = d(gen);
	}
}

// The sigmoid function.
void Sigmoid(ublas::vector<double>& v) {
	for (auto& iz : v) {
		iz = 1.0 / (1.0 + std::exp(-iz));
	}
}

// Derivative of the sigmoid function.
void SigmoidDerivative(ublas::vector<double>& v) {
	for (auto& iz : v) {
		iz = 1.0 / (1.0 + std::exp(-iz));
		iz = iz * (1 - iz);
	}
}

NeuralNetwork::NeuralNetwork(const std::vector<int>& sizes) : m_sizes_(sizes) {
	PopulateZeroWeightsAndBiases(weights_, biases_);

	for (auto& bias : biases_) {
		Randomize(bias);
	}

	for (auto& weight : weights_) {
		Randomize(weight);
	}
}

// Initialise the vector of Biases and Matrix of weights
void NeuralNetwork::PopulateZeroWeightsAndBiases(WeightsVector& w, BiasesVector& b) {
	b.clear();
	w.clear();

	for (size_t i = 1; i < m_sizes_.size(); ++i) {
		b.push_back(ublas::zero_vector<double>(m_sizes_[i]));
		w.push_back(ublas::zero_matrix<double>(m_sizes_[i], m_sizes_[i - 1]));
	}
}

// Returns the output of the network if the input is a
NeuralNetwork::OutputElem NeuralNetwork::FeedForward(InputElem a) const {
	for (auto i = 0; i < biases_.size(); ++i) {
		ublas::vector<double> c = prod(weights_[i], a) + biases_[i];
		Sigmoid(c);
		a = c;
	}
	return a;
}

//	Train the neural network using mini-batch stochastic
//	gradient descent.The training_data is a vector of pairs
// representing the training inputs and the desired
//	outputs.The other non - optional parameters are
//	self - explanatory.If test_data is provided then the
//	network will be evaluated against the test data after each
//	epoch, and partial progress printed out.This is useful for
//	tracking progress, but slows things down substantially.
void NeuralNetwork::SGD(std::vector<TrainingData> training_data, int epochs, int mini_batch_size,
								double eta, std::vector<TrainingData> test_data) {
	std::random_device rd;
	std::mt19937 gen(rd());
	for (auto j = 0; j < epochs; j++) {
		std::shuffle(training_data.begin(), training_data.end(), gen);
		for (auto i = 0; i < training_data.size(); i += mini_batch_size) {
			size_t remaining = training_data.size() - i;
			int batch = static_cast<int>(std::min<size_t>(mini_batch_size, remaining));
			auto iter = training_data.begin();
			std::advance(iter, i);
			UpdateMiniBatch(iter, batch, eta);
		}
		if (test_data.size() != 0)
			std::cout << "Epoch " << j << ": " << evaluate(test_data) << " / " << test_data.size()
						 << std::endl;
		else
			std::cout << "Epoch " << j << " complete" << std::endl;
	}
}

// Populates the gradient for the cost function for the biases in the vector nabla_b
// and the weights in nabla_w
void NeuralNetwork::BackProp(const InputElem& td_elem, const OutputElem& td_res,
									  BiasesVector& nabla_b, WeightsVector& nabla_w) {
	InputElem activation = td_elem;
	std::vector<InputElem> activations; // Stores the activations of each layer
	activations.push_back(td_elem);
	std::vector<ublas::vector<double>> zs; // The z vectors layer by layer
	for (size_t i = 0; i < biases_.size(); ++i) {
		ublas::vector<double> z = prod(weights_[i], activation) + biases_[i];
		zs.push_back(z);
		activation = z;
		Sigmoid(activation);
		activations.push_back(activation);
	}
	// backward pass
	auto activations_it = activations.end() - 1;
	auto zs_it = zs.end() - 1;
	SigmoidDerivative(*zs_it);
	ublas::vector<double> delta = element_prod(cost_derivative(*activations_it, td_res), *zs_it);
	auto b_it = nabla_b.end() - 1;
	auto w_it = nabla_w.end() - 1;
	*b_it = delta;
	activations_it--;
	*w_it = outer_prod(delta, trans(*activations_it));

	auto weights_it = weights_.end();
	while (activations_it != activations.begin()) {
		zs_it--;
		weights_it--;
		activations_it--;
		b_it--;
		w_it--;
		SigmoidDerivative(*zs_it);
		delta = element_prod(prod(trans(*weights_it), delta), *zs_it);
		*b_it = delta;
		*w_it = outer_prod(delta, trans(*activations_it));
	}
}

// Update the network's weights and biases by applying
//	gradient descent using backpropagation to a single mini batch.
//	The "mini_batch" is a list of tuples "(x, y)", and "eta"
//	is the learning rate."""
void NeuralNetwork::UpdateMiniBatch(TrainingDataIterator td, int mini_batch_size, double eta) {
	BiasesVector nabla_b;
	WeightsVector nabla_w;
	PopulateZeroWeightsAndBiases(nabla_w, nabla_b);
	for (auto i = 0; i < mini_batch_size; ++i, td++) {
		auto& test_data = td->first;
		auto& expected_result = td->second;
		BiasesVector delta_nabla_b;
		WeightsVector delta_nabla_w;
		PopulateZeroWeightsAndBiases(delta_nabla_w, delta_nabla_b);
		BackProp(test_data, expected_result, delta_nabla_b, delta_nabla_w);
		for (auto k = 0; k < biases_.size(); ++k) {
			nabla_b[k] += delta_nabla_b[k];
			nabla_w[k] += delta_nabla_w[k];
		}
	}
	for (auto i = 0; i < biases_.size(); ++i) {
		biases_[i] -= (eta / mini_batch_size) * nabla_b[i];
		weights_[i] -= (eta / mini_batch_size) * nabla_w[i];
	}
}

// Return the number of test inputs for which the neural
//	network outputs the correct result. Note that the neural
//	network's output is assumed to be the index of whichever
//	neuron in the final layer has the highest activation.
int NeuralNetwork::evaluate(const std::vector<TrainingData>& td) const {
	return count_if(td.begin(), td.end(), [this](const TrainingData& test_element) {
		double eps = 0.01;
		auto res = FeedForward(test_element.first);
		return (
			 std::distance(res.begin(), std::max_element(res.begin(), res.end())) ==
			 std::distance(test_element.second.begin(),
								std::max_element(test_element.second.begin(), test_element.second.end())));
	});
}

// Return the vector of partial derivatives \partial C_x /
//	\partial a for the output activations.
ublas::vector<double>
NeuralNetwork::cost_derivative(const ublas::vector<double>& output_activations,
										 const ublas::vector<double>& y) const {
	return output_activations - y;
}
