#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "perlin.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.6f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));

int main()
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH*2.0, SCR_HEIGHT*2.0, "Free Camera", NULL, NULL);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Free Camera", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK)
    {
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int VAO, VBO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texture attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader("vertShader.glsl", "fragShader.glsl");
    Shader shaderTwo("vertShader.glsl", "fragShader2.glsl");

    //load and create texture
    unsigned int texture1, texture2, texture3;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
     
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //loading textures
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failure to load texture 1" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("space.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failure to load texture 2" << std::endl;
    }
    stbi_image_free(data);

    //loading background image
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("universe.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failure to load texture 3" << std::endl;
    }
    stbi_image_free(data);

    int texture1Loc = glGetUniformLocation(shader.ID, "texture1");
    int texture2Loc = glGetUniformLocation(shader.ID, "texture2");
    int visibilityLoc = glGetUniformLocation(shader.ID, "mixValue");
    int modelLoc = glGetUniformLocation(shader.ID, "model");
    int viewLoc = glGetUniformLocation(shader.ID, "view");
    int projectionLoc = glGetUniformLocation(shader.ID, "projection");

    int texture3Loc = glGetUniformLocation(shaderTwo.ID, "texture3");
    int modelLoc2 = glGetUniformLocation(shaderTwo.ID, "model");
    int viewLoc2 = glGetUniformLocation(shaderTwo.ID, "view");
    int projectionLoc2 = glGetUniformLocation(shaderTwo.ID, "projection");

    //int modelLoc3 = glGetUniformLocation(shaderTwo.ID, "model");

    shader.use();
    glUniform1i(texture1Loc, 0);
    glUniform1i(texture2Loc, 1);
    shaderTwo.use();
    glUniform1i(texture3Loc, 0);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
    };

    const int gridSize = 100;  // Adjust the grid size as needed
    std::vector<glm::vec3> cubes(gridSize * gridSize);  //create the cube grid array on the heap
    float spacing = 1.0f;  // Adjust the spacing between cubes as needed
    for (int row = 0; row < gridSize; ++row)
    {
        for (int col = 0; col < gridSize; ++col)
        {
            int index = row * gridSize + col;
            cubes[index] = glm::vec3(col * spacing, 0.0f, row * spacing);
        }
    }

    Perlin perlin;
    int index;
    float amp;
    float freq;
    float val;
    /*
    for (int x = 0; x < gridSize; x++)
    {
        for (int y = 0; y < gridSize; y++)
        {
            index = x * gridSize + y;
            val = 0;
            freq = 1;
            amp = 1;

            for (int i = 0; i < 12; i++)
            {
                val += perlin.perlin(x * freq / gridSize, y * freq / gridSize) * amp;
                freq *= 2;
                amp += 2;
            }
            cubes[index].y = abs(val);
        }
    }
    
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 10000, &cubes[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
    */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view;
        projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 5000.0f);
        view = camera.GetViewMatrix();

        shader.use();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(visibilityLoc, mixValue);
        /*
        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubes);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1000);
        */
        
        int index;
        float amp;
        float freq;
        float val;
        for (int x = 0; x < gridSize; x++)
        {
            for (int y = 0; y < gridSize; y++)
            {
                index = x * gridSize + y;
                val = 0;
                freq = 1;
                amp = 1;

                for (int i = 0; i < 12; i++)
                {
                    val += perlin.perlin(x * freq / gridSize, y * freq / gridSize) * amp;
                    freq *= 2;
                    amp += 2;
                }

                cubes[index].y = val;
                for (int i = 0; i <= abs(val); i++)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(cubes[index].x, i, cubes[index].z)); 
                    //model = glm::rotate(model, glm::radians(cubes[i].y * float(glfwGetTime())), glm::vec3(0.2f, 0.3f, 0.7f));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        

        shaderTwo.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);

        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, cubePositions[0]);
        model2 = glm::scale(model2, glm::vec3(4000.0f, 4000.0f, 4000.0f));
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window)
{
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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}