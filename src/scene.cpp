#include "scene.h"

SceneManager::SceneManager() : currentSceneIndex(0) {
    initializeScenes();
}

void SceneManager::initializeScenes() {
    // Scene 0: DAY - Bright, clear daylight
    scenes[DAY].name = "Day Scene";
    scenes[DAY].backgroundColor = glm::vec3(0.53f, 0.81f, 0.92f);  // Light sky blue
    scenes[DAY].lightPosition = glm::vec3(5.0f, 8.0f, 5.0f);       // High sun position
    scenes[DAY].lightColor = glm::vec3(1.0f, 1.0f, 0.95f);         // Bright white-yellow
    scenes[DAY].groundColor = glm::vec3(0.4f, 0.7f, 0.4f);         // Green grass

    // Scene 1: NIGHT - Dark, cool moonlight
    scenes[NIGHT].name = "Night Scene";
    scenes[NIGHT].backgroundColor = glm::vec3(0.02f, 0.02f, 0.15f); // Dark blue-black
    scenes[NIGHT].lightPosition = glm::vec3(3.0f, 6.0f, 4.0f);      // Lower moon position
    scenes[NIGHT].lightColor = glm::vec3(0.4f, 0.5f, 0.8f);         // Cool blue moonlight
    scenes[NIGHT].groundColor = glm::vec3(0.15f, 0.15f, 0.25f);     // Dark bluish ground

    // Scene 2: SUNSET - Warm, orange-red lighting
    scenes[SUNSET].name = "Sunset Scene";
    scenes[SUNSET].backgroundColor = glm::vec3(0.85f, 0.45f, 0.25f); // Orange-red sky
    scenes[SUNSET].lightPosition = glm::vec3(7.0f, 3.0f, 5.0f);      // Low sun angle
    scenes[SUNSET].lightColor = glm::vec3(1.0f, 0.6f, 0.3f);         // Warm orange light
    scenes[SUNSET].groundColor = glm::vec3(0.5f, 0.35f, 0.25f);      // Warm brown ground
}

void SceneManager::setScene(SceneType type) {
    if (type >= 0 && type < SCENE_COUNT) {
        currentSceneIndex = type;
    }
}

const Scene& SceneManager::getCurrentScene() const {
    return scenes[currentSceneIndex];
}

int SceneManager::getCurrentSceneIndex() const {
    return currentSceneIndex;
}

void SceneManager::nextScene() {
    currentSceneIndex = (currentSceneIndex + 1) % SCENE_COUNT;
}
