# FileSystem
OS Project is on FAT FileSystem.

File attributes are:

      startBlock

      fileName


Functions implemented are:

      create

      delete

      open

      close

      read

      write

      rename

      list of file


Structure of file system is:

      super block   :   address of directory and fat table and start of data blocks

      directory     :	file names, and its attributes

      fat table

      data blocks


Size of the virtual disk and its partions:

      Total size = 21504 Kbytes = 21 mb

      Block Size = 2048 bytes

      number of blocks = 10752

      superblock = 0th blocks

      fat table = 1-20 blocks

      directory = 30 - 511

Maximum 32 files can be opened at a given time.
