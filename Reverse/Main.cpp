# include <Siv3D.hpp>

void DrawGrid(const Grid<int32>& grid)
{
	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			const RectF rect{ (x * 100), (y * 100), 100 };
			rect.stretched(-1).draw();
		}
	}
}

void Main()
{
	Window::Resize(800, 800);
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	Grid<int32> grid(8, 8);

	while (System::Update())
	{
		DrawGrid(grid);
	}
}
