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

RandomPlayer::RandomPlayer(int id, std::string name, Chesspiece& cp) :Player(id, name, cp), gen(rd()), distrib(0) {}

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
		ava.at(distrib(gen) % ava.size()).coord(x, y);
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
		Player& player = oj.whoisNext();
		int x = -1, y = -1;
		player.chess(oj, &x, &y);
		oj.chess(x, y);
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
		Player& player = oj.whoisNext();
		int x = -1, y = -1;
		player.chess(oj, &x, &y);
		if (oj.chess(x, y))
		{
			rollout_step[player.id() == player2->id()].emplace_back(x, y);
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
	for (auto& i : rollout_step)
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
	:p(p), score(score), player1(p1), player2(p2), root_player_id(root_player_id) {}

void MCTSOthelloParallelisation::ThreadRollout::start()
{
	Othellojudge oj(*(Othellojudge*)p->data);
	oj.changePlayer(PLAYER1, *player1);
	oj.changePlayer(PLAYER2, *player2);
	while (1)
	{
		Player* win_player = nullptr;
		if (oj.gameover(&win_player))
		{
			if (win_player != nullptr)
			{
				if (win_player->id() == root_player_id)
					(*score) = (*score) + 1.0;
				else
					(*score) = (*score) - 1.0;
			}
			break;
		}
		Player& player = oj.whoisNext();
		int x = -1, y = -1;
		player.chess(oj, &x, &y);
		oj.chess(x, y);
		
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
		thread_rollout_list.emplace_back(p, &score, player1, player2, root_player_id);
	}
	std::list<std::future<void>> return_info;
	for (auto& i : thread_rollout_list)
	{
		return_info.emplace_back(tp.pushTask(&ThreadRollout::start, &i));
	}
	for (auto& i : return_info)
	{
		try
		{
			i.get();
		}
		catch (...)
		{
		}
	}
	return score;
}

void MCTSOthelloParallelisation::backup(MCT::Ptr& p, double score)
{
	for (MCT::Ptr i = p; i.valid(); i = i.parent())
	{
		i->score += score;
		i->total += thread_num;
	}
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

	mct = nullptr;
	mcts = nullptr;
}

void AIPlayer_Root_Para::merge()
{
	std::vector<MCT::Ptr>* player_mct = new std::vector<MCT::Ptr>[para_num];
	for (int i = 0; i < para_num; i++)
	{
		auto& mct_list = player_mct[i];
		mct_list.push_back(player_list[i].mct->getRoot());
		for (int j = 0; j < mct_list.size(); j++)
		{
			
			auto& ptr = mct_list[j];
			for (auto iter = ptr.childIterator(); !iter.end(); iter.next())
			{
				player_mct[i].emplace_back(iter.get());
			}
		}
	}
	auto find_max_size = [&]() {
		int max_size = 0;
		for (int i = 0; i < para_num; i++)
		{
			if (player_mct[i].size() > max_size)
				max_size = player_mct[i].size();
		}
		return max_size;
	};
	int max_size = find_max_size();
	for (int n = 0; n < max_size; n++)
	{

		double score = 0, value = 0;
		int total = 0;
		for (int i = 0; i < para_num; i++)
		{
			auto& mct_list = player_mct[i];
			if (n >= mct_list.size())
				continue;
			auto ptr = mct_list[n];
			score += ptr->score;
			total += ptr->total;
			value += ptr->value;
		}
		for (int i = 0; i < para_num; i++)
		{
			auto& mct_list = player_mct[i];
			if (n >= mct_list.size())
				continue;
			auto& ptr = mct_list[n];
			ptr->score = score;
			ptr->total = total;
			ptr->value = value;
		}
	}
	delete[] player_mct;
	return;
}

AIPlayer_Root_Para::AIPlayer_Root_Para(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth, ThreadPool& tp, int para_num)
	:Player(id, name, cp), time_ms(time_ms), iterations(iterations), max_depth(max_depth), tp(tp), para_num(para_num)
{
	last_time = 0;
	last_iterations = 0;
	for (int i = 0; i < para_num; i++)
	{
		player_list.emplace_back(id * 10 + i, name, cp, time_ms, iterations, max_depth);
	}
}


void AIPlayer_Root_Para::chess(const Chessjudge& cj, int* x, int* y)
{
	std::vector<std::future<void>> ret;
	for (auto& i:player_list)
	{
		ret.push_back(tp.pushTask(&AIPlayer_for_root::chess, &i, std::ref(cj), nullptr, nullptr));
	}
	for (auto& i : ret)
	{
		i.get();
	}
	merge();
	player_list[0].chess_merge(x, y);
}

void AIPlayer_Root_Para::init()
{
	for (auto& i : player_list)
	{
		i.init();
	}
}

MCTSOthello_Progressivebias::MCTSOthello_Progressivebias(MCT& mct)
	:MCTSOthello(mct)
{
}

double MCTSOthello_Progressivebias::selectFunction(MCT::Ptr& p) const
{
	MCT::Ptr root_ptr = mct.getRoot();
	Othellojudge* root_oj = (Othellojudge*)root_ptr->data;
	Othellojudge* p_oj = (Othellojudge*)p->data;
	double heuristic_value = p->value / ((double)p->total + 1);
	if (root_oj->whoisNext().id() == p_oj->last_player_id)
	{
		return p->total ? (p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total) + heuristic_value) : DBL_MAX;
	}
	else
	{
		return p->total ? (1.0 - p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total) + heuristic_value) : DBL_MAX;
	}
}

