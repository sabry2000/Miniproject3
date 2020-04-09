#include <iostream>
#include <fstream>
using namespace std ;

typedef int int32; // 4-byte int
typedef char byte; // alias char as byte

class MyFileSystem{
private:
  byte free_block[128]; //if the i-th byte is 0, it indicates that the block is free, else it is in use

  /* Inode block : total size = 48 bytes*/
  struct Inode{
    string name;            // file name
    int32 size;               // file size (in number of blocks)
    int32 blockPointers[8]; // direct block pointers
    int32 used;               // 0 => inode is free; 1 => in use  
  };

  Inode inodes[16];

  friend ifstream& operator>>(std::ifstream&, MyFileSystem::Inode&);
  friend ofstream& operator<<(std::ofstream&, MyFileSystem::Inode&);

public:
  //the file system is named disk0
  string DISK_NAME;

  MyFileSystem(string diskName){
   // Open the file with name diskName
   DISK_NAME = diskName;
   
   // Read the first 1KB and parse it to structs/objecs representing the super block
   // 	An easy way to work with the 1KB memory chunk is to move a pointer to a
   //	position where a struct/object begins. You can use the sizeof operator to help
   //	cleanly determine the position. Next, cast the pointer to a pointer of the
   //	struct/object type.

   ifstream diskReader(DISK_NAME);

    // read free-block list
    diskReader.read(free_block, 128);  
    // read inodes
    for(int i = 0; i < 16; i++)
      diskReader >> inodes[i];

   // Be sure to close the file in a destructor or otherwise before
   // the process exits.
    diskReader.close();
  }

  int createFile(char name[8], int32 size){
    //create a file with this name and this size
    string filename(name);
    // high level pseudo code for creating a new file
    if(size < 1 || size > 8){
      cout << "Wrong size\n";
      return 0;
    }

    // Step 1: Look for a free inode by searching the collection of objects
    // representing inodes within the super block object.
    // If none exist, then return an error.
    // Also make sure that no other file in use with the same name exists.
    int chosenInode = 16;
    for(int i=0;i<16; i++){
      if(inodes[i].used == 1 && filename == inodes[i].name){
          cout << filename << " already exists\n";
          return 0;
      }else if (inodes[i].used == 0){
        chosenInode = i;
        break;
      }
    if(chosenInode == 16){
      cout << "No space left on disk\n";
      return 0;
    }
    }

    // Step 2: Look for a number of free blocks equal to the size variable
    // passed to this method. If not enough free blocks exist, then return an error.

    // Step 3: Now we know we have an inode and free blocks necessary to
    // create the file. So mark the inode and blocks as used and update the rest of
    // the information in the inode.
    inodes[chosenInode].name = filename;
    inodes[chosenInode].size = size;
    inodes[chosenInode].used = 1;
    for(int i = 0; i < size; i++)
      inodes[chosenInode].blockPointers[i] = (chosenInode+1)*1024 + i*127;

    // Step 4: Write the entire super block back to disk.
    //	An easy way to do this is to seek to the beginning of the disk
    //	and write the 1KB memory chunk.
    ofstream diskWriter (DISK_NAME , ofstream::binary | ofstream::app);
    diskWriter.seekp(128 + 48*(chosenInode));
    diskWriter << inodes[chosenInode];
    diskWriter.close();
    return 1;
  }; // End Create

  int deleteFile (char name[8]){
    // Delete the file with this name
    string filename(name);

    // Step 1: Look for an inode that is in use with given name
    // by searching the collection of objects
    // representing inodes within the super block object.
    // If it does not exist, then return an error.
    int chosenInode = 16;
    for(int i=0;i<16; i++){
      if(filename == inodes[i].name){
        chosenInode = i;
      }
    }
    if(chosenInode == 16){
      cout << filename << " : File not found\n";
      return 0;
    }

    // Step 2: Free blocks of the file being deleted by updating
    // the object representing the free block list in the super block object.

    // Step 3: Mark inode as free.
    inodes[chosenInode].used = 0;

    // Step 4: Write the entire super block back to disk.
    ofstream diskWriter (DISK_NAME , ofstream::binary | ofstream::app);
    diskWriter.seekp(128 + 48*(chosenInode));
    diskWriter << inodes[chosenInode];
    diskWriter.close();
    return 1;
  }; // End Delete

  int ls(void){
    // List names of all files on disk
    // Step 1: Print the name and size fields of all used inodes.
    cout << "Listing all files in " << DISK_NAME  << endl;
    for (int i = 0; i < 16; i++)
    {
      if (inodes[i].used == 1)
        cout << inodes[i].name << endl;
    }
    return 1;
  }; // End ls

