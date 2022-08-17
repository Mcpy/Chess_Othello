#include "Othello.h"

//Othellojudge
Othellojudge::Othellojudge(const Chessborad& cb, Player& player_black, Player& player_white) :Chessjudge(cb, player_black, player_white)
{
	initborad();
	last_chess[0] = -1;
	last_chess[1] = -1;
	last_player_id = -1;
}

Othellojudge::Othellojudge(const Othellojudge& oj) : Chessjudge(oj)
{
	player_chess_num[0] = oj.player_chess_num[0];
	player_chess_num[1] = oj.player_chess_num[1];
	availability_point = oj.availability_point;
	last_chess[0] = oj.last_chess[0];
	last_chess[1] = oj.last_chess[1];
	last_player_id = oj.last_player_id;
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
	Chessborad cb_temp(cb.getSize(0), cb.getSize(1)); //用来计数可落子点、避免重复
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
	Player *p_temp = nullptr;
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
				p_temp = player_list[0];
			}
			if (player_chess_num[0] < player_chess_num[1])
			{
				p_temp = player_list[1];
			}
		}
	}
	if (p != nullptr)
		*p = p_temp;
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
		last_chess[0] = x;
		last_chess[1] = y;
		last_player_id = player_list[bell_flag]->id();
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
	if (root_oj->whoisNext().id() == p_oj->last_player_id)
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
	if(oj->gameover())
	{
		p->termination_flag = 1;
		return 0;
	}
	else
	{
		auto ava_list = oj->getAvailability();
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

double MCTSOthello::rollout(MCT::Ptr& p)
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

AIPlayer::AIPlayer(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth) 
	:Player(id, name, cp), time_ms(time_ms), iterations(iterations) 
{
	last_iterations = 0;
	last_time = 0;
	this->max_depth = max_depth;
	mct = nullptr;
	mcts = nullptr;
}

AIPlayer::~AIPlayer()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

void delData(void* data)
{
	Othellojudge *oj = (Othellojudge*)data;
	delete oj;
}

void AIPlayer::AIPlayer::chess(const Chessjudge& cj, int* x, int* y)
{
	
	if (mct == nullptr)
	{
		Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
		mct = new MCT((void*)root_oj, deldata);
		mcts = new MCTSOthello(*mct);
	}
	else
	{
		auto ptr = mct->find(&cj, findFunc); //寻找节点，继承已经迭代的数据
		if (ptr.valid())
		{
			mct->changeRoot(ptr);
		}
		else // 没有节点 重新建立树
		{
			init();
			Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
			mct = new MCT((void*)root_oj, deldata);
			mcts = new MCTSOthello(*mct);
		}
	}
	auto ptr = mcts->search(time_ms, iterations, max_depth, &last_time, &last_iterations);
	mct->changeRoot(ptr);
	Othellojudge* temp_oj = (Othellojudge*)ptr->data;
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
}

void AIPlayer::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;

	mct = nullptr;
	mcts = nullptr;
}

void deldata(void* data)
{
	Othellojudge* oj = (Othellojudge*)data;
	delete oj;
}

bool findFunc(const void* data1, const void* data2)
{
	const Chessjudge* cj1 = (const Chessjudge*)data1;
	const Chessjudge* cj2 = (const Chessjudge*)data2;
	return (cj1->getBorad() == cj2->getBorad());
}

AMAFOthello::AMAFOthello(MCT& mct) :MCTSOthello(mct) {}

double AMAFOthello::rollout(MCT::Ptr& p)
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
		if (oj.chess(x, y))
		{
			rollout_step[player.id() == player2->id()].emplace_back(x, y);
		}
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

void AMAFOthello::backup(MCT::Ptr& p, double score)
{
	for (MCT::Ptr i = p; i.valid(); i = i.parent())
	{
		i->score += score;
		i->total++;
		for (auto j = i.childIterator(); !j.end(); j.next())
		{
			auto j_ptr = j.get();
			Othellojudge* oj = (Othellojudge*)j_ptr->data;
			for (auto& k : rollout_step[oj->last_player_id == player2->id()])
			{
				if (k.x == oj->last_chess[0] && k.y == oj->last_chess[1])
				{
					j_ptr->score += score;
					j_ptr->total++;
					break;
				}
			}
		}
	}
	for (auto i : rollout_step)
	{
		i.clear();
	}
}

