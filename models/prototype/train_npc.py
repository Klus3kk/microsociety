"""
MicroSociety Deep Q-Learning Training Script
Trains a DQN model using data collected from C++ game simulation
"""

import os
import json
import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow import keras
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from collections import deque
import matplotlib.pyplot as plt
import argparse
from datetime import datetime

class MicroSocietyDQN:
    def __init__(self, state_size=7, action_size=11, learning_rate=0.001):
        self.state_size = state_size
        self.action_size = action_size
        self.learning_rate = learning_rate
        
        # DQN hyperparameters
        self.gamma = 0.99          # Discount factor
        self.epsilon = 1.0         # Exploration rate
        self.epsilon_min = 0.01
        self.epsilon_decay = 0.995
        self.batch_size = 64
        self.memory = deque(maxlen=10000)
        
        # Build neural network
        self.q_network = self._build_model()
        self.target_network = self._build_model()
        self.update_target_network()
        
    def _build_model(self):
        """Build the neural network model"""
        model = keras.Sequential([
            keras.layers.Dense(128, activation='relu', input_shape=(self.state_size,)),
            keras.layers.Dropout(0.2),
            keras.layers.Dense(128, activation='relu'),
            keras.layers.Dropout(0.2),
            keras.layers.Dense(64, activation='relu'),
            keras.layers.Dense(self.action_size, activation='linear')
        ])
        
        model.compile(
            optimizer=keras.optimizers.Adam(learning_rate=self.learning_rate),
            loss='mse',
            metrics=['mae']
        )
        
        return model
    
    def update_target_network(self):
        """Copy weights from main network to target network"""
        self.target_network.set_weights(self.q_network.get_weights())
    
    def remember(self, state, action, reward, next_state, done):
        """Store experience in replay memory"""
        self.memory.append((state, action, reward, next_state, done))
    
    def act(self, state):
        """Choose action using epsilon-greedy policy"""
        if np.random.random() <= self.epsilon:
            return np.random.choice(self.action_size)
        
        q_values = self.q_network.predict(state.reshape(1, -1), verbose=0)
        return np.argmax(q_values[0])
    
    def replay(self):
        """Train the model on a batch of experiences"""
        if len(self.memory) < self.batch_size:
            return
        
        batch = np.random.choice(len(self.memory), self.batch_size, replace=False)
        states = np.array([self.memory[i][0] for i in batch])
        actions = np.array([self.memory[i][1] for i in batch])
        rewards = np.array([self.memory[i][2] for i in batch])
        next_states = np.array([self.memory[i][3] for i in batch])
        dones = np.array([self.memory[i][4] for i in batch])
        
        # Predict Q-values for current states
        current_q_values = self.q_network.predict(states, verbose=0)
        
        # Predict Q-values for next states using target network
        next_q_values = self.target_network.predict(next_states, verbose=0)
        
        # Update Q-values using Bellman equation
        for i in range(self.batch_size):
            if dones[i]:
                current_q_values[i][actions[i]] = rewards[i]
            else:
                current_q_values[i][actions[i]] = rewards[i] + self.gamma * np.max(next_q_values[i])
        
        # Train the model
        history = self.q_network.fit(states, current_q_values, epochs=1, verbose=0)
        
        # Decay epsilon
        if self.epsilon > self.epsilon_min:
            self.epsilon *= self.epsilon_decay
            
        return history.history['loss'][0]

class DataProcessor:
    def __init__(self):
        self.scaler = StandardScaler()
        self.action_mapping = {
            1: 0,   # Move
            2: 1,   # ChopTree
            3: 2,   # MineRock
            4: 3,   # GatherBush
            5: 4,   # StoreItem
            6: 5,   # UpgradeHouse
            7: 6,   # RegenerateEnergy
            8: 7,   # TakeOutItems
            9: 8,   # BuyItem
            10: 9,  # SellItem
            11: 10  # Rest
        }
    
    def load_data_from_csv(self, filename):
        """Load training data from CSV file exported by C++ game"""
        print(f"Loading data from {filename}...")
        df = pd.read_csv(filename)
        
        print(f"Loaded {len(df)} experiences")
        print(f"Columns: {df.columns.tolist()}")
        
        # Extract states, actions, rewards, next_states, dones
        state_cols = ['state_posX', 'state_posY', 'state_nearbyTrees', 
                     'state_nearbyRocks', 'state_nearbyBushes', 
                     'state_energyLevel', 'state_inventoryLevel']
        
        next_state_cols = ['nextState_posX', 'nextState_posY', 'nextState_nearbyTrees',
                          'nextState_nearbyRocks', 'nextState_nearbyBushes',
                          'nextState_energyLevel', 'nextState_inventoryLevel']
        
        states = df[state_cols].values
        next_states = df[next_state_cols].values
        actions = df['action'].map(self.action_mapping).values
        rewards = df['reward'].values
        dones = df['done'].astype(bool).values
        
        # Normalize states
        states = self.scaler.fit_transform(states)
        next_states = self.scaler.transform(next_states)
        
        return states, actions, rewards, next_states, dones
    
    def load_data_from_json(self, filename):
        """Load training data from JSON file exported by C++ game"""
        print(f"Loading data from {filename}...")
        with open(filename, 'r') as f:
            data = json.load(f)
        
        experiences = data['data'] if 'data' in data else data['experiences']
        print(f"Loaded {len(experiences)} experiences")
        
        states = []
        actions = []
        rewards = []
        next_states = []
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
            
            next_state = [
                exp['nextState']['posX'],
                exp['nextState']['posY'],
                exp['nextState']['nearbyTrees'],
                exp['nextState']['nearbyRocks'],
                exp['nextState']['nearbyBushes'],
                exp['nextState']['energyLevel'],
                exp['nextState']['inventoryLevel']
            ]
            
            states.append(state)
            next_states.append(next_state)
            actions.append(self.action_mapping.get(exp['action'], 0))
            rewards.append(exp['reward'])
            dones.append(exp['done'])
        
        states = np.array(states)
        next_states = np.array(next_states)
        
        # Normalize states
        states = self.scaler.fit_transform(states)
        next_states = self.scaler.transform(next_states)
        
        return np.array(states), np.array(actions), np.array(rewards), np.array(next_states), np.array(dones)

