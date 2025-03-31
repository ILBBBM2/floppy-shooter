#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#define RLIGHTS_IMPLEMENTATION
//#include "rlights.h"
#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else 
    #define GLSL_VERSION            100
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#define MAX_LIGHTS  4
#define cscolour (Color){ 32, 186, 166 }



/*
$$\                           $$\                                     $$\                   $$\     $$\       $$\           $$\ $$\ 
$$ |                          \__|                                    $$ |                  $$ |    $$ |      \__|          $$ |$$ |
$$$$$$$\   $$$$$$\   $$$$$$\  $$\       $$$$$$\$$$$\   $$$$$$\   $$$$$$$ | $$$$$$\        $$$$$$\   $$$$$$$\  $$\  $$$$$$$\ $$ |$$ |
$$  __$$\  \____$$\ $$  __$$\ $$ |      $$  _$$  _$$\  \____$$\ $$  __$$ |$$  __$$\       \_$$  _|  $$  __$$\ $$ |$$  _____|$$ |$$ |
$$ |  $$ | $$$$$$$ |$$ |  \__|$$ |      $$ / $$ / $$ | $$$$$$$ |$$ /  $$ |$$$$$$$$ |        $$ |    $$ |  $$ |$$ |\$$$$$$\  \__|\__|
$$ |  $$ |$$  __$$ |$$ |      $$ |      $$ | $$ | $$ |$$  __$$ |$$ |  $$ |$$   ____|        $$ |$$\ $$ |  $$ |$$ | \____$$\         
$$ |  $$ |\$$$$$$$ |$$ |      $$ |      $$ | $$ | $$ |\$$$$$$$ |\$$$$$$$ |\$$$$$$$\         \$$$$  |$$ |  $$ |$$ |$$$$$$$  |$$\ $$\ 
\__|  \__| \_______|\__|      \__|      \__| \__| \__| \_______| \_______| \_______|         \____/ \__|  \__|\__|\_______/ \__|\__|                                                                                                                                   
*/




struct Box {
    Vector3 position;
    bool isActive;
    Vector3 velocity;
    int health;
};
//sdasdssss
bool CheckRayCollisionBox(Ray ray, BoundingBox box) {
    Vector3 invDir = { 1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z };
    Vector3 tMin = Vector3Multiply(Vector3Subtract(box.min, ray.position), invDir);
    Vector3 tMax = Vector3Multiply(Vector3Subtract(box.max, ray.position), invDir);
    Vector3 t1 = Vector3Min(tMin, tMax);
    Vector3 t2 = Vector3Max(tMin, tMax);
    float tNear = fmax(fmax(t1.x, t1.y), t1.z);
    float tFar = fmin(fmin(t2.x, t2.y), t2.z);
    return tNear <= tFar && tFar >= 0.0f;
}
//sadassdasssssssssssssssssssssssssss
std::vector<float> LoadAccuracy() {
    std::vector<float> accuracies;
    std::ifstream file("accuracy.txt");
    std::string line;
    while (std::getline(file, line)) {
        try {
            accuracies.push_back(std::stof(line));
        } catch (const std::invalid_argument& e) {
            std::cerr << "invalid data ion acurracy.txt " << line << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "out of range in accuracy.txt " << line << std::endl;
        }
    }
    file.close();
    
    return accuracies;
}

std::vector<float> clearaccuracy(){
    
    std::vector<float> accuracies;
    std::ifstream file("accuracy.txt");
    std::string line;
    file.clear();
    file.close();
    return accuracies;
}

void DrawGraph(const std::vector<float>& accuracies, int screenWidth, int screenHeight) {
    int graphWidth = screenWidth - 40;
    int graphHeight = 200;
    int graphX = 20;
    int graphY = screenHeight - graphHeight - 20;

    DrawRectangleLines(graphX, graphY, graphWidth, graphHeight, BLACK);

    if (accuracies.empty()) return;

    float maxAccuracy = 100.0f;
    float minAccuracy = 0.0f;

    for (size_t i = 1; i < accuracies.size(); ++i) {
        float x1 = graphX + (i - 1) * (graphWidth / (accuracies.size() - 1));
        float y1 = graphY + graphHeight - ((accuracies[i - 1] - minAccuracy) / (maxAccuracy - minAccuracy) * graphHeight);
        float x2 = graphX + i * (graphWidth / (accuracies.size() - 1));
        float y2 = graphY + graphHeight - ((accuracies[i] - minAccuracy) / (maxAccuracy - minAccuracy) * graphHeight);
        DrawLine(x1, y1, x2, y2, RED);
        DrawText(TextFormat("%.2f", accuracies[i - 1]), x1, y1 - 10, 10, BLACK);
    }
    //labeling the points
    float lastX = graphX + (accuracies.size() - 1) * (graphWidth / (accuracies.size() - 1));
    float lastY = graphY + graphHeight - ((accuracies.back() - minAccuracy) / (maxAccuracy - minAccuracy) * graphHeight);
    DrawText(TextFormat("%.2f", accuracies.back()), lastX, lastY - 10, 10, BLACK);
}


