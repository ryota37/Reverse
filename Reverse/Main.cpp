# include <Siv3D.hpp>

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

enum class StoneColor
{
	None,
	Black,
	White
};

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
	boardState[3][3] = StoneColor::Black;
	boardState[4][4] = StoneColor::Black;
	boardState[3][4] = StoneColor::White;
	boardState[4][3] = StoneColor::White;
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

void PutNewStoneWhenClicked(Grid<StoneColor>& boardState, Grid<RectF>& gridContainer)
{
	if (auto clicked = onGridLeftClick(gridContainer))
	{
		Vec2 pos = *clicked;
		if (isGridEmpty(boardState, pos.x, pos.y))
		{
			PutNewStone(boardState, StoneColor::White, pos.x, pos.y); //Debug
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

void SearchAllDirections(const Grid<StoneColor>& boardState, int x, int y)
{
	auto up = GetLineStones(boardState, x, y, 0, -1);
	auto down = GetLineStones(boardState, x, y, 0, 1);
	auto left = GetLineStones(boardState, x, y, -1, 0);
	auto right = GetLineStones(boardState, x, y, 1, 0);

	auto upleft = GetLineStones(boardState, x, y, -1, -1);
	auto upright = GetLineStones(boardState, x, y, 1, -1);
	auto downleft = GetLineStones(boardState, x, y, -1, 1);
	auto downright = GetLineStones(boardState, x, y, 1, 1);

	Print << U"Up:";
	for (auto cell : up) Print << ToString(cell);
	Print << U"Down:";
	for (auto cell : down) Print << ToString(cell);
	Print << U"Left:";
	for (auto cell : left) Print << ToString(cell);
	Print << U"Right:";
	for (auto cell : right) Print << ToString(cell);

	Print << U"Upleft:";
	for (auto cell : upleft) Print << ToString(cell);
	Print << U"Uprihgt:";
	for (auto cell : upright) Print << ToString(cell);
	Print << U"Downleft:";
	for (auto cell : downleft) Print << ToString(cell);
	Print << U"Downright:";
	for (auto cell : downright) Print << ToString(cell);
}

void RightClickEvent(Grid<StoneColor>& boardState, Grid<RectF>& gridContainer)
{
	if (auto clicked = onGridRightClick(gridContainer))
	{
		Vec2 pos = *clicked;
		if (isGridEmpty(boardState, pos.x, pos.y))
		{
			//PutNewStone(boardState, StoneColor::Black, pos.x, pos.y); //Debug
			//Call UpperSearch after I finish implementing
			SearchAllDirections(boardState, pos.x, pos.y);
		}
	}
}



void Main()
{
	Window::Resize(800, 800);

	Grid<int32> grid(8, 8);
	Grid<StoneColor> boardState(8, 8, StoneColor::None);
	Grid<RectF> gridContainer(8,8);

	CreateGridContainer(grid, gridContainer);
	InitializeBoardState(boardState);

	while (System::Update())
	{
		DrawGrid(grid,gridContainer);
		DrawStone(boardState);

		PutNewStoneWhenClicked(boardState, gridContainer);
		RightClickEvent(boardState, gridContainer);
	}
}
