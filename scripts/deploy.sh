set -e

mkdir -p deploy
mkdir -p deploy/turnip-vk
mkdir -p deploy/turnip-gl

cp -r res/ deploy/turnip-gl
cp -r res/ deploy/turnip-vk
cp build/turnip-gl/turnip-sandbox/turnip-sandbox deploy/turnip-gl/turnip-sandbox-gl
cp build/turnip-vk/turnip-sandbox/turnip-sandbox deploy/turnip-vk/turnip-sandbox-vk