import numpy as np
import tensorflow as tf
import random
import os
import matplotlib.pyplot as plt
import csv
import warnings
warnings.filterwarnings('ignore')

class Environment:
    def __init__(self, grid_size=(64, 64)):
        self.grid_size = grid_size
        self.reset()
    
    def reset(self):
        self.player_position = [self.grid_size[0] // 2, self.grid_size[1] // 2]
        self.resources = {
            "wood": random.sample([(x, y) for x in range(10) for y in range(10)], 5),
            "stone": random.sample([(x, y) for x in range(20, 30) for y in range(20, 30)], 5),
            "food": random.sample([(x, y) for x in range(40, 50) for y in range(40, 50)], 5),
        }
        self.inventory = {"wood": 0, "stone": 0, "food": 0}
        self.done = False
        return self.get_state()
    
    def get_state(self):
        state = {
            "player_position": np.array(self.player_position) / np.array(self.grid_size),
            "inventory": np.array(list(self.inventory.values())),
        }
        return state
    
    def step(self, action):
        if action == 0:
            self.move_player()
        elif action == 1:
            self.gather_resource("wood")
        elif action == 2:
            self.gather_resource("stone")
        elif action == 3:
            self.gather_resource("food")
        
        reward = self.calculate_reward(action)
        self.done = self.check_done()
        
        return self.get_state(), reward, self.done
    
    def move_player(self):
        direction = random.choice(["up", "down", "left", "right"])
        if direction == "up" and self.player_position[1] > 0:
            self.player_position[1] -= 1
        elif direction == "down" and self.player_position[1] < self.grid_size[1] - 1:
            self.player_position[1] += 1
        elif direction == "left" and self.player_position[0] > 0:
            self.player_position[0] -= 1
        elif direction == "right" and self.player_position[0] < self.grid_size[0] - 1:
            self.player_position[0] += 1
    
    def gather_resource(self, resource_type):
        pos = tuple(self.player_position)
        if pos in self.resources[resource_type]:
            self.inventory[resource_type] += 1
            self.resources[resource_type].remove(pos)
    
    def calculate_reward(self, action):
        if action in [1, 2, 3]:  
            return 10
        return -1  
    
    def check_done(self):
        return sum(self.inventory.values()) >= 10

class DQLAgent:
    def __init__(self, state_size, action_size, model_path='models/prototype/saved_model.keras'):
        self.state_size = state_size
        self.action_size = action_size
        self.model_path = model_path
        self.epsilon = 1.0  # Exploration rate
        self.epsilon_min = 0.1
        self.epsilon_decay = 0.995
        self.model = self.load_or_build_model()
    
    def load_or_build_model(self):
        if os.path.exists(self.model_path):
            print("Loading saved model...")
            return tf.keras.models.load_model(self.model_path)
        else:
            print("Building new model...")
            return self._build_model()

    def _build_model(self):
        model = tf.keras.Sequential([
            tf.keras.layers.Dense(24, input_dim=self.state_size, activation="relu"),
            tf.keras.layers.Dense(24, activation="relu"),
            tf.keras.layers.Dense(self.action_size, activation="linear")
        ])
        model.compile(optimizer="adam", loss="mse")
        return model
    
    def save_model(self):
        os.makedirs(os.path.dirname(self.model_path), exist_ok=True)
        self.model.save(self.model_path, save_format='keras')
    
    def act(self, state):
        if np.random.rand() <= self.epsilon:
            return random.randrange(self.action_size)
        state_input = np.array([state["player_position"].tolist() + state["inventory"].tolist()])
        act_values = self.model.predict(state_input)
        return np.argmax(act_values[0])

# Main loop for training with logging and visualization as before
if __name__ == "__main__":
    env = Environment()
    agent = DQLAgent(state_size=5, action_size=4)
    
    episodes = 5
    rewards = []
    successes = []
    epsilons = []
    
    for episode in range(episodes):
        state = env.reset()
        done = False
        cumulative_reward = 0
        success = 0
        step = 0
        
        while not done:
            action = agent.act(state)
            next_state, reward, done = env.step(action)
            cumulative_reward += reward
            if done:
                success = 1
            state = next_state
            step += 1
        
        rewards.append(cumulative_reward)
        successes.append(success)
        epsilons.append(agent.epsilon)
        
        print(f"Episode {episode + 1}/{episodes} - Reward: {cumulative_reward}, Success: {success}, Epsilon: {agent.epsilon}")
        
        if agent.epsilon > agent.epsilon_min:
            agent.epsilon *= agent.epsilon_decay
            
        agent.save_model()

    # Visualization (same as before)
    plt.figure(figsize=(12, 8))
    # Plot cumulative reward
    plt.subplot(3, 1, 1)
    plt.plot(rewards, label="Cumulative Reward")
    plt.xlabel("Episode")
    plt.ylabel("Cumulative Reward")
    plt.legend()
    
    # Plot success rate
    plt.subplot(3, 1, 2)
    plt.plot(successes, label="Success (1=Success, 0=Failure)")
    plt.xlabel("Episode")
    plt.ylabel("Success")
    plt.legend()
    
    # Plot epsilon decay
    plt.subplot(3, 1, 3)
    plt.plot(epsilons, label="Epsilon (Exploration Rate)")
    plt.xlabel("Episode")
    plt.ylabel("Epsilon")
    plt.legend()
    
    plt.tight_layout()
    plt.show()