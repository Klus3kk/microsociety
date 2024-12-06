import numpy as np
import tensorflow as tf
import random
import os
import matplotlib.pyplot as plt
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
        self.house = (random.randint(0, self.grid_size[0] - 1), random.randint(0, self.grid_size[1] - 1))
        self.markets = [(random.randint(0, self.grid_size[0] - 1), random.randint(0, self.grid_size[1] - 1)) for _ in range(2)]
        self.energy = 100
        self.money = 100
        self.done = False
        return self.get_state()
    
    def get_state(self):
        # Encode the state
        state = np.array(
            [self.player_position[0] / self.grid_size[0], self.player_position[1] / self.grid_size[1]]
            + list(self.inventory.values())
            + [self.energy / 100, self.money / 100]
        )
        return state
    
    def step(self, action):
        reward = -1  # Default reward for step
        if action == 0:
            self.move_player("up")
        elif action == 1:
            self.move_player("down")
        elif action == 2:
            self.move_player("left")
        elif action == 3:
            self.move_player("right")
        elif action == 4:
            reward += self.gather_resource("wood")
        elif action == 5:
            reward += self.gather_resource("stone")
        elif action == 6:
            reward += self.gather_resource("food")
        elif action == 7:
            reward += self.regenerate_energy()
        elif action == 8:
            reward += self.upgrade_house()
        elif action == 9:
            reward += self.trade_at_market("wood", "buy")
        elif action == 10:
            reward += self.trade_at_market("wood", "sell")
        elif action == 11:
            reward += self.trade_at_market("stone", "buy")
        elif action == 12:
            reward += self.trade_at_market("stone", "sell")
        
        reward += self.calculate_reward()
        self.done = self.check_done()
        return self.get_state(), reward, self.done
    
    def move_player(self, direction):
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
            return 10  # Positive reward
        return -1  # Penalty for invalid action
    
    def regenerate_energy(self):
        if tuple(self.player_position) == self.house:
            self.energy = min(100, self.energy + 20)
            return 10  # Reward for regenerating energy
        return -5  # Penalty for trying to regenerate at the wrong location
    
    def upgrade_house(self):
        if tuple(self.player_position) == self.house and self.money >= 50:
            self.money -= 50
            return 20  # Reward for upgrading house
        return -10  # Penalty for insufficient funds or wrong location
    
    def trade_at_market(self, resource, action):
        if tuple(self.player_position) in self.markets:
            if action == "buy" and self.money >= 10:
                self.inventory[resource] += 1
                self.money -= 10
                return 15  # Reward for successful trade
            elif action == "sell" and self.inventory[resource] > 0:
                self.inventory[resource] -= 1
                self.money += 10
                return 15  # Reward for successful sale
        return -5  # Penalty for invalid trade
    
    def calculate_reward(self):
        return sum(self.inventory.values())  # Reward for collecting resources
    
    def check_done(self):
        return sum(self.inventory.values()) >= 10


class DQLAgent:
    def __init__(self, state_size, action_size, model_path='saved_model.keras'):
        self.state_size = state_size
        self.action_size = action_size
        self.model_path = model_path
        self.epsilon = 1.0
        self.epsilon_min = 0.1
        self.epsilon_decay = 0.995
        self.gamma = 0.95
        self.batch_size = 32
        self.memory = []
        self.model = self.load_or_build_model()
    
    def load_or_build_model(self):
        if os.path.exists(self.model_path):
            return tf.keras.models.load_model(self.model_path)
        return self._build_model()
    
    def _build_model(self):
        model = tf.keras.Sequential([
            tf.keras.layers.Dense(128, input_dim=self.state_size, activation="relu"),
            tf.keras.layers.Dense(128, activation="relu"),
            tf.keras.layers.Dense(self.action_size, activation="linear")
        ])
        model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001), loss="mse")
        return model
    
    def remember(self, state, action, reward, next_state, done):
        self.memory.append((state, action, reward, next_state, done))
    
    def act(self, state):
        if np.random.rand() <= self.epsilon:
            return random.randrange(self.action_size)
        act_values = self.model.predict(np.array([state]))
        return np.argmax(act_values[0])
    
    def replay(self):
        if len(self.memory) < self.batch_size:
            return
        minibatch = random.sample(self.memory, self.batch_size)
        for state, action, reward, next_state, done in minibatch:
            target = reward
            if not done:
                target += self.gamma * np.amax(self.model.predict(np.array([next_state]))[0])
            target_f = self.model.predict(np.array([state]))
            target_f[0][action] = target
            self.model.fit(np.array([state]), target_f, epochs=1, verbose=0)
        if self.epsilon > self.epsilon_min:
            self.epsilon *= self.epsilon_decay
    
    def save_model(self):
        os.makedirs(os.path.dirname(self.model_path), exist_ok=True)
        self.model.save(self.model_path, save_format='keras')


# Training loop
if __name__ == "__main__":
    env = Environment()
    agent = DQLAgent(state_size=7, action_size=13)  # Update state size and action count
    episodes = 50
    rewards = []

    for episode in range(episodes):
        state = env.reset()
        cumulative_reward = 0
        for _ in range(200):
            action = agent.act(state)
            next_state, reward, done = env.step(action)
            agent.remember(state, action, reward, next_state, done)
            state = next_state
            cumulative_reward += reward
            if done:
                break
        agent.replay()
        rewards.append(cumulative_reward)
        print(f"Episode {episode + 1}/{episodes}, Reward: {cumulative_reward}")
        if (episode + 1) % 50 == 0:
            agent.save_model()

    agent.save_model()

    # Visualize rewards
    plt.plot(rewards)
    plt.xlabel("Episode")
    plt.ylabel("Reward")
    plt.title("Training Progress")
    plt.show()
