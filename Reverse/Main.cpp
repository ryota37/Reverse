# include <Siv3D.hpp>

enum class StoneColor
{
	None,
	Black,
	White
};

enum class Direction
{
	Up,
	Down,
	Left,
	Right,
	Upleft,
	Upright,
	Downleft,
	Downright
};

bool CanPutStoneOnGrid(int, int, const Grid<StoneColor>&, StoneColor);
void SwitchPlayerTurn(StoneColor& playercolor);
Array<Array<StoneColor>> SearchAllDirections(const Grid<StoneColor>& boardState, int x, int y);
bool AbleToPutStone(StoneColor playercolor, Array<StoneColor> linestones);
bool JudgeWhetherGameEnds(Grid<StoneColor>& boardState);

void CreateGridContainer(const Grid<int32>& grid, Grid<RectF>& gridContainer)
{
	for (int y = 0; y < grid.height(); ++y)
	{
		for (int x = 0; x < grid.width(); ++x)
		{
			const RectF rect{ (x * 100), (y * 100), 100 };
			gridContainer[y][x] = rect;
		}
	}
}

void DrawGrid(const Grid<int32>& grid, const Grid<RectF>& gridContainer)
{
	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			gridContainer[y][x].stretched(-1).draw(Palette::Green);
		}
	}
}

StoneColor reverseColor(StoneColor color)
{
	if (color == StoneColor::None) return StoneColor::None;
	if (color == StoneColor::Black) return StoneColor::White;
	if (color == StoneColor::White) return StoneColor::Black;
}

void DrawStone(const Grid<StoneColor>& boardState)
{
	for (int y = 0; y < boardState.height(); ++y)
	{
		for (int x = 0; x < boardState.width(); ++x)
		{
			Circle stone{50 + 100*x, 50 + 100 * y, 25};
			if (boardState[y][x] == StoneColor::Black) stone.draw(Palette::Black);
			if (boardState[y][x] == StoneColor::White) stone.draw(Palette::White);
			//if (boardState[y][x] == StoneColor::None) stone.draw(Palette::Red); // Debug
		}
	}
}

void InitializeBoardState(Grid<StoneColor>& boardState)
{
	boardState[3][3] = StoneColor::White;
	boardState[4][4] = StoneColor::White;
	boardState[3][4] = StoneColor::Black;
	boardState[4][3] = StoneColor::Black;
}

void PutNewStone(Grid<StoneColor>& boardState, StoneColor color, int x, int y)
{
	boardState[y][x] = color;
}

bool isGridEmpty(Grid<StoneColor>& boardState, int x, int y)
{
	if (boardState[y][x] == StoneColor::None) return true;
	return false;
}

Optional<Vec2> onGridLeftClick(Grid<RectF>& gridContainer)
{
	for (int32 y = 0; y < gridContainer.height(); ++y)
	{
		for (int32 x = 0; x < gridContainer.width(); ++x)
		{
			if (gridContainer[y][x].leftClicked()) return Vec2{ x,y };
		}
	}
	return none;
}

Optional<Vec2> onGridRightClick(Grid<RectF>& gridContainer)
{
	for (int32 y = 0; y < gridContainer.height(); ++y)
	{
		for (int32 x = 0; x < gridContainer.width(); ++x)
		{
			if (gridContainer[y][x].rightClicked()) return Vec2{ x,y };
		}
	}
	return none;
}

