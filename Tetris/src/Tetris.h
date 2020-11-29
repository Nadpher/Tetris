#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#include <array>
#include <chrono>

#include "util/Coord.h"

namespace Tetris
{
	class Tetris
	{
	public:

		Tetris();
		bool update();

		static constexpr int NUM_PIECES = 7;
		static constexpr int MATRIX_SIZE = 4;
		static constexpr Coord BOARD_SIZE = { 10, 25 };

		using Matrix = std::array<int, Tetris::MATRIX_SIZE* Tetris::MATRIX_SIZE>;

		enum PieceIDs
		{
			I = 1,
			L,
			J,
			Z,
			S,
			O,
			T
		};

		int readBoardCell(Coord pos) const { return m_board[pos.x + pos.y * BOARD_SIZE.x];			    }
		int readPieceCell(Coord pos) const { return m_currentPiece.matrix[pos.x + pos.y * MATRIX_SIZE]; }
		int getScore()				 const { return m_score;											}
		Coord getPiecePosition()	 const { return m_currentPiece.pos;									}

		void moveLeft();
		void moveRight();

		void rotatePieceClockwise();
		void rotatePieceCounterClockwise();
		void accelerate() { m_delayDivisor = 10; }
		void decelerate() { m_delayDivisor = 1;  }

	private:

		struct Piece
		{
			Coord pos;
			Matrix matrix;
		};

		const int m_startFallDelay = 1;
		int m_delayDivisor = 1;
		int m_fallDelay = m_startFallDelay; // initially 1 second
		std::chrono::steady_clock::time_point m_lastTimePoint = std::chrono::steady_clock::now();
		int m_score = 0;
		int m_scoreIncrement = 100;

		void setBoardCell(Coord pos, int val) { m_board[pos.x + pos.y * BOARD_SIZE.x] = val; }

		Piece genPiece();
		bool fits(Piece piece);
		void translate();
		int clearLines();

		Matrix rotateMatrixClockwise(Matrix matrix);
		Matrix rotateMatrixCounterClockwise(Matrix matrix);

		Piece m_currentPiece = {};
		std::array<int, BOARD_SIZE.x * BOARD_SIZE.y> m_board = { };
		static const std::array<Matrix, NUM_PIECES> m_pieces;
	};
}

#endif // TETRIS_TETRIS_H

