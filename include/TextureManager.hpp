#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class TextureManager {
private:
    std::unordered_map<std::string, sf::Texture> textures;

public:
    static TextureManager& getInstance() {
        static TextureManager instance;
        return instance;
    }

    // Load a texture if not already loaded
    const sf::Texture& getTexture(const std::string& name, const std::string& path) {
        auto it = textures.find(name);
        if (it == textures.end()) {
            sf::Texture texture;
            if (!texture.loadFromFile(path)) {
                throw std::runtime_error("Failed to load texture: " + path);
            }
            textures[name] = std::move(texture);
        }
        return textures[name];
    }

    // Delete copy constructor and assignment operator
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

private:
    TextureManager() = default;
};

#endif