void SaveAccuracy(float accuracy) {
    std::ofstream file("accuracy.txt", std::ios::app);
    if (file.is_open()) {
        //just slime me out bro
        file << accuracy << std::endl;
        file.close();
    }
}

void ResetGame(float &shots, float &hits, float &accuracy, std::vector<Box> &redBoxes) {
    shots = 0;
    hits = 0;
    accuracy = 100;
    for (auto& box : redBoxes) {
        box.position = (Vector3){ GetRandomValue(-10, 10), GetRandomValue(2, 5), GetRandomValue(-10, 10) };
        
        box.isActive = true;
    }
}

void SpawnMovingBox(std::vector<Box> &redBoxes, const Camera &camera) {
    redBoxes.clear();
    Box movingBox;

    //calculate the player's forward directionsssss
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));

    //keep trying to spawn the box until it is within the player's FOV
    bool validSpawn = false;
    while (!validSpawn) {
        movingBox.position = (Vector3){
            GetRandomValue(-10, 10), //x pos
            (float)GetRandomValue(1, 5),                    //y pos (rn its just a fixed value)
            GetRandomValue(-10, 10) //z pos
        };

        //calculate the direction from the player to the box
        Vector3 toBox = Vector3Normalize(Vector3Subtract(movingBox.position, camera.position));

        //FOV and angle check
        float dotProduct = Vector3DotProduct(forward, toBox);
        float angle = acosf(dotProduct) * RAD2DEG; //convert to degrees because radians

        if (angle <= 45.0f) { //using 90 degree fov (change in future if no longer true)
            validSpawn = true;
        }
    }
    //sdasdsadas
    movingBox.isActive = true;
    movingBox.velocity = (Vector3){ GetRandomValue(0, 1) == 0 ? -2.0f : 2.0f, 0.0f, 0.0f }; //moving shitttt
    movingBox.health = 2; //how many seconds HEAVY APPROXIMATION
    redBoxes.push_back(movingBox);
}



//light types
typedef enum {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT,
    LIGHT_SPOT
} LightType;

//light data
typedef struct {
    int type;
    int enabled;
    Vector3 position;
    Vector3 target;
    float color[4];
    float intensity;

    //shader lgithe parameter locs
    int typeLoc;
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int intensityLoc;
} Light;


static int lightCount = 0;     



void UpdateLightValues(Shader shader, Light light) {
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

    float position[3] = { light.position.x, light.position.y, light.position.z };
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

    float target[3] = { light.target.x, light.target.y, light.target.z };
    SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

    SetShaderValue(shader, light.colorLoc, light.color, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, light.intensityLoc, &light.intensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.colorLoc, light.color, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, light.intensityLoc, &light.intensity, SHADER_UNIFORM_FLOAT);
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);
    
}

static Light CreateLight(int type, Vector3 position, Vector3 target, Color color, float intensity, Shader shader) {
    Light light = { 0 };

    if (lightCount < MAX_LIGHTS) {
        light.enabled = 1;
        light.type = type;
        light.position = position;
        light.target = target;
        light.color[0] = (float)color.r / 255.0f;
        light.color[1] = (float)color.g / 255.0f;
        light.color[2] = (float)color.b / 255.0f;
        light.color[3] = (float)color.a / 255.0f;
        light.intensity = intensity;

        //get shader locs for light
        light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightCount));
        light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", lightCount));
        light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", lightCount));
        light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", lightCount));
        light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", lightCount));
        light.intensityLoc = GetShaderLocation(shader, TextFormat("lights[%i].intensity", lightCount));

        UpdateLightValues(shader, light);
        
        lightCount++;
    }

    return light;
}


