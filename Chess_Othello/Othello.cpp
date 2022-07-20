#include "Othello.h"

//Othellojudge
Othellojudge::Othellojudge(const Chessborad& cb, Player& player_black, Player& player_white) :Chessjudge(cb, player_black, player_white)
{
	initborad();
}

Othellojudge::Othellojudge(const Othellojudge& oj) : Chessjudge(oj)
{
	player_chess_num[0] = oj.player_chess_num[0];
	player_chess_num[1] = oj.player_chess_num[1];
	availability_point = oj.availability_point;
}

void Othellojudge::initborad()
{
	cb.init();
	int borad_mid=cb.getSize()/2-1;
	cb.set(borad_mid, borad_mid, player_list[1]->piece().id());
	cb.set(borad_mid, borad_mid + 1, player_list[0]->piece().id());
	cb.set(borad_mid + 1, borad_mid, player_list[0]->piece().id());
	cb.set(borad_mid + 1, borad_mid + 1, player_list[1]->piece().id());
	player_chess_num[0] = 2;
	player_chess_num[1] = 2;
	bell_flag = 0;
	availability();
}

void Othellojudge::availability()
{
	availability_point.clear();
	const int dir_x[] = { 1,1,0 ,-1,-1,-1,0,1 };
	const int dir_y[] = { 0,1,1 ,1,0,-1,-1,-1 };
	int direction[2] = { 0,0 };
	Chessborad cb_temp(cb.getSize(0), cb.getSize(1));
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		if (cb.get(i) == player_list[bell_flag]->piece().id()) //以该子为中心搜索可落子
		{
			for (int n = 0; n < 8; n++)
			{
				bool first_flag = 1;
				auto j = i.copy();
				direction[0] = dir_x[n];
				direction[1] = dir_y[n];
				for (j.move(direction); j.valid(); j.move(direction))
				{
					if (first_flag)
					{
						first_flag = !first_flag;
						if (cb.get(j) == player_list[!bell_flag]->piece().id()) //该子周围有对手子才有可能有可落子的点
							continue;
						else
							break;
					}
					else
					{
						if (cb.get(j) == player_list[!bell_flag]->piece().id())
							continue;
						else if (cb.get(j) == player_list[bell_flag]->piece().id() || cb.get(j) == -1)
							break;
						else if (cb.get(j) == 0)
						{
							int x, y;
							j.coord(&x, &y);
							if(cb_temp[x][y]==0)
								availability_point.push_back(j.copy());
							cb_temp[x][y]++;
							break;
						}
					}
				}
			}
		}
	}
}

int Othellojudge::index(const Player& p)
{
	if (p == *player_list[0])
		return 0;
	else if (p == *player_list[1])
		return 1;
	else
		throw("error player!");
}

