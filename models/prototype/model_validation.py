"""
Model Validation and Testing for MicroSociety DQN
Provides tools to validate, test, and analyze trained models
"""

import os
import json
import numpy as np
import tensorflow as tf
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import classification_report, confusion_matrix
import argparse

class ModelValidator:
    def __init__(self, model_path, data_path):
        self.model_path = model_path
        self.data_path = data_path
        self.model = None
        self.data = None
        
        # Action names for better reporting
        self.action_names = {
            0: 'Move',
            1: 'ChopTree', 
            2: 'MineRock',
            3: 'GatherBush',
            4: 'StoreItem',
            5: 'UpgradeHouse',
            6: 'RegenerateEnergy',
            7: 'TakeOutItems',
            8: 'BuyItem',
            9: 'SellItem',
            10: 'Rest'
        }
        
    def load_model(self):
        """Load the trained model"""
        if self.model_path.endswith('.h5'):
            self.model = tf.keras.models.load_model(self.model_path)
            print(f"Loaded Keras model: {self.model_path}")
        elif self.model_path.endswith('.tflite'):
            self.interpreter = tf.lite.Interpreter(model_path=self.model_path)
            self.interpreter.allocate_tensors()
            print(f"Loaded TensorFlow Lite model: {self.model_path}")
        else:
            raise ValueError("Model must be .h5 or .tflite format")
    
    def load_data(self):
        """Load test data"""
        if self.data_path.endswith('.csv'):
            df = pd.read_csv(self.data_path)
            
            # Extract features
            state_cols = ['state_posX', 'state_posY', 'state_nearbyTrees', 
                         'state_nearbyRocks', 'state_nearbyBushes', 
                         'state_energyLevel', 'state_inventoryLevel']
            
            self.states = df[state_cols].values
            self.actions = df['action'].values - 1  # Convert to 0-based indexing
            self.rewards = df['reward'].values
            self.dones = df['done'].values
            
        elif self.data_path.endswith('.json'):
            with open(self.data_path, 'r') as f:
                data = json.load(f)
            
            experiences = data.get('data', data.get('experiences', []))
            
            states = []
            actions = []
            rewards = []
            dones = []
            
            for exp in experiences:
                state = [
                    exp['state']['posX'],
                    exp['state']['posY'], 
                    exp['state']['nearbyTrees'],
                    exp['state']['nearbyRocks'],
                    exp['state']['nearbyBushes'],
                    exp['state']['energyLevel'],
                    exp['state']['inventoryLevel']
                ]
                states.append(state)
                actions.append(exp['action'] - 1)  # Convert to 0-based
                rewards.append(exp['reward'])
                dones.append(exp['done'])
            
            self.states = np.array(states)
            self.actions = np.array(actions)
            self.rewards = np.array(rewards)
            self.dones = np.array(dones)
        
        # Normalize states
        from sklearn.preprocessing import StandardScaler
        scaler = StandardScaler()
        self.states = scaler.fit_transform(self.states)
        
        print(f"Loaded {len(self.states)} test samples")
    
    def predict_actions(self, states):
        """Predict actions for given states"""
        if hasattr(self, 'interpreter'):
            # TensorFlow Lite inference
            input_details = self.interpreter.get_input_details()
            output_details = self.interpreter.get_output_details()
            
            predictions = []
            for state in states:
                # Set input
                self.interpreter.set_tensor(input_details[0]['index'], 
                                          state.reshape(1, -1).astype(np.float32))
                
                # Run inference
                self.interpreter.invoke()
                
                # Get output
                q_values = self.interpreter.get_tensor(output_details[0]['index'])
                predicted_action = np.argmax(q_values[0])
                predictions.append(predicted_action)
            
            return np.array(predictions)
        else:
            # Keras model inference
            q_values = self.model.predict(states, verbose=0)
            return np.argmax(q_values, axis=1)
    
    def validate_action_predictions(self):
        """Validate action prediction accuracy"""
        print("\n=== Action Prediction Validation ===")
        
        predicted_actions = self.predict_actions(self.states)
        
        # Overall accuracy
        accuracy = np.mean(predicted_actions == self.actions)
        print(f"Overall Accuracy: {accuracy:.3f}")
        
        # Per-action accuracy
        print("\nPer-Action Accuracy:")
        for action_id in range(11):
            mask = self.actions == action_id
            if np.sum(mask) > 0:
                action_acc = np.mean(predicted_actions[mask] == self.actions[mask])
                count = np.sum(mask)
                print(f"  {self.action_names[action_id]:15}: {action_acc:.3f} ({count} samples)")
        
        # Classification report
        print("\nDetailed Classification Report:")
        action_labels = [self.action_names[i] for i in range(11)]
        print(classification_report(self.actions, predicted_actions, 
                                  target_names=action_labels, zero_division=0))
        
        return accuracy, predicted_actions
    
    def analyze_q_values(self, num_samples=1000):
        """Analyze Q-value predictions"""
        print("\n=== Q-Value Analysis ===")
        
        # Sample random states
        indices = np.random.choice(len(self.states), min(num_samples, len(self.states)), replace=False)
        sample_states = self.states[indices]
        sample_actions = self.actions[indices]
        sample_rewards = self.rewards[indices]
        
        if hasattr(self, 'interpreter'):
            print("Q-value analysis not available for TensorFlow Lite models")
            return
        
        # Get Q-values
        q_values = self.model.predict(sample_states, verbose=0)
        
        # Analyze Q-value distributions
        max_q_values = np.max(q_values, axis=1)
        min_q_values = np.min(q_values, axis=1)
        chosen_q_values = q_values[np.arange(len(sample_actions)), sample_actions]
        
        print(f"Q-Value Statistics (n={len(sample_states)}):")
        print(f"  Max Q-values  - Mean: {np.mean(max_q_values):.3f}, Std: {np.std(max_q_values):.3f}")
        print(f"  Min Q-values  - Mean: {np.mean(min_q_values):.3f}, Std: {np.std(min_q_values):.3f}")
        print(f"  Chosen Q-vals - Mean: {np.mean(chosen_q_values):.3f}, Std: {np.std(chosen_q_values):.3f}")
        
        # Q-value vs reward correlation
        correlation = np.corrcoef(chosen_q_values, sample_rewards[indices])[0, 1]
        print(f"  Q-value vs Reward Correlation: {correlation:.3f}")
        
        return q_values, chosen_q_values
    
    def plot_action_distribution(self, predicted_actions):
        """Plot action distribution comparison"""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        
        # Actual actions
        actual_counts = np.bincount(self.actions, minlength=11)
        ax1.bar(range(11), actual_counts, alpha=0.7, color='blue')
        ax1.set_title('Actual Action Distribution')
        ax1.set_xlabel('Action')
        ax1.set_ylabel('Count')
        ax1.set_xticks(range(11))
        ax1.set_xticklabels([self.action_names[i][:8] for i in range(11)], rotation=45)
        
        # Predicted actions
        pred_counts = np.bincount(predicted_actions, minlength=11)
        ax2.bar(range(11), pred_counts, alpha=0.7, color='red')
        ax2.set_title('Predicted Action Distribution')
        ax2.set_xlabel('Action')
        ax2.set_ylabel('Count')
        ax2.set_xticks(range(11))
        ax2.set_xticklabels([self.action_names[i][:8] for i in range(11)], rotation=45)
        
        plt.tight_layout()
        plt.savefig('action_distribution_comparison.png', dpi=150, bbox_inches='tight')
        plt.show()
    
    def plot_confusion_matrix(self, predicted_actions):
        """Plot confusion matrix"""
        cm = confusion_matrix(self.actions, predicted_actions)
        
        plt.figure(figsize=(12, 10))
        sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                   xticklabels=[self.action_names[i][:8] for i in range(11)],
                   yticklabels=[self.action_names[i][:8] for i in range(11)])
        plt.title('Action Prediction Confusion Matrix')
        plt.xlabel('Predicted Action')
        plt.ylabel('Actual Action')
        plt.xticks(rotation=45)
        plt.yticks(rotation=0)
        plt.tight_layout()
        plt.savefig('confusion_matrix.png', dpi=150, bbox_inches='tight')
        plt.show()
    
    def reward_analysis(self):
        """Analyze reward patterns"""
        print("\n=== Reward Analysis ===")
        
        # Reward statistics by action
        print("Average Reward by Action:")
        for action_id in range(11):
            mask = self.actions == action_id
            if np.sum(mask) > 0:
                avg_reward = np.mean(self.rewards[mask])
                std_reward = np.std(self.rewards[mask])
                count = np.sum(mask)
                print(f"  {self.action_names[action_id]:15}: {avg_reward:6.2f} ± {std_reward:.2f} ({count} samples)")
        
        # Plot reward distribution
        plt.figure(figsize=(12, 8))
        
        # Reward histogram
        plt.subplot(2, 2, 1)
        plt.hist(self.rewards, bins=50, alpha=0.7, color='green')
        plt.title('Reward Distribution')
        plt.xlabel('Reward')
        plt.ylabel('Frequency')
        
        # Reward by action boxplot
        plt.subplot(2, 2, 2)
        reward_by_action = [self.rewards[self.actions == i] for i in range(11)]
        plt.boxplot(reward_by_action, labels=[self.action_names[i][:8] for i in range(11)])
        plt.title('Reward Distribution by Action')
        plt.ylabel('Reward')
        plt.xticks(rotation=45)
        
        # Reward vs state features
        plt.subplot(2, 2, 3)
        plt.scatter(self.states[:, 5], self.rewards, alpha=0.5)  # Energy vs reward
        plt.xlabel('Energy Level (normalized)')
        plt.ylabel('Reward')
        plt.title('Reward vs Energy Level')
        
        plt.subplot(2, 2, 4)
        plt.scatter(self.states[:, 6], self.rewards, alpha=0.5)  # Inventory vs reward
        plt.xlabel('Inventory Level (normalized)')
        plt.ylabel('Reward')
        plt.title('Reward vs Inventory Level')
        
        plt.tight_layout()
        plt.savefig('reward_analysis.png', dpi=150, bbox_inches='tight')
        plt.show()
    
    def state_analysis(self):
        """Analyze state space coverage"""
        print("\n=== State Space Analysis ===")
        
        # State feature statistics
        feature_names = ['PosX', 'PosY', 'NearbyTrees', 'NearbyRocks', 
                        'NearbyBushes', 'EnergyLevel', 'InventoryLevel']
        
        print("State Feature Statistics:")
        for i, name in enumerate(feature_names):
            mean_val = np.mean(self.states[:, i])
            std_val = np.std(self.states[:, i])
            min_val = np.min(self.states[:, i])
            max_val = np.max(self.states[:, i])
            print(f"  {name:15}: {mean_val:6.3f} ± {std_val:.3f} [{min_val:.3f}, {max_val:.3f}]")
        
        # Plot state distributions
        fig, axes = plt.subplots(2, 4, figsize=(16, 8))
        axes = axes.flatten()
        
        for i, name in enumerate(feature_names):
            axes[i].hist(self.states[:, i], bins=30, alpha=0.7, color='purple')
            axes[i].set_title(f'{name} Distribution')
            axes[i].set_xlabel(name)
            axes[i].set_ylabel('Frequency')
        
        # Remove empty subplot
        axes[7].remove()
        
        plt.tight_layout()
        plt.savefig('state_analysis.png', dpi=150, bbox_inches='tight')
        plt.show()
    
    def run_full_validation(self):
        """Run complete validation suite"""
        print("Starting comprehensive model validation...")
        
        # Load model and data
        self.load_model()
        self.load_data()
        
        # Run all analyses
        accuracy, predicted_actions = self.validate_action_predictions()
        self.analyze_q_values()
        self.reward_analysis()
        self.state_analysis()
        
        # Generate plots
        self.plot_action_distribution(predicted_actions)
        self.plot_confusion_matrix(predicted_actions)
        
        print(f"\nValidation complete! Overall model accuracy: {accuracy:.3f}")
        print("Generated visualization files:")
        print("  - action_distribution_comparison.png")
        print("  - confusion_matrix.png") 
        print("  - reward_analysis.png")
        print("  - state_analysis.png")
        
        return accuracy