def analyze_data(states, actions, rewards, next_states, dones):
    """Analyze the training data quality"""
    print("\n=== Data Analysis ===")
    print(f"Total experiences: {len(states)}")
    print(f"State dimensions: {states.shape}")
    print(f"Unique actions: {np.unique(actions)}")
    print(f"Action distribution:")
    unique, counts = np.unique(actions, return_counts=True)
    for action, count in zip(unique, counts):
        print(f"  Action {action}: {count} ({count/len(actions)*100:.1f}%)")
    
    print(f"\nReward statistics:")
    print(f"  Mean: {np.mean(rewards):.3f}")
    print(f"  Std: {np.std(rewards):.3f}")
    print(f"  Min: {np.min(rewards):.3f}")
    print(f"  Max: {np.max(rewards):.3f}")
    
    print(f"\nTerminal states: {np.sum(dones)} ({np.sum(dones)/len(dones)*100:.1f}%)")

def plot_training_progress(losses, rewards_per_episode=None):
    """Plot training progress"""
    fig, axes = plt.subplots(1, 2, figsize=(12, 4))
    
    # Plot training loss
    axes[0].plot(losses)
    axes[0].set_title('Training Loss')
    axes[0].set_xlabel('Training Step')
    axes[0].set_ylabel('Loss')
    axes[0].grid(True)
    
    # Plot rewards if available
    if rewards_per_episode is not None:
        axes[1].plot(rewards_per_episode)
        axes[1].set_title('Average Reward per Episode')
        axes[1].set_xlabel('Episode')
        axes[1].set_ylabel('Average Reward')
        axes[1].grid(True)
    else:
        axes[1].text(0.5, 0.5, 'No episode data\navailable', 
                    ha='center', va='center', transform=axes[1].transAxes)
        axes[1].set_title('Episode Rewards')
    
    plt.tight_layout()
    plt.savefig('training_progress.png', dpi=150, bbox_inches='tight')
    plt.show()

