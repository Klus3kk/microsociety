#ifndef TENSORFLOW_WRAPPER_HPP
#define TENSORFLOW_WRAPPER_HPP

#include <string>
#include <vector>
#include <memory>

#include "ActionType.hpp"
#include "State.hpp"

// Only include TensorFlow headers if actually using TensorFlow
#ifdef USE_TENSORFLOW
#include <tensorflow/c/c_api.h>
#else

// Forward declare only when TensorFlow is not available
struct TF_Graph;
struct TF_Session;
struct TF_Status;
struct TF_Tensor;
#endif

class TensorFlowWrapper {
private:
    // TensorFlow session and related objects
#ifdef USE_TENSORFLOW
    TF_Graph* graph = nullptr;
    TF_Session* session = nullptr;
    TF_Status* status = nullptr;
#else
    void* graph = nullptr;      // Placeholder when TF not available
    void* session = nullptr;
    void* status = nullptr;
#endif
    
    bool isInitialized = false;
    std::string modelPath;
    
    // Input and output tensor information
    std::string inputOpName;
    std::string outputOpName;
    
    // Helper methods
    void cleanupTensorFlow();
    
#ifdef USE_TENSORFLOW
    TF_Tensor* createInputTensor(const std::vector<float>& stateVector) const;
    ActionType interpretOutput(TF_Tensor* outputTensor) const;
#else
    void* createInputTensor(const std::vector<float>& stateVector) const { return nullptr; }
    ActionType interpretOutput(void* outputTensor) const { 
        return static_cast<ActionType>(1 + rand() % static_cast<int>(ActionType::Rest)); 
    }
#endif
    
public:
    TensorFlowWrapper();
    ~TensorFlowWrapper();
    
    // Initialize TF model
    bool initialize(const std::string& modelPath);
    
    // Predict action using TF model
    ActionType predictAction(const State& state);
    
    // Convert state to vector for TF input
    std::vector<float> stateToVector(const State& state) const;
    
    // Check if TF is properly initialized
    bool isModelLoaded() const { return isInitialized; }
};

#endif