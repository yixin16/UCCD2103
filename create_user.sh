#!/bin/bash

# create user
sudo useradd -m -s /bin/bash $1

# create manual and backup dir in user's home dir
sudo mkdir -p /home/$1/manual
sudo mkdir -p /home/$1/backup

# copy readme.txt to the user's manual dir
sudo cp /home/tanyixin/readme.txt /home/$1/manual

# set file ownership to the user
sudo chown -R $1:$1 /home/$1
sudo chown -R $1:$1 /home/$1/manual
sudo chown -R $1:$1 /home/$1/backup

# set the permission
sudo chmod 700 /home/$1
sudo chmod 600 /home/$1/manual
sudo chmod 600 /home/$1/backup
sudo chmod 600 /home/$1/backup/readme.txt

echo "User $1 is created successfully."