bool MCTSOthello_Progressivebias::expansion(MCT::Ptr& p) const
{
	Othellojudge* oj = (Othellojudge*)p->data;
	if (oj->gameover())
	{
		p->termination_flag = 1;
		return 0;
	}
	else
	{
		auto ava_list = oj->getAvailability();
		double max_heuristic = -DBL_MAX, min_heuristic = DBL_MAX;
		auto update_max_min = [&max_heuristic, &min_heuristic](double heuristic) {
			if (heuristic > max_heuristic)
				max_heuristic = heuristic;
			if (heuristic < min_heuristic)
				min_heuristic = heuristic;
		};
		auto zero_one_standardization = [&](int scaling = 1, double bias = 0) {
			bool set_zero_flag = 0;
			bool minus_min_flag = 0;
			double minus_min = 0;
			if (max_heuristic == min_heuristic)
				set_zero_flag = 1;
			if (min_heuristic < 0)
			{
				minus_min_flag = 1;
				minus_min = min_heuristic;
				max_heuristic -= minus_min;
				min_heuristic -= minus_min;
			}
			for (auto i = p.childIterator(); !i.end(); i.next())
			{
				auto j = i.get();
				j->value = minus_min_flag ? j->value - minus_min : j->value;
				//j->value = set_zero_flag ? 0 : ((j->value - min_heuristic) / (max_heuristic + min_heuristic)) * scaling + bias;
				if (j->value < 0)
					int aa = 0;
			}
		};
		for (auto i = ava_list.begin(); i != ava_list.end(); i++)
		{
			Othellojudge* temp_oj = new Othellojudge(*oj);
			int x = -1, y = -1;
			i->coord(&x, &y);
			if (!temp_oj->chess(x, y))
				throw("unexpected error");
			double heuristic_value = getHeuristic_fixedweights(*temp_oj);
			update_max_min(heuristic_value);
			mct.addChild(p, temp_oj, getHeuristic_fixedweights(*temp_oj));
		}
		zero_one_standardization(10, 0);
		return 1;
	}
}