AMAFPlayer::AMAFPlayer(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth)
	:Player(id, name, cp), time_ms(time_ms), iterations(iterations)
{
	last_iterations = 0;
	last_time = 0;
	this->max_depth = max_depth;
	mct = nullptr;
	mcts = nullptr;
}

AMAFPlayer::~AMAFPlayer()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

void AMAFPlayer::chess(const Chessjudge& cj, int* x, int* y)
{
	if (mct == nullptr)
	{
		Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
		mct = new MCT((void*)root_oj, deldata);
		mcts = new AMAFOthello(*mct);
	}
	else
	{
		auto ptr = mct->find(&cj, findFunc); //寻找节点，继承已经迭代的数据
		if (ptr.valid())
		{
			mct->changeRoot(ptr);
		}
		else // 没有节点 重新建立树
		{
			init();
			Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
			mct = new MCT((void*)root_oj, deldata);
			mcts = new AMAFOthello(*mct);
		}
	}
	auto ptr = mcts->search(time_ms, iterations, max_depth, &last_time, &last_iterations);
	mct->changeRoot(ptr);
	Othellojudge* temp_oj = (Othellojudge*)ptr->data;
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
}

void AMAFPlayer::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;

	mct = nullptr;
	mcts = nullptr;
}

MCTSOthelloParallelisation::ThreadRollout::ThreadRollout(MCT::Ptr& p, std::atomic<double>* score, Player* p1, Player* p2, int root_player_id)
	: p(p), score(score), player1(p1), player2(p2), root_player_id(root_player_id) {}

void MCTSOthelloParallelisation::ThreadRollout::start()
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
			if (win_player != nullptr)
			{
				if (win_player->id() == root_player_id)
					score++;
				else
					score--;
			}
		}
	}
}

MCTSOthelloParallelisation::MCTSOthelloParallelisation(MCT& mct, ThreadPool& tp, int thread_num)
	:MCTSOthello(mct), thread_num(thread_num), tp(tp)
{
}

double MCTSOthelloParallelisation::rollout(MCT::Ptr& p)
{
	std::atomic<double> score = 0.0;
	int root_player_id = ((Othellojudge*)mct.getRoot()->data)->whoisNext().id();
	std::list<ThreadRollout> thread_rollout_list;
	for (int i = 0; i < thread_num; i++)
	{
		thread_rollout_list.emplace_back(p, score, player1, player2, root_player_id);
	}
	for (auto& i : thread_rollout_list)
	{
		tp.pushTask(&i);
	}
	while (tp.runningNum())
	{
		std::this_thread::yield();
	}
	return score;
}

AIPlayer_Thread::AIPlayer_Thread(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth, ThreadPool& tp, int thread_num)
	:Player(id, name, cp), time_ms(time_ms), iterations(iterations),tp(tp),thread_num(thread_num)
{
	last_iterations = 0;
	last_time = 0;
	this->max_depth = max_depth;
	mct = nullptr;
	mcts = nullptr;
}

AIPlayer_Thread::~AIPlayer_Thread()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

void AIPlayer_Thread::chess(const Chessjudge& cj, int* x, int* y)
{
	if (mct == nullptr)
	{
		Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
		mct = new MCT((void*)root_oj, deldata);
		mcts = new MCTSOthelloParallelisation(*mct, tp, thread_num);
	}
	else
	{
		auto ptr = mct->find(&cj, findFunc); //寻找节点，继承已经迭代的数据
		if (ptr.valid())
		{
			mct->changeRoot(ptr);
		}
		else // 没有节点 重新建立树
		{
			init();
			Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
			mct = new MCT((void*)root_oj, deldata);
			mcts = new MCTSOthelloParallelisation(*mct, tp, thread_num);
		}
	}
	auto ptr = mcts->search(time_ms, iterations, max_depth, &last_time, &last_iterations);
	mct->changeRoot(ptr);
	Othellojudge* temp_oj = (Othellojudge*)ptr->data;
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
}

void AIPlayer_Thread::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