void ReverseLine(Grid<StoneColor>& boardState, StoneColor playercolor, int x, int y, Direction direction)
{
	int width = boardState.width();
	int height = boardState.height();
	int dx, dy;

	if (direction == Direction::Up) { dx = 0; dy = -1; }
	if (direction == Direction::Down) { dx = 0; dy = 1; }
	if (direction == Direction::Left) { dx = -1; dy = 0; }
	if (direction == Direction::Right) { dx = 1; dy = 0; }
	if (direction == Direction::Upleft) { dx = -1; dy = -1; }
	if (direction == Direction::Upright) { dx = 1; dy = -1; }
	if (direction == Direction::Downleft) { dx = -1; dy = 1; }
	if (direction == Direction::Downright) { dx = 1; dy = 1; }

	x += dx;
	y += dy;

	while (0 <= x && x < width && 0 <= y && y < height)
	{
		// Reverse Stones
		boardState[y][x] = playercolor;

		// Stop when sandwitch zone ends
		if (x + dx == -1 || x + dx == 8) break;
		if (y + dy == -1 || y + dy == 8) break;
		if (boardState[y + dy][x + dx] == playercolor) break;

		x += dx;
		y += dy;
	}
}

void ReverseStones(Grid<StoneColor>& boardState, StoneColor& playercolor, int x, int y)
{
	auto search_result = SearchAllDirections(boardState, x, y);
	int i = 0;
	for (auto line : search_result)
	{
		if (AbleToPutStone(playercolor, line))
		{
			Direction direction = static_cast<Direction>(i);
			ReverseLine(boardState, playercolor, x, y, direction);
		}
		++i;
	}
}

void PutNewStoneWhenClicked(Grid<StoneColor>& boardState, Grid<RectF>& gridContainer, StoneColor& playercolor, Grid<int32> grid)
{
	if (auto clicked = onGridLeftClick(gridContainer))
	{
		Vec2 pos = *clicked;
		if (isGridEmpty(boardState, pos.x, pos.y) && CanPutStoneOnGrid(pos.x, pos.y, boardState, playercolor) )
		{
			PutNewStone(boardState, playercolor, pos.x, pos.y);
			ReverseStones(boardState, playercolor, pos.x, pos.y);
			SwitchPlayerTurn(playercolor);
		}
	}
}

Array<StoneColor> GetLineStones(const Grid<StoneColor>& boardState, int x, int y, int dx, int dy)
{
	Array<StoneColor> result;
	int width = boardState.width();
	int height = boardState.height();

	x += dx;
	y += dy;

	while (0 <= x && x < width && 0 <= y && y < height)
	{
		result << boardState[y][x];
		x += dx;
		y += dy;
	}

	return result;
}

String ToString(StoneColor color)
{
	switch (color)
	{
	case StoneColor::Black: return U"Black";
	case StoneColor::White: return U"White";
	case StoneColor::None: return U"None";
	}
}

Array<Array<StoneColor>> SearchAllDirections(const Grid<StoneColor>& boardState, int x, int y)
{
	auto up = GetLineStones(boardState, x, y, 0, -1);
	auto down = GetLineStones(boardState, x, y, 0, 1);
	auto left = GetLineStones(boardState, x, y, -1, 0);
	auto right = GetLineStones(boardState, x, y, 1, 0);

	auto upleft = GetLineStones(boardState, x, y, -1, -1);
	auto upright = GetLineStones(boardState, x, y, 1, -1);
	auto downleft = GetLineStones(boardState, x, y, -1, 1);
	auto downright = GetLineStones(boardState, x, y, 1, 1);

	return { up,down,left,right,upleft,upright,downleft,downright };
}

void RightClickEvent(Grid<StoneColor>& boardState, Grid<RectF>& gridContainer)
{
	if (auto clicked = onGridRightClick(gridContainer))
	{
		Vec2 pos = *clicked;
		if (isGridEmpty(boardState, pos.x, pos.y))
		{
			// Debug
			ReverseLine(boardState, StoneColor::Black, pos.x, pos.y, Direction::Down);
			Print << U"Right Clicked!"; 
		}
	}
}

