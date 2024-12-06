import numpy as np
import random
import tensorflow as tf
import matplotlib.pyplot as plt


class TestEnvironment:
    def __init__(self, grid_size=(10, 10), resource_count=5):
        self.grid_size = grid_size
        self.resource_count = resource_count
        self.reset()
    
    def reset(self):
        self.player_position = [0, 0]  
        self.resources = {
            "wood": random.sample([(x, y) for x in range(self.grid_size[0]) for y in range(self.grid_size[1])], self.resource_count),
            "stone": random.sample([(x, y) for x in range(self.grid_size[0]) for y in range(self.grid_size[1])], self.resource_count),
            "food": random.sample([(x, y) for x in range(self.grid_size[0]) for y in range(self.grid_size[1])], self.resource_count),
        }
        self.inventory = {"wood": 0, "stone": 0, "food": 0}
        self.steps = 0
        self.done = False
        return self.get_state()
    
    def get_state(self):
        state = np.array([
            self.player_position[0] / self.grid_size[0], 
            self.player_position[1] / self.grid_size[1],
            self.inventory["wood"] / self.resource_count,
            self.inventory["stone"] / self.resource_count,
            self.inventory["food"] / self.resource_count,
            1.0,  # Placeholder for energy (normalized)
            1.0   # Placeholder for money (normalized)
        ])
        return state

    def step(self, action):
        self.steps += 1
        reward = -1  # Default penalty for taking a step
        
        # Movement actions
        if action == 0 and self.player_position[1] > 0:  # Move up
            self.player_position[1] -= 1
        elif action == 1 and self.player_position[1] < self.grid_size[1] - 1:  # Move down
            self.player_position[1] += 1
        elif action == 2 and self.player_position[0] > 0:  # Move left
            self.player_position[0] -= 1
        elif action == 3 and self.player_position[0] < self.grid_size[0] - 1:  # Move right
            self.player_position[0] += 1
        elif action == 4:  # Gather resource
            reward += self.gather_resource()
        
        # Check if done
        if sum(self.inventory.values()) == self.resource_count * 3:
            self.done = True
            reward += 50  # Bonus for completing the task
        
        # End episode after a fixed number of steps
        if self.steps >= 100:  # Limit max steps 
            self.done = True  # Force the episode to end
        
        return self.get_state(), reward, self.done

    def gather_resource(self):
        pos = tuple(self.player_position)
        reward = 0
        for resource, locations in self.resources.items():
            if pos in locations:
                self.inventory[resource] += 1
                self.resources[resource].remove(pos)
                reward += 10  # Reward for gathering
        return reward


class DQLAgent:
    def __init__(self, state_size, action_size, model_path):
        self.state_size = state_size
        self.action_size = action_size
        self.model_path = model_path
        self.model = tf.keras.models.load_model(self.model_path)
    
    def act(self, state, epsilon=0.1):
        if np.random.rand() <= epsilon:  
            return random.randrange(self.action_size)
        state_input = np.array([state])
        print(f"State Input to Model: {state_input}")  
        act_values = self.model.predict(state_input, verbose=0)  
        print(f"Action Values: {act_values}")  
        return np.argmax(act_values[0])


def test_model(agent, environment, episodes=10):
    success_count = 0
    rewards = []
    
    for episode in range(episodes):
        state = environment.reset()
        done = False
        cumulative_reward = 0
        while not done:
            action = agent.act(state, epsilon=0.05)  # Low exploration rate
            print(f"Action taken: {action}")  
            next_state, reward, done = environment.step(action)
            print(f"Position: {environment.player_position}, Inventory: {environment.inventory}")  
            state = next_state
            cumulative_reward += reward
        rewards.append(cumulative_reward)
        if sum(environment.inventory.values()) == environment.resource_count * 3:
            success_count += 1
        print(f"Episode {episode + 1}/{episodes}, Reward: {cumulative_reward}")
    
    success_rate = (success_count / episodes) * 100
    print(f"Success Rate: {success_rate}%")
    return rewards


if __name__ == "__main__":
    # Load the trained model
    model_path = 'saved_model.keras'  
    agent = DQLAgent(state_size=7, action_size=5, model_path=model_path)  

    # Create a new testing environment
    test_env = TestEnvironment()

    # Test the model
    test_rewards = test_model(agent, test_env, episodes=20)

    # Visualize testing results
    plt.plot(test_rewards)
    plt.xlabel("Episode")
    plt.ylabel("Reward")
    plt.title("Testing Rewards")
    plt.show()