  int read(char name[8], int32 blockNum, char buf[1024]){
    // Return an error if and when appropriate. For instance, make sure
    // blockNum does not exceed size-1.
    if(blockNum < 0 || blockNum > 7){
      cout << "Wrong block number\n";
      return 0;
    }
    string filename(name);
    // Step 1: Locate the inode for this file as in Step 1 of delete.
    for(int i=0;i<16; i++){
      if(filename == inodes[i].name){
        if(inodes[i].size < blockNum + 1){
          cout << "Wrong block number\n";
          return 0;
        }
        // read this block from this file
        // Step 2: Seek to blockPointers[blockNum] and read the block
        // from disk to buf.         
        cout << "Copying block " << blockNum << " from " << filename << " to " << DISK_NAME<< endl;
        
        ifstream diskReader(DISK_NAME, ifstream::binary);
        diskReader.seekg(inodes[i].blockPointers[blockNum], diskReader.beg);
        diskReader.read(buf, 1024);
        diskReader.close();  
        return 1;
      }
    }     
    cout << filename << " : File not found\n";
    return 0;
  }; // End read

  int write(char name[8], int32 blockNum, char buf[1024]){
    // write this block to this file
    // Return an error if and when appropriate.
    if(blockNum < 0 || blockNum > 7){
      cout << "Wrong block number\n";
      return 0;
    }
    // Step 1: Locate the inode for this file as in Step 1 of delete.
    string filename(name);
    for(int i=0; i<16; i++){
      if(filename == inodes[i].name){
        if(inodes[i].size < blockNum + 1){
          cout << "Wrong block number\n";
          return 0;
        }
        // Step 2: Seek to blockPointers[blockNum] and write buf to disk.
        cout << "Writing block " << blockNum << " from " << filename << " to " << DISK_NAME << endl;
        ofstream diskWriter(DISK_NAME, ofstream::binary | ofstream::app);
        diskWriter.seekp(inodes[i].blockPointers[blockNum], diskWriter.beg);
        diskWriter.write(buf, 1024);
        diskWriter.close();          
        return 1;
      }
    }
    cout << filename << " : File not found\n";
    return 0;  
  };// end write

  //terminate using the filesystem
  void terminate(){
    ofstream diskWriter(DISK_NAME);
    
    // write free-block list
    diskWriter.write(free_block, 128);
    // write inodes
    for(int i = 0; i < 16; i++)
        diskWriter << inodes[i];
    
    diskWriter.close();
  };
};

ifstream& operator >> (std::ifstream& in, MyFileSystem::Inode& dest)
{
   char name[9];
   in.read(name, 8);
   name[8] = 0;
   dest.name = string(name);
   in >> dest.size;
   for(int i=0;i<8;i++)
       in >> dest.blockPointers[i];
   in >> dest.used;
}

ofstream& operator << (std::ofstream& out, MyFileSystem::Inode& src)
{
   out.write(src.name.c_str(), 8);
   out << src.size;
   for(int i=0;i<8;i++)
       out << src.blockPointers[i];
   out << src.used;
}

int main(int argc, char* argv[]){
  if (argc < 2){
    cout << "Program expect an input file as its argument" << endl;
    return 1;
  }

  string filename = argv[1];
  ifstream filereader(filename);                                              //start reading the file
  
  if (!filereader.is_open()){
    cout << filename << " : file not found " << endl;
    return 1;
  }

  string diskname;
  filereader >> diskname;
  MyFileSystem fs(diskname);                                   //create a filesystem with the following diskname

  //possible arguments from input file
  string command;
  string fname;
  int blockNum;
  char buf[1024];
  
  cout << "Beginning operations on file system " << endl;
  while(! filereader.eof()){
    //read the first string, i.e. command
    filereader >> command;
  
    if(command == "L"){fs.ls();continue;}
    
    //if it is not L, then there is another argument
    filereader >> fname;
  
    if(command == "D"){fs.deleteFile(const_cast<char*>(fname.c_str()));continue;}
  
    //if it is not D, then there is another argument
    filereader >> blockNum;
  
    if(command == "C"){fs.createFile(const_cast<char*>(fname.c_str()), blockNum);}
    else if(command == "W"){fs.write(const_cast<char*>(fname.c_str()), blockNum, buf);}
    else if(command == "R"){fs.read(const_cast<char*>(fname.c_str()), blockNum, buf);}
  }
  cout << "Completed operations on file system " << endl;
  filereader.close();
  fs.terminate();
  return 0;
}