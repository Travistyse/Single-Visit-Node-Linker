#include "PathingLibrary.h"

#include "stdafx.h"
#include <math.h>
#include <utility>
#include <vector>
#include <algorithm>
#include <set>
#include <iterator>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <tuple>
#include <functional>
#include <map>
#include <iostream>
#include <string>



namespace SVNL
{

enum class PATH_REVISION {
  Disabled,
  ReviseOnce, // Revise once before submitting the path
  ReviseTwice // Revise once before submitting the path and revise again using another path if still below acceptable levels
};

/*
dir2/foo2.h.
A blank line
C system files.
C++ system files.
A blank line
Other libraries' .h files.
Your project's .h files.
*/


/*
Alright, so the goal is to make this usable and as generic as possible so that it can solve more problems.
The library takes numbers in and sends numbers out
Functions:
Create containers by which to limit the connections (Actually, let them do that and pass it in)
^ So effectively the end user will input a list of every "Beast" and I'll return the path
Create Groups of connections
Create DirectConnections Trees
Return a path from a group of connections

The end goal is to create a path so there doesn't need to be too much modularity involved
I won't be including OpenCV or anything else so I can't assume they'll be available

Currently:
Want to have X, Y accepted as Double / Float / Integer , 2D Array / Vector of Pairs

So after I've determined the ranges there's actually no longer any need to use those X, Y values
so I can ignore them. Meaning I only need to have 3 versions of DirectConnections buuut it looks
like I only really have a single thing this is performing so.. the only thing for the user to
call is "CreatePath" or w/e

01234567890123456789012345678901234567890123456789012345678901234567890123456789 <- That's the 80 column width
*/


double PointDistance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double PointDistance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double PointDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool IsInRange(std::pair<int, int> A, std::pair<int, int> B, float MaxDistance) {
  if (PointDistance(A.first, A.second, B.first, B.second) <= MaxDistance) {
    return true;
  }
  return false;
}

bool IsInRange(std::pair<float, float> A, std::pair<float, float> B, float MaxDistance) {
  if (PointDistance(A.first, A.second, B.first, B.second) <= MaxDistance) {
    return true;
  }
  return false;
}

bool IsInRange(std::pair<double, double> A, std::pair<double, double> B, float MaxDistance) {
  if (PointDistance(A.first, A.second, B.first, B.second) <= MaxDistance) {
    return true;
  }
  return false;
}

bool FindInSortedVector(const std::vector<int> & Vec, int Elem) {

	if (std::binary_search(Vec.begin(), Vec.end(), Elem)) {
		return true;
	}

	return false;
}

bool FindInUnsortedVector(const std::vector<int> & Vec, int Elem) {

	if (std::find(Vec.begin(), Vec.end(), Elem) != Vec.end()) {
		return true;
	}
	return false;
}

bool FindInUnsortedVector(const std::vector<int> & Vec, int Elem, int VecSize) {

  // So Vec.end is 18 or w/e
  // VecSize is like 15 or w/e
  // So It'll be Vec.Begin() + 15 or w/e 

  //Check if this Tsum is in the removed pile
  if (std::find(Vec.begin(), Vec.begin() + VecSize, Elem) != Vec.begin() + VecSize) {
    return true;
  }
  return false;
}

int FindInUnsortedVectorReturnIndex(const std::vector<int> & Vec, int Elem) { //Returns -1 if unsuccessful

  auto Iterator = std::find(Vec.begin(), Vec.end(), Elem);

  //Check if this Tsum is in the removed pile
  if (Iterator != Vec.end()) {
    return std::distance(Vec.begin(), Iterator);
  }
  return -1;
}

void CreateNodeDirectConnections(const std::vector<std::pair<int, int>> & Nodes, 
	std::vector<std::vector<int>> & NodeDirectConnections, float MaxDistance) {

	int NumNodes = Nodes.size();

	// For every Node (i is the index of Node "A" ) (Skipping last Node)
	for (int i = 0; i < (NumNodes - 1); i++) {
		
		// Check if any remaining Nodes are directly connected
		for (int i2 = i + 1; i2 < NumNodes; i2++) {
			

			if (IsInRange(Nodes[i], Nodes[i2], MaxDistance)) {
				
				NodeDirectConnections[i].emplace_back(i2); //Add B to A
				NodeDirectConnections[i2].emplace_back(i); //Add A to B
			}
		}
	}
}

void CreateGroups(const std::vector<std::vector<int>> & NodeDirectConnections,
  std::vector<std::vector<int>> & Paths, int MinimumGroupSize, 
  std::vector<std::vector<int>> & NodeGroups) {

	//Takes the HostChildConnections list and uses it to make Groups, uses those to make paths. Paths = all that's needed.

	std::set<int, std::greater <> > RemovedNodes;
	std::set<int, std::greater <> > NodesToCheck;

	std::vector<int> CurrentNodeGroup;

	int NDC_Size = NodeDirectConnections.size();

	CurrentNodeGroup.reserve(NDC_Size);


	for (int i = 0; i < NDC_Size; i++) {
		//If the Host has at least 1 child..
		if (!NodeDirectConnections[i].empty()) {
			//i Will be representing whichever "NodeDirectConnections" we're starting with
			auto it = RemovedNodes.find(i); //std::set< int >::iterator
			if (it == RemovedNodes.end()) {
				// Not removed!
				//Assign the "host" number (the one the other numbers are in range of) to 0 and then remove it from the pool
				RemovedNodes.insert(i); //0 Can't be added to any other Group

				CurrentNodeGroup.emplace_back(i);
				for (int ChildNodeBeingChecked : NodeDirectConnections[i]) {
					//i2 Will be representing the numbers held by the host - This is the "11" the first loop and "10" the next

					//Check if removed
					auto it2 = RemovedNodes.find(ChildNodeBeingChecked);
					if (it2 == RemovedNodes.end()) {
						CurrentNodeGroup.emplace_back(ChildNodeBeingChecked); //NodeGroups[0][0] = NodeDirectConnections[0][0] = 11 | NodeGroups[0][1] = NodeDirectConnections[0][1] = 10
						RemovedNodes.insert(ChildNodeBeingChecked); //Add 11 and 10 to the list of removed and list of Nodes to check
						NodesToCheck.insert(ChildNodeBeingChecked);
					}
				}

				auto it3 = NodesToCheck.begin();
				while (it3 != NodesToCheck.end()) {

					int HostNodeBeingChecked = *it3;
					it3 = NodesToCheck.erase(it3); // This erases it3 without messing up the iterator

					for (int ChildNodeBeingChecked : NodeDirectConnections[HostNodeBeingChecked]) { //For each of the Host's (ie 10) children (ie 8, 11, and 0) [10][0] = 8, [10][1] [10][2]
																							   //The 8
						auto it4 = RemovedNodes.find(ChildNodeBeingChecked);
						if (it4 == RemovedNodes.end()) {
							//If not removed, add it to removed and the current Group
							CurrentNodeGroup.emplace_back(ChildNodeBeingChecked); //NodeGroups[0][0] = NodeDirectConnections[0][0] = 11 | NodeGroups[0][1] = NodeDirectConnections[0][1] = 10
							RemovedNodes.insert(ChildNodeBeingChecked); //Add 8 to the list of removed and list of Nodes to check
							NodesToCheck.insert(ChildNodeBeingChecked);
						}
					}

					it3 = NodesToCheck.begin(); //This starts back at the beginning (Which will also be the end if nothing is there)

				}



				//Alright, so this is the end of this group. Let's check the size and see if it can be added before iterating
				if (CurrentNodeGroup.size() > MinimumGroupSize)
				{
					NodeGroups.emplace_back(CurrentNodeGroup);
				}

				CurrentNodeGroup.clear();
				NodesToCheck.clear();
			}
		}
	}
}

int ContinueNodePathCrawl(std::vector<int> & LongestPath, std::vector<int> & CurrentPath, const std::vector<int> & FirstPath,
  const std::vector<std::vector<int>> & NodeDirectConnections, int MaxPaths, int & PathsTested, int CurrentPathSize, int & LongestPathSize, int MaxPathSize) {
  // Blacklist Version
  int CurrentNode = CurrentPath[CurrentPathSize - 1];
  int ReturnValue;

  if (PathsTested >= MaxPaths) {
    return 50; // Stop Path Crawling
  }

  for (int i = 0; i < NodeDirectConnections[CurrentNode].size(); i++) { //For every branching connection
    int NextNode = NodeDirectConnections[CurrentNode][i];
    if (!FindInUnsortedVector(CurrentPath, NextNode, CurrentPathSize)) {
      if (!FindInUnsortedVector(FirstPath, NextNode)) {
        CurrentPath[CurrentPathSize] = NextNode; //Set the next point to that connection
        ReturnValue = ContinueNodePathCrawl(LongestPath, CurrentPath, FirstPath, NodeDirectConnections, MaxPaths, PathsTested, CurrentPathSize + 1, LongestPathSize, MaxPathSize);
        if (ReturnValue != 0) {
          return (ReturnValue - 1);
        }
      }
    }
  }


  //No choices remain so add this to the list of paths and "Take a step back" (or stop PathFinding)

  PathsTested += 1;

  if (CurrentPathSize > LongestPathSize) {
    LongestPath = CurrentPath;
    LongestPathSize = CurrentPathSize;
    if (LongestPathSize >= MaxPathSize) {
      return 50;
    }
  }

  if (PathsTested >= MaxPaths) {
    return 50; // Stop Path Crawling
  }

  return 0;
}

int ContinueNodePathCrawl(std::vector<int> & LongestPath, std::vector<int> & CurrentPath,
  const std::vector<std::vector<int>> & NodeDirectConnections, int MaxPaths, int & PathsTested, int CurrentPathSize, int & LongestPathSize, int MaxPathSize) {

  int ReturnValue;
  int CurrentNode = CurrentPath[CurrentPathSize - 1];

  if (PathsTested >= MaxPaths) {
    return 50; // Stop Path Crawling
  }

  for (int i = 0; i < NodeDirectConnections[CurrentNode].size(); i++) { //For every branching connection
    int NextNode = NodeDirectConnections[CurrentNode][i];
    if (!FindInUnsortedVector(CurrentPath, NextNode, CurrentPathSize)) {
      CurrentPath[CurrentPathSize] = NextNode; //Set the next point to that connection
      ReturnValue = ContinueNodePathCrawl(LongestPath, CurrentPath, NodeDirectConnections, MaxPaths, PathsTested, CurrentPathSize + 1, LongestPathSize, MaxPathSize);
      if (ReturnValue != 0) {
        return (ReturnValue - 1);
      }
    }
  }


  //No choices remain so add this to the list of paths and "Take a step back" (or stop PathFinding)

  PathsTested += 1;

  if (CurrentPathSize > LongestPathSize) {
    LongestPath = CurrentPath;
    LongestPathSize = CurrentPathSize;
    if (LongestPathSize >= MaxPathSize) {
      return 50;
    }
  }

  if (PathsTested >= MaxPaths) {
    return 50; // Stop Path Crawling
  }

  return 0;
}

int ContinueNodePathCrawl(std::vector<int> & LongestPath, int & PathsTested, std::vector<int> & CurrentPath,
  const std::vector<std::vector<int>> & NodeDirectConnections, const std::vector<int> & TWRGroup, int MaxPaths, int EndingPoint, int CurrentPathSize, int & LongestPathSize) {
  // Redirection Version (Beginning and End Points)
  int CurrentNode = CurrentPath[CurrentPathSize - 1];

  if (CurrentNode != EndingPoint) { // If this is the ending point don't continue - it's possible that this isn't a dead end but by design it makes no sense to continue

    int ReturnValue;
    bool CanSelectEndPoint = CurrentPathSize > TWRGroup.size() - 2; //Allow selecting the "end" point only for the last or second to last path (The latter option only in case of non-fillable NodeGroups)


    for (int i = 0; i < NodeDirectConnections[CurrentNode].size(); i++) { //For every branching connection
      int NextNode = NodeDirectConnections[CurrentNode][i];

      if (FindInUnsortedVector(TWRGroup, NextNode)) {
        if (CanSelectEndPoint || NextNode != EndingPoint) {
          if (!FindInUnsortedVector(CurrentPath, NextNode, CurrentPathSize)) {
            CurrentPath[CurrentPathSize] = NextNode; //Set the next point to that connection
            ReturnValue = ContinueNodePathCrawl(LongestPath, PathsTested, CurrentPath, NodeDirectConnections, TWRGroup, MaxPaths, EndingPoint, CurrentPathSize + 1, LongestPathSize);
            if (ReturnValue != 0) {
              return (ReturnValue - 1);
            }
          }
        }
      }
    }
  }

  //No choices remain so add this to the list of paths and "Take a step back" (or stop PathFinding)

  PathsTested += 1;

  if (CurrentPathSize > LongestPathSize) {
    LongestPath = CurrentPath;
    LongestPathSize = CurrentPathSize;
    if (LongestPathSize >= TWRGroup.size()) {
      return 50;
    }
  }

  if (PathsTested >= MaxPaths) {
    return 50; // Stop Path Crawling
  }

  return 0;
}


void BeginNodePathCrawl(std::vector<int> & LongestPath, int & PathsTested, int StartingPoint, int EndingPoint,
  const std::vector<std::vector<int>> & NodeDirectConnections, int MaxPaths, const std::vector<int> & TWRGroup, int & LongestPathSize) {
  // Redirection Version (Beginning and End Points)
  LongestPathSize = 0;
  std::vector<int> CurrentPath(50);
  CurrentPath[0] = StartingPoint;

  int MaxPathLength = TWRGroup.size();

  int NumBranches = NodeDirectConnections[StartingPoint].size();
  int ReturnValue;
  bool CanSelectEndPoint = MaxPathLength < 3;

  for (int i = 0; i < NumBranches; i++) {
    CurrentPath[1] = NodeDirectConnections[StartingPoint][i];

    if (CurrentPath[1] != EndingPoint || CanSelectEndPoint) {
      if (CurrentPath[1] != StartingPoint || CanSelectEndPoint) {
        if (FindInUnsortedVector(TWRGroup, CurrentPath[1]))
        {
          ReturnValue = ContinueNodePathCrawl(LongestPath, PathsTested, CurrentPath, NodeDirectConnections, TWRGroup, MaxPaths, EndingPoint, 2, LongestPathSize);
          if (ReturnValue != 0) {
            break;
          }
        }
      }
    }
  }

  LongestPath.resize(LongestPathSize);
}

void BeginNodePathCrawl(std::vector<int> & LongestPath, int StartingPoint, const std::vector<std::vector<int>> & NodeDirectConnections,
  const std::vector<int>& BlackList, int MaxPaths, int & PathsTested, int & LongestPathSize, int MaxPathSize) {
  //Same as the default one except that this one removes all of the connections from the blacklist as options (ignoring the first entry)
  LongestPathSize = 0;
  std::vector<int> CurrentPath(50);
  CurrentPath[0] = StartingPoint;


  int NumBranches = NodeDirectConnections[StartingPoint].size(); //This is the number of branches
  int NextNode;
  int ReturnValue;
  for (int i = 0; i < NumBranches; i++) {
    NextNode = NodeDirectConnections[StartingPoint][i];
    if (!FindInUnsortedVector(BlackList, NextNode)) {
      CurrentPath[1] = NextNode;
      ReturnValue = ContinueNodePathCrawl(LongestPath, CurrentPath, BlackList, NodeDirectConnections, MaxPaths, PathsTested, 2, LongestPathSize, MaxPathSize);
      if (ReturnValue != 0) {
        break;
      }
    }
  }

  LongestPath.resize(LongestPathSize);
}

void BeginNodePathCrawl(std::vector<int> & LongestPath, int StartingPoint, const std::vector<std::vector<int>> & NodeDirectConnections,
  int MaxPaths, int & PathsTested, int & LongestPathSize, int MaxPathSize) {

  std::vector<int> CurrentPath(50);
  CurrentPath[0] = StartingPoint;
  LongestPathSize = 0;

  //int MaxPathLength = TWRGroup.size(); I'll need to pass this in - Better than passing in an entire Group

  int NumBranches = NodeDirectConnections[StartingPoint].size(); //This is the number of branches
  int ReturnValue;

  for (int i = 0; i < NumBranches; i++) {
    CurrentPath[1] = NodeDirectConnections[StartingPoint][i];
    ReturnValue = ContinueNodePathCrawl(LongestPath, CurrentPath, NodeDirectConnections, MaxPaths, PathsTested, 2, LongestPathSize, MaxPathSize);
    if (ReturnValue != 0) {
      break;
    }
  }

  LongestPath.resize(LongestPathSize);
}

void CreateGroupsFromUnvisitedNodes(const std::vector<std::vector<int>> & NodeDirectConnections,
  const std::set<int, std::greater <> > & UnvisitedNodes,
  std::vector<std::vector<int>> & NodeGroups,
  std::vector<std::set<int, std::greater <> > > & PathNodesConnectedToGroup) {																																																													//This is meant for function use - not to be called in place of CreateNodeGroups ; It's basically the same thing except it doesn't call Paths

  std::vector<int> CurrentGroup;
  std::set<int, std::greater <> > RemovedNodes;
  std::set<int, std::greater <> > NodesToCheck;
  std::set<int, std::greater <> > PathNodes;

  CurrentGroup.reserve(UnvisitedNodes.size());
  NodeGroups.reserve(5);

  auto UnvisitedNodesIterator = UnvisitedNodes.begin();

  // With every Unvisited Node 
  while (UnvisitedNodesIterator != UnvisitedNodes.end()) {

    int Node = *UnvisitedNodesIterator;
    bool SkipNode = false;

    // Check every Group to see if this Node is a member, if so skip it
    for (const std::vector<int>& Group : NodeGroups) {
      if (FindInUnsortedVector(Group, Node)) {
        SkipNode = true;
        break;
      }
    }

    if (SkipNode) {
      UnvisitedNodesIterator++;
      continue;
    }


    auto it = RemovedNodes.find(Node);
    if (it == RemovedNodes.end()) {

      // Add Node to the current Group
      CurrentGroup.emplace_back(Node);
      RemovedNodes.insert(Node);

      // With every Node this Node is directly connected to
      for (int ConnectedNode : NodeDirectConnections[Node]) {


        auto it2 = RemovedNodes.find(ConnectedNode);

        if (it2 != RemovedNodes.end()) {
          continue;
        }

        RemovedNodes.insert(ConnectedNode);
        it2 = UnvisitedNodes.find(ConnectedNode);

        if (it2 != UnvisitedNodes.end()) {

          CurrentGroup.emplace_back(ConnectedNode);
          NodesToCheck.insert(ConnectedNode);
        }
        else {
          // Connected Node is in the path
          PathNodes.insert(ConnectedNode);
        }
      }

      auto it3 = NodesToCheck.begin();
      while (it3 != NodesToCheck.end()) {

        Node = *it3;
        it3 = NodesToCheck.erase(it3);  // This erases it3 without messing up the iterator

        for (int ConnectedNode : NodeDirectConnections[Node]) {
          auto it4 = RemovedNodes.find(ConnectedNode);
          if (it4 == RemovedNodes.end()) {
            continue;
          }

          // If the ConnectedNode is Unvisited and Group-less, Group and remove it
          RemovedNodes.insert(ConnectedNode);
          it4 = UnvisitedNodes.find(ConnectedNode);
          if (it4 != UnvisitedNodes.end()) {
            CurrentGroup.emplace_back(ConnectedNode);
            NodesToCheck.insert(ConnectedNode);
          }
          else {
            // Connected Node is in the path
            PathNodes.insert(ConnectedNode);
          }
        }

        it3 = NodesToCheck.begin(); //This starts back at the beginning (Which will also be the end if nothing is there)

      }

      // Store the current group, Connected Path Nodes, and move to the next Group
      if (!CurrentGroup.empty())
      {
        NodeGroups.emplace_back(CurrentGroup);
        CurrentGroup.clear();
      }

      if (!PathNodes.empty()) {
        PathNodesConnectedToGroup.emplace_back(PathNodes);
        PathNodes.clear();
      }

      RemovedNodes.clear();
      NodesToCheck.clear();
    }

    UnvisitedNodesIterator++;
  }
}

void RevisePath(std::vector<int> & Path,
  const std::vector<std::vector<int>> & NodeDirectConnections,
  const std::vector<int> & SortedGroup, std::vector<int> & LongestPath) {

  // Takes a Path, figures out which Nodes were left behind, and attempts
  // to work them back in without otherwise altering the path

  std::vector<int> SortedPath = Path;
  std::sort(SortedPath.begin(), SortedPath.end());

  std::vector<int> TempRemainingNodes;
  TempRemainingNodes.resize((SortedGroup.size() * 2));
  std::vector<int>::iterator VectorIterator;
  VectorIterator = std::set_difference(SortedGroup.begin(), SortedGroup.end(), SortedPath.begin(), SortedPath.end(), TempRemainingNodes.begin());
  TempRemainingNodes.resize(VectorIterator - TempRemainingNodes.begin());

  // Convert RemainingNodes to a set
  std::set<int, std::greater <> > RemainingNodes(std::make_move_iterator(TempRemainingNodes.begin()),
    std::make_move_iterator(TempRemainingNodes.end()));

  // Use that list to create Groups, no size limit
  std::vector<std::vector<int>> RemainingNodeGroups;
  std::vector<std::set<int, std::greater <> >> PathNodes;
  PathNodes.reserve(10);

  std::map<int, std::map<int, int>> RemToPath;
  std::map<int, std::map<int, int>> PathToRem;

  std::vector<int> PathRedirectionPairs; // I'm effectively taking two consecutive Nodes in the path (A to B) and sandwiching my new path into it (A C D E F B)
  PathRedirectionPairs.reserve(30);

  CreateGroupsFromUnvisitedNodes(NodeDirectConnections, RemainingNodes,
    RemainingNodeGroups, PathNodes); // RemToPath, PathToRem); //This creates an Group out of all of the remaining Nodes - has nothing to do with the paths


                                     //So now I have a list of the Path Nodes. 0, 5, 7, 8, 9, 10, 13, 16, 21, 25, 27, 33, 34 (Which SHOULD be sorted but safety first)
                                     // I want to go through all of them and create the R2P and P2R lists from the RemainingNodes container (Which is sorted)


  for (int PathGroupIterator = 0; PathGroupIterator < PathNodes.size(); PathGroupIterator++) { //For each Host Path connected to each Node Group
    auto it = PathNodes[PathGroupIterator].begin();
    while (it != PathNodes[PathGroupIterator].end()) { // Until done iterating through every number in the path that has a connection to a RemainingNode
      int NodePathIndex = *it; // Iterates through every number in the path
      for (int i2 = 0; i2 < NodeDirectConnections[NodePathIndex].size(); i2++) { //For each connection this Node (in the path) has
                                                                                 //Check if Path[PathNodes][Iteration] is in RemainingNodes *it
        auto it2 = RemainingNodes.find(NodeDirectConnections[NodePathIndex][i2]); //*it2 will be the index number that PathNode was found in
        if (it2 != RemainingNodes.end()) { //If this Child Node Connection is one of the Remaining Nodes
                                           //If so, network

                                           // *it is the Element ; it is the Index
          PathToRem[NodePathIndex][PathToRem[NodePathIndex].size()] = *it2;
          RemToPath[*it2][RemToPath[*it2].size()] = NodePathIndex;

        }
        else {
          // That means this isn't a remaining Node sooo check if it's a member of the path and if so add it and the host
          //I'm looking to create pairs - in this case, 8, 7 ; 7, 5 ; and 5, 0
          // 0: 8
          // 1: 7
          // 2: 7
          // 3: 5
          // 4: 5
          // 5: 0

          auto it3 = PathNodes[PathGroupIterator].find(NodeDirectConnections[NodePathIndex][i2]); // If this Host is connected to a Remaining Node
          if (it3 != PathNodes[PathGroupIterator].end()) {

            //Find Host, if 1 index later is the child add it Host, child. If 1 Index prior is the child, add it Child, Host
            int HostsPathIndex = FindInUnsortedVectorReturnIndex(Path, NodePathIndex);
            if (HostsPathIndex != -1) {

              //CHeck if HostPath +/- 1 is out of range and if not check for the Child and if found add the pairing
              bool WasChildFound = false;
              if (HostsPathIndex + 1 < Path.size()) {
                if (Path[HostsPathIndex + 1] == *it3) {
                  WasChildFound = true;
                  PathRedirectionPairs.emplace_back(NodePathIndex); //Add the host (Same nums..)
                  PathRedirectionPairs.emplace_back(*it3); //Add the child
                }
              }

              if (!WasChildFound) {
                if (HostsPathIndex > 0) {
                  if (Path[HostsPathIndex - 1] == *it3) {
                    PathRedirectionPairs.emplace_back(*it3); //Add the child
                    PathRedirectionPairs.emplace_back(NodePathIndex); //Add the host (Same nums..)
                  }
                }
              }
            }
          }
        }
      }
      it++;
    }

    //This is the entire data set for the path and Group

    //If there are any redirection pairs...
    if (!PathRedirectionPairs.empty()) {
      //Simple Shortcut for really small paths - Just Grab the surrounding path and insert this Node in the middle
      if (RemainingNodeGroups[PathGroupIterator].size() == 1) {
        int HostsPathIndex = FindInUnsortedVectorReturnIndex(Path, PathRedirectionPairs[0]);
        if (HostsPathIndex != -1) {
          Path.insert(Path.begin() + HostsPathIndex + 1, RemainingNodeGroups[PathGroupIterator][0]);
        }
      }
      else {
        //Pick the Starting / Ending Redirection Pairs based on combined Most Connections to the Remaining Node Group
        int MostConnectedIndex = 0; //Host aka 0, not 1 (So + 1 for the Child)
        int MostConnectedSize = 0;
        int CurrentPairSize;
        int PairStartSize = 0;
        int PairEndSize = 0;
        // Each Pair is 1 after the other. It's 0, 1 and 2, 3 and 4, 5 (So i += 2)
        for (int i2 = 0; i2 < PathRedirectionPairs.size(); i2 += 2) {
          PairStartSize = PathToRem[i2].size();
          PairEndSize = PathToRem[i2 + 1].size();

          if (PairStartSize == 0 || PairEndSize == 0) {
            continue;
          }

          // Need to replace this vvv with a much more elegant continue based version. No need for so much code repetition

          if (PairStartSize == 1 || PairEndSize == 1) {
            if (PairStartSize == 1 && PairEndSize == 1) {
              // These are both only connected to RTI by a single connection, check if that connection is the same
              if (PathToRem[i2] != PathToRem[i2 + 1]) {
                //Not the same so everything is fine
                CurrentPairSize = PairStartSize + PairEndSize;

                if (MostConnectedSize < CurrentPairSize) {
                  MostConnectedSize = CurrentPairSize;
                  MostConnectedIndex = i2;
                }
              }
            }
            else {
              // Only one of the pair have a single connection so everything is fine
              CurrentPairSize = PairStartSize + PairEndSize;

              if (MostConnectedSize < CurrentPairSize) {
                MostConnectedSize = CurrentPairSize;
                MostConnectedIndex = i2;
              }
            }
          }
          else {
            // Both have 2 or more so no need to worry about it
            CurrentPairSize = PairStartSize + PairEndSize;

            if (MostConnectedSize < CurrentPairSize) {
              MostConnectedSize = CurrentPairSize;
              MostConnectedIndex = i2;
            }
          }
        }

        int StartingRedP = PathRedirectionPairs[MostConnectedIndex];
        int EndingRedP = PathRedirectionPairs[MostConnectedIndex + 1];
        int StartingRemNode;
        int EndingRemNode;

        int LeastConnectedNode = 0;
        int LeastConnectedSize = 100;
        int CurrentRemSize = 100;

        // Choose the start and end points based on fewest connections to other RemainingNode (And of course access to the respective start / end Redirection Point)
        //For each Remaining Node connection the starting path has, get its size of RemainingNode connections

        if (PathToRem[EndingRedP].size() == 1) {
          EndingRemNode = PathToRem[EndingRedP][0];

          for (int i2 = 0; i2 < PathToRem[StartingRedP].size(); i2 += 1) {
            StartingRemNode = PathToRem[StartingRedP][i2]; //This is a RemainingNode that the Starting Point is connected to
            if (EndingRemNode != StartingRemNode) {
              CurrentRemSize = (NodeDirectConnections[StartingRemNode].size() - RemToPath[StartingRemNode].size());

              if (CurrentRemSize < LeastConnectedSize) {
                LeastConnectedSize = CurrentRemSize;
                LeastConnectedNode = StartingRemNode;
              }
            }
          }

          StartingRemNode = LeastConnectedNode;
        }
        else {
          for (int i2 = 0; i2 < PathToRem[StartingRedP].size(); i2 += 1) {
            StartingRemNode = PathToRem[StartingRedP][i2]; //This is a RemainingNode that the Starting Point is connected to
            CurrentRemSize = (NodeDirectConnections[StartingRemNode].size() - RemToPath[StartingRemNode].size());

            if (CurrentRemSize < LeastConnectedSize) {
              LeastConnectedSize = CurrentRemSize;
              LeastConnectedNode = StartingRemNode;
            }
          }

          StartingRemNode = LeastConnectedNode;
          //End Point:
          LeastConnectedNode = 0;
          LeastConnectedSize = 100;
          CurrentRemSize = 100;

          for (int i2 = 0; i2 < PathToRem[EndingRedP].size(); i2 += 1) {
            EndingRemNode = PathToRem[EndingRedP][i2]; //This is a RemainingNode that the Starting Point is connected to
            if (EndingRemNode != StartingRemNode) {
              CurrentRemSize = (NodeDirectConnections[EndingRemNode].size() - RemToPath[EndingRemNode].size());

              if (CurrentRemSize < LeastConnectedSize) {
                LeastConnectedSize = CurrentRemSize;
                LeastConnectedNode = EndingRemNode;
              }
            }
          }

          EndingRemNode = LeastConnectedNode;
        }

        std::vector<int> RemainingNodeGroupPath;
        std::vector<int> RemainingNodeGroupChoices = RemainingNodeGroups[PathGroupIterator];
        RemainingNodeGroupPath.reserve(15);
        RemainingNodeGroupPath.emplace_back(StartingRemNode);
        // So now I have the two starting and end points along with the
        // redirection locations - Let's redirect the line through the Group

        int PathsTested = 0;
        int LongestPathSize = 0;

        BeginNodePathCrawl(LongestPath, PathsTested, StartingRemNode, EndingRemNode,
          NodeDirectConnections, 50, RemainingNodeGroupChoices, LongestPathSize);


        int HostsPathIndex = FindInUnsortedVectorReturnIndex(Path, PathRedirectionPairs[0]);
        if (HostsPathIndex != -1) {
          Path.insert(Path.begin() + HostsPathIndex + 1, LongestPath.begin(), LongestPath.end());
        }

      }
    }

    // Clear variables for the next loop
    RemToPath.clear();
    PathToRem.clear();
    PathRedirectionPairs.clear();

  }
}

void CreateNodePaths(const std::vector<std::vector<int>> & NodeDirectConnections,
  const std::vector<std::vector<int>> & Groups, std::vector<std::vector<int>> & Paths,
  int Iterations, float AcceptableGroupToPathSizeRatio, PATH_REVISION PathRevision,
  bool Recrawl, int MinimumNodesPerPath) {

  // TODO:
  // 1: <NVM, handled by acceptable level> Find a 2nd Path if 1st path prevents cheap path ReCrawl
  // 2: Enable Path-ReCrawl (Moderate cost increase but cheaper than more iterations)
  // 3: Find another Path if previous Path is below acceptable levels after Re-Crawl
  // 4: Implement Acceptable Level (% of Nodes in Group that should be visited)
  // 5: Rename to Path ReCrawl
  // 6: Implement better path Crawl (with the starting over and time tracking and whatnot)

  int PathTargetPopulation;

  int RootNodeIndex;
  int RootNodeSize = 0;

  // Create the longest path possible from each Group - one path per group

  for (const auto & NodeGroup : Groups) {

    // The starting Node is the one with the most connections

    RootNodeSize = 0;
    RootNodeIndex = 0;
    PathTargetPopulation = std::min (
      static_cast<int>( NodeGroup.size() ), 
      static_cast<int>( NodeGroup.size() * AcceptableGroupToPathSizeRatio) );


    for (int Node : NodeGroup) {
      if (NodeDirectConnections[Node].size() > RootNodeSize) {
        RootNodeSize = NodeDirectConnections[Node].size();
        RootNodeIndex = Node;
      }
    }


    /* ***** Generate The Paths *****

      Begin by starting from the center and making a path
      Afterward start from the same location and make another path
      Merge them together and the majority of the time this works and is cheap
      If it doesn't work start from the last Node in the first Path and recrawl
      That usually works - even if it doesn't, revision almost always solves it
      Revision is cheap because the Nodes it has to work with are few and more
      optimizations can be made due to having more data due to previous pathing
    */

    std::vector<int> LongestPath;
    LongestPath.reserve(NodeGroup.size());
    int PathsTested = 0; //This is a counter that will be incremented each time a path is completed
    int LongestPathSize = 0;

    BeginNodePathCrawl(LongestPath, RootNodeIndex, NodeDirectConnections,
      Iterations, PathsTested, LongestPathSize, NodeGroup.size());

    /*
    If, somehow, the best path that could be found isn't even meeting the minimum
    then the best course of action is to immediately recrawl. It's not impossible
    that this COULD have been a viable path - it's just highly unlikely.
    */
    if (LongestPath.size() < MinimumNodesPerPath) {

      if (RootNodeIndex == LongestPath.back()) {

        RootNodeSize = 0;

        for (int Node : NodeGroup) {
          if (NodeDirectConnections[Node].size() <= RootNodeSize) { continue; }
          if (RootNodeIndex == LongestPath.back()) { continue; }

          RootNodeSize = NodeDirectConnections[Node].size();
          RootNodeIndex = Node;
        }
      }
      else {
        RootNodeIndex = LongestPath.back();
      }

      LongestPath.clear();

      if (Recrawl) {

        BeginNodePathCrawl(LongestPath, RootNodeIndex, NodeDirectConnections,
          Iterations, PathsTested, LongestPathSize, NodeGroup.size());

        // This could mean that it's still >= the MINIMUM nodes per path, but I 
        // See little reason NOT to run a revision if at all possible
        if (LongestPath.size() >= PathTargetPopulation) {
          Paths.emplace_back(LongestPath);
          continue;
        }
      }

      // Recrawl must be disabled or it STILL wasn't good enough

      if (PathRevision != PATH_REVISION::Disabled) {
        // Longest Path is being reused rather than allocating another vector,
        // SecondPath will be the revised path created by RevisePath()

        std::vector<int> SecondPath = LongestPath;
        SecondPath.reserve(NodeGroup.size());

        std::vector<int> SortedGroup = NodeGroup;
        std::sort(SortedGroup.begin(), SortedGroup.end());

        RevisePath(SecondPath, NodeDirectConnections, SortedGroup, LongestPath);

        if (SecondPath.size() >= MinimumNodesPerPath) {
          Paths.emplace_back(SecondPath);
          continue;
        }
      }

      if (LongestPath.size() >= MinimumNodesPerPath) {
        Paths.emplace_back(LongestPath);
      }

      continue;
    }


    if (LongestPath.size() == NodeGroup.size()) {
      Paths.emplace_back(LongestPath);
      break;
    }

    // Store the longest path
    std::vector<int> FirstPath = LongestPath;
    FirstPath.reserve(NodeGroup.size());

    PathsTested = 0;

    // ***** Generate The Second Path For This Group *****
    LongestPathSize = 0;

    BeginNodePathCrawl(LongestPath, RootNodeIndex, NodeDirectConnections, FirstPath, Iterations, PathsTested, LongestPathSize, NodeGroup.size() - FirstPath.size());
    PathsTested = 0;
    LongestPathSize = 0;

    if (LongestPath.empty()) {
      // No paths could be made from the host number after the first path.
      // Therefore we will now check if anything better can be done by starting from the other end

      //FinalPaths.reserve(RemoveThrees ? TotalSize - TotalThrees : TotalSize);

      std::vector<int> SortedGroup;

      if (PathRevision != PATH_REVISION::Disabled) {

        SortedGroup = NodeGroup;
        std::sort(SortedGroup.begin(), SortedGroup.end());
        RevisePath(FirstPath, NodeDirectConnections, SortedGroup, LongestPath);

        if (FirstPath.size() >= PathTargetPopulation) {
          Paths.emplace_back(FirstPath);
          break;
        }
      }
      // Revision didn't improve the path enough, going to try a different route and repair again

      if (Recrawl) {

        BeginNodePathCrawl(LongestPath, FirstPath.back(), NodeDirectConnections, Iterations, PathsTested, LongestPathSize, NodeGroup.size());

        if (LongestPath.size() >= PathTargetPopulation) {
          Paths.emplace_back(LongestPath);
          break;
        }

        // Longest Path is a storage container so have to store SecondPath elsewhere
        std::vector<int> SecondPath = LongestPath;
        SecondPath.reserve(NodeGroup.size());
        RevisePath(SecondPath, NodeDirectConnections, SortedGroup, LongestPath);


        //Alright, now let's compare the path sizes

        if (SecondPath.size() >= FirstPath.size()) {
          Paths.emplace_back(SecondPath);
          continue;
        }
        else {
          Paths.emplace_back(FirstPath);
          continue;
        }
      }
      else {
        // Second Path is completely disabled so push the first path
        Paths.emplace_back(FirstPath);
      }
    }

    else {
      //The Second Path wasn't empty so we'll continue



      //Combine the paths together
      //They both start with the same number (ie 12) and that number is the "middle" of the path
      // So Path 1 might be: 12, 13, 14, 7, 6, 5, 4, 3, 2, 18 (which actually leaves Path 2 completely gimped without triggering my "if empty, try 6" failsafe because 15, 17 is available
      //It might be a reasonable rule to prevent the first path from taking a highly blocking node without going down its "dead end"
      //Back on track: 
      //Path1: 12, 13, 14, 7, 8, 10, 11, 0
      //Path2: 12, 6, 5, 4, 3, 2, 1
      //End goal is to start with 0 or to start with 1
      // 0, 11, 10, 8, 7, 14, 13, 12, 6, 5, 4, 3, 2, 1
      // 1, 2, 3, 4, 5, 6, 12, 13, 14, 7, 8, 10, 11, 0
      //So if I add Path1 First In Last Out (and skip 12) I can just add Path 2 to the end of it

      int CombinedPathSize = (FirstPath.size() + LongestPath.size()) - 1;

      if (CombinedPathSize == NodeGroup.size()) {
        //There's no need to continue - this path is actually the longest it can possibly be
        int FinalPathsSize = Paths.size();
        Paths.resize(FinalPathsSize + 1); // Increase size by 1 (Size is now 6, last index is 5)

                                                // Insert Path 1 in reverse
        Paths[FinalPathsSize].insert(Paths[FinalPathsSize].end(), FirstPath.rbegin(), FirstPath.rend());
        // Insert Path 2 but skip index 0 (as that's the duplicate starting point)
        Paths[FinalPathsSize].insert(Paths[FinalPathsSize].end(), LongestPath.begin() + 1, LongestPath.end());
        break;
      }

      std::vector<int> SecondPath;
      SecondPath.reserve(NodeGroup.size());

      // Insert Path 1 in reverse
      SecondPath.insert(SecondPath.end(), FirstPath.rbegin(), FirstPath.rend());
      // Insert Path 2 but skip index 0 (as that's the duplicate starting point)
      SecondPath.insert(SecondPath.end(), LongestPath.begin() + 1, LongestPath.end());

      std::vector<int> SortedGroup = NodeGroup;
      std::sort(SortedGroup.begin(), SortedGroup.end());
      RevisePath(SecondPath, NodeDirectConnections, SortedGroup, LongestPath);

      if (FirstPath.size() == NodeGroup.size()) {
        //There's no need to continue - this path is actually the longest it can possibly be
        Paths.emplace_back(SecondPath);
        break;
      }

      // That still didn't improve the path enough, going to try a different route and repair again
      PathsTested = 0;
      LongestPathSize = 0;

      BeginNodePathCrawl(LongestPath, SecondPath.back(), NodeDirectConnections, Iterations, PathsTested, LongestPathSize, NodeGroup.size());

      if (LongestPath.size() == NodeGroup.size()) {
        //There's no need to continue - this path is actually the longest it can possibly be
        Paths.emplace_back(LongestPath);
        break;
      }

      // Longest Path is a storage container so have to store SecondPath elsewhere
      FirstPath = LongestPath;
      RevisePath(FirstPath, NodeDirectConnections, SortedGroup, LongestPath);


      //Alright, now let's compare the path sizes

      if (SecondPath.size() >= FirstPath.size()) {
        Paths.emplace_back(SecondPath);
        continue;
      }
      else {
        Paths.emplace_back(FirstPath);
        continue;
      }

    }


  }

}





/*
TODO: Create alternate method which replaces sets with vectors for performance reasons
TODO: NodeGroups Reserve can probably be better handled outside of this function

Create Groups From Unvisited Nodes attempts to take the Nodes which weren't used
in the Path and form groups from them. 

RemovedNodes is the list of Nodes which have already been added to a group
PathNodesConnectedToGroup is the list of Nodes which are in the path AND connected to Node(s) in Group[x]
Unvisited Nodes is a list of Nodes which aren't in the path
NodeGroups is the list of NodeGroups and the list of Nodes within
NodeDirectConnections is the list of every Node any Node is directly connected to
CurrentGroup is the list of Nodes that are going to be added to a new NodeGroup
NodesToCheck is the list of Nodes connected to the current Group but not yet added


*/


bool CreatePaths(const std::vector<std::pair<int, int>> & Nodes,
  std::vector<std::vector<int>> & Paths, float MaxDistance, int MinimumGroupSize,
  int ExpectedMaxNumberOfGroups, int PathIterations, PATH_REVISION PathRevision,
  float AcceptableGroupToPathSizeRatio, bool Recrawl, int MinimumNodesPerPath) {

  if (Nodes.empty()) { return false; }

  std::vector<std::vector<int>> NodeDirectConnections;
  NodeDirectConnections.resize(Nodes.size());
  std::vector<std::vector<int>> NodeGroups;
  NodeGroups.reserve(ExpectedMaxNumberOfGroups);

  // Fill NodeDirectConnections with directly connected Nodes

  CreateNodeDirectConnections(Nodes, NodeDirectConnections, MaxDistance);

  // Create groups of indirectly connected Nodes
  CreateGroups(NodeDirectConnections, Paths, MinimumGroupSize, NodeGroups);

  // Run Pathfinding

  if (NodeGroups.empty()) { return false; }
  CreateNodePaths(NodeDirectConnections, NodeGroups, Paths, PathIterations,
    AcceptableGroupToPathSizeRatio, PathRevision, Recrawl, MinimumNodesPerPath);


  return true;
}


int main()
{
  std::vector<std::pair<int, int>> Nodes;
  std::vector<std::vector<int>> Paths;
  Paths.reserve(5);

  Nodes.emplace_back(50, 50);
  Nodes.emplace_back(550, 501);
  Nodes.emplace_back(570, 550);
  Nodes.emplace_back(590, 570);
  Nodes.emplace_back(70, 20);
  Nodes.emplace_back(25, 40);
  Nodes.emplace_back(84, 20);

  CreatePaths(Nodes, Paths, 75, 3, 5,
    50, PATH_REVISION::ReviseTwice, 0.95, true, 3);

  std::cout << "Pathing: \n";

  for (const auto & Path : Paths) {
    std::cout << "Next Path: \n";
    for (const auto & Node : Path) {
      std::cout << "Node Loc: X: " + std::to_string(Nodes[Node].first) + ", Y: " + std::to_string(Nodes[Node].second) + " \n";
    }
  }

  std::cin.get();
  return 0;

}
} // end of namespace SVNL

int main()
{
  SVNL::main();
  return 0;
}