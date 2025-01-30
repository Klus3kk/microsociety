#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

// TextureManager is a singleton class responsible for loading and managing textures efficiently.
class TextureManager {
private:
    // Stores textures using a map where the key is a texture name and the value is an sf::Texture.
    std::unordered_map<std::string, sf::Texture> textures;

public:
    // Singleton instance: Ensures only one instance of TextureManager exists.
    static TextureManager& getInstance() {
        static TextureManager instance; // Static ensures it's created once and persists throughout the program.
        return instance;
    }

    // Loads a texture if it's not already loaded, otherwise returns the cached texture.
    const sf::Texture& getTexture(const std::string& name, const std::string& path) {
        auto it = textures.find(name); // Check if texture is already loaded
        if (it == textures.end()) { // If texture is not found, load it
            sf::Texture texture;
            if (!texture.loadFromFile(path)) { // Load texture from file
                throw std::runtime_error("Failed to load texture: " + path); // Handle error if loading fails
            }
            textures[name] = std::move(texture); // Store the texture in the map
        }
        return textures[name]; // Return the existing or newly loaded texture
    }

    // Prevent copying to enforce singleton pattern
    TextureManager(const TextureManager&) = delete; // Deleted copy constructor
    TextureManager& operator=(const TextureManager&) = delete; // Deleted assignment operator

private:
    // Private constructor ensures no external instantiation (singleton pattern)
    TextureManager() = default;
};

#endif // TEXTURE_MANAGER_HPP