static TextureCubemap GenTextureCubemap(Shader shader, Texture2D panorama, int size, int format)
{
    TextureCubemap cubemap = { 0 };

    rlDisableBackfaceCulling();

    unsigned int rbo = rlLoadTextureDepth(size, size, true);
    cubemap.id = rlLoadTextureCubemap(0, size, format, 1);

    unsigned int fbo = rlLoadFramebuffer();
    rlFramebufferAttach(fbo, rbo, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X, 0);


    if (rlFramebufferComplete(fbo)) TraceLog(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", fbo);

    rlEnableShader(shader.id);


    Matrix matFboProjection = MatrixPerspective(90.0*DEG2RAD, 1.0, rlGetCullDistanceNear(), rlGetCullDistanceFar());
    rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_PROJECTION], matFboProjection);

    Matrix fboViews[6] = {
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){  1.0f,  0.0f,  0.0f }, (Vector3){ 0.0f, -1.0f,  0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ -1.0f,  0.0f,  0.0f }, (Vector3){ 0.0f, -1.0f,  0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){  0.0f,  1.0f,  0.0f }, (Vector3){ 0.0f,  0.0f,  1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){  0.0f, -1.0f,  0.0f }, (Vector3){ 0.0f,  0.0f, -1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){  0.0f,  0.0f,  1.0f }, (Vector3){ 0.0f, -1.0f,  0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){  0.0f,  0.0f, -1.0f }, (Vector3){ 0.0f, -1.0f,  0.0f })
    };

    rlViewport(0, 0, size, size);   
    

    rlActiveTextureSlot(0);
    rlEnableTexture(panorama.id);

    for (int i = 0; i < 6; i++)
    {

        rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_VIEW], fboViews[i]);
        

        rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X + i, 0);
        rlEnableFramebuffer(fbo);


        rlClearScreenBuffers();
        rlLoadDrawCube();
    }

    rlDisableShader();          
    rlDisableTexture();         
    rlDisableFramebuffer();     
    rlUnloadFramebuffer(fbo);   

    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    rlEnableBackfaceCulling();


    cubemap.width = size;
    cubemap.height = size;
    cubemap.mipmaps = 1;
    cubemap.format = format;

    return cubemap;
}


