#include "Chess.h"

// Chessborad

//Chessborad::Ptr
Chessborad::Ptr::Ptr(const int(&size)[2]) :ptr_x(0), ptr_y(0), num(0)
{
	this->size[0] = size[0];
	this->size[1] = size[1];
	num_max = size[0] * size[1];
};
Chessborad::Ptr::Ptr(const Chessborad::Ptr& p)
{
	ptr_x = p.ptr_x;
	ptr_y = p.ptr_y;
	num = p.num;
	num_max = p.num_max;
	size[0] = p.size[0];
	size[1] = p.size[1];
}

bool Chessborad::Ptr::end()
{
	bool flag = 0;
	if (num >= num_max)
		flag = 1;
	return flag;
}

void Chessborad::Ptr::coord(int* x, int* y)
{
	*x = ptr_x;
	*y = ptr_y;
};

Chessborad::Ptr Chessborad::Ptr::operator++(int)
{
	Chessborad::Ptr p = *this;
	num++;
	int x = ptr_x+1;
	if (x > size[0])
	{
		ptr_x = 0;
		ptr_y++;
	}
	else
	{
		ptr_x = x;
	}
	return p;
}

void Chessborad::Ptr::next()
{
	num++;
	int x = ptr_x + 1;
	if (x >= size[0])
	{
		ptr_x = 0;
		ptr_y++;
	}
	else
	{
		ptr_x = x;
	}
}

Chessborad::Ptr Chessborad::Ptr::copy()
{
	return Chessborad::Ptr(*this);
}


void Chessborad::Ptr::move(const int direction[2], int path)
{
	ptr_x += direction[0]*path;
	ptr_y += direction[1] * path;
	num += path * direction[0] + path * direction[1] * size[1];
}

bool Chessborad::Ptr::valid()
{
	if (ptr_x >= 0 && ptr_x < size[0] && ptr_y >= 0 && ptr_y < size[1])
		return 1;
	else
		return 0;
}

Chessborad::Chessborad(int size_x, int size_y)
{
	size[0] = size_x;
	size[1] = size_y;
	malloc();
}

Chessborad::Chessborad(const Chessborad& cb)
{
	size[0] = cb.size[0];
	size[1] = cb.size[1];
	malloc();
	copy(cb.borad);
}

Chessborad::~Chessborad()
{
	del();
}

void Chessborad::init()
{
	for (auto i = begin(); !i.end(); i.next())
	{
		set(i, 0);
	}
}

void Chessborad::malloc()
{
	borad = new int* [size[0]];
	for (int i = 0; i < size[0]; i++)
	{
		borad[i] = new int[size[1]]();
	}
}

void Chessborad::del()
{
	for (int i = 0; i < size[0]; i++)
	{
		delete[] borad[i];
	}
	delete[] borad;
	borad = nullptr;
}

void Chessborad::copy(int** b)
{
	for (int i = 0; i < size[0]; i++)
	{
		/*for (int j = 0; j < size[1]; j++)
		{
			borad[i][j] = b[i][j];
		}*/
		std::copy(b[i], b[i] + size[1], borad[i]);
	}
}

int Chessborad::get(int x, int y) const
{
	if (x >= 0 && x < size[0] && y >= 0 && y < size[1])
		return borad[x][y];
	else
		return -1;
}

int Chessborad::get(Chessborad::Ptr& p) const
{
	int x = 0, y = 0;
	p.coord(&x, &y);
	return get(x, y);
}

void Chessborad::set(int x, int y, int chess_id)
{
	borad[x][y] = chess_id;
}

void Chessborad::set(Chessborad::Ptr& p, int chess_id)
{
	int x = 0, y = 0;
	p.coord(&x, &y);
	set(x, y, chess_id);
}

int Chessborad::getSize(bool dim)
{
	if (dim)
		return size[1];
	else
		return size[0];
}


Chessborad::Ptr Chessborad::begin()
{
	return Chessborad::Ptr(size);
}

void Chessborad::operator=(const Chessborad& cb)
{
	del();
	size[0] = cb.size[0];
	size[1] = cb.size[1];
	malloc();
	copy(cb.borad);
}

int* Chessborad::operator[](const int x)
{
	return borad[x];
}

bool Chessborad::operator==(const Chessborad& cb)
{
	bool flag = 1;
	if (cb.size[0] == size[0] && cb.size[1] == size[1])
	{
		for (auto i = begin(); !i.end(); i.next())
		{
			if (get(i) != cb.get(i))
			{
				flag = 0;
				break;
			}
		}
	}
	else
	{
		flag = 0;
	}
	return flag;
}


//Chesspiece
Chesspiece::Chesspiece(int id, std::string name, void (*dsp)(int, int)) :chess_id(id), chess_name(name), dsp(dsp) 
{
	if (chess_id < 1)
	{
		throw "chess_id cannot < 1";
	}
};

int Chesspiece::id() const
{
	return chess_id;
}

std::string Chesspiece::name() const
{
	return chess_name;
}


//Player
Player::Player(int id, std::string name, Chesspiece& cp) :player_id(id), player_name(name), cp(&cp) 
{
	if (player_id < 1)
	{
		throw "player_id cannot < 1";
	}
};

int Player::id() const
{
	return player_id;
}

std::string Player::name() const
{
	return player_name;
}

Chesspiece& Player::piece() const
{
	return *cp;
}

void Player::chess(Chessborad cb, int* x, int* y)
{
	*x = -1;
	*y = -1;
}

void Player::chess(const Chessjudge& cj, int* x, int* y)
{
	*x = -1;
	*y = -1;
}


bool Player::operator==(const Player& p) const
{
	if (p.id() == player_id)
		return 1;
	else
		return 0;
}

//Chessjudge
Chessjudge::Chessjudge(const Chessborad& cb, Player& player1, Player& player2) :cb(cb), bell_flag(0)
{
	player_list[0] = &player1;
	player_list[1] = &player2;
}

Chessjudge::Chessjudge(const Chessjudge& cj):cb(cj.cb),bell_flag(cj.bell_flag)
{
	player_list[0] = cj.player_list[0];
	player_list[1] = cj.player_list[1];
}

void Chessjudge::changePlayer(bool player_flag, Player& player)
{
	player_list[player_flag] = &player;
}

Chessborad Chessjudge::getBorad() const
{
	return cb;
}

Player& Chessjudge::getCurrentPlayer() const
{
	return *(player_list[bell_flag]);
}

Player& Chessjudge::getPlayer(bool player_flag) const
{
	return *(player_list[player_flag]);
}