# include <Siv3D.hpp>

void CreateGridContainer(const Grid<int32>& grid, Array<RectF>& gridContainer)
{
	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			const RectF rect{ (x * 100), (y * 100), 100 };
			gridContainer.push_back(rect);
		}
	}
}

void DrawGrid(Array<RectF>& gridContainer)
{
	for (auto& rect : gridContainer)
	{
		rect.stretched(-1).draw(Palette::Green);
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
	if (boardState[y][x] == StoneColor::Black) return true;
	return false;
}

void Main()
{
	Window::Resize(800, 800);

	Grid<int32> grid(8, 8);
	Grid<StoneColor> boardState(8, 8, StoneColor::None);
	Array<RectF> gridContainer;

	CreateGridContainer(grid, gridContainer);
	InitializeBoardState(boardState);

	while (System::Update())
	{
		DrawGrid(gridContainer);
		DrawStone(boardState);

		PutNewStone(boardState, StoneColor::Black, 1, 2); //Debug
	}
}
