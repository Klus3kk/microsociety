#ifndef AICONTROLLER_HPP
#define AICONTROLLER_HPP

#include "Player.hpp"
#include "Tile.hpp"
#include "Actions.hpp"
#include "debug.hpp"
#include <random>
#include <vector>
#include <memory>
#include <tensorflow/c/c_api.h>


class AIController {
private:
    TF_Graph* graph;
    TF_Session* session;
    TF_Status* status;

    // Load TensorFlow model from a saved file
    void loadModel(const std::string& modelPath) {
        TF_Buffer* runOptions = nullptr;
        TF_SessionOptions* options = TF_NewSessionOptions();
        session = TF_LoadSessionFromSavedModel(options, runOptions, modelPath.c_str(), nullptr, graph, status);
        TF_DeleteSessionOptions(options);
        TF_DeleteBuffer(runOptions);

        if (TF_GetCode(status) != TF_OK) {
            getDebugConsole().log("Error", "Failed to load TensorFlow model: " + std::string(TF_Message(status)));
        } else {
            getDebugConsole().log("AIController", "TensorFlow model successfully loaded from " + modelPath);
        }
    }
public:
    AIController(const std::string& modelPath) {
        status = TF_NewStatus();
        graph = TF_NewGraph();
        loadModel(modelPath);
    }

    ~AIController() {
        TF_DeleteSession(session, status);
        TF_DeleteGraph(graph);
        TF_DeleteStatus(status);
    }

    void decideAction(PlayerEntity& npc, std::vector<std::unique_ptr<Action>>& actions, Tile& tile) {
        if (session && graph) {
            // Prepare input tensor (example input state)
            std::vector<float> inputValues = prepareInputState(npc, tile);
            TF_Tensor* inputTensor = createInputTensor(inputValues);

            // Define input and output operations
            TF_Output inputOp = {TF_GraphOperationByName(graph, "input_node"), 0};
            TF_Output outputOp = {TF_GraphOperationByName(graph, "output_node"), 0};

            // Run the TensorFlow model
            TF_Tensor* outputTensor = nullptr;
            TF_SessionRun(session, nullptr, &inputOp, &inputTensor, 1, &outputOp, &outputTensor, 1, nullptr, 0, nullptr, status);

            if (TF_GetCode(status) == TF_OK) {
                // Extract output and decide action
                float* outputData = static_cast<float*>(TF_TensorData(outputTensor));
                int actionIndex = static_cast<int>(outputData[0]);  // Assuming model outputs a single action index

                if (actionIndex >= 0 && actionIndex < actions.size()) {
                    actions[actionIndex]->perform(npc, tile);
                    getDebugConsole().log("AIController", "NPC " + npc.getName() + " performed action: " + actions[actionIndex]->getActionName());
                }
            } else {
                getDebugConsole().log("Error", "Failed to run TensorFlow session: " + std::string(TF_Message(status)));
            }

            TF_DeleteTensor(inputTensor);
            TF_DeleteTensor(outputTensor);
        } else {
            // Fallback to random action if model is not available
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, actions.size() - 1);

            int actionIndex = dist(gen);
            actions[actionIndex]->perform(npc, tile);
            getDebugConsole().log("AIController", "NPC " + npc.getName() + " performed action: " + actions[actionIndex]->getActionName());
        }
    }


    void runAI(PlayerEntity& npc, Tile& tile) {
        // Define available actions
        std::vector<std::unique_ptr<Action>> actions;
        actions.emplace_back(std::make_unique<MoveAction>());
        actions.emplace_back(std::make_unique<TreeAction>());
        actions.emplace_back(std::make_unique<StoneAction>());
        actions.emplace_back(std::make_unique<BushAction>());
        actions.emplace_back(std::make_unique<TradeAction>());
        actions.emplace_back(std::make_unique<RegenerateEnergyAction>());
        actions.emplace_back(std::make_unique<UpgradeHouseAction>());
        actions.emplace_back(std::make_unique<StoreItemAction>("wood", 1));

        // Decide which action to perform
        decideAction(npc, actions, tile);
    }

    std::vector<float> prepareInputState(const PlayerEntity& npc, const Tile& tile) {
        // Prepare input tensor values based on NPC and tile state (TO BE CHANGED)
        std::vector<float> inputState;
        inputState.push_back(static_cast<float>(npc.getHealth()));
        inputState.push_back(static_cast<float>(npc.getEnergy()));
        inputState.push_back(static_cast<float>(tile.hasObject() ? 1.0f : 0.0f));
        return inputState;
    }

    TF_Tensor* createInputTensor(const std::vector<float>& inputValues) {
        // Create a TensorFlow tensor for input values
        int64_t dims[] = {static_cast<int64_t>(inputValues.size())};
        TF_Tensor* tensor = TF_NewTensor(TF_FLOAT, dims, 1, const_cast<float*>(inputValues.data()), sizeof(float) * inputValues.size(), [](void*, size_t, void*) {}, nullptr);
        return tensor;
    }


#endif 