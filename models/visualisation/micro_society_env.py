import numpy as np
import gymnasium as gym  
from gymnasium import spaces

class MicroSocietyEnv(gym.Env):
    """Custom Reinforcement Learning Environment for NPCs"""

    def __init__(self):
        super(MicroSocietyEnv, self).__init__()

        # Actions: Move, Gather, Trade, Rest
        self.action_space = spaces.Discrete(4)  

        # Observations: [Health, Energy, Wood, Stone, Food, Money]
        self.observation_space = spaces.Box(
            low=np.array([0, 0, 0, 0, 0, 0]), 
            high=np.array([100, 100, 10, 10, 10, 1000]), 
            dtype=np.float32
        )

        # Initial State
        self.state = np.array([100, 100, 0, 0, 0, 50], dtype=np.float32)  

    def step(self, action):
        """Defines how NPC interacts with the world"""

        health, energy, wood, stone, food, money = self.state

        # Move → Slight energy loss
        if action == 0:
            energy -= 5  
            reward = 1  # Small reward for movement

        # Gather resources → Costs energy but increases resources
        elif action == 1:
            if energy > 10:
                wood += np.random.randint(1, 3)
                stone += np.random.randint(1, 3)
                energy -= 10
                reward = 5 + (wood + stone) * 2  # Reward based on resources
            else:
                reward = -10  # Penalize if low energy

        # Trade → NPC sells resources for money
        elif action == 2:
            if wood > 0 or stone > 0:
                money += (wood + stone) * 3
                wood, stone = 0, 0
                reward = 10
            else:
                reward = -5  # Penalize if no resources to trade

        # Rest → Regains energy but takes time
        elif action == 3:
            energy = min(energy + 15, 100)
            reward = 2  # Slight reward

        # Penalty for energy depletion
        terminated = False
        truncated = False

        if energy <= 0:
            reward = -50  
            terminated = True  # NPC dies

        # Update state
        self.state = np.array([health, energy, wood, stone, food, money], dtype=np.float32)

        return self.state, reward, terminated, truncated, {}

    def reset(self, seed=None, options=None):
        """Resets the environment"""
        self.state = np.array([100, 100, 0, 0, 0, 50], dtype=np.float32)
        return self.state, {}

    def render(self):
        """Visual representation (optional)"""
        print(f"NPC State: {self.state}")
