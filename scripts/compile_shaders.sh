shader_dir=./res/shaders

for directory in "$shader_dir"/*
do
    for file in "$directory"/*
    do
        extension="${file##*.}"

        if [[ $extension == "vert" || $extension == "frag" ]]; then
            directory=$(dirname -- "$file")
            basename=$(basename -- $file)
            filename="${basename%.*}"

            glslc -O $file -o $directory/"$filename"_$extension.spv
        fi
    done
done