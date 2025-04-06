#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image/stb_image.h>

#include <vector>
#include <string>
#include <cmath>

#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "utility.hpp"

void glfw_error(const char* msg);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow* create_window();
void process_input(GLFWwindow* window, float deltaTime);
void process_joystick_input(float deltaTime);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLuint load_cubemap(std::vector<std::string> faces);
GLuint create_cube();

float windowWidth = 800.0f;
float windowHeight = 600.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;

bool debugDraw = false;

int main(void) {
    GLFWwindow* window = create_window();
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> faces {
        "./assets/images/skybox/right.jpg",
        "./assets/images/skybox/left.jpg",
        "./assets/images/skybox/top.jpg",
        "./assets/images/skybox/bottom.jpg",
        "./assets/images/skybox/front.jpg",
        "./assets/images/skybox/back.jpg"
    };

    GLuint cubemapTexture = load_cubemap(faces);
    GLuint skybox = create_cube();

    Shader shader("./assets/shaders/model.vert", nullptr, "./assets/shaders/model.frag");
    Shader skyboxShader("./assets/shaders/skybox.vert", nullptr, "./assets/shaders/skybox.frag");

    constexpr unsigned int NUM_ROWS = 100;
    constexpr unsigned int NUM_COLUMNS = 100;
    constexpr unsigned int NUM_SLICES = 100;

    std::vector<glm::mat4> modelMatrices;
    modelMatrices.reserve(NUM_ROWS * NUM_COLUMNS * NUM_SLICES);

    for (unsigned int x = 0; x < NUM_ROWS; x++) {
        for (unsigned int y = 0; y < NUM_COLUMNS; y++) {
            for (unsigned int z = 0; z < NUM_SLICES; z++) {
                glm::mat4 modelMatrix(1.0f);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x * 5.0f, y * 5.0f, z * -5.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

                modelMatrices.push_back(modelMatrix);
            }
        }
    }

    Model model("./assets/models/cube/scene.gltf", modelMatrices);

    float lastFrame = 0.0f;

    std::cout << modelMatrices.size() << " models instancated!\n";

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window, deltaTime);
        process_joystick_input(deltaTime);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), windowWidth / windowHeight, 0.1f, 100.0f);

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        shader.Use();
        shader.Set("projection", projection);
        shader.Set("view", view);

        model.Draw(shader);

        GL_CHECK(glDepthFunc(GL_LEQUAL));

        skyboxShader.Use();
        skyboxShader.Set("projection", projection);
        skyboxShader.Set("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));

        GL_CHECK(glBindVertexArray(skybox));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
        GL_CHECK(glBindVertexArray(0));

        GL_CHECK(glDepthFunc(GL_LESS));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}

void glfw_error(const char* msg) {
    const char* description;
    int code = glfwGetError(&description);
    std::cerr << msg << ": [" << code << "] " << (description ? description : "Unknown error") << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
    GL_CHECK(glViewport(0, 0, width, height));
}

GLFWwindow* create_window() {
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error [" << error << "]: " << description << std::endl;
    });

    if (!glfwInit()) {
        glfw_error("Failed to initialize GLFW");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(static_cast<int>(windowWidth), static_cast<float>(windowHeight), "Learn OpenGL", nullptr, nullptr);
    if (!window) {
        glfw_error("Failed to create GLFW window");
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // stbi_set_flip_vertically_on_load(true);

    GL_CHECK(glClearColor(0.01f, 0.01f, 0.01f, 1.0f));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glEnable(GL_CULL_FACE));

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
        std::cout << "Gamepad connected: " << glfwGetGamepadName(GLFW_JOYSTICK_1) << std::endl;
    }

    return window;
}

void process_input(GLFWwindow* window, float deltaTime) {
    static bool lineMode = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        lineMode = true;

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        lineMode = false;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        debugDraw = true;

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        debugDraw = false;

    if (lineMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void process_joystick_input(float deltaTime) {
    if (!glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        return;
    }

    int axisCount;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);

    if (axisCount >= 5) {
        float moveX = axes[0];
        float moveY = axes[1];

        float deadzone = 0.1f;

        if (fabs(moveY) > deadzone) {
            camera.ProcessKeyboard(moveY < 0 ? FORWARD : BACKWARD, deltaTime * fabs(moveY));
        }

        if (fabs(moveX) > deadzone) {
            camera.ProcessKeyboard(moveX < 0 ? LEFT : RIGHT, deltaTime * fabs(moveX));
        }

        float lookX = axes[3];
        float lookY = axes[4];

        if (fabs(lookX) > deadzone || fabs(lookY) > deadzone) {
            float joystickSensitivity = 1000.0f;
            camera.ProcessMouseMovement(lookX * joystickSensitivity * deltaTime, -lookY * joystickSensitivity * deltaTime);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    static bool firstMouse = true;
    static float lastX = windowWidth / 2.0f;
    static float lastY = windowHeight / 2.0f;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

GLuint load_cubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cerr << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

GLuint create_cube() {
    float vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GLuint VAO;
    GL_CHECK(glGenVertexArrays(1, &VAO));
    GL_CHECK(glBindVertexArray(VAO));

    GLuint VBO;
    GL_CHECK(glGenBuffers(1, &VBO));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW));

    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0)));
    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    return VAO;
}