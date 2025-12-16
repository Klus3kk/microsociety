import os
import json
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import classification_report, confusion_matrix
import argparse

# Fix TensorFlow warnings
import warnings
warnings.filterwarnings('ignore')

try:
    import tensorflow as tf
    print(f"TensorFlow version: {tf.__version__}")
    
    # Configure TensorFlow for better compatibility
    tf.config.set_visible_devices([], 'GPU')  # Disable GPU for validation to avoid memory issues
    
except ImportError:
    print("TensorFlow not installed. Please install with: pip install tensorflow")
    exit(1)

class FixedModelValidator:
    def __init__(self, model_path, data_path):
        self.model_path = model_path
        self.data_path = data_path
        self.model = None
        self.data = None
        
        # Action names for better reporting
        self.action_names = {
            0: 'None',
            1: 'Move',
            2: 'ChopTree', 
            3: 'MineRock',
            4: 'GatherBush',
            5: 'StoreItem',
            6: 'UpgradeHouse',
            7: 'RegenerateEnergy',
            8: 'TakeOutItems',
            9: 'BuyItem',
            10: 'SellItem',
            11: 'Rest'
        }
        
    def load_model_safely(self):
        """Load model with compatibility fixes"""
        print(f"Loading model: {self.model_path}")
        
        try:
            if self.model_path.endswith('.h5'):
                # Method 1: Try loading with custom_objects
                try:
                    self.model = tf.keras.models.load_model(
                        self.model_path, 
                        custom_objects={'mse': tf.keras.losses.MeanSquaredError()}
                    )
                    print("✓ Model loaded with custom_objects fix")
                    return True
                except Exception as e1:
                    print(f"Method 1 failed: {e1}")
                    
                    # Method 2: Try loading with compile=False
                    try:
                        self.model = tf.keras.models.load_model(self.model_path, compile=False)
                        
                        # Recompile the model
                        self.model.compile(
                            optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                            loss=tf.keras.losses.MeanSquaredError(),
                            metrics=[tf.keras.metrics.MeanAbsoluteError()]
                        )
                        print("Model loaded without compilation and recompiled")
                        return True
                    except Exception as e2:
                        print(f"Method 2 failed: {e2}")
                        
                        # Method 3: Load weights only
                        try:
                            # Create a new model with same architecture
                            self.model = self._create_model_architecture()
                            self.model.load_weights(self.model_path)
                            print("Model weights loaded into new architecture")
                            return True
                        except Exception as e3:
                            print(f"Method 3 failed: {e3}")
                            
            elif self.model_path.endswith('.tflite'):
                # TensorFlow Lite model
                self.interpreter = tf.lite.Interpreter(model_path=self.model_path)
                self.interpreter.allocate_tensors()
                print("TensorFlow Lite model loaded")
                return True
                
        except Exception as e:
            print(f"Failed to load model: {e}")
            return False
            
        return False
    
    def _create_model_architecture(self):
        """Create model architecture matching the training script"""
        model = tf.keras.Sequential([
            tf.keras.layers.Dense(128, activation='relu', input_shape=(7,)),
            tf.keras.layers.Dropout(0.2),
            tf.keras.layers.Dense(128, activation='relu'),
            tf.keras.layers.Dropout(0.2),
            tf.keras.layers.Dense(64, activation='relu'),
            tf.keras.layers.Dense(11, activation='linear')  # 11 actions
        ])
        
        model.compile(
            optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
            loss=tf.keras.losses.MeanSquaredError(),
            metrics=[tf.keras.metrics.MeanAbsoluteError()]
        )
        
        return model
    
    def load_data(self):
        """Load test data"""
        print(f"Loading data: {self.data_path}")
        
        try:
            if self.data_path.endswith('.csv'):
                df = pd.read_csv(self.data_path)
                
                # Extract features
                state_cols = ['state_posX', 'state_posY', 'state_nearbyTrees', 
                             'state_nearbyRocks', 'state_nearbyBushes', 
                             'state_energyLevel', 'state_inventoryLevel']
                
                if not all(col in df.columns for col in state_cols):
                    print(f"Missing columns in CSV. Available: {df.columns.tolist()}")
                    return False
                
                self.states = df[state_cols].values
                self.actions = df['action'].values
                
                # Convert actions to 0-based indexing if needed
                if self.actions.min() > 0:
                    self.actions = self.actions - 1
                    
                self.rewards = df['reward'].values if 'reward' in df.columns else np.zeros(len(self.states))
                self.dones = df['done'].values if 'done' in df.columns else np.zeros(len(self.states))
                
            elif self.data_path.endswith('.json'):
                with open(self.data_path, 'r') as f:
                    data = json.load(f)
                
                experiences = data.get('data', data.get('experiences', []))
                
                if not experiences:
                    print("No experiences found in JSON file")
                    return False
                
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
            
            print(f"✓ Loaded {len(self.states)} test samples")
            print(f"  State shape: {self.states.shape}")
            print(f"  Action range: {self.actions.min()} to {self.actions.max()}")
            print(f"  Reward range: {self.rewards.min():.2f} to {self.rewards.max():.2f}")
            
            return True
            
        except Exception as e:
            print(f"Failed to load data: {e}")
            return False
    
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
            try:
                q_values = self.model.predict(states, verbose=0)
                return np.argmax(q_values, axis=1)
            except Exception as e:
                print(f"Prediction failed: {e}")
                return np.random.randint(0, 11, len(states))
    
    def validate_model(self):
        """Run comprehensive model validation"""
        print("\n=== Model Validation Results ===")
        
        # Sample data for testing (use subset for faster testing)
        test_size = min(1000, len(self.states))
        indices = np.random.choice(len(self.states), test_size, replace=False)
        test_states = self.states[indices]
        test_actions = self.actions[indices]
        test_rewards = self.rewards[indices]
        
        print(f"Testing on {test_size} samples...")
        
        # Get predictions
        predicted_actions = self.predict_actions(test_states)
        
        # Overall accuracy
        accuracy = np.mean(predicted_actions == test_actions)
        print(f"\n✓ Overall Accuracy: {accuracy:.3f}")
        
        # Per-action accuracy
        print("\nPer-Action Performance:")
        for action_id in range(12):  # 0-11
            mask = test_actions == action_id
            if np.sum(mask) > 0:
                action_acc = np.mean(predicted_actions[mask] == test_actions[mask])
                count = np.sum(mask)
                action_name = self.action_names.get(action_id, f"Action{action_id}")
                print(f"  {action_name:15}: {action_acc:.3f} ({count:3d} samples)")
        
        # Action distribution comparison
        print("\nAction Distribution:")
        print("  Actual vs Predicted")
        for action_id in range(12):
            actual_count = np.sum(test_actions == action_id)
            pred_count = np.sum(predicted_actions == action_id)
            actual_pct = actual_count / len(test_actions) * 100
            pred_pct = pred_count / len(predicted_actions) * 100
            action_name = self.action_names.get(action_id, f"Action{action_id}")
            print(f"  {action_name:15}: {actual_pct:5.1f}% vs {pred_pct:5.1f}%")
        
        # Reward correlation
        if hasattr(self, 'model'):
            try:
                q_values = self.model.predict(test_states[:100], verbose=0)
                chosen_q_values = q_values[np.arange(len(test_actions[:100])), test_actions[:100]]
                correlation = np.corrcoef(chosen_q_values, test_rewards[:100])[0, 1]
                print(f"\n✓ Q-value vs Reward Correlation: {correlation:.3f}")
            except:
                print("\n⚠ Could not compute Q-value correlation")
        
        return accuracy

def main():
    parser = argparse.ArgumentParser(description='Validate MicroSociety DQN model (Fixed)')
    parser.add_argument('--model', type=str, required=True, help='Path to model file (.h5 or .tflite)')
    parser.add_argument('--data', type=str, required=True, help='Path to test data (.csv or .json)')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.model):
        print(f"Model file not found: {args.model}")
        return 1
        
    if not os.path.exists(args.data):
        print(f"Data file not found: {args.data}")
        return 1
    
    print("=== MicroSociety Model Validator (Fixed) ===")
    
    validator = FixedModelValidator(args.model, args.data)
    
    # Load model
    if not validator.load_model_safely():
        print("Failed to load model")
        return 1
    
    # Load data
    if not validator.load_data():
        print("Failed to load test data")
        return 1
    
    # Run validation
    accuracy = validator.validate_model()

    print(f"\n Accuracy: {accuracy:.1%}")
    return 0

if __name__ == "__main__":
    exit(main())