bool AbleToPutStone(StoneColor playercolor, Array<StoneColor> linestones)
{
	if (linestones.isEmpty()) return false;

	// If the neighboring stone color is the same as playercolor, the return value should be false.
	if (linestones[0] != reverseColor(playercolor)) return false;

	// When all the stones are reverse color, the return value should be false because the stones can't be sandwitched.
	if (linestones.all([&](const StoneColor& color) { return color == reverseColor(playercolor); })) return false;

	// When None comes before reverseColor(playercolor), the return value should be false.
	for (int i = 0; i < linestones.size(); ++i)
	{
		if (linestones[i] == playercolor) break;
		if (linestones[i] == StoneColor::None) return false;
	}

	return true;
}

void HighlightValidgrid(const Grid<int32>& grid, const Grid<StoneColor>& boardState, const Grid<RectF>& gridContainer, StoneColor playercolor)
{
	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			if (boardState[y][x] != StoneColor::None) continue;
			auto search_result = SearchAllDirections(boardState, x, y);
			if (search_result.any([&](Array<StoneColor> line) {return AbleToPutStone(playercolor, line); }))
			{
				if (playercolor == StoneColor::White) gridContainer[y][x].stretched(-1).draw(Palette::Green.lerp(Palette::White, 0.5));
				if (playercolor == StoneColor::Black) gridContainer[y][x].stretched(-1).draw(Palette::Green.lerp(Palette::Black, 0.5));
			}
		}
	}
}

bool CanPutStoneOnGrid(int x, int y, const Grid<StoneColor>& boardState, StoneColor playercolor)
{
	auto search_result = SearchAllDirections(boardState, x, y);
	if (search_result.any([&](Array<StoneColor> line) {return AbleToPutStone(playercolor, line); })) return true;
	return false;
}

void SwitchPlayerTurn(StoneColor& playercolor)
{
	playercolor = reverseColor(playercolor);
}

bool JudgeWhetherGameEnds(Grid<StoneColor>& boardState)
{
	// When all grids are occupied by stones, end game
	bool isAllGridOccupied = std::all_of(boardState.begin(), boardState.end(), [&](StoneColor color)
		{
			return color == StoneColor::Black || color == StoneColor::White;
		});
	if (isAllGridOccupied) return true;

	// When All the stones are (Black/White) or None, end game
	bool nowhite = std::all_of(boardState.begin(), boardState.end(), [&](StoneColor color) { return color != StoneColor::White; });
	bool noblack = std::all_of(boardState.begin(), boardState.end(), [&](StoneColor color) { return color != StoneColor::Black; });
	if (nowhite || noblack)
	{
		return true;
	}
	return false;
}

void DisplayStones(Grid<StoneColor>& boardState)
{
	int blackstones = 0;
	int whitestones = 0;
	for (auto cell : boardState)
	{
		if (cell == StoneColor::Black) { ++blackstones; }
		if (cell == StoneColor::White) { ++whitestones; }
	}

	Print << U"Black: " << blackstones;
	Print << U"White: " << whitestones;
	if (blackstones > whitestones) { Print << U"Black wins!"; }
	else if (whitestones > blackstones) { Print << U"White wins!"; }
	else { Print << U"Draw."; }
}

void Main()
{
	Window::Resize(800, 800);

	Grid<int32> grid(8, 8);
	Grid<StoneColor> boardState(8, 8, StoneColor::None);
	Grid<RectF> gridContainer(8,8);

	StoneColor playercolor = StoneColor::Black; // Preliminary

	CreateGridContainer(grid, gridContainer);
	InitializeBoardState(boardState);

	while (System::Update())
	{
		DrawGrid(grid,gridContainer);
		PutNewStoneWhenClicked(boardState, gridContainer, playercolor, grid);
		RightClickEvent(boardState, gridContainer);
		HighlightValidgrid(grid, boardState, gridContainer, playercolor);
		DrawStone(boardState);

		// When a player can put no stone, press S to skip the player.
		if (KeyS.pressed()) SwitchPlayerTurn(playercolor);

		// When the game ends, count the number of stones and judge that which player wins.
		if (JudgeWhetherGameEnds(boardState))
		{
			DisplayStones(boardState);
		}
	}
}
