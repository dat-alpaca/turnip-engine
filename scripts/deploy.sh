set -e

mkdir -p deploy

cp -r res/ deploy/
cp build/gl/turnip-sandbox/turnip-sandbox deploy/turnip-sandbox-gl
cp build/vk/turnip-sandbox/turnip-sandbox deploy/turnip-sandbox-vk