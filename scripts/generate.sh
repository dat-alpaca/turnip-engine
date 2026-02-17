set -e

# OpenGL:
(
    cmake --preset turnip-gl
) &

(
    cmake --preset turnip-vk
) &

wait

echo "Generating finished"