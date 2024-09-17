// Termm--Fall 2024

#pragma once

#include <utility>

// Holds data about the maze and the player.
// Manages player movement.
class Maze
{
public:
	Maze( size_t dim );
	~Maze();

	void reset();

	size_t getDim() const;

	int getValue(const int & x, const int & y ) const;

	void setValue(const int & x,const int & y, const int & h );

	void digMaze();
	void printMaze(); // for debugging

	void movePlayerToStart(); // call after digMaze()
	std::pair<int, int> getPlayerPos() const;

	void movePlayerRight();
	void movePlayerLeft();
	void movePlayerDown();
	void movePlayerUp();

private:
	void movePlayerTo(const int & x, const int & y);
	void resetPlayerPos();

	std::pair<int, int> m_playerPos;
	size_t m_dim;
	int *m_values;
	void recDigMaze(int r, int c);
	int numNeighbors(int r, int c);
	inline std::pair<int, int> getStartPos() const;
	inline bool isInBounds(const int & x, const int & y);
	inline bool isOccupied(const int & x, const int & y);
};