double MCTSOthello_Progressivebias::getHeuristic(Othellojudge& oj) const
{
	//Player& max_player = ((Othellojudge*)mct.getRoot()->data)->whoisNext();
	Player& max_player = oj.getPlayer(oj.last_player_id);
	bool max_player_flag = (max_player.id()== player2->id());
	Player& min_player = oj.getPlayer(!max_player_flag);
	auto calValue = [](double max_player, double min_player)->double {return (max_player - min_player) / (max_player + min_player); };

	// Coin Parity
	int max_player_coins = oj.getPiecesNum(max_player_flag);
	int min_player_coins = oj.getPiecesNum(!max_player_flag);
	double coin_value = calValue(max_player_coins, min_player_coins);

	Chessborad cb = oj.getBorad();
	//0 max_player 1 min_player
	auto getindex = [min_player](int piece_id)->bool {return piece_id == min_player.piece().id(); };
	//Corners Captured
	int player_corner[2] = { 0,0 };
	int player_corner_potential[2] = { 0,0 };

	auto isCorner = [&cb](int x, int y)->bool {
		if ((x == 0 || x == (cb.getSize(1) - 1)) && (y == 0 || y == (cb.getSize(0) - 1)))
			return 1;
		else
			return 0;
	};

	//Mobility
	int actual_mobility[2] = { 0,0 };
	int potential_mobility[2] = { 0,0 };
	//Stability
	int stable[2] = { 0,0 };
	int semistable[2] = { 0,0 };
	int unstable[2] = { 0,0 };

	const int x_dir[] = { 1,1,0,-1,-1,-1,0,1 };
	const int y_dir[] = { 0,1,1,1,0,-1,-1,-1 };
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		int x, y;
		i.coord(&x, &y);

		//Mobility 可落子和潜在可落子
		if (cb.get(i) == 0)
		{
			for (int n = 0; n < 8; n++)
			{
				auto j(i);
				const int dir[2] = { x_dir[n],y_dir[n] };
				bool potential_mobility_flag[2] = { 0,0 };
				bool actual_mobility_flag[2] = { 0,0 };
				//它相邻是否有棋子
				j.move(dir);
				int near_piece_id = cb.get(j);
				//如果没有则不可落子；
				if (near_piece_id == 0 || near_piece_id == -1)
					continue;
				//否则向该方向遍历
				else
				{
					for (j.move(dir); ; j.move(dir))
					{
						int p_id = cb.get(j);
						if (p_id != near_piece_id)
						{
							//potential
							if (p_id == 0)
							{
								// max 0 min 1
								//保证计算一次;若是实际落子就不是潜在落子
								if (potential_mobility_flag[!getindex(near_piece_id)] == 0 && actual_mobility_flag[!getindex(near_piece_id)] == 0)
								{
									potential_mobility[!getindex(near_piece_id)]++;
									potential_mobility_flag[!getindex(near_piece_id)] = 1;
								}
							}
							else if (p_id == -1)
							{
								break;
							}
							//actual
							else
							{
								if (actual_mobility_flag[!getindex(near_piece_id)] == 0)
								{
									actual_mobility[!getindex(near_piece_id)]++;
									actual_mobility_flag[!getindex(near_piece_id)] = 1;
									if (potential_mobility_flag[!getindex(near_piece_id)] == 1)
									{
										potential_mobility[!getindex(near_piece_id)]--;
										potential_mobility_flag[!getindex(near_piece_id)] = 0;
									}
								}
								if (isCorner(x, y))
									player_corner_potential[!getindex(near_piece_id)]++;
							}
							break;
						}
					}
				}
				if (actual_mobility_flag[0] && actual_mobility_flag[1])
					break;
			}
		}
		//Mobility 永不被翻\可被翻\将被翻
		else
		{
			int piece_id = cb.get(i);
			if (isCorner(x, y))
			{
				player_corner[getindex(piece_id)]++;
				stable[getindex(piece_id)]++;
			}
			else
			{
				int stable_dir = 0;
				bool unstable_flag = 0;
				for (int n = 0; n < 4; n++)
				{
					int dir[2] = { x_dir[n],y_dir[n] };
					int r_dir[2] = { -x_dir[n],-y_dir[n] };
					int current_p_id = piece_id, r_current_p_id = piece_id;
					auto j(i);
					//向一个方向遍历
					for (j.move(dir);; j.move(dir))
					{
						current_p_id = cb.get(j);
						if (current_p_id != piece_id)
							break;
					}
					//该方向稳定 continue
					if (current_p_id == -1)
					{
						stable_dir++;
						continue;
					}
					//不稳定向另一个方向遍历
					else
					{
						auto k(i);
						for (k.move(r_dir);; k.move(r_dir))
						{
							r_current_p_id = cb.get(k);
							if (r_current_p_id != piece_id)
								break;
						}
						//另一个方向稳定continue
						if (r_current_p_id == -1)
						{
							stable_dir++;
							continue;
						}
						//另一个方向不稳定
						else
						{
							//两边都没对方棋子，该方向可被翻；但无法确定是否将被翻，还需遍历其他方向
							if (current_p_id == 0 && r_current_p_id == 0)
							{
								continue;
							}
							//一边为空一边为对方棋子，将被翻
							else
							{
								unstable[getindex(piece_id)]++;
								unstable_flag = 1;
								break;
							}
						}
					}
				}
				if (!unstable_flag)
				{
					//4个方向都稳定，不可被翻
					if(stable_dir == 4)
						stable[getindex(piece_id)]++;
					else
						semistable[getindex(piece_id)]++;
				}
			}
		}
	}

	// get Heuristic value

	//Mobility
	double actual_mobility_value = actual_mobility[0] + actual_mobility[1] != 0 ? calValue(actual_mobility[0], actual_mobility[1]) : 0;
	double potential_mobility_value = potential_mobility[0] + potential_mobility[1] != 0 ? calValue(potential_mobility[0], potential_mobility[1]) : 0;
	//Corners Captured
	//corners captured 2; potential corners 1;
	int corner_sum[2] = { player_corner[0] * 2 + player_corner_potential[0],player_corner[1] * 2 + player_corner_potential[1] };
	double corner_value = corner_sum[0] + corner_sum[1] != 0 ? calValue(corner_sum[0], corner_sum[1]) : 0;
	//Stability
	// 1 for stable coins, -1 for unstable coins and 0 for semi - stable coins
	int stable_sum[2] = { stable[0] - unstable[0],stable[1] - unstable[1] };
	double stable_value = stable_sum[0] + stable_sum[1] != 0 ? calValue(stable_sum[0], stable_sum[1]) : 0;

	//weight
	//corner 30/90; mobility 5/90; stability 25/90; coin 25/90

	double heuristic_value = (30 * corner_value + 5 * (actual_mobility_value + potential_mobility_value) + 25 * stable_value + 25 * coin_value)/90;
	return heuristic_value;

}

