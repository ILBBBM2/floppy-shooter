# **a 3D game that will fit on a formatted floppy disk**




> [!NOTE]
> this is a pre-release build, expect some bugs and compatability issues

pre-release build for windows: [download](https://github.com/ILBBBM2/floppy-shooter/releases/download/pre-release/finished-game-main.zip)


linux port offered by [random0550](https://github.com/Random0550/floppy-shooter) much love <3 : [download](https://github.com/Random0550/floppy-shooter/releases/download/v1.0.0/floppy-shooter.tar.gz)

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


## **a background on RayLib and floppy disk games**

RayLib is not a game engine, rather a wrapper for openGL, which directly communicates with the hardware. Due to this i had to make my own "game engine". this essentially meant making camera movement, physics, collision and objects all from scratch whihc was the only way this game would have even been possible. Games were never meant to be put on floppy disks, even in the early days, they were just a installer that would run and copy the game files into your systems hard drive. DOOM a classic game took 4 floppy disks to install. other games made to fit in a floppy disk are usually rudementary text based games, or side scrolling 2D games. Even doom uses "2.5D" which is just 2D cleverly disguised to look like 3D.


## **challenges of raylib and openGL**

RayLib can hardly be able to be called a game engine. missing most of the notable and required features, especially for 3D rendering, and a aging series of examples, with some being downright wrong or depreciated, wokring on this project was a nightmare. to optimise for binary size i was pretty much required to write the game in a low level language, such as C and C++. This caused alot of problems very early on as there are no defiitive guides on how to use RayLib, as it is a pretty niche thing, This made it so the only option to learn was to read their documentation which is especially difficult without even knowing the basics of how to make a game engine. After about 4 or 5 hours of learning and trrial and error, i got my first build of the game running, i added some basic objects and the ability to jump:

https://github-production-user-asset-6210df.s3.amazonaws.com/166126131/428557366-1e435921-5d15-4ec7-8444-4e0c24f358c6.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAVCODYLSA53PQK4ZA%2F20250331%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20250331T092848Z&X-Amz-Expires=300&X-Amz-Signature=c5334652c28d617d5bb9cee59e2458413cc104cf7c6758d7bc7709928f0f12cd&X-Amz-SignedHeaders=host


as is pretty evident, the camera did not function as intended as you could not look up or down, but it was a step towards the right direction. another problem was collisions. RayLib doesnt have any commands for collsions, leading to be being stuck between a rock and a hrad place, with a horrible collision systtem and a jump function that wasz barely working i decided to call it quits for a parkour game and decided to make a aim trainer instead. a major reason i did this is because of the reecent bloating of game sizes. i play games in my free time and i strive to become better. one way to do this is by aim training, which is essentially a software that allows you to train your aim to help you hit shots in the real game. one such software is aimlabs, which despite being quite redimentary in what it offers, it needs some pretty decent specs to run with it taking up nearly 20 gigabytes on my hard drive

## **a new mission and implementation**

after deciding on what to do i was left with the question of how to implement them. a aimtrainer has to have these three things in my opinion:
- a shooting mode where you freehand shoot boxes to see your accuracy
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

https://private-user-images.githubusercontent.com/166126131/428563739-9680a805-e8fd-4794-a738-753ed9cdbc7a.mp4?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NDM0MTQ3NjYsIm5iZiI6MTc0MzQxNDQ2NiwicGF0aCI6Ii8xNjYxMjYxMzEvNDI4NTYzNzM5LTk2ODBhODA1LWU4ZmQtNDc5NC1hNzM4LTc1M2VkOWNkYmM3YS5tcDQ_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjUwMzMxJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI1MDMzMVQwOTQ3NDZaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT0zNTg2OTFiNjU1NjgyNmRmMWJhYzNlMzk2ODAzNDVkN2I0YWUxMGMyODg1NTgwNDdmZGNjNjUxOTgyNTEzM2YyJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.7-N-LngjCDJuA-4qFKX7xO598viR1R-sQk5EWZiKH6I

now that i got the base mechanics working its time to make the game look good..... or so i thought


## models, lighting and textures

to make the game look better i decided to add lighting. one easy way to do this is to have static lighting with it baked into texures. this would certainly be easier but would end up looking un-authentic due to my beginner level skills in textues and models. so i decided to make a dynamic lighting system, that would not only look better but also allow me to have active reflections! making the lighting system was surprisingly easy, with the only major problem that i encountered being the fact that raylibs own documentation was outdated, leading to me making my own function for making lights and updating lights

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


https://private-user-images.githubusercontent.com/166126131/428564479-13f3d916-ddcc-42b0-acff-a52d2574e8e8.mp4?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NDM0MTQ5MDUsIm5iZiI6MTc0MzQxNDYwNSwicGF0aCI6Ii8xNjYxMjYxMzEvNDI4NTY0NDc5LTEzZjNkOTE2LWRkY2MtNDJiMC1hY2ZmLWE1MmQyNTc0ZThlOC5tcDQ_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjUwMzMxJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI1MDMzMVQwOTUwMDVaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT00NWI5MTQxYmY1YjZlYTgyYTc5ZDJkODUyNGEwNWFkMWI4MmRlMTA3ZTY0ZmIyNTRiN2Q1NmEzMGYzMzlhNTUwJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.fUSXC273nhpvm0Hc8UsJOVP9X__4542gtDIdQkHxZ0k


no matter what i did nothing seemed to work. but then i stumbled upon somone on stackoverflow describing a way to amke a skybox with nothing but a box, only problem is that he did it in unity. with nothing to lose i went for it and made a cube with the texture of the cube and disabled backface culling before rendering. and it worked flawlessly!

after just three days of work i go the game looking much more presentable.

## binary size optimisations

the game looking good is just one part ofthe puzzle. all of this work would be for naught. So came for the reckoning that proved to be optimisation
when working with such a small file size it is a practical death sentence to not use any compiler flags. so i decided to compile a list of compiler flags that i used to reduce binary size:
```
-Oz -fno-inline -static -s -Wl,-nmagic -fdata-sections -Wl,--gc-sections -fdata-sections -fno-unroll-loops
```

of these compiler flags i will say that three metter more than the rest, -Oz, -s and -static. -Oz stands for Optimise for binary size and performs similar to Os which is optimise for speed. -s is strip and allows me to reduce binary size drastically. the -static flag also produces subtantial results.

from this my exe went from 4.3 megabytes to 2.03 megabytes. another thing i did was further optimisation of my code, deleting anything i didnt need, culling all headerfiles that could be excluded. despite a large culling of useless code and header files, the binary size went from 2.03 megabytes to 1.98 megabytes. i then used upx to further bring down the file size to 629 kilobytes.

BUT optimising the exe is not enough! optimising the assets was also neccasary, with 2 megabytes in assets alone i needed to do some serious work. after going through some lossless compression through deflate/inflate algorithms, similar to those used in zip programs like 7zip. then i did some serious work in recuing the colour depth. all png's are usually stored with 256 colours, but when optimising for size like this lossy compression is bound to happen. i just pushed as far i could the colour depth for it to look presentable and not to intefere with any calculations, with heightmaps and lightmaps. after multiple rounds of optimsations i got the file size of all assets down to use 627 kilobytes. that leaves us with a total file size of just 1256 kilobytes! leaving us comfortably in the 1.44 megabyte limit of a formatted floppy disk!


## game optimisation

as you may have seen in the previous videos, the game is not very optimised, despite looking like a kid drew the game, it struggles to pull even 300 fps in the early shooting build. Optimisations go a long way in making it more accessible. specifically for those gamers who want to get better at cs 1.6 with my game ;). To optimise my performance, i made some changes to utilise the cpu cores better, making it more multithreaded. i also made block by block occulsion culling, removing the tiling of the floor is your its not in the player's FOV. these changes and some others, skyrocketed my performance from ~297 fps to over 2K!!! 

first version:


![oger builds](https://github.com/user-attachments/assets/4b023813-7791-4280-9a22-bc2adb13be18)


first shooter build:

![early builds](https://github.com/user-attachments/assets/e0beeb00-3708-48c9-949b-f80fd31c36bd)

final build:


![FPS](https://github.com/user-attachments/assets/ecb9bd82-d86a-4fb3-b36c-e25ae965c2f9)




After testing on a virtual machine i can verify that i can run on a 64 bit os with 3d support!



## vicory lap and show offs

with it done i present to you the final product!



https://private-user-images.githubusercontent.com/166126131/428564176-d94ab930-ac5f-4034-9afb-a8d340d4de8f.mp4?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NDM0MTQ4NTksIm5iZiI6MTc0MzQxNDU1OSwicGF0aCI6Ii8xNjYxMjYxMzEvNDI4NTY0MTc2LWQ5NGFiOTMwLWFjNWYtNDAzNC05YWZiLWE4ZDM0MGQ0ZGU4Zi5tcDQ_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjUwMzMxJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI1MDMzMVQwOTQ5MTlaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT04NDZlMzJiZGRiMzAwNmEyYWY5OGJiMjg4ZDEyMzVhYjhkYzZmZTRlMjI0ODdmYmQyOTU4MzUyMmJmYmVlMjRhJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.NLDgFiZTlj0Do7K48oxmpvC3Pj1YKhx2vWPr41W3fTU


## changes to be made

- make a optimal sens detector





