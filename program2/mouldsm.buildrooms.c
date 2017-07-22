/*Max Moulds */
#include "header.h"


bool IsGraphFull();
void AddRandomConnection();
bool IsSameRoom(Room x, Room y); 
Room GetRandomRoom();
bool CanAddConnectionFrom(Room x); 
void ConnectRoom(Room x, Room y); 

int main() {
  //first init all rooms?
  Room rooms[MAX_CONNECTED_ROOMS];
  for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
    //INIT_ROOM(rooms[i]);
  }
  // Create all connections in graph
  while (IsGraphFull() == false)
  {
    AddRandomConnection();
  }
  return 0;
}
// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()  
{
  return false;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()  
{
  Room A;  //Maybe a struct, maybe global arrays of ints
  Room B;

  while(true)
  {
    A = GetRandomRoom();

    if (CanAddConnectionFrom(A) == true)
      break;
  }

  do
  {
    B = GetRandomRoom();
  }
  while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true);

  ConnectRoom(A, B);
  ConnectRoom(B, A);
}

// Returns a random Room, does NOT validate if connection can be added
Room GetRandomRoom()
{
}

// Returns true if a connection can be added from Room x, false otherwise
bool CanAddConnectionFrom(Room x) 
{
  return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(Room x, Room y) 
{

}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(Room x, Room y) 
{

}