int main(void)
{
    //sujhydabjhgwvjghsadads
    int screenWidth = 800;
    int screenHeight = 600; 
    bool aimlabs = false;
    InitWindow(screenWidth, screenHeight, "hawk tuah!");
    Texture2D boxTexture = LoadTexture("assets/box.png");
    Texture2D groundTexture = LoadTexture("assets/box.png");
    Texture2D wallTexture = LoadTexture("assets/box.png");
    Image icon = LoadImage("assets/icon.png");
    SetWindowIcon(icon);
     
    

    Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model boxModel = LoadModelFromMesh(cubeMesh);
    Mesh wallMesh = GenMeshCube(1.0f, 10.0f, 100.0f);
    Mesh wallMesh2 = GenMeshCube(50.00f, 10.0f, 1.0f);
    Model wallModel = LoadModelFromMesh(wallMesh);
    Model wallModel2 = LoadModelFromMesh(wallMesh2);
    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTexture;
    wallModel2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTexture;
    // Create a plane model for the ground
    Mesh groundMesh = GenMeshPlane(1000.0f, 1000.0f, 1, 1);
    Model groundModel = LoadModelFromMesh(groundMesh);
    groundModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = groundTexture;
    boxModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = boxTexture;   
    
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };   
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          
    camera.fovy = 60.0f;                                
    camera.projection = CAMERA_PERSPECTIVE;             
    int cameraMode = CAMERA_FIRST_PERSON;
    Vector3 pillarPosition = { 0.0f, 1.0f, 0.0f };
    float pillarHeight = 2.0f;
    float pillarWidth = 1.0f;
    float pillarDepth = 1.0f;
    float pillarSpeed = 2.0f;
    Vector3 playerPosition = { 0.0f, 1.0f, 0.0f };
    float playerVelocityY = 0.0f;
    const float gravity = -9.81f;   
    float actualsens = 1;
    float shots = 0;
    float hits = 0;
    float accuracy = 100;
    int bruh = 0;
    bool isshowinggraph = false;
    std::vector<Box> redBoxes;
    Light burstLight;          
    bool burstLightActive = false; 
    float burstLightTimer = 0.0f;
    // Load PBR shader and setup all required locations
    Shader shader = LoadShader(TextFormat("assets/pbr.vs", GLSL_VERSION),
                               TextFormat("assets/pbr.fs", GLSL_VERSION));
    shader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(shader, "albedoMap");
    Vector2 floorTextureTiling = (Vector2){ 10.0f, 10.0f }; // Increase tiling to repeat the texture more
    // WARNING: Metalness, roughness, and ambient occlusion are all packed into a MRA texture
    // They are passed as to the SHADER_LOC_MAP_METALNESS location for convenience,
    // shader already takes care of it accordingly
    shader.locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(shader, "mraMap");
    shader.locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(shader, "normalMap");
    // WARNING: Similar to the MRA map, the emissive map packs different information 
    // into a single texture: it stores height and emission data
    // It is binded to SHADER_LOC_MAP_EMISSION location an properly processed on shader
    shader.locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(shader, "emissiveMap");
    shader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(shader, "albedoColor");

    // Setup additional required shader locations, including lights data
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    int lightCountLoc = GetShaderLocation(shader, "numOfLights");
    int maxLightCount = MAX_LIGHTS;
    SetShaderValue(shader, lightCountLoc, &maxLightCount, SHADER_UNIFORM_INT);

    // Setup ambient color and intensity parameters
    float ambientIntensity = 0.02f;
    Color ambientColor = (Color){ 26, 32, 135, 255 };
    Vector3 ambientColorNormalized = (Vector3){ ambientColor.r/255.0f, ambientColor.g/255.0f, ambientColor.b/255.0f };
    SetShaderValue(shader, GetShaderLocation(shader, "ambientColor"), &ambientColorNormalized, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, "ambient"), &ambientIntensity, SHADER_UNIFORM_FLOAT);

    // Get location for shader parameters that can be modified in real time
    int emissiveIntensityLoc = GetShaderLocation(shader, "emissivePower");
    int emissiveColorLoc = GetShaderLocation(shader, "emissiveColor");
    int textureTilingLoc = GetShaderLocation(shader, "tiling");

    Model floor = LoadModel("assets/plane.glb");
    floor.materials[0].shader = shader;
    floor.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;
    floor.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.0f;
    floor.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
    floor.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
    floor.materials[0].maps[MATERIAL_MAP_EMISSION].color = BLACK;
    floor.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("assets/road_a.png");
    floor.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadTexture("assets/road_mra.png");
    floor.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture("assets/road_n.png");

    floorTextureTiling = (Vector2){ 10.0f, 10.00f };

    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -1.0f, 10.0f, -2.0f }, (Vector3){ 0.0f, 0.0f, 0.0f }, YELLOW, 100.0f, shader);
    int usage = 1;
    SetShaderValue(shader, GetShaderLocation(shader, "useTexAlbedo"), &usage, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, "useTexNormal"), &usage, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, "useTexMRA"), &usage, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, "useTexEmissive"), &usage, SHADER_UNIFORM_INT);
    
    

    // Load the floor texture
    Vector2 walltexturetiling = (Vector2){2, 0.5f};
    Texture2D floorTexture = LoadTexture("assets/road_a.png");

    // Assign the floor texture to the wall models
    wallModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = floorTexture;
    wallModel.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadTexture("assets/road_mra.png");
    wallModel.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture("assets/road_n.png");

    wallModel2.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = floorTexture;
    wallModel2.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadTexture("assets/road_mra.png");
    wallModel2.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture("assets/road_n.png");

    // Assign the same shader to the wall models
    wallModel.materials[0].shader = shader;
    wallModel2.materials[0].shader = shader;

    // Set the material properties for the walls
    wallModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;
    wallModel.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.0f;
    wallModel.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
    wallModel.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
    wallModel.materials[0].maps[MATERIAL_MAP_EMISSION].color = BLACK;

    wallModel2.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;
    wallModel2.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.0f;
    wallModel2.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
    wallModel2.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
    wallModel2.materials[0].maps[MATERIAL_MAP_EMISSION].color = BLACK;



    // Load skybox shader
    Shader skyboxShader = LoadShader(TextFormat("assets/skybox.vs", GLSL_VERSION),
    TextFormat("assets/skybox.fs", GLSL_VERSION));

    // Load cube map texture for the skybox
    Image skyboxImage = LoadImage("assets/box.png");
    TextureCubemap skyboxTexture = LoadTextureCubemap(skyboxImage, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(skyboxImage); // Unload the image after creating the cubemap texture
    
    // Create a cube mesh for the skybox
    Mesh skyboxMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model skyboxModel = LoadModelFromMesh(skyboxMesh);

    // Assign the skybox texture and shader to the model
    skyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = skyboxTexture;
    skyboxModel.materials[0].shader = skyboxShader;

    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model skybox = LoadModelFromMesh(cube);


    bool useHDR = false;

    
    skybox.materials[0].shader = LoadShader(TextFormat("assets/skybox.vs", GLSL_VERSION),
                                            TextFormat("assets/skybox.fs", GLSL_VERSION));

    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"), (int[1]){ MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), (int[1]) { useHDR ? 1 : 0 }, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), (int[1]){ useHDR ? 1 : 0 }, SHADER_UNIFORM_INT);

    
    Shader shdrCubemap = LoadShader(TextFormat("assets/cubemap.vs", GLSL_VERSION),
                                    TextFormat("assets/cubemap.fs", GLSL_VERSION));

    SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), (int[1]){ 0 }, SHADER_UNIFORM_INT);

    char skyboxFileName[256] = { 0 };
    

    Image skyboxImg = LoadImage("assets/skybox.png");
    if (skyboxImg.data == NULL) {
        TraceLog(LOG_ERROR, "Failed to load skybox texture!");
    } else {
        skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(skyboxImg, CUBEMAP_LAYOUT_AUTO_DETECT);
        if (skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture.id == 0) {
            std::cout<< "failed";
        }
        UnloadImage(skyboxImg);
    }

    skybox.materials[0].shader = LoadShader(TextFormat("assets/skybox.vs", GLSL_VERSION),
                                            TextFormat("assets/skybox.fs", GLSL_VERSION));
    if (skybox.materials[0].shader.id == 0) {
        std::cout<< "failed";
    }



    for (int i = 0; i < 5; i++) {
        redBoxes.push_back({ (Vector3){ GetRandomValue(-10, 10), GetRandomValue(2, 5), GetRandomValue(-10, 10) }, true });
    }
    DisableCursor();
    SetTargetFPS(99999);
    float damageTimer = 0.0f;

    skybox.materials[0].shader = LoadShader(TextFormat("assets/skybox.vs", GLSL_VERSION),
    TextFormat("assets/skybox.fs", GLSL_VERSION));
    
    SetShaderValue(skybox.materials[0].shader, 
    GetShaderLocation(skybox.materials[0].shader, "environmentMap"), 
    (int[1]){ MATERIAL_MAP_CUBEMAP }, 
    SHADER_UNIFORM_INT);
    
    SetShaderValue(skybox.materials[0].shader, 
    GetShaderLocation(skybox.materials[0].shader, "doGamma"), 
    (int[1]){ useHDR ? 1 : 0 }, 
    SHADER_UNIFORM_INT);
    
    SetShaderValue(skybox.materials[0].shader, 
    GetShaderLocation(skybox.materials[0].shader, "vflipped"), 
    (int[1]){ useHDR ? 1 : 0 }, 
    SHADER_UNIFORM_INT);
    Image cubemapImg = LoadImage("assets/sky.png");
        skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(cubemapImg, CUBEMAP_LAYOUT_AUTO_DETECT);    // CUBEMAP_LAYOUT_PANORAMA
        UnloadImage(cubemapImg);

    
    std::vector<Light> redBoxLights; 
    
    for (const auto& box : redBoxes) {
        Light redBoxLight = CreateLight(LIGHT_POINT, box.position, (Vector3){ 0.0f, 0.0f, 0.0f }, (Color){ 255, 50, 50, 255 }, 9.0f, shader);
        redBoxLights.push_back(redBoxLight);
    }


    while (!WindowShouldClose())
    {
        float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        UpdateLightValues(shader, lights[0]);


        for (size_t i = 0; i < redBoxes.size(); ++i) {
            if (redBoxes[i].isActive) {
                redBoxLights[i].position = redBoxes[i].position;
                UpdateLightValues(shader, redBoxLights[i]);
            } else {
                redBoxLights[i].enabled = 0; 
                UpdateLightValues(shader, redBoxLights[i]);
            }
        }

        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1)         
            {
                if (IsFileExtension(droppedFiles.paths[0], ".png;.jpg;.hdr;.bmp;.tga"))
                {
                    
                    UnloadTexture(skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
                    
                    if (useHDR)
                    {
                        
                        Texture2D panorama = LoadTexture(droppedFiles.paths[0]);

                        
                        skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = GenTextureCubemap(shdrCubemap, panorama, 1024, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
                        
                        UnloadTexture(panorama);    
                    }
                    else
                    {
                        Image img = LoadImage(droppedFiles.paths[0]);
                        skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
                        UnloadImage(img);
                    }

                    TextCopy(skyboxFileName, droppedFiles.paths[0]);
                }
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        


        if (IsKeyPressed(KEY_F4)) {
            aimlabs = false;
            //std::cout << "og fort";
            ResetGame(shots, hits, accuracy, redBoxes);
            for (int i = 0; i < 5; i++) {
                //repsawn the reb boxes
                redBoxes.push_back({ 
                    (Vector3){ GetRandomValue(-10, 10), GetRandomValue(2, 5), GetRandomValue(-10, 10) }, 
                    true 
                });
            }
        }

        //std::cout<< "bruh";
        if (IsKeyPressed(KEY_F5)) {
            aimlabs = true;
            //std::cout << "aimlabs mode activated";
            ResetGame(shots, hits, accuracy, redBoxes);
            SpawnMovingBox(redBoxes, camera);
        }
        if (IsKeyPressed(KEY_F1))
        {
            cameraMode = CAMERA_FIRST_PERSON;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        }
        if (IsKeyPressed(KEY_F2))
        {
            cameraMode = CAMERA_ORBITAL;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        }
        if (IsKeyPressed(KEY_PERIOD)){
            if(actualsens > 0){
                actualsens += 1;
                //std::cout << "worked";
            }
        }
        if(IsKeyPressed(KEY_COMMA)){
            if(actualsens > 0){
                actualsens -= 1;
            }
        }
        if(IsKeyPressed(KEY_R)){
            shots = 0;
            hits = 0;
            accuracy = 100;
            for (auto& box : redBoxes) {
                box.position = (Vector3){ GetRandomValue(-10, 10), GetRandomValue(2, 5), GetRandomValue(-10, 10) };
                box.isActive = true;
            }
        }
        if(IsKeyPressed(KEY_DELETE)){
            clearaccuracy();
        }
        if(IsKeyPressed(KEY_F9)){
            SetWindowSize(1920, 1080);
            SetWindowPosition(0, 0);
            screenWidth = 1920;
            screenHeight = 1080;
        }
        if(IsKeyPressed(KEY_F10)){
            SetWindowSize(800, 600);
            SetWindowPosition(1920/4 + 100, 1080/4);
            screenWidth = 800;
            screenHeight = 600;
        }
        if (IsKeyPressed(KEY_P))
        {
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 100.0f, 0.0f };
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f;
                //CameraYaw(&camera, -135 * DEG2RAD, true);
                //CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
            }
            else if (camera.projection == CAMERA_ORTHOGRAPHIC)
            {
                cameraMode = CAMERA_FIRST_PERSON;
                camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }

        UpdateCamera(&camera, cameraMode);        
        if (cameraMode == CAMERA_FIRST_PERSON)
        {
            Vector2 mouseDelta = GetMouseDelta();
            float sensitivity = 0.005f;
            //std::cout<<actualsens;
            
            sensitivity = actualsens * 0.0005f;
            //std::cout << sensitivity;
            camera.target = Vector3Add(camera.target, Vector3Scale(camera.up, -mouseDelta.y * sensitivity));
            camera.target = Vector3Add(camera.target, Vector3Scale(Vector3CrossProduct(camera.up, Vector3Subtract(camera.target, camera.position)), -mouseDelta.x * sensitivity));
        }
        int boxhealth = 100;
        int currboxhealth = 100;
        if (aimlabs) {
            shots++;
            // Update the moving box
            if (!redBoxes.empty()) {
                Box &movingBox = redBoxes[0];
                movingBox.position.x += movingBox.velocity.x * GetFrameTime();

                // Reverse direction if it hits boundaries
                if (movingBox.position.x > 10.0f || movingBox.position.x < -10.0f) {
                    movingBox.velocity.x *= -1;
                }
            }

            // Constant laser firing
            Ray ray = GetMouseRay((Vector2){ screenWidth / 2, screenHeight / 2 }, camera);
            if (!redBoxes.empty()) {
                Box &movingBox = redBoxes[0];
                BoundingBox boxBounds = { 
                    (Vector3){ movingBox.position.x - 0.5f, movingBox.position.y - 0.5f, movingBox.position.z - 0.5f },
                    (Vector3){ movingBox.position.x + 0.5f, movingBox.position.y + 0.5f, movingBox.position.z + 0.5f }
                };

                if (movingBox.isActive && CheckRayCollisionBox(ray, boxBounds)) {
                    damageTimer += GetFrameTime();
                    hits ++;

                    if (damageTimer >= 1.0f) {
                        movingBox.health -= 1;
                        damageTimer = 0.0f;
                        if (movingBox.health <= 0) {
                            movingBox.isActive = false;
                            hits += 1;
                            accuracy = (hits / (shots + 1)) * 100;
                            SpawnMovingBox(redBoxes, camera);
                        }
                    }
                } else {
                    damageTimer = 0.0f;
                }
            }
        }


        pillarPosition.x += pillarSpeed * GetFrameTime();
        if (pillarPosition.x > 15.0f || pillarPosition.x < -15.0f)
        {
            pillarSpeed *= -1;
        }

        playerVelocityY += gravity * GetFrameTime();
        playerPosition.y += playerVelocityY * GetFrameTime();

        //collision with ground
        if (playerPosition.y <= 1.0f)
        {
            playerPosition.y = 1.0f;
            playerVelocityY = 0.0f;
            
        }
        
        if(cameraMode == CAMERA_FIRST_PERSON){
            camera.position.y = playerPosition.y + 1.0f;

            playerPosition.x = camera.position.x;
            playerPosition.z = camera.position.z;
        }
        //camera.position.y = playerPosition.y + 1.0f;
        //playerPosition.x = camera.position.x;
        //playerPosition.z = camera.position.z;
        
        //shots
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !aimlabs){
            if (cameraMode == CAMERA_FIRST_PERSON)
            {
                Ray ray = GetMouseRay((Vector2){screenWidth / 2, screenHeight / 2}, camera);
                shots += 1;
            
                burstLight.position = camera.position;
                burstLight.enabled = true;
                burstLightTimer = 0.0f;
                UpdateLightValues(shader, burstLight);
            
                
                for (auto& box : redBoxes)
                {
                    BoundingBox boxBounds = { 
                        (Vector3){ box.position.x - 0.5f, box.position.y - 0.5f, box.position.z - 0.5f },
                        (Vector3){ box.position.x + 0.5f, box.position.y + 0.5f, box.position.z + 0.5f }
                    };
                
                    if (box.isActive && CheckRayCollisionBox(ray, boxBounds))
                    {
                        
                        box.isActive = false;
                        hits += 1;
                    
                        
                        box.position = (Vector3){ GetRandomValue(-10, 10), GetRandomValue(2, 5), GetRandomValue(-10, 10) };
                        box.isActive = true;
                    
                        
                        break;
                    }
                }
            }
        }
        if (burstLight.enabled) {
            burstLightTimer += GetFrameTime();
            if (burstLightTimer >= 0.2f) {
                burstLight.enabled = false;
                UpdateLightValues(shader, burstLight);
            }
        }
        if(shots > 0){
            accuracy = (hits/shots) * 100;
        }

        if (IsKeyPressed(KEY_F7)) {
            SaveAccuracy(accuracy);
            ResetGame(shots, hits, accuracy, redBoxes);
        }

        std::vector<float> accuracies = LoadAccuracy();
        
        if(IsKeyPressed(KEY_HOME)){
            if(isshowinggraph){
                bruh = 1;
                
            }
            if(!isshowinggraph){
                isshowinggraph = true;
            }
            
        }
        //std::cout << "bruh";
        if(bruh == 1){
            isshowinggraph = false;
            bruh = 0;
        }
        BeginDrawing();
            if (aimlabs && !redBoxes.empty()) {
                Box &movingBox = redBoxes[0];
                if (movingBox.isActive) {
                    DrawModel(boxModel, movingBox.position, 1.0f, RED);
                    //DrawCubeWires(movingBox.position, 1.0f, 1.0f, 1.0f, DARKPURPLE);
                }
            }
            if(isshowinggraph){
                ClearBackground(RAYWHITE);
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Accuracy Graph", 20, 20, 20, BLACK);
                DrawGraph(accuracies, screenWidth, screenHeight);
                EndDrawing();
            }
            //bruh = 1;
            if(!isshowinggraph){
                
                //bruh = 0;
                ClearBackground(RAYWHITE);

                BeginMode3D(camera);
                    rlDisableBackfaceCulling();
                    rlDisableDepthMask();
                        DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);
                    rlEnableBackfaceCulling();
                    rlEnableDepthMask();
                    //DrawSphere((Vector3){ 0.0f, 10.0f, 0.0f }, 0.5f, YELLOW);
                    //for (int i = 0; i < MAX_LIGHTS; i++) {
                    // Draw the ground with a texture
                    //DrawModel(groundModel, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
                    //DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 1000.0f, 1000.0f }, LIGHTGRAY); // Floor

                    SetShaderValue(shader, textureTilingLoc, &floorTextureTiling, SHADER_UNIFORM_VEC2);
                    Vector4 floorEmissiveColor = ColorNormalize(floor.materials[0].maps[MATERIAL_MAP_EMISSION].color);
                    SetShaderValue(shader, emissiveColorLoc, &floorEmissiveColor, SHADER_UNIFORM_VEC4);

                    DrawModel(floor, (Vector3){ 0.0f, 0.0f, 0.0f }, 100.0f, WHITE);   // Draw floor model

                    // Set floor model texture tiling and emissive color parameters on shader
                    //SetShaderValue(shader, textureTilingLoc, &floorTextureTiling, SHADER_UNIFORM_VEC2);
                    //Vector4 floorEmissiveColor = ColorNormalize(floor.materials[0].maps[MATERIAL_MAP_EMISSION].color);
                    //SetShaderValue(shader, emissiveColorLoc, &floorEmissiveColor, SHADER_UNIFORM_VEC4);
//
                    //DrawModel(floor, (Vector3){ 0.0f, 0.0f, 0.0f }, 5.0f, WHITE);   // Draw floor model

                    //DrawSphereWires(lights[1].position, 0.2f, 8, 8, ColorAlpha(lights[1].color, 0.3f));
                    // Draw the walls with textures


                    //wall shit
                    //SetShaderValue(shader, textureTilingLoc, &walltexturetiling, SHADER_UNIFORM_VEC2);
                    //DrawModel(wallModel, (Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, WHITE); // Blue wall
                    //DrawModel(wallModel, (Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, WHITE); // Green wall
                    //DrawModel(wallModel2, (Vector3){ 0.0f, 2.5f, 16.0f }, 1.0f, WHITE); // Yellow wall


                    //the movidng pillar

                    //RIP THE MOVING PILLAR
                    //DrawCube(pillarPosition, pillarWidth, pillarHeight, pillarDepth, RED);
                    //DrawCubeWires(pillarPosition, pillarWidth, pillarHeight, pillarDepth, MAROON);

                    //player cube
                    //DrawCube(playerPosition, 0.5f, 0.5f, 0.5f, PURPLE);
                    //DrawCubeWires(playerPosition, 0.5f, 0.5f, 0.5f, DARKPURPLE);

                    //floating red boxes
                    for (const auto& box : redBoxes)
                    {
                        if (box.isActive)
                        {
                            DrawModel(boxModel, box.position, 1.0f, WHITE);
                            //DrawCubeWires(box.position, 1.0f, 1.0f, 1.0f, DARKPURPLE);
                        }
                    }

                    //debug ray
                    Ray ray = GetMouseRay((Vector2){screenWidth / 2, screenHeight / 2}, camera);
                    DrawRay(ray, GREEN);
                    EndShaderMode();
                EndMode3D();

                //crosshair
                int centerX = screenWidth / 2;
                int centerY = screenHeight / 2;
                DrawLine(centerX - 10, centerY, centerX + 10, centerY, GRAY);
                DrawLine(centerX, centerY - 10, centerX, centerY + 10, GRAY);

                //info boxes
                DrawText("a short project by hari", 15, 5, 20, WHITE);
                DrawText(TextFormat("pos: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, WHITE);
                DrawText(TextFormat("tar: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, WHITE);
                DrawText(TextFormat("up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, WHITE);
                DrawText(TextFormat("sens: (%06.3f)", actualsens), 15, 45, 10, WHITE);
                DrawText(TextFormat("accuracy: (%06.3f)", accuracy), 15, 55, 10, WHITE);
                DrawText("press ESC to leave", 15, 30, 10, WHITE);
                EndDrawing();
            }
            


    }

    CloseWindow();

    //unload shit to save mem :)
    //bruhsssss
    UnloadTexture(boxTexture); 
    UnloadModel(boxModel); 

    //if(!accuracy == 100){
    //    SaveAccuracy(accuracy);
    //}
    
    //CloseWindow();

    return 0;
}