def train_model(data_file, epochs=50, save_path='models'):
    """Main training function"""
    print("=== MicroSociety Deep Q-Learning Training ===")
    
    # Create models directory
    os.makedirs(save_path, exist_ok=True)
    
    # Load and process data
    processor = DataProcessor()
    
    if data_file.endswith('.csv'):
        states, actions, rewards, next_states, dones = processor.load_data_from_csv(data_file)
    elif data_file.endswith('.json'):
        states, actions, rewards, next_states, dones = processor.load_data_from_json(data_file)
    else:
        raise ValueError("Data file must be .csv or .json")
    
    # Analyze data quality
    analyze_data(states, actions, rewards, next_states, dones)
    
    # Split data for validation
    train_indices, val_indices = train_test_split(range(len(states)), test_size=0.2, random_state=42)
    
    # Initialize DQN
    dqn = MicroSocietyDQN(state_size=states.shape[1], action_size=11)
    
    # Training loop
    print(f"\n=== Training for {epochs} epochs ===")
    losses = []
    
    for epoch in range(epochs):
        # Add training data to memory
        epoch_indices = np.random.choice(train_indices, min(1000, len(train_indices)), replace=False)
        
        for i in epoch_indices:
            dqn.remember(states[i], actions[i], rewards[i], next_states[i], dones[i])
        
        # Train the model
        if len(dqn.memory) >= dqn.batch_size:
            loss = dqn.replay()
            if loss is not None:
                losses.append(loss)
        
        # Update target network every 10 epochs
        if epoch % 10 == 0:
            dqn.update_target_network()
        
        # Print progress
        if epoch % 10 == 0:
            avg_loss = np.mean(losses[-100:]) if losses else 0
            print(f"Epoch {epoch}/{epochs}, Loss: {avg_loss:.4f}, Epsilon: {dqn.epsilon:.3f}")
    
    # Evaluate on validation set
    print("\n=== Validation ===")
    val_losses = []
    for i in val_indices[:1000]:  # Evaluate on subset
        state = states[i].reshape(1, -1)
        target = rewards[i]
        if not dones[i]:
            next_q = np.max(dqn.target_network.predict(next_states[i].reshape(1, -1), verbose=0))
            target += dqn.gamma * next_q
        
        current_q = dqn.q_network.predict(state, verbose=0)[0][actions[i]]
        val_losses.append((target - current_q) ** 2)
    
    print(f"Validation Loss: {np.mean(val_losses):.4f}")
    
    # Save models
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    # Save Keras model
    keras_path = os.path.join(save_path, f'npc_dqn_model_{timestamp}.h5')
    dqn.q_network.save(keras_path)
    print(f"Saved Keras model to {keras_path}")
    
    # Convert to TensorFlow Lite
    converter = tf.lite.TFLiteConverter.from_keras_model(dqn.q_network)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    tflite_model = converter.convert()
    
    tflite_path = os.path.join(save_path, f'npc_rl_model_{timestamp}.tflite')
    with open(tflite_path, 'wb') as f:
        f.write(tflite_model)
    print(f"Saved TensorFlow Lite model to {tflite_path}")
    
    # Also save the latest model with standard name for C++ to use
    latest_tflite_path = os.path.join(save_path, 'npc_rl_model.tflite')
    with open(latest_tflite_path, 'wb') as f:
        f.write(tflite_model)
    print(f"Saved latest model as {latest_tflite_path}")
    
    # Plot training progress
    if losses:
        plot_training_progress(losses)
    
    # Save training metadata
    metadata = {
        'timestamp': timestamp,
        'epochs': epochs,
        'training_samples': len(train_indices),
        'validation_samples': len(val_indices),
        'final_epsilon': dqn.epsilon,
        'final_loss': np.mean(losses[-10:]) if losses else 0,
        'validation_loss': np.mean(val_losses),
        'data_file': data_file,
        'state_size': states.shape[1],
        'action_size': 11
    }
    
    metadata_path = os.path.join(save_path, f'training_metadata_{timestamp}.json')
    with open(metadata_path, 'w') as f:
        json.dump(metadata, f, indent=2)
    print(f"Saved training metadata to {metadata_path}")
    
    return dqn, losses

def test_model(model_path, data_file, num_samples=1000):
    """Test the trained model on data"""
    print(f"\n=== Testing Model: {model_path} ===")
    
    # Load model
    if model_path.endswith('.h5'):
        model = keras.models.load_model(model_path)
    else:
        print("TensorFlow Lite testing not implemented in this script")
        return
    
    # Load test data
    processor = DataProcessor()
    if data_file.endswith('.csv'):
        states, actions, rewards, next_states, dones = processor.load_data_from_csv(data_file)
    else:
        states, actions, rewards, next_states, dones = processor.load_data_from_json(data_file)
    
    # Test on random sample
    test_indices = np.random.choice(len(states), min(num_samples, len(states)), replace=False)
    
    correct_predictions = 0
    total_predictions = 0
    
    for i in test_indices:
        state = states[i].reshape(1, -1)
        predicted_q_values = model.predict(state, verbose=0)
        predicted_action = np.argmax(predicted_q_values[0])
        actual_action = actions[i]
        
        if predicted_action == actual_action:
            correct_predictions += 1
        total_predictions += 1
    
    accuracy = correct_predictions / total_predictions
    print(f"Action Prediction Accuracy: {accuracy:.3f} ({correct_predictions}/{total_predictions})")
    
    # Test Q-value predictions
    q_errors = []
    for i in test_indices[:100]:  # Smaller sample for Q-value testing
        state = states[i].reshape(1, -1)
        predicted_q = model.predict(state, verbose=0)[0][actions[i]]
        
        # Calculate target Q-value
        target_q = rewards[i]
        if not dones[i]:
            next_state = next_states[i].reshape(1, -1)
            next_q_max = np.max(model.predict(next_state, verbose=0))
            target_q += 0.99 * next_q_max
        
        q_errors.append(abs(predicted_q - target_q))
    
    print(f"Average Q-value Error: {np.mean(q_errors):.3f}")

def main():
    parser = argparse.ArgumentParser(description='Train Deep Q-Learning model for MicroSociety')
    parser.add_argument('--data', type=str, required=True, help='Path to training data (CSV or JSON)')
    parser.add_argument('--epochs', type=int, default=100, help='Number of training epochs')
    parser.add_argument('--test', type=str, help='Path to model for testing')
    parser.add_argument('--output', type=str, default='models', help='Output directory for models')
    
    args = parser.parse_args()
    
    if args.test:
        test_model(args.test, args.data)
    else:
        train_model(args.data, args.epochs, args.output)

if __name__ == "__main__":
    main()