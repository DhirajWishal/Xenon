# Shaders

Shaders are used for multiple purposes. They are used to position vertices in the 3D space (vertex shaders), color pixels (fragment shaders) and even for general purposes (compute shaders). In Xenon, there are some rules to follow when building your shader. The bindings, inputs and some outputs are defined to have some properties which developers need to be aware of.

The primary shader language supported by Xenon is HLSL. We provide a couple of tools to translate HLSL shader sources to compatible binaries that are used by the engine. To use this tool, include the `Tools/CMake/ShaderCompiler.CMake` file from the repository in your `CMakeLists.txt` file and use the `CompileShaders` tool.

There are some features provided by the engine, mainly some presets that can be used by the HLSL files. They are placed in the `Engine/XenonShaderBank/Core` directory. This directory is automatically included by the shader compiler tool when compiling the shaders.

## Shader compiler

We provide a CMake tool to convert HLSL shader source files to a Xenon-compatible format. This format is just a couple of C++ header files which contain the compiled binaries. To understand the process, consider the following scenario.
There is a vertex shader file in HLSL `Shader.vert.hlsl`. The `CompileShaders` CMake function does not take any inputs. But it will go through all the directories in the calling `CMAKE_CURRENT_SOURCE_DIR` path and recursively find all the HLSL files. In our case, it'll find the `Shader.vert.hlsl` file. The tool will then create a new directory called `Generated` and in there, it'll generate two header files, called `Shader.vert.hlsl.dxil.hpp` and `Shader.vert.hlsl.spirv.hpp`. A new header file is created next to the `Shader.vert.hlsl` called `Shader.vert.hpp` which will include the two generated files. The `Shader.vert.hpp` file will look like this,

```cpp
// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "E:/Nexonous/Xenon/Engine/XenonBackend/Shader.hpp"

#include <cstdint>

#include "Generated/Shader.vert.hlsl.dxil.hpp"
#include "Generated/Shader.vert.hlsl.spirv.hpp"

namespace Xenon { namespace Generated { [[nodisacrd]] inline Xenon::Backend::Shader CreateShader_vert() { return Xenon::Backend::Shader::Create(g_SHADER_VERT_SPIRV, g_SHADER_VERT_DXIL); } } }

```

Once this file is generated, include this file when needed and use the `CreateShader_vert()` function to create the `Xenon::Backend::Shader` instance.

Note that when generating the header files, the shader source should look like this: `<Shader Name>.<Shader Type>.hlsl`. Any `*.hlsli` files will be ignored since they are treated like include files (header files in C++).

## GLSL Shaders

Xenon does not support native GLSL to binary conversion tools (we might add it in the future but not right now). So any shaders that are written in GLSL has to be converted to two C++ header files. Please take a look at the Shader compiler section for more information.

### Resources

Resources to the shaders are passed with three descriptors.

1. `location(set = 0, binding = x)` is used to pass in user data (uniform buffers, storage buffers and images).
2. `location(set = 1, binding = x)` is used to pass in material data (buffers and textures).
3. `location(set = 2, binding = x)` is used to pass in the camera buffers.

The camera descriptor depends on the `Xenon::Camera` class, and all of its inherited classes (the two provided classes (`Xenon::MonoCamera`, `Xenon::StereoCamera`) and any custom user-defined classes).

The user data descriptor does not have any binding order. This depends on what the user wants to input into the shader. This may include any uniform data (like the model matrix), lighting information, or any other information which the shaders might need.

