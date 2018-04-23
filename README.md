# Single Visit Node Linker

###### Alternate names are:

###### Single Visit Node Pathing for Maximum Nodes Traveled Within Distance 

###### Between Individual Nodes

Visit Maximizing Node Linker Within Distance Between Individual Nodes

The original purpose for this code was to find paths for InTsumnia which would send them to InTsumniApp where they'd be sent as inputs to an Android emulator. An example video can be seen here: https://www.youtube.com/watch?v=PrNEHAkbyLE

I began work on an alternative algorithm for the same purpose that you may find to be an interesting problem to solve: https://pastebin.com/raw/r4chxRpp

This project comes in the form of a command line tool that requires building and modifying the source data to demo BUT is meant to simply be included into another project for use. The header file contains an overview for most of the functions and the source file walks through each of the functions with comments.

Below is a copy and paste of the already included example that the project comes saddled with. I highly recommend removing both "main" functions as they serve no other purpose.

```
namespace SVNL
{
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


This outputs
Pathing:
Next Path:
Node Loc: X: 50, Y: 50
Node Loc: X: 70, Y: 20
Node Loc: X: 25, Y: 40
Node Loc: X: 84, Y: 20
```