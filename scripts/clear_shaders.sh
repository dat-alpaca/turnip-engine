shader_dir=./res/shaders

for directory in "$shader_dir"/*
do
    for file in "$directory"/*
    do
        extension="${file##*.}"
        if [[ $extension == "spv" ]]
        then
            rm $file
        fi
    done
done