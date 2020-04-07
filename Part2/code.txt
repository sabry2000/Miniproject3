#include <iostream>
#include <fstream>
using namespace std;

typedef int int32; // 4-byte int
typedef char byte; // alias char as byte

class myFileSystem{
  //data structures

  //This is the super block class, i.e. the first 1KB block
  private class SuperBlock{

    //Inode block : total size = 48 bytes
    class Inode{
      char name[8];            // file name
      int32 size;               // file size (in number of blocks)
      int32 blockPointers[8]; // direct block pointers
      int32 used;               // 0 => inode is free; 1 => in use  

      byte data[48];

      //initialize the index node
      void initialize(){
        for(int i<0; i<8; i++){
          name[i] = data[i]
        }
        for(int i=8; i<12; i++){

        }
        for(int i = 12; i<44; i++){

        }
        for(int i =44; i<48; i++){

        }
      }
    };

    byte free_block_list[128];    //if the i-th byte is 0, it indicates that the block is free, else it is in use
    Inode inodes[16];             //16 index nodes

    //method to intialize the superblock
    public SuperBlock(byte firstKB[1000]){

      int i,j,k;
      //initialize free block list
      for(i = 0; i<128; i++){
        free_block_list[i] = firstKB[i]
      }
      //partition 768 bytes to 16 groups of 48 bytes
      for(j=0;j<16;j++){
        for (k=0; k<48; k++){
          inodes[j].data[k] = firstKB[i + (j * k)]
        }
      }
      //initialize the 16 index nodes
      for(j=0; j<16;j++){
        inodes[j].initialize()
      }
    }
  };
  string DISK_NAME;            //the file system is named disk0
  SuperBlock *sb;

  public myFileSystem(string diskName){
    DISK_NAME = diskName;
    // Read the first 1KB and parse it to structs/objecs representing the super block
    // 	An easy way to work with the 1KB memory chunk is to move a pointer to a
    //	position where a struct/object begins. You can use the sizeof operator to help
    //	cleanly determine the position. Next, cast the pointer to a pointer of the
    //	struct/object type.

    byte buffer[1000];
    ifstream diskreader(DISK_NAME);
    diskreader.seekg(0, ios.end());
    size_t length = diskreader.tellg();
    diskreader.seekg(0, ios::beg());

    if (length > sizeof(buffer)){
      length = sizeof(buffer);
    }

    diskreader.read(buffer, length);

    SuperBlock temp(buffer);                //initalize the superblock
    sb=&temp;

    // Be sure to close the file in a destructor or otherwise before
    // the process exits.
    }

  public int create(char name[8], int32 size)
    { //create a file with this name and this size

      // high level pseudo code for creating a new file

      // Step 1: Look for a free inode by searching the collection of objects
      // representing inodes within the super block object.
      // If none exist, then return an error.
      // Also make sure that no other file in use with the same name exists.

      // Step 2: Look for a number of free blocks equal to the size variable
      // passed to this method. If not enough free blocks exist, then return an error.

      // Step 3: Now we know we have an inode and free blocks necessary to
      // create the file. So mark the inode and blocks as used and update the rest of
      // the information in the inode.

      // Step 4: Write the entire super block back to disk.
      //	An easy way to do this is to seek to the beginning of the disk
      //	and write the 1KB memory chunk.
    } // End Create

  public int delete (char name[8]){
      // Delete the file with this name
      string filename(name);

      // Step 1: Look for an inode that is in use with given name
      // by searching the collection of objects
      // representing inodes within the super block object.
      // If it does not exist, then return an error.

      // Step 2: Free blocks of the file being deleted by updating
      // the object representing the free block list in the super block object.

      // Step 3: Mark inode as free.

      // Step 4: Write the entire super block back to disk.

    } // End Delete

  public int ls(void){
    // List names of all files on disk
    // Step 1: Print the name and size fields of all used inodes.
    for (int i = 0; i < 16; i++)
    {
      if (sb.inodes[i].used == 1)
        cout << sb.inodes[i].name << endl;
    }

  } // End ls

