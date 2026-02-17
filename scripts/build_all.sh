set -e


# OpenGL:
(
    cmake --build --preset turnip-gl-build
) &

(
    cmake --build --preset turnip-vk-build
) &

wait

echo "Build finished"