bool Othellojudge::turn(int x, int y, const int dir[2])
{
	if (cb.get(x, y) == player_list[!bell_flag]->piece().id())
	{
		if (turn(x + dir[0], y + dir[1], dir))
		{
			cb.set(x, y, player_list[bell_flag]->piece().id());
			player_chess_num[bell_flag] += 1;
			player_chess_num[!bell_flag] -= 1;
			return 1;
		}
		else
			return 0;
	}
	else if (cb.get(x, y) == player_list[bell_flag]->piece().id())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool Othellojudge::gameover(Player** p)
{
	*p = nullptr;
	bool over_flag = 0;
	if (availability_point.empty())
	{
		bell_flag = !bell_flag;
		availability();
		if (availability_point.empty()) // all player cannot chess game over;
		{
			over_flag = 1;
			if (player_chess_num[0] > player_chess_num[1])
			{
				*p = player_list[0];
			}
			if (player_chess_num[0] < player_chess_num[1])
			{
				*p = player_list[1];
			}
		}
	}
	return over_flag;
}

bool Othellojudge::chess(int x, int y)
{
	int x_temp = 0, y_temp = 0;
	bool valid_flag = 0;
	for (auto i = availability_point.begin(); i != availability_point.end(); i++)
	{
		i->coord(&x_temp, &y_temp);
		if (x == x_temp && y == y_temp)
		{
			valid_flag = 1;
			break;
		}
	}
	if (valid_flag)
	{
		cb.set(x, y, player_list[bell_flag]->piece().id());
		player_chess_num[bell_flag]++;
		const int dir_x[] = { 1,1,0 ,-1,-1,-1,0,1 };
		const int dir_y[] = { 0,1,1 ,1,0,-1,-1,-1 };
		int direction[2] = { 0,0 };
		for (int i = 0; i < 8; i++)
		{
			direction[0] = dir_x[i];
			direction[1] = dir_y[i];
			x_temp = x + direction[0];
			y_temp = y + direction[1];
			if (cb.get(x_temp, y_temp) == player_list[!bell_flag]->piece().id())
			{
				turn(x_temp, y_temp, direction);
			}
		}
		bell_flag = !bell_flag;
		availability();
	}
	return valid_flag;
}

Player& Othellojudge::whoisNext()
{
	if (availability_point.empty())
	{
		bell_flag = !bell_flag;
		availability();
		return *player_list[bell_flag];
	}
	else
	{

		return *player_list[bell_flag];
	}
}

Chesspiece& Othellojudge::getPiece(bool player_flag) const
{
	return player_list[player_flag]->piece();
}

const std::vector<Chessborad::Ptr>& Othellojudge::getAvailability() const
{
	return availability_point;
}

int Othellojudge::getPiecesNum(bool player_flag) const
{
	return player_chess_num[player_flag];
}

//Displayothello
Displayothello::Displayothello(const Othellojudge& cj, int x1, int y1, int x2, int y2, std::string tags) :cj(&cj),black(cj.getPiece(BLACKPIECE)),white(cj.getPiece(WHITEPIECE))
{
	this->tags = tags;
	left_top[0] = x1;
	left_top[1] = y1;
	right_bottom[0] = x2;
	right_bottom[1] = y2;
	Chessborad cb = this->cj->getBorad();
	x_path = (right_bottom[0] - left_top[0]) / cb.getSize(0);
	y_path= (right_bottom[1] - left_top[1]) / cb.getSize(1);
}

void Displayothello::display()
{
	Chessborad cb = cj->getBorad();
	auto &availability = cj->getAvailability();

	//draw borad
	rectangle(left_top[0], left_top[1], right_bottom[0], right_bottom[1]);
	int x1, x2, y1, y2;
	for (int i = 1; i < cb.getSize(0); i++)
	{
		x1 = left_top[0] + i * x_path;
		y1 = left_top[1];
		x2 = x1;
		y2 = right_bottom[1];
		line(x1, y1, x2, y2);
	}
	for (int i = 1; i < cb.getSize(1); i++)
	{
		x1 = left_top[0];
		y1 = left_top[1] + i * y_path;
		x2 = right_bottom[0];
		y2 = y1;
		line(x1, y1, x2, y2);
	}

	//draw piece
	int b_x = 0, b_y = 0, p_x = 0, p_y = 0;
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		if (cb.get(i) == black.id())
		{
			i.coord(&b_x, &b_y);
			b2p(b_x, b_y, &p_x, &p_y);
			setfillcolor(BLACK); //有边框
			solidcircle(p_x, p_y, x_path / 2-2);
			continue;
		}
		if (cb.get(i) == white.id())
		{
			i.coord(&b_x, &b_y);
			b2p(b_x, b_y, &p_x, &p_y);
			setfillcolor(WHITE); //有边框
			solidcircle(p_x, p_y, x_path / 2-2);
			continue;
		}
	}

	//draw ava
	auto ava = cj->getAvailability();
	for (auto i = ava.begin(); i != ava.end(); i++)
	{
		i->coord(&b_x, &b_y);
		b2p(b_x, b_y, &p_x, &p_y);
		if (cj->getCurrentPlayer().piece().id() == black.id())
		{
			setfillcolor(BLACK); //有边框
			solidcircle(p_x, p_y, 3);
			continue;
		}
		if (cj->getCurrentPlayer().piece().id() == white.id())
		{
			setfillcolor(WHITE); //有边框
			solidcircle(p_x, p_y, 3);
			continue;
		}
	}
}

void Displayothello::clear()
{
	return;
}

void Displayothello::b2p(int b_x, int b_y, int* p_x, int* p_y)
{
	*p_x = left_top[0] + x_path / 2 + x_path * b_x;
	*p_y = left_top[1] + y_path / 2 + y_path * b_y;
}

//HumanPlayer
HumanPlayer::HumanPlayer(int id, std::string name, Chesspiece& cp, const int left_top[2], const int right_bottom[2], const int size[2]) :Player(id, name, cp)
{
	this->left_top[0] = left_top[0];
	this->left_top[1] = left_top[1];
	this->right_bottom[0] = right_bottom[0];
	this->right_bottom[1] = right_bottom[1];
	this->size[0] = size[0];
	this->size[1] = size[1];
	this->x_path = (right_bottom[0] - left_top[0]) / size[0];
	this->y_path = (right_bottom[1] - left_top[1]) / size[1];
};

void HumanPlayer::chess(Chessborad cb, int* x, int* y)
{
	ExMessage msg;
	*x = -1;
	*y = -1;
	if (peekmessage(&msg, EM_MOUSE))
	{
		if (msg.message == WM_LBUTTONUP)
		{
			int p_x = msg.x;
			int p_y = msg.y;
			p2b(p_x, p_y, x, y);
		}
	}
}

void HumanPlayer::chess(const Chessjudge& oj, int* x, int* y)
{
	ExMessage msg;
	*x = -1;
	*y = -1;
	if (peekmessage(&msg, EM_MOUSE))
	{
		if (msg.message == WM_LBUTTONUP)
		{
			int p_x = msg.x;
			int p_y = msg.y;
			p2b(p_x, p_y, x, y);
		}
	}
}

