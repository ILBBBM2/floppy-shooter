# **a 3D game that will fit on a formatted floppy disk**




finished game download link: (https://github.com/ILBBBM2/finished-game/archive/refs/heads/main.zip)

```
$$\                        $$\                                   $$\                  $$\    $$\      $$\         $$\$$\ 
$$ |                       \__|                                  $$ |                 $$ |   $$ |     \__|        $$ $$ |
$$$$$$$\  $$$$$$\  $$$$$$\ $$\       $$$$$$\$$$$\  $$$$$$\  $$$$$$$ |$$$$$$\        $$$$$$\  $$$$$$$\ $$\ $$$$$$$\$$ $$ |
$$  __$$\ \____$$\$$  __$$\$$ |      $$  _$$  _$$\ \____$$\$$  __$$ $$  __$$\       \_$$  _| $$  __$$\$$ $$  _____$$ $$ |
$$ |  $$ |$$$$$$$ $$ |  \__$$ |      $$ / $$ / $$ |$$$$$$$ $$ /  $$ $$$$$$$$ |        $$ |   $$ |  $$ $$ \$$$$$$\ \__\__|
$$ |  $$ $$  __$$ $$ |     $$ |      $$ | $$ | $$ $$  __$$ $$ |  $$ $$   ____|        $$ |$$\$$ |  $$ $$ |\____$$\       
$$ |  $$ \$$$$$$$ $$ |     $$ |      $$ | $$ | $$ \$$$$$$$ \$$$$$$$ \$$$$$$$\         \$$$$  $$ |  $$ $$ $$$$$$$  $$\$$\ 
\__|  \__|\_______\__|     \__|      \__| \__| \__|\_______|\_______|\_______|         \____/\__|  \__\__\_______/\__\__|
                                                                                                                         
                                                                                                                         
                                                                                                                         
```


written in entirely c++ and GLSL. With the raylib openGL wrapper. NO external dependencies

requirements:

- processor: 2ghz+ clock (core duo)
- ram: 128mb or greater
- graphics card: 3dfx Voodoo Graphics 4 MB +
- os: windows xp 64bit+
- storage: 1.25mb


## **the idea:**

to begin with this project was very hard to come up with in the first place, not many things fit in a floppy disk, which stores just 1.44 megabytes, to begin with, for some context a usual 4k png would take up 5 megabytes, more than 4 times the amount that we currently have to our disposal


with such restrictions a classical game engine would not cut it, i would need to use something extremely lightweight.
my first thoght was to use pygame as it was relatively easy but after some tests, just a simple pong game was 5 megabytes. after some web surfing i came across a wrapper for openGL that had a low system requirement, with little to no bloat, RayLib.


## **challenges of raylib and openGL**

RayLib can hardly be able to be called a game engine. missing most of the notable and required features, especially for 3D rendering, and a aging series of examples, with some being downright wrong or depreciated, wokring on this project was a nightmare. to optimise for binary size i was pretty much required to write the game in a low level language, such as C and C++. This caused alot of problems very early on as there are no defiitive guides on how to use RayLib, as it is a pretty niche thing, This made it so the only option to learn was to read their documentation which is especially difficult without even knowing the basics of the "engine". After about 4 or 5 hours of learning and trrial and error, i got my first build of the game running, i added some basic objects and the ability to jump:

[https://github.com/user-attachments/assets/44dc8707-2414-4513-a5da-557b856b0e20](https://github-production-user-asset-6210df.s3.amazonaws.com/166126131/428463143-44dc8707-2414-4513-a5da-557b856b0e20.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAVCODYLSA53PQK4ZA%2F20250331%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20250331T092107Z&X-Amz-Expires=300&X-Amz-Signature=9a5670ff75237c141fdaa5a2f278f461b532c7bb5a3654a9289cd53b928d1be3&X-Amz-SignedHeaders=host)

as is pretty evident, the camera did not functio9n as intended as you could not look up or down, but it was a step towards the right direction. another problem was collisions. RayLib doesnt have any commands for collsions, leading to be being stuck between a rock and a hrad place, with a horrible collision systtem and a jump function that wasz barely working i decided to call it quits for a parkour game and decided to make a aim trainer instead. a major reason i did this is because of the reecent bloating of game sizes. i play games in my free time and i strive to become better. one way to do this is by aim training, which is essentially a software that allows you to train your aim to help you hit shots in the real game. 

## **a new mission and implementation**

after deciding on what to do i was left with the question of how to implement them. a aimtrainer has to have these three things in my opinion:
- a shooting mode  where you freehand shoot boxes to see your accuracy
- a tracking mode to train your tracing and tracking for a real game
- a grapher to show your progress and to see where you went wrong and how to adjust to it

i decided to get to work and for the first order of business was to fix my camera movement. Making boxes and appear in random places was easy, but implementing the shooting was a whole world of pain. despite the name RayLib, raylib doesnt support shot rays in a conventional way as to how other softwares usually do it. for example, in unity shooting a ray is as simple as  (ray.orgin, transform.forward, out hit). but in RayLib you get no such thing. just a general GetMouseRay function, which oyu have to manually set to where it fires. To make a ray that shot semi-realistically from the camera i made it to screenwidth/2, and screenheight/2 from the position of the camera object. shooting the ray was just one problem, since RayLib doesnt have a collisionfunction, i had to write a script/formula that uses the ray info and the box info to see if they intersect and anypoint and call a hit. 
```
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
```

after that headache it got it running pretty swelliingly. then came the problem of graphing, writing the actual graphing script wasnt hard but figuring out a way to store info across runs was. after some thinking i decided to use a .txt file to store accuracies as it was the simplest way to implement it and also allowed for easy debugging. using fstream i wrote simple fucntions to load store and clear accuracy.

https://github.com/user-attachments/assets/3d648699-9feb-4a66-a7eb-aa1035fbd131

now that i got the base mechanics working its time to make the game look good..... or so i thought


## models, lighting and textures

to make the game look better i decided to add lighting. one easy way to do this is to have static lighting with it baked into texures. this would certainly be easier but would end up looking un-authentic due to my beginner level skills in textues and models. so i decided to make a dynamic lighting system, that would not only look better but also allow me to have active reflections! making the lighting system was surprisingly easy, with the only majo0r problem that i encountered being the fac that raylibs own documentation was outdated, leading to me making my ownm function for making lights and updating lights

```
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
```

```
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
```
the main problem came in making the textures for the floor. after some thinking i decided it would be more atmospheric if there were no walls and it was a endless floor with a moody lighitng environment with a dark sky skybox. so i started by making as simple shader script is GLSL for the flooring and lighting, then applying it to my floor. the problem with making a endless floor is that you cant use conventional models and textures to make it happen. number one the file size for a model that big would be too big and it was impractical and would cause a performance hit. so i used a new way for flooring and that was through tiling with a base texture, a heightmap and a lightmap. You might think that having three textures for the floor would be impractical but it was practically needed for tiling to work. In practice, tiling is just repeating the floor textures again and again, allowing for scalable models with ease of use. use heightmaps also allowed me to cut down on complexity asa model with that much detail would take up atleast 12 megabytes in my testing.

Then came the task of adding a skybox. Despite seeming like the easiest thing to add, it proved to be the hardest because of the fact that Raylibs documentation and examples are out of date. an example given by RayLib themselves didnt work because of depreciation and feature creep. this lead to me going down a rabbithole of how to make a skybox. without any definitive proof it could even be done i spent five hours just figuring out and testing ideas on how to make a skybox 

https://github.com/user-attachments/assets/0f63cf58-555a-40b4-91fa-21adc801dc04

no matter what i did nothing seemed to work. but then i stumbled upon somone on stackoverflow describing a way to amke a skybox with nothing but a box, only problem is that he did it in unity. with nothing to lose i went for it and made a cube with the texture of the cube and disabled backface culling before rendering. and it worked flawlessly!

after just three days of work i go the game looking much more presentable.

## binary size optimisations

the game looking good is just one part ofthe puzzle. all of this work would be for naught. So came for the reckoning that proved to be optimisation
when working with such a small file size it is a practical death sentence to not use any compiler flags. so i decided to compile a list of compiler flags that i used to reduce binary size:
```
-Oz -fno-inline -static -s -Wl,-nmagic -fdata-sections -Wl,--gc-sections -fdata-sections -fno-unroll-loops
```

of these compiler flags i will say that three metter more than the rest, -Oz, -s and -static. -Oz stands for Optimise for binary size and performs similar to Os which is optimise for speed. -s is strip and allows me to reduce binary size drastically. the -static flag also produces subtantial results.

from this my exe went from 4.3 megabytes to 2.03 megabytes. another thing i did was further optimisation of my code, deleting anything i didnt need, culling all headerfiles that could be excluded. despite a large culling of useless code and header files, the binary size went from 2.03 megabytes to 1.98 megabytes. i then used unix to further bring downthe file size to 629 kilobytes.

BUT optimising the exe is not enough! optimising the assets was also neccasary, with 2 megabytes in assets alone i needed to do some serious work. after going through some lossless compression through deflate/inflate algorithms, similar to those used in zip programs like 7zip. then i did some serious work in recuing the colour depth. all png's are usually stored with 256 colours, but when optimising for size like this lossy compression is bound to happen. i just pushed as far i could the colour depth for it to look presentable and not to intefere with any calculations, with heightmaps and lightmaps. after multiple rounds of optimsations i got the file size of all assets down to use 627 kilobytes. that leaves us with a total file size of just 1256 kilobytes! leaving us comfortably in the 1.44 megabyte limit of a formatted floppy disk!


## game optimisation

as you may have seen in the previous videos, the game is not very optimised, despite looking like a kid drew the game, it struggles to pull even 300 fps in the early shooting build. Optimisations go a long way in making it more accessible. specifically for those gamers who want to get better at cs 1.6 with my game ;). To optimise my performance, i made some changes to utilise the cpu cores better, making it more multithreaded. i also made block by block occulsion culling, removing the tiling of the floor is your its not in the player's FOV. these changes and some others, skyrocketed my performance from ~297 fps to over 2K!!! 

first version:


![Image](https://github.com/user-attachments/assets/cdde946d-3698-45ea-8b80-7e099378143f)


first shooter build:


![Image](https://github.com/user-attachments/assets/4f37ed1f-50f8-4c4d-9c88-31587051e14c)

final build:


![image](https://github.com/user-attachments/assets/b92f2c68-f117-484c-8156-d6121f57932d)


After testing on a virtual machine i can verify that i can run on a 64 bit os with 3d support!



## vicory lap and show offs

with it done i present to you the final product!

https://github.com/user-attachments/assets/4df330ee-9aea-4079-a270-8aea22a81a84

## changes to be made

- make a optimal sens detector