Out of the three descriptors, the material descriptor has a strict binding table.
| SPIR-V GLSL                                            | Xenon resource type                         | Description                                                                                              |
|--------------------------------------------------------|---------------------------------------------|----------------------------------------------------------------------------------------------------------|
| `location(set = 2, binding = 0) uniform sampler2D`     | `::Xenon::ResourceType::Diffuse`            | This is used to pass in the diffuse image used by a mesh.                                                |
| `location(set = 2, binding = 1) uniform sampler2D`     | `::Xenon::ResourceType::Albedo`             | This is used to pass in the albedo image used by a mesh.                                                 |
| `location(set = 2, binding = 2) uniform sampler2D`     | `::Xenon::ResourceType::Detail`             | This is used to pass in the detail image used by a mesh.                                                 |
| `location(set = 2, binding = 3) uniform sampler2D`     | `::Xenon::ResourceType::Gradient`           | This is used to pass in the gradient image used by a mesh.                                               |
| `location(set = 2, binding = 4) uniform sampler2D`     | `::Xenon::ResourceType::Transparency`       | This is used to pass in the transparency image used by a mesh.                                           |
| `location(set = 2, binding = 5) uniform sampler2D`     | `::Xenon::ResourceType::Bump`               | This is used to pass in the bump image used by a mesh.                                                   |
| `location(set = 2, binding = 6) uniform sampler2D`     | `::Xenon::ResourceType::Normal`             | This is used to pass in the normal image used by a mesh.                                                 |
| `location(set = 2, binding = 7) uniform sampler2D`     | `::Xenon::ResourceType::RadiosityNormal`    | This is used to pass in the radiosity normal image used by a mesh.                                       |
| `location(set = 2, binding = 8) uniform sampler2D`     | `::Xenon::ResourceType::Displacement`       | This is used to pass in the displacement image used by a mesh.                                           |
| `location(set = 2, binding = 9) uniform sampler2D`     | `::Xenon::ResourceType::Height`             | This is used to pass in the height map image used by a mesh.                                             |
| `location(set = 2, binding = 10) uniform sampler2D`    | `::Xenon::ResourceType::VectorDisplacement` | This is used to pass in the vector displacement map image used by a mesh.                                |
| `location(set = 2, binding = 11) uniform sampler2D`    | `::Xenon::ResourceType::Wrinkle`            | This is used to pass in the wrinkle map image used by a mesh.                                            |
| `location(set = 2, binding = 12) uniform sampler2D`    | `::Xenon::ResourceType::Parallax`           | This is used to pass in the parallax image used by a mesh.                                               |
| `location(set = 2, binding = 13) uniform sampler2D`    | `::Xenon::ResourceType::DuDv`               | This is used to pass in the DuDv image used by a mesh.                                                   |
| `location(set = 2, binding = 14) uniform sampler2D`    | `::Xenon::ResourceType::Flow`               | This is used to pass in the flow map image used by a mesh.                                               |
| `location(set = 2, binding = 15) uniform sampler2D`    | `::Xenon::ResourceType::Curvature`          | This is used to pass in the curvature image used by a mesh.                                              |
| `location(set = 2, binding = 16) uniform sampler2D`    | `::Xenon::ResourceType::Specular`           | This is used to pass in the specular image used by a mesh.                                               |
| `location(set = 2, binding = 17) uniform sampler2D`    | `::Xenon::ResourceType::Gloss`              | This is used to pass in the gloss image used by a mesh.                                                  |
| `location(set = 2, binding = 18) uniform sampler2D`    | `::Xenon::ResourceType::Roughness`          | This is used to pass in the roughness image used by a mesh.                                              |
| `location(set = 2, binding = 19) uniform sampler2D`    | `::Xenon::ResourceType::Reflectivity`       | This is used to pass in the reflectivity image used by a mesh.                                           |
| `location(set = 2, binding = 20) uniform sampler2D`    | `::Xenon::ResourceType::Metallic`           | This is used to pass in the metallic image used by a mesh.                                               |
| `location(set = 2, binding = 21) uniform sampler2D`    | `::Xenon::ResourceType::Anisotrophic`       | This is used to pass in the anisotropy image used by a mesh.                                            |
| `location(set = 2, binding = 22) uniform sampler2D`    | `::Xenon::ResourceType::BRDF`               | This is used to pass in the bidirectional reflectance distribution function (BRDF) image used by a mesh. |
| `location(set = 2, binding = 23) uniform sampler2D`    | `::Xenon::ResourceType::Fresnel`            | This is used to pass in the fresnel image used by a mesh.                                                |
| `location(set = 2, binding = 24) uniform samplerCube`  | `::Xenon::ResourceType::CubeMap`            | This is used to pass in the cube map image used by a mesh.                                                |
| `location(set = 2, binding = 25) uniform samplerCube`  | `::Xenon::ResourceType::DiffuselyCubeMap`   | This is used to pass in the diffuse cube map image used by a mesh.                                      |
| `location(set = 2, binding = 26) uniform ssamplerCube` | `::Xenon::ResourceType::SphericalEnvMap`    | This is used to pass in the spherical environment map image used by a mesh.                              |
| `location(set = 2, binding = 27) uniform sampler2D`    | `::Xenon::ResourceType::LightMap`           | This is used to pass in the light map image used by a mesh.                                              |
| `location(set = 2, binding = 28) uniform sampler2D`    | `::Xenon::ResourceType::AmbientOcclusion`   | This is used to pass in the ambient occlusion image used by a mesh.                                      |
| `location(set = 2, binding = 29) uniform sampler2D`    | `::Xenon::ResourceType::CavityMap`          | This is used to pass in the cavity map image used by a mesh.                                             |
| `location(set = 2, binding = 30) uniform sampler2D`    | `::Xenon::ResourceType::Emissive`           | This is used to pass in the emissive image used by a mesh.                                               |
| `location(set = 2, binding = 31) uniform sampler2D`    | `::Xenon::ResourceType::Thickness`          | This is used to pass in the thickness image used by a mesh.                                              |
| `location(set = 2, binding = 32) uniform`              | `::Xenon::ResourceType::UserDefined_0`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 33) uniform`              | `::Xenon::ResourceType::UserDefined_1`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 34) uniform`              | `::Xenon::ResourceType::UserDefined_2`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 35) uniform`              | `::Xenon::ResourceType::UserDefined_3`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 36) uniform`              | `::Xenon::ResourceType::UserDefined_4`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 37) uniform`              | `::Xenon::ResourceType::UserDefined_5`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 38) uniform`              | `::Xenon::ResourceType::UserDefined_6`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |
| `location(set = 2, binding = 39) uniform`              | `::Xenon::ResourceType::UserDefined_7`      | This is used to pass in any user-attached image used by a mesh. This can be anything.                    |