def compare_models(model_paths, data_path):
    """Compare multiple models on the same dataset"""
    print("=== Model Comparison ===")
    
    results = {}
    
    for model_path in model_paths:
        print(f"\nEvaluating {model_path}...")
        validator = ModelValidator(model_path, data_path)
        
        try:
            validator.load_model()
            validator.load_data()
            accuracy, _ = validator.validate_action_predictions()
            results[model_path] = accuracy
        except Exception as e:
            print(f"Error evaluating {model_path}: {e}")
            results[model_path] = 0.0
    
    print("\n=== Comparison Results ===")
    sorted_results = sorted(results.items(), key=lambda x: x[1], reverse=True)
    
    for model_path, accuracy in sorted_results:
        print(f"{model_path}: {accuracy:.3f}")
    
    return results

def main():
    parser = argparse.ArgumentParser(description='Validate MicroSociety DQN models')
    parser.add_argument('--model', type=str, required=True, help='Path to model file (.h5 or .tflite)')
    parser.add_argument('--data', type=str, required=True, help='Path to test data (.csv or .json)')
    parser.add_argument('--compare', nargs='+', help='Compare multiple models')
    
    args = parser.parse_args()
    
    if args.compare:
        compare_models(args.compare, args.data)
    else:
        validator = ModelValidator(args.model, args.data)
        validator.run_full_validation()

if __name__ == "__main__":
    main()