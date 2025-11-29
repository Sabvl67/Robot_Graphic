#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <string>

// Scene structure containing all environment properties
struct Scene {
    std::string name;
    glm::vec3 backgroundColor;   // Clear color for the scene
    glm::vec3 lightPosition;     // Position of the light source
    glm::vec3 lightColor;        // Color/intensity of the light
    glm::vec3 groundColor;       // Color for ground plane (if we add one)
};

// Scene manager class
class SceneManager {
public:
    enum SceneType {
        DAY = 0,
        NIGHT = 1,
        SUNSET = 2,
        SCENE_COUNT = 3
    };

    SceneManager();

    // Switch to a specific scene
    void setScene(SceneType type);

    // Get current scene
    const Scene& getCurrentScene() const;

    // Get current scene index
    int getCurrentSceneIndex() const;

    // Cycle to next scene
    void nextScene();

private:
    Scene scenes[SCENE_COUNT];
    int currentSceneIndex;

    void initializeScenes();
};

#endif
