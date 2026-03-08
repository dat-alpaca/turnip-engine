set -e


# OpenGL:
(
    cmake --build --preset turnip-gl-build
) #&

# Vulkan:
#(
#    cmake --build --preset turnip-vk-build
#) &

wait

echo "Build finished"