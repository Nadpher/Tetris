#include "Tetris.h"

#include <random>

namespace Tetris
{
	const std::array<Tetris::Matrix, Tetris::NUM_PIECES> Tetris::m_pieces =
	{ {
		{
			0,0,I,0,
			0,0,I,0,
			0,0,I,0,
			0,0,I,0
		},
		{
			0,L,0,0,
			0,L,0,0,
			0,L,L,0,
			0,0,0,0
		},
		{
			0,0,J,0,
			0,0,J,0,
			0,J,J,0,
			0,0,0,0
		},
		{
			0,Z,Z,0,
			0,0,Z,Z,
			0,0,0,0,
			0,0,0,0
		},
		{
			0,S,S,0,
			S,S,0,0,
			0,0,0,0,
			0,0,0,0
		},
		{
			0,0,0,0,
			0,O,O,0,
			0,O,O,0,
			0,0,0,0
		},
		{
			0,T,0,0,
			0,T,T,0,
			0,T,0,0,
			0,0,0,0
		}
	} };

	Tetris::Tetris()
		: m_currentPiece(genPiece())
	{

	}

	Tetris::Piece Tetris::genPiece()
	{
		Piece out;
		std::mt19937 gen(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, NUM_PIECES - 1);
		out.matrix = m_pieces[dist(gen)];

		out.pos = { BOARD_SIZE.x / 2 - 1, BOARD_SIZE.y - 1 }; // starts the piece out in the middle-top

		return out;
	}

	void Tetris::clearLines()
	{
		for (int i = 0; i < BOARD_SIZE.y; ++i)
		{
			bool isFull = true;

			for (int j = 0; j < BOARD_SIZE.x; ++j)
			{
				if (!readBoardCell({ j, i }))
				{
					isFull = false;
				}
			}

			if (isFull)
			{
				for (int j = 0; j < BOARD_SIZE.x; ++j)
				{
					setBoardCell({j, i}, 0);
				}
			}
			else
			{
				for (int j = 0; j < i; ++j)
				{
					bool isEmpty = true;
					for (int k = 0; k < BOARD_SIZE.x; ++k)
					{
						if (readBoardCell({ k, j }))
						{
							isEmpty = false;
						}
					}

					if (isEmpty)
					{
						for (int k = 0; k < BOARD_SIZE.x; ++k)
						{
							std::swap(m_board[i * BOARD_SIZE.x + k], m_board[j * BOARD_SIZE.x + k]);
						}
						break;
					}

				}
			}
		}
	}

	bool Tetris::update()
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - m_lastTimePoint;
		if (elapsed.count() > static_cast<double>(m_fallDelay) / m_delayDivisor)
		{
			if (fits({{m_currentPiece.pos.x, m_currentPiece.pos.y - 1}, m_currentPiece.matrix }))
			{
				m_currentPiece.pos.y -= 1;
			}
			else
			{
				translate();
				clearLines();
				m_currentPiece = genPiece();
				if (!fits(m_currentPiece))
				{
					return false;
				}
			}
			m_lastTimePoint = now;
		}

		return true;
	}

	bool Tetris::fits(Piece piece)
	{
		for (int i = 0; i < MATRIX_SIZE; ++i)
		{
			for (int j = 0; j < MATRIX_SIZE; ++j)
			{
				if (piece.matrix[i + MATRIX_SIZE * j])
				{
					if (piece.pos.x + i < 0 || piece.pos.x + i > BOARD_SIZE.x - 1)
					{
						return false;
					}
					else if (piece.pos.y - j < 0)
					{
						return false;
					}
					else if (readBoardCell({ piece.pos.x + i, piece.pos.y - j})) // if a piece overlaps
					{
						return false;
					}
				}
				
			}
		}

		return true;
	}

	void Tetris::rotatePieceClockwise()
	{
		if (fits({ m_currentPiece.pos, rotateMatrixClockwise(m_currentPiece.matrix) }))
		{
			m_currentPiece.matrix = rotateMatrixClockwise(m_currentPiece.matrix);
		}
	}

	void Tetris::rotatePieceCounterClockwise()
	{
		if (fits({ m_currentPiece.pos, rotateMatrixCounterClockwise(m_currentPiece.matrix) }))
		{
			m_currentPiece.matrix = rotateMatrixCounterClockwise(m_currentPiece.matrix);
		}
	}

	Tetris::Matrix Tetris::rotateMatrixClockwise(Matrix matrix)
	{
		for (int i = 0; i < MATRIX_SIZE / 2; ++i)
		{
			int last = MATRIX_SIZE - 1 - i;

			for (int j = i; j < last; ++j)
			{
				int offset = j - i;

				int top = matrix[i * MATRIX_SIZE + j];
				matrix[i * MATRIX_SIZE + j] = matrix[(last - offset) * MATRIX_SIZE + i];
				matrix[(last - offset) * MATRIX_SIZE + i] = matrix[last * MATRIX_SIZE + last - offset];
				matrix[last * MATRIX_SIZE + last - offset] = matrix[j * MATRIX_SIZE + last];
				matrix[j * MATRIX_SIZE + last] = top;
			}
		}

		return matrix;
	}

	Tetris::Matrix Tetris::rotateMatrixCounterClockwise(Matrix matrix)
	{
		std::reverse(matrix.begin(), matrix.end());
		matrix = rotateMatrixClockwise(matrix);
		return matrix;
	}

	void Tetris::moveLeft()
	{
		if (fits({ {m_currentPiece.pos.x - 1, m_currentPiece.pos.y}, m_currentPiece.matrix }))
		{
			m_currentPiece.pos.x -= 1;
		}
	}

	void Tetris::moveRight()
	{
		if (fits({ {m_currentPiece.pos.x + 1, m_currentPiece.pos.y}, m_currentPiece.matrix }))
		{
			m_currentPiece.pos.x += 1;
		}
	}

	void Tetris::translate()
	{
		for (int i = 0; i < MATRIX_SIZE; ++i)
		{
			for (int j = 0; j < MATRIX_SIZE; ++j)
			{
				if (readPieceCell({ i, j }))
				{
					setBoardCell({ m_currentPiece.pos.x + i, m_currentPiece.pos.y - j }, readPieceCell({ i, j }));
				}
			}
		}
	}
}

