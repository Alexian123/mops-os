#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <os_image>"
    exit 1
fi

OS_IMAGE=$1
MOUNT_POINT=/mnt/temp
TEMP_FILE=./.dummy_file.txt

# Create dummy file if it doesn't exist
if ! [ -f $TEMP_FILE ]; then
    touch $TEMP_FILE
    echo "hello from file" > $TEMP_FILE
fi

# Create mount dir
if ! [ -d $MOUNT_POINT ]; then 
    sudo mkdir -p $MOUNT_POINT
fi

# Mount fs
sudo mount -t vfat $OS_IMAGE $MOUNT_POINT

# Move dummy file
sudo cp $TEMP_FILE "$MOUNT_POINT/hello.txt"

# Unmount fs
sudo umount $MOUNT_POINT