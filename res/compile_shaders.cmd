%VULKAN_SDK%\Bin\glslc.exe ./shaders/quad/quad_vk.vert -o ./shaders/quad/quad_vk_vert.spv
%VULKAN_SDK%\Bin\glslc.exe ./shaders/quad/quad_vk.frag -o ./shaders/quad/quad_vk_frag.spv

%VULKAN_SDK%\Bin\glslc.exe ./shaders/instanced/iquad_vk.vert -o ./shaders/instanced/iquad_vk_vert.spv
%VULKAN_SDK%\Bin\glslc.exe ./shaders/instanced/iquad_vk.frag -o ./shaders/instanced/iquad_vk_frag.spv

%VULKAN_SDK%\Bin\glslc.exe ./shaders/basic/basic_vk.vert -o ./shaders/basic/basic_vert.spv
%VULKAN_SDK%\Bin\glslc.exe ./shaders/basic/basic_vk.frag -o ./shaders/basic/basic_frag.spv