  public int read(char name[8], int32 blockNum, char buf[1024]){
    // Return an error if and when appropriate. For instance, make sure
    // blockNum does not exceed size-1.
    if(blockNum < 0 || blockNum > 7){
      cout << "Wrong block number\n";
      exit(1);
    }
    string filename(name);
    // Step 1: Locate the inode for this file as in Step 1 of delete.
    for(int i=0;i<16; i++){
      if(filename == sb.inodes[i].name){
        if(sb.inodes[i].size < blockNum + 1){
          cout << "Wrong block number\n";
          exit(1);
        }
        // read this block from this file
        // Step 2: Seek to blockPointers[blockNum] and read the block
        // from disk to buf.         
        ReadBlock(sb.inodes[i].blockPointers[blockNum], buf);          
        return 1;
      }
    }     
    cout << filename << " : File not found\n";
    exit(1);

  } // End read

  public int write(char name[8], int32 blockNum, char buf[1024]){
    // write this block to this file
    // Return an error if and when appropriate.
    if(blockNum < 0 || blockNum > 7){
      cout << "Wrong block number\n";
      exit(1);
    }
    // Step 1: Locate the inode for this file as in Step 1 of delete.
    string filename(name);
    for(int i=0; i<16; i++){
      if(filename == sb.inodes[i].name){
        if(sb.inodes[i].size < blockNum + 1){
          cout << "Wrong block number\n";
          exit(1);
        }
        // Step 2: Seek to blockPointers[blockNum] and write buf to disk.
        writeBlock(sb.inodes[i].blockPointers[blockNum], buf);          
        return 1;
      }
    }
    cout << filename << " : File not found\n";
    exit(1);  
  }// end write
  
  // public methods that are used when initializing and terminating the program
  
  //intitialize the filesystem
  public void initialize(){
    ifstream diskReader(DISK_NAME);

    // read free-block list
    diskReader.read(sb.free_block, 128);  
    // read inodes
    for(int i = 0; i < 16; i++)
      diskReader >> sb.inodes[i];

    diskReader.close();
  }
  
  //terminate using the filesystem
  public void terminate(){
   ofstream diskWriter(DISK_NAME);
  
   // write free-block list
   diskWriter.write(sb.free_block, 128);
   // write inodes
   for(int i = 0; i < 16; i++)
      diskWriter << sb.inodes[i];
  
   diskWriter.close();
  }

  //private methods used internally
  
  //private method to commit to the disk
  private void commitToDisk(int inode){
    ofstream diskWriter (DISK_NAME , ofstream::binary | ofstream::app);
    diskWriter.seekp(128 + 48*(inode));
    diskWriter << sb.inodes[inode];
    diskWriter.close();
  }

  //helper method to read a block
  private void readBlock(int32 blockPtr, char *buf){
  ifstream diskReader(DISK_NAME, ifstream::binary);
  diskReader.seekg(blockPtr, diskReader.beg);
  diskReader.read(buf, 1024);
  diskReader.close();          
  }

  //helper method to write a block
  private void writeBlock(int32 blockPtr, const char *buf){
    ofstream diskWriter(DISK_NAME, ofstream::binary | ofstream::app);
    diskWriter.seekp(blockPtr, diskWriter.beg);
    diskWriter.write(buf, 1024);
    diskWriter.close();
  }

};

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
  myFileSystem filesystem(diskname);                                   //create a filesystem with the following diskname

  filesystem.initialize()

  //possible arguments from input file
  string command;
  string filename;
  int blockNum;
  char buf[1024];
  
  cout << "Beginning operations on file system " << endl;
  while(! filereader.eof()){
    //read the first string, i.e. command
    filereader >> command;
  
    if(command == "L"){filesystem.ls();continue;}
    
    //if it is not L, then there is another argument
    filereader >> filename;
  
    if(command == "D"){filesystem.delete(filename.c_str());continue;}
  
    //if it is not D, then there is another argument
    filereader >> blockNum;
  
    if(command == "C"){filesystem.create(filename.c_str(), blockNum);}
    else if(command == "W"){filesystem.write(filename.c_str(), blockNum, buf);}
    else if(command == "R"){filesystem.read(filename.c_str(), blockNum, buf);}
  }
  cout << "Completed operations on file system " << endl;
  filereader.close();
  filesystem.terminate();
  return 0;
}
