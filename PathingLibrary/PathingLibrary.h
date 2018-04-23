/*

Feel free to use this code in any manner you see fit with the only 'requirement'
being that I'm credited.

Copyright (c) 2018-2018 Travis Foster

*/

#pragma once
#ifndef PathingLibrary_H_
#define PathingLibrary_H_

#include "stdafx.h"
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <tuple>
#include <functional>
#include <set>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace SVNL
{

/*
	CreatePaths is the only function meant to be directly called by users.
	Its purpose is to receive the Nodes and MaxDistance allowed between them
	And fill the Paths container with X, Y data 
 */

  bool CreatePaths(const std::vector<std::pair<int, int>> & Nodes,
    std::vector<std::vector<int>> & Paths, float MaxDistance, int MinimumGroupSize,
    int ExpectedMaxNumberOfGroups, int PathIterations, PATH_REVISION PathRevision,
    float AcceptableGroupToPathSizeRatio, bool Recrawl, int MinimumNodesPerPath);

/*
	CreateNodeDirectConnections creates a 2D Vector of the Nodes each Node is
	directly connected to. The index is the same as Nodes, which is how the
	coordinates are retrieved. Nodes are only checked against each other once
	by not checking previous Nodes (which have already checked against the
	current Node).
*/

  void CreateNodeDirectConnections(const std::vector<std::pair<int, int>> & Nodes,
    std::vector<std::vector<int>> & NodeDirectConnections, float MaxDistance);

// PointDistance returns the distance between two points

double PointDistance(float x1, float y1, float x2, float y2);

double PointDistance(double x1, double y1, double x2, double y2);

double PointDistance(int x1, int y1, int x2, int y2);

// IsInRange returns whether two Nodes are in range of one another

bool IsInRange(std::pair<int, int> A, std::pair<int, int> B, float MaxDistance);

bool IsInRange(std::pair<float, float> A, std::pair<float, float> B, float MaxDistance);

bool IsInRange(std::pair<double, double> A, std::pair<double, double> B, float MaxDistance);

bool FindInSortedVector(const std::vector<int> & Vec, int Elem);

bool FindInUnsortedVector(const std::vector<int> & Vec, int Elem);

} // namespace SVNL

#endif  // PathingLibrary_H_