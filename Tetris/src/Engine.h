#ifndef TETRIS_ENGINE_H
#define TETRIS_ENGINE_H

#include "Tetris.h"
#include <SDL2/SDL.h>

namespace Tetris
{
	class Engine
	{
	public:

		Engine() = delete;
		Engine(Coord windowSize, const char* title = "Tetris");

		~Engine();

		void run();

		static constexpr int SECTIONS = 3;

	private:

		SDL_Window*   m_win      = nullptr;
		SDL_Renderer* m_renderer = nullptr;
		Tetris m_game;
		const Coord m_windowSize;
		const int m_offset;

		bool m_run = true;

		void pickColor(int id);
		void draw();
		void drawBoundaries();
		void drawBoard();
		void drawPiece();

		void handleEvents();
	};
}

#endif // TETRIS_ENGINE_H