### Vertex Shader

#### Inputs

| SPIR-V GLSL                     | Xenon macro                          | Description                                                                                                                                                                                                                       |
|---------------------------------|--------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `location(layout = 0) in`       | `XENON_VERTEX_INPUT_POSITION`        | This is where the engine will provide the shader with the position data loaded by the asset file, or given in by the user.                                                                                                        |
| `location(layout = 1) in`       | `XENON_VERTEX_INPUT_NORMAL`          | This is where the engine will provide the shader with the normal data loaded by the asset file, or given in by the user.                                                                                                          |
| `location(layout = 2) in`       | `XENON_VERTEX_INPUT_TANGENT`         | This is where the engine will provide the shader with the tangent data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 3) in`       | `XENON_VERTEX_INPUT_BI_TANGENT`      | This is where the engine will provide the shader with the bi-tangent data loaded by the asset file, or given in by the user.                                                                                                      |
| `location(layout = 4) in`       | `XENON_VERTEX_INPUT_COLOR_0`         | This is where the engine will provide the shader with the color 0 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 5) in`       | `XENON_VERTEX_INPUT_COLOR_1`         | This is where the engine will provide the shader with the color 1 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 6) in`       | `XENON_VERTEX_INPUT_COLOR_2`         | This is where the engine will provide the shader with the color 2 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 7) in`       | `XENON_VERTEX_INPUT_COLOR_3`         | This is where the engine will provide the shader with the color 3 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 8) in`       | `XENON_VERTEX_INPUT_COLOR_4`         | This is where the engine will provide the shader with the color 4 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 9) in`       | `XENON_VERTEX_INPUT_COLOR_5`         | This is where the engine will provide the shader with the color 5 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 10) in`      | `XENON_VERTEX_INPUT_COLOR_6`         | This is where the engine will provide the shader with the color 6 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 11) in`      | `XENON_VERTEX_INPUT_COLOR_7`         | This is where the engine will provide the shader with the color 7 data loaded by the asset file, or given in by the user.                                                                                                         |
| `location(layout = 12) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_0` | This is where the engine will provide the shader with the texture coordinate 0 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 13) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_1` | This is where the engine will provide the shader with the texture coordinate 1 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 14) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_2` | This is where the engine will provide the shader with the texture coordinate 2 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 15) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_3` | This is where the engine will provide the shader with the texture coordinate 3 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 16) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_4` | This is where the engine will provide the shader with the texture coordinate 4 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 17) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_5` | This is where the engine will provide the shader with the texture coordinate 5 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 18) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_6` | This is where the engine will provide the shader with the texture coordinate 6 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 19) in`      | `XENON_VERTEX_INPUT_TEXTURE_COORD_7` | This is where the engine will provide the shader with the texture coordinate 7 data loaded by the asset file, or given in by the user.                                                                                            |
| `location(layout = 20) in int`  | `XENON_INSTANCE_INPUT_INSTANCE_ID`   | This is where the engine will provide the shader with the instance ID. The instance ID is generated internally by the engine for each instance.                                                                                   |
| `location(layout = 21) in vec3` | `XENON_INSTANCE_INPUT_POSITION`      | This is where the engine will provide the shader with the instance position data. This is submitted to the engine when instancing a model. Note that the only accepted data type is `vec3` and the element type will be `float`.  |
| `location(layout = 22) in vec3` | `XENON_INSTANCE_INPUT_ROTATION`      | This is where the engine will provide the shader with the instance rotation data. This is submitted to the engine when instancing a model. Note that the only accepted data type is `vec3` and the element type will be `float`.  |
| `location(layout = 23) in vec3` | `XENON_INSTANCE_INPUT_SCALE`         | This is where the engine will provide the shader with the instance scale data. This is submitted to the engine when instancing a model. Note that the only accepted data type is `vec3` and the element type will be `float`.     |

### Fragment Shader

The fragment shader inputs don't have any restrictions or conventions that the user needs to follow. However, there is a strict set of rules which needs to be followed for fragment shader outputs.
| SPIR-V GLSL                      | Output type   | Description                                                                                                        |
|----------------------------------|---------------|--------------------------------------------------------------------------------------------------------------------|
| `layout(location = 0) out vec4`  | Color         | This fragment shader output is used for the color buffer.                                                          |
| `layout(location = 1) out float` | Mouse Picking | This fragment shader output is used for the ID buffer. This ID buffer is then used for mouse picking.              |
| `layout(location = 2) out vec3`  | Normal        | This fragment shader output is used for the normal buffer. This image contains the normal value for a given pixel. |

### Compute shaders

For compute shaders, there are no multiple descriptors set support. We only support the user-defined descriptor set type and it is enforced by the backend.
