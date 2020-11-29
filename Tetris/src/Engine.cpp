#include "Engine.h"

#include <stdexcept>
#include <string>

namespace Tetris
{
	Engine::Engine(Coord windowSize, const char* title)
		: m_windowSize(windowSize), m_offset(m_windowSize.x / SECTIONS)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			throw std::runtime_error("Could not initialize sdl2.");
		}
		if (TTF_Init() == -1)
		{
			throw std::runtime_error("Could not initialize sdl2_ttf.");
		}

		m_win = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			m_windowSize.x,
			m_windowSize.y,
			SDL_WINDOW_SHOWN
		);
		if (m_win == nullptr)
		{
			throw std::runtime_error("Could not create sdl2 window.");
		}

		m_renderer = SDL_CreateRenderer(
			m_win,
			-1,
			SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_PRESENTVSYNC
		);
		if (m_renderer == nullptr)
		{
			throw std::runtime_error("Could not create sdl2 renderer.");
		}
	}

	Engine::~Engine()
	{
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_win);
		SDL_Quit();
	}

	void Engine::run()
	{
		while (m_run)
		{
			m_run = m_game.update();
			handleEvents();
			draw();
		}
	}

	void Engine::handleEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				m_run = false;
				break;

			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_LEFT:
					m_game.moveLeft();
					break;

				case SDLK_RIGHT:
					m_game.moveRight();
					break;

				case SDLK_DOWN:
					m_game.accelerate();
					break;

				case SDLK_a:
					m_game.rotatePieceCounterClockwise();
					break;

				case SDLK_d:
					m_game.rotatePieceClockwise();
					break;

				default:
					break;
				}
				break;

			case SDL_KEYUP:
				switch (e.key.keysym.sym)
				{
				case SDLK_DOWN:
					m_game.decelerate();
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
	}

	void Engine::draw()
	{
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		drawBoard();
		drawPiece();
		drawBoundaries();
		drawScore();

		SDL_RenderPresent(m_renderer);
	}
	
	// temporary
	void Engine::drawScore()
	{
		TTF_Font* font = TTF_OpenFont("res/roboto.ttf", 128);
		SDL_Surface* msg = TTF_RenderText_Solid(font, std::to_string(m_game.getScore()).c_str(), { 255, 255, 255, 255 });
		SDL_Texture* text = SDL_CreateTextureFromSurface(m_renderer, msg);

		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = std::to_string(m_game.getScore()).size() * 16;
		rect.h = 32;

		SDL_RenderCopy(m_renderer, text, NULL, &rect);

		SDL_DestroyTexture(text);
		SDL_FreeSurface(msg);
		TTF_CloseFont(font);
	}

	void Engine::drawBoard()
	{
		int cellSize = m_offset / Tetris::Tetris::BOARD_SIZE.x;
		SDL_Rect rect;
		rect.w = cellSize;
		rect.h = cellSize;

		for (int i = 0; i < Tetris::Tetris::BOARD_SIZE.x; ++i)
		{
			for (int j = 0; j < Tetris::Tetris::BOARD_SIZE.y; ++j)
			{
				rect.x = cellSize * i + m_offset;
				rect.y = (m_windowSize.y - cellSize * j) - cellSize;

				if (m_game.readBoardCell({ i, j }))
				{
					pickColor(m_game.readBoardCell({ i, j }));
					SDL_RenderFillRect(m_renderer, &rect);
				}
			}
		}
	}

	void Engine::drawPiece()
	{
		int cellSize = m_offset / Tetris::Tetris::BOARD_SIZE.x;
		SDL_Rect rect;
		rect.w = cellSize;
		rect.h = cellSize;

		Coord pos = m_game.getPiecePosition();
		for (int i = 0; i < Tetris::Tetris::MATRIX_SIZE; ++i)
		{
			for (int j = 0; j < Tetris::Tetris::MATRIX_SIZE; ++j)
			{
				rect.x = cellSize * (pos.x + i) + m_offset;
				rect.y = (m_windowSize.y - cellSize * (pos.y - j)) - cellSize;
				if (m_game.readPieceCell({ i, j }))
				{
					pickColor(m_game.readPieceCell({ i, j }));
					SDL_RenderFillRect(m_renderer, &rect);
				}
			}
		}
	}

	void Engine::drawBoundaries()
	{
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

		SDL_RenderDrawLine(m_renderer, m_offset, 0, m_offset, m_windowSize.y);
		SDL_RenderDrawLine(m_renderer, m_offset * 2, 0, m_offset * 2, m_windowSize.y);
	}

	void Engine::pickColor(int id)
	{
		switch (id)
		{
		case Tetris::I:
			SDL_SetRenderDrawColor(m_renderer, 0, 255, 255, 255); // cyan
			break;

		case Tetris::J:
			SDL_SetRenderDrawColor(m_renderer, 255, 128, 0, 255); // orange
			break;

		case Tetris::L:
			SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);   // blue
			break;

		case Tetris::S:
			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);	// green
			break;

		case Tetris::Z:
			SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);   // red
			break;

		case Tetris::O:
			SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255); // yellow
			break;

		case Tetris::T:
			SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255); // magenta
			break;

		default:
			break;
		}
	}
}