set -e


# OpenGL:
(
    cmake --preset turnip-gl
    cmake --build --preset turnip-gl-build
) &

(
    cmake --preset turnip-vk
    cmake --build --preset turnip-vk-build
) &

wait

echo "Build finished"