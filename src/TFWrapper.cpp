#include "TFWrapper.hpp"
#include "debug.hpp"
#include <fstream>
#include <algorithm>

TensorFlowWrapper::TensorFlowWrapper()
    : modelPath("models/npc_rl_model.tflite"),
      inputOpName("serving_default_input_1"),
      outputOpName("StatefulPartitionedCall") {
}

TensorFlowWrapper::~TensorFlowWrapper() {
    cleanupTensorFlow();
}

void TensorFlowWrapper::cleanupTensorFlow() {
#ifdef USE_TENSORFLOW
    if (session) {
        TF_DeleteSession(session, status);
        session = nullptr;
    }
    
    if (graph) {
        TF_DeleteGraph(graph);
        graph = nullptr;
    }
    
    if (status) {
        TF_DeleteStatus(status);
        status = nullptr;
    }
#endif
    isInitialized = false;
}

bool TensorFlowWrapper::initialize(const std::string& path) {
#ifdef USE_TENSORFLOW
    // Clean up any previous model
    cleanupTensorFlow();
    
    modelPath = path;
    
    // Check if model file exists
    std::ifstream modelFile(modelPath, std::ios::binary);
    if (!modelFile.good()) {
        getDebugConsole().log("TensorFlow", "Model file not found: " + modelPath, LogLevel::Error);
        return false;
    }
    
    // Load model
    status = TF_NewStatus();
    graph = TF_NewGraph();
    
    // Create session options
    TF_SessionOptions* session_opts = TF_NewSessionOptions();
    
    // Create a TF session
    session = TF_NewSession(graph, session_opts, status);
    
    // Clean up session options
    TF_DeleteSessionOptions(session_opts);
    
    if (TF_GetCode(status) != TF_OK) {
        getDebugConsole().log("TensorFlow", "Failed to create session: " + 
                            std::string(TF_Message(status)), LogLevel::Error);
        cleanupTensorFlow();
        return false;
    }
    
    getDebugConsole().log("TensorFlow", "Model loaded successfully: " + modelPath);
    isInitialized = true;
    return true;
#else
    getDebugConsole().log("TensorFlow", "TensorFlow support not compiled in", LogLevel::Warning);
    return false;
#endif
}

ActionType TensorFlowWrapper::predictAction(const State& state) {
#ifdef USE_TENSORFLOW
    if (!isInitialized) {
        getDebugConsole().log("TensorFlow", "Model not initialized, using random action", LogLevel::Warning);
        return static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::Rest)));
    }
    
    // Convert state to input vector
    std::vector<float> inputVector = stateToVector(state);
    
    // Create input tensor
    TF_Tensor* inputTensor = createInputTensor(inputVector);
    if (!inputTensor) {
        getDebugConsole().log("TensorFlow", "Failed to create input tensor", LogLevel::Error);
        return static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::Rest)));
    }
    
    // For now, return random action until we implement full TF inference
    // This is a placeholder for proper TF model inference
    TF_DeleteTensor(inputTensor);
    
    // TODO: Implement proper TF inference here
    return static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::Rest)));
#else
    // Fallback to random action if TF not available
    return static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::Rest)));
#endif
}

#ifdef USE_TENSORFLOW
TF_Tensor* TensorFlowWrapper::createInputTensor(const std::vector<float>& stateVector) const {
    // Create tensor for state input (batch_size=1, features=state size)
    int64_t dims[] = {1, static_cast<int64_t>(stateVector.size())};
    size_t dataSize = stateVector.size() * sizeof(float);
    
    // Create tensor
    TF_Tensor* tensor = TF_AllocateTensor(TF_FLOAT, dims, 2, dataSize);
    
    // Copy data to tensor
    float* tensorData = static_cast<float*>(TF_TensorData(tensor));
    std::copy(stateVector.begin(), stateVector.end(), tensorData);
    
    return tensor;
}

ActionType TensorFlowWrapper::interpretOutput(TF_Tensor* outputTensor) const {
    if (!outputTensor) {
        return static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::Rest)));
    }
    
    // Get data from tensor
    float* outputData = static_cast<float*>(TF_TensorData(outputTensor));
    
    // Find action with highest Q-value
    int actionCount = static_cast<int>(ActionType::Rest) + 1;
    int bestActionIndex = 0;
    float bestValue = outputData[0];
    
    for (int i = 1; i < actionCount; i++) {
        if (outputData[i] > bestValue) {
            bestValue = outputData[i];
            bestActionIndex = i;
        }
    }
    
    return static_cast<ActionType>(bestActionIndex);
}
#endif

std::vector<float> TensorFlowWrapper::stateToVector(const State& state) const {
    // Convert State struct to a flat vector for TF input
    std::vector<float> stateVector = {
        static_cast<float>(state.posX),
        static_cast<float>(state.posY),
        static_cast<float>(state.nearbyTrees),
        static_cast<float>(state.nearbyRocks),
        static_cast<float>(state.nearbyBushes),
        static_cast<float>(state.energyLevel),
        static_cast<float>(state.inventoryLevel)
    };
    
    return stateVector;
}