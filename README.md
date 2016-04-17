# FileSystem
For OS Project. 

FAT FileSystem

File attributes

startBlock

fileName


Functions implemented

create

delete

open

close

read

write

rename

list of file


Structure of file system

super block 	====			address of directory and fat table and start of data blocks

directory		====			file names, and its attributes

fat table

data blocks


Total size = 21504 Kbytes = 21 mb

Block Size = 2048 bytes

number of blocks = 10752

superblock = 0th blocks

fat table = 1-16 blocks

directory = 17 - 511

Maximum 32 files can be opened at a given time.