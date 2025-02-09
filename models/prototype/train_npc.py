import gymnasium as gym  
import numpy as np
import tensorflow as tf
from tensorflow import keras
from collections import deque
import random
from micro_society_env import MicroSocietyEnv  

# Create RL Environment
env = MicroSocietyEnv()

# Define Q-Network
model = keras.Sequential([
    keras.layers.Dense(64, activation="relu", input_shape=(6,)),  # More neurons for better learning
    keras.layers.Dense(64, activation="relu"),
    keras.layers.Dense(env.action_space.n, activation="linear")  # Output layer for action selection
])

model.compile(optimizer=keras.optimizers.Adam(learning_rate=0.001), loss="mse")

# Training Parameters
EPISODES = 5000  
GAMMA = 0.99  
EPSILON = 1.0  
EPSILON_MIN = 0.01  
EPSILON_DECAY = 0.995  
MEMORY = deque(maxlen=5000)  
BATCH_SIZE = 64  

# Training Loop
for episode in range(EPISODES):
    state, _ = env.reset()  
    state = state.reshape(1, 6)
    total_reward = 0
    done = False

    while not done:
        # Choose action (Explore vs Exploit)
        if np.random.rand() < EPSILON:
            action = np.random.choice(env.action_space.n)
        else:
            action = np.argmax(model.predict(state, verbose=0))

        next_state, reward, terminated, truncated, _ = env.step(action)  
        next_state = next_state.reshape(1, 6)

        done = terminated or truncated  

        # Store experience
        MEMORY.append((state, action, reward, next_state, done))
        state = next_state
        total_reward += reward

        # Train on Mini-Batch
        if len(MEMORY) > BATCH_SIZE:
            batch = random.sample(MEMORY, BATCH_SIZE)
            for s, a, r, ns, d in batch:
                target = model.predict(s, verbose=0)
                if d:
                    target[0][a] = r
                else:
                    target[0][a] = r + GAMMA * np.max(model.predict(ns, verbose=0))
                model.fit(s, target, epochs=1, verbose=0)

    # Decay Exploration Rate
    if EPSILON > EPSILON_MIN:
        EPSILON *= EPSILON_DECAY

    print(f"Episode {episode + 1}: Total Reward: {total_reward}")

# Save Model
model.save("npc_rl_model.h5")
