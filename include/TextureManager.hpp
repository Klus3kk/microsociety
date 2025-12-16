#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class TextureManager {
private:
    // Stores textures using a map where the key is a texture name and the value is an sf::Texture.
    std::unordered_map<std::string, sf::Texture> textures;

public:
    // Ensures only one instance of TextureManager exists.
    static TextureManager& getInstance() {
        static TextureManager instance; // Static ensures it's created once and persists throughout the program.
        return instance;
    }

    // Loads a texture if it's not already loaded, otherwise returns the cached texture.
    const sf::Texture& getTexture(const std::string& name, const std::string& path) {
        auto it = textures.find(name); 
        if (it == textures.end()) { 
            sf::Texture texture;
            if (!texture.loadFromFile(path)) { 
                throw std::runtime_error("Failed to load texture: " + path); 
            }
            textures[name] = std::move(texture); // Store the texture in the map
        }
        return textures[name]; // Return the existing or newly loaded texture
    }

    // Prevent copying to enforce singleton pattern
    TextureManager(const TextureManager&) = delete; 
    TextureManager& operator=(const TextureManager&) = delete; 

private:
    TextureManager() = default;
};

#endif 