void HumanPlayer::p2b(int p_x, int p_y, int* b_x, int* b_y) const
{
	*b_x = -1;
	*b_y = -1;
	if (p_x<left_top[0] || p_x>right_bottom[0] || p_y<left_top[1] || p_y>right_bottom[1])
		return;
	for (int i = 0; i < size[0]; i++)
	{
		if (p_x > left_top[0] + i * x_path && p_x < left_top[0] + (i + 1) * x_path)
		{
			*b_x = i;
		}
		if (p_y > left_top[1] + i * y_path && p_y < left_top[1] + (i + 1) * y_path)
		{
			*b_y = i;
		}
		if (*b_x != -1 && *b_y != -1)
			break;
	}
}

//RandomPlayer

RandomPlayer::RandomPlayer(int id, std::string name, Chesspiece& cp) :Player(id, name, cp) {}

void RandomPlayer::chess(const Chessjudge& oj, int* x, int* y)
{
	auto ava = dynamic_cast<const Othellojudge*>(&oj)->getAvailability();
	if (ava.empty())
	{
		*x = -1;
		*y = -1;
	}
	else
	{
		srand(time(0));
		ava.at(rand() % ava.size()).coord(x, y);
	}
}

// MCTSOthello
MCTSOthello::MCTSOthello(MCT& mct) :MCTS(mct) 
{
	Othellojudge* oj = (Othellojudge*)mct.getRoot()->data;
	Player& p1 = oj->getPlayer(PLAYER1);
	Player& p2 = oj->getPlayer(PLAYER2);
	player1 = new RandomPlayer(p1.id(),p1.name(),p1.piece());
	player2 = new RandomPlayer(p2.id(), p2.name(), p2.piece());
}

MCTSOthello::~MCTSOthello()
{
	delete player1;
	delete player2;
}


double MCTSOthello::selectFunction(MCT::Ptr& p) const
{
	MCT::Ptr root_ptr = mct.getRoot();
	Othellojudge* root_oj = (Othellojudge*)root_ptr->data;
	Othellojudge* p_oj = (Othellojudge*)p->data;
	if (root_oj->whoisNext() == p_oj->whoisNext())
	{
		return p->total ? (p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total)) : DBL_MAX;
	}
	else
	{
		return p->total ? (1.0 - p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total)) : DBL_MAX;
	}
}

bool MCTSOthello::expansion(MCT::Ptr& p) const
{
	Othellojudge* oj = (Othellojudge*)p->data;
	auto ava_list = oj->getAvailability();
	if (ava_list.empty()) //无法扩展的情况
	{
		p->termination_flag = 1;
		return 0;
	}
	else
	{
		for (auto i = ava_list.begin(); i != ava_list.end(); i++)
		{
			Othellojudge* temp_oj = new Othellojudge(*oj);
			int x = -1, y = -1;
			i->coord(&x, &y);
			if (!temp_oj->chess(x, y))
				throw("unexpected error");
			mct.addChild(p, temp_oj);
		}
		return 1;
	}
}

double MCTSOthello::rollout(MCT::Ptr& p) const
{
	Othellojudge oj(*(Othellojudge*)p->data);
	oj.changePlayer(PLAYER1, *player1);
	oj.changePlayer(PLAYER2, *player2);
	while (1)
	{
		Player* win_player = nullptr;
		Player& player = oj.whoisNext();
		int x = -1, y = -1;
		player.chess(oj, &x, &y);
		oj.chess(x, y);
		if (oj.gameover(&win_player))
		{
			if (win_player == nullptr)
			{
				return 0.0;
			}
			else
			{
				if (win_player->id() == ((Othellojudge*)mct.getRoot()->data)->whoisNext().id())
					return 1.0;
				else
					return -1.0;
			}
		}
	}
}

AIPlayer::AIPlayer(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth) :Player(id, name, cp), time_ms(time_ms), iterations(iterations) 
{
	last_iterations = 0;
	last_time = 0;
	this->max_depth = max_depth;
}

void delData(void* data)
{
	Othellojudge *oj = (Othellojudge*)data;
	delete oj;
}

void AIPlayer::AIPlayer::chess(const Chessjudge& cj, int* x, int* y)
{
	Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
	MCT mct((void*)root_oj, deldata);
	MCTSOthello mcts(mct);
	auto ptr = mcts.search(time_ms, iterations, max_depth, &last_time, &last_iterations);
	auto ava_list = root_oj->getAvailability();
	auto j = ava_list.begin();
	for (auto i = mct.getRoot().childIterator(); !i.end(); i.next())
	{
		if (i.get() == ptr)
			break;
		j++;
	}
	j->coord(x, y);
}

void deldata(void* data)
{
	Othellojudge* oj = (Othellojudge*)data;
	delete oj;
}