double MCTSOthello_Progressivebias::getHeuristic_fixedweights(Othellojudge& oj) const
{
	double heuristic_value = 0;
	Player& max_player = oj.getPlayer(oj.last_player_id);
	//bool max_player_flag = (max_player.id() == player2->id());
	//Player& min_player = oj.getPlayer(!max_player_flag);
	const double fixed_weights[8][8] = {
		{1.0,-0.25,0.1,0.05,0.05,0.1,-0.25,1.0},
		{-0.25,-0.25,0.01,0.01,0.01,0.01,-0.25,-0.25},
		{0.1,0.01,0.05,0.02,0.02,0.05,0.01,0.1},
		{0.05,0.01,0.02,0.01,0.01,0.02,0.01,0.05},
		{0.05,0.01,0.02,0.01,0.01,0.02,0.01,0.05},
		{0.1,0.01,0.05,0.02,0.02,0.05,0.01,0.1},
		{-0.25,-0.25,0.01,0.01,0.01,0.01,-0.25,-0.25},
		{1.0,-0.25,0.1,0.05,0.05,0.1,-0.25,1.0}
	};
	Chessborad cb = oj.getBorad();
	int max_player_piece_id = max_player.piece().id();
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		int piece_id = cb.get(i);
		if (piece_id != 0)
		{
			int x, y;
			i.coord(&x, &y);
			if (piece_id == max_player_piece_id)
				heuristic_value += fixed_weights[x][y];
			else
				heuristic_value -= fixed_weights[x][y];
		}
	}
	return heuristic_value;
}

AIplayer_Progressivebias::AIplayer_Progressivebias(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth)
	:Player(id, name, cp), time_ms(time_ms), iterations(iterations)
{
	last_iterations = 0;
	last_time = 0;
	this->max_depth = max_depth;
	mct = nullptr;
	mcts = nullptr;
}

AIplayer_Progressivebias::~AIplayer_Progressivebias()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

void AIplayer_Progressivebias::chess(const Chessjudge& cj, int* x, int* y)
{

	if (mct == nullptr)
	{
		Othellojudge* root_oj = new Othellojudge(*(dynamic_cast<const Othellojudge*>(&cj)));
		mct = new MCT((void*)root_oj, deldata);
		mcts = new MCTSOthello_Progressivebias(*mct);
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
			mcts = new MCTSOthello_Progressivebias(*mct);
		}
	}
	auto ptr = mcts->search(time_ms, iterations, max_depth, &last_time, &last_iterations);
	Othellojudge* temp_oj = (Othellojudge*)ptr->data;
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
	mct->changeRoot(ptr);
}

void AIplayer_Progressivebias::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;

	mct = nullptr;
	mcts = nullptr;
}

AIPlayer_for_root::AIPlayer_for_root(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth)
	:AIPlayer(id,name,cp,time_ms,iterations,max_depth)
{
}

void AIPlayer_for_root::chess(const Chessjudge& cj, int* x, int* y)
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
	//mct->changeRoot(ptr);
	if (x != nullptr && y != nullptr)
	{
		Othellojudge* temp_oj = (Othellojudge*)ptr->data;
		*x = temp_oj->last_chess[0];
		*y = temp_oj->last_chess[1];
	}
}

void AIPlayer_for_root::chess_merge(int* x, int* y)
{
	int max_total = -1;
	auto max_p = mct->getRoot().childIterator().get();
	for (auto i = mct->getRoot().childIterator(); !i.end(); i.next())
	{
		auto ptr = i.get();
		if (ptr->total > max_total)
		{
			max_total = ptr->total;
			max_p = ptr;
		}
	}
	Othellojudge* temp_oj = (Othellojudge*)max_p->data;
	mct->changeRoot(max_p);
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
}

void AIPlayer_for_root::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;

	mct = nullptr;
	mcts = nullptr;
}

