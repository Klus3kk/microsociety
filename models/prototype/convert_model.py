import tensorflow as tf

# Load trained model
model = tf.keras.models.load_model("npc_rl_model.h5")

# Convert to TensorFlow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save model for C++
with open("npc_rl_model.tflite", "wb") as f:
    f.write(tflite_model)

print("Model successfully converted to TensorFlow Lite!")
