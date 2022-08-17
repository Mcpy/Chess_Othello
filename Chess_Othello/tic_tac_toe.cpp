#include "tic_tac_toe.h"

Ticjudge::Ticjudge(const Chessborad& cb, Player& player1, Player& player2)
	:Chessjudge(cb,player1,player2)
{
	initborad();
	last_chess[0] = -1;
	last_chess[1] = -1;
	last_player_id = -1;
}

Ticjudge::Ticjudge(const Ticjudge& cj)
	:Chessjudge(cj)
{
	last_chess[0] = -1;
	last_chess[1] = -1;
	last_player_id = !bell_flag;
}

void Ticjudge::initborad()
{
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		cb.set(i, 0);
	}
}

bool Ticjudge::chess(int x, int y)
{
	if (cb.get(x, y) == 0)
	{
		cb.set(x, y, player_list[bell_flag]->piece().id());
		last_player_id = player_list[bell_flag]->piece().id();
		last_chess[0] = x;
		last_chess[1] = y;
		bell_flag = !bell_flag;
		return 1;
	}
	else
		return 0;
}

bool Ticjudge::gameover(Player** p)
{
	Player *win_p = nullptr;
	const int step_x[] = { 1,1,0,-1,-1,-1,0,1 };
	const int step_y[] = { 0,1,1,1,0,-1,-1,-1 };
	bool over_flag = 0;
	int chess_num = 0;
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		int chess_id = cb.get(i);
		if (chess_id != 0 && chess_id != -1)
		{
			chess_num++;
			for (int n = 0; n < 8; n++)
			{
				auto ptr = i.copy();
				const int dir[] = { step_x[n],step_y[n] };
				bool win_flag = 1;
				for (int m = 0; m < 2; m++)
				{
					ptr.move(dir);
					if (cb.get(ptr) != chess_id)
					{
						win_flag = 0;
						break;
					}
				}
				if (win_flag)
				{
					if (chess_id == player_list[0]->piece().id())
					{
						win_p = player_list[0];
					}
					else
					{
						win_p = player_list[1];
					}
					over_flag = 1;
					//
					break;
				}

			}
		}
		if (over_flag)
		{
			break;
		}
	}
	if (chess_num == cb.getSize(0) * cb.getSize(1))
		over_flag = 1;
	if (p != nullptr)
		*p = win_p;
	return over_flag;
}

Player& Ticjudge::whoisNext()
{
	return *(player_list[bell_flag]);
}

TicRandomPlayer::TicRandomPlayer(int id, std::string name, Chesspiece& cp) 
	:Player(id, name, cp)
{
}

void TicRandomPlayer::chess(const Chessjudge& tj, int* x, int* y)
{
	*x = -1;
	*y = -1;
	Chessborad cb = tj.getBorad();
	int empty_num = 0;
	std::vector<Chessborad::Ptr> ava;
	for (auto i = cb.begin(); !i.end(); i.next())
	{
		if (cb.get(i) == 0)
		{
			ava.push_back(i.copy());
		}
	}
	if (!ava.empty())
	{
		srand(time(0));
		ava.at(rand() % ava.size()).coord(x, y);
	}
}

MCTSTic::MCTSTic(MCT& mct)
	:MCTS(mct)
{
	Ticjudge* tj = (Ticjudge*)mct.getRoot()->data;
	Player& p1 = tj->getPlayer(PLAYER1);
	Player& p2 = tj->getPlayer(PLAYER2);
	player1 = new TicRandomPlayer(p1.id(), p1.name(), p1.piece());
	player2 = new TicRandomPlayer(p2.id(), p2.name(), p2.piece());
}

MCTSTic::~MCTSTic()
{
	delete player1;
	delete player2;
}

double MCTSTic::selectFunction(MCT::Ptr& p) const
{
	MCT::Ptr root_ptr = mct.getRoot();
	Ticjudge* root_tj = (Ticjudge*)root_ptr->data;
	Ticjudge* p_tj = (Ticjudge*)p->data;
	if (root_tj->whoisNext().id() == p_tj->last_player_id)
	{
		return p->total ? (p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total)) : DBL_MAX;
	}
	else
	{
		return p->total ? (1.0 - p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total)) : DBL_MAX;
	}
}

bool MCTSTic::expansion(MCT::Ptr& p) const
{
	Ticjudge* tj = (Ticjudge*)p->data;
	if (tj->gameover())
	{
		p->termination_flag = 1;
		return 0;
	}
	else
	{
		Chessborad cb = tj->getBorad();
		for (auto i = cb.begin(); !i.end(); i.next())
		{
			if (cb.get(i) == 0)
			{
				Ticjudge* temp_tj = new Ticjudge(*tj);
				int x = -1, y = -1;
				i.coord(&x, &y);
				if (!temp_tj->chess(x, y))
					throw("MCTSTic::expension unknown error");
				mct.addChild(p, temp_tj);
			}
		}
		return 1;
	}
}

double MCTSTic::rollout(MCT::Ptr& p)
{
	Ticjudge tj(*(Ticjudge*)p->data);
	tj.changePlayer(PLAYER1, *player1);
	tj.changePlayer(PLAYER2, *player2);
	while (1)
	{
		Player* win_p = nullptr;
		Player& player = tj.whoisNext();
		int x = -1, y = -1;
		player.chess(tj, &x, &y);
		tj.chess(x, y);
		if (tj.gameover(&win_p))
		{
			if (win_p == nullptr)
			{
				return 0.0;
			}
			else
			{
				if (win_p->id() == ((Ticjudge*)mct.getRoot()->data)->whoisNext().id())
					return 1.0;
				else
					return -1.0;
			}
		}
	}

}

TicAIPlayer::TicAIPlayer(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth)
	:Player(id,name,cp),time_ms(time_ms),iterations(iterations),max_depth(max_depth)
{
	last_iterations = 0;
	last_time = 0;
	mct = nullptr;
	mcts = nullptr;
}

TicAIPlayer::~TicAIPlayer()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

void TicAIPlayer::chess(const Chessjudge& cj, int* x, int* y)
{
	if (mct == nullptr)
	{
		Ticjudge* root_oj = new Ticjudge(*(dynamic_cast<const Ticjudge*>(&cj)));
		mct = new MCT((void*)root_oj, [](void* data) {Ticjudge* oj = (Ticjudge*)data;delete oj; });
		mcts = new MCTSTic(*mct);
	}
	else
	{
		auto ptr = mct->find(&cj, [](const void* data1, const void* data2) {const Chessjudge* cj1 = (const Chessjudge*)data1; const Chessjudge* cj2 = (const Chessjudge*)data2; return (cj1->getBorad() == cj2->getBorad()); }); //寻找节点，继承已经迭代的数据
		if (ptr.valid())
		{
			mct->changeRoot(ptr);
		}
		else // 没有节点 重新建立树
		{
			init();
			Ticjudge* root_oj = new Ticjudge(*(dynamic_cast<const Ticjudge*>(&cj)));
			mct = new MCT((void*)root_oj, [](void* data) {Ticjudge* oj = (Ticjudge*)data; delete oj; });
			mcts = new MCTSTic(*mct);
		}
	}
	auto ptr_ = mcts->search(time_ms, iterations, max_depth, &last_time, &last_iterations);
	mct->changeRoot(ptr_);
	Ticjudge* temp_oj = (Ticjudge*)ptr_->data;
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
}

void TicAIPlayer::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;

	mct = nullptr;
	mcts = nullptr;
}

Player* ticgame(Player &p1,Player &p2)
{
	Chessborad cb(3, 3);
	Chesspiece cp_o(1, "o"), cp_x(2, "x");
	//TicRandomPlayer p1(1, "1", cp_o);
	Ticjudge tj(cb, p1, p2);

	Player* win_player = nullptr;
	while (1)
	{
		Player& p = tj.whoisNext();
		int x = -1, y = -1;
		p.chess(tj, &x, &y);
		tj.chess(x, y);
		//tj.getBorad().print();
		if (tj.gameover(&win_player))
		{
			/*if (win_player == nullptr)
			{
				std::cout << "draw" << std::endl;
			}
			else
			{
				std::cout << win_player->name() << " win!" << std::endl;
			}*/
			return win_player;
		}

	}
}

MCTSAMAFTic::MCTSAMAFTic(MCT& mct):MCTS(mct)
{
	Ticjudge* tj = (Ticjudge*)mct.getRoot()->data;
	Player& p1 = tj->getPlayer(PLAYER1);
	Player& p2 = tj->getPlayer(PLAYER2);
	player1 = new TicRandomPlayer(p1.id(), p1.name(), p1.piece());
	player2 = new TicRandomPlayer(p2.id(), p2.name(), p2.piece());
}

MCTSAMAFTic::~MCTSAMAFTic()
{
	delete player1;
	delete player2;
}

double MCTSAMAFTic::selectFunction(MCT::Ptr& p) const
{
	MCT::Ptr root_ptr = mct.getRoot();
	Ticjudge* root_tj = (Ticjudge*)root_ptr->data;
	Ticjudge* p_tj = (Ticjudge*)p->data;
	if (root_tj->whoisNext().id() == p_tj->last_player_id)
	{
		return p->total ? (p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total)) : DBL_MAX;
	}
	else
	{
		return p->total ? (1.0 - p->score / p->total + 2 * sqrt(log(root_ptr->total) / p->total)) : DBL_MAX;
	}
}

bool MCTSAMAFTic::expansion(MCT::Ptr& p) const
{
	Ticjudge* tj = (Ticjudge*)p->data;
	if (tj->gameover())
	{
		p->termination_flag = 1;
		return 0;
	}
	else
	{
		Chessborad cb = tj->getBorad();
		for (auto i = cb.begin(); !i.end(); i.next())
		{
			if (cb.get(i) == 0)
			{
				Ticjudge* temp_tj = new Ticjudge(*tj);
				int x = -1, y = -1;
				i.coord(&x, &y);
				if (!temp_tj->chess(x, y))
					throw("MCTSTic::expension unknown error");
				mct.addChild(p, temp_tj);
			}
		}
		return 1;
	}
}

double MCTSAMAFTic::rollout(MCT::Ptr& p)
{
	Ticjudge tj(*(Ticjudge*)p->data);
	tj.changePlayer(PLAYER1, *player1);
	tj.changePlayer(PLAYER2, *player2);
	while (1)
	{
		Player* win_p = nullptr;
		Player& player = tj.whoisNext();
		int x = -1, y = -1;
		player.chess(tj, &x, &y);
		if (tj.chess(x, y))
		{
			int *temp = new int[2];
			temp[0] = x;
			temp[1] = y;
			rollout_step[player.id() == player2->id()].push_back(temp);
		}
		if (tj.gameover(&win_p))
		{
			if (win_p == nullptr)
			{
				return 0.0;
			}
			else
			{
				if (win_p->id() == ((Ticjudge*)mct.getRoot()->data)->whoisNext().id())
					return 1.0;
				else
					return -1.0;
			}
		}
	}
}

void MCTSAMAFTic::backup(MCT::Ptr& p, double score)
{
	for (MCT::Ptr i = p; i.valid(); i = i.parent())
	{
		i->score += score;
		i->total++;
		for (auto j = i.childIterator(); !j.end(); j.next())
		{
			Ticjudge* tj = (Ticjudge*)j.get()->data;
			for (auto k = rollout_step[tj->last_player_id==player2->id()].begin(); k != rollout_step[tj->last_player_id == player2->id()].end(); k++)
			{
				if ((*k)[0] == tj->last_chess[0] && (*k)[1] == tj->last_chess[1])
				{
					j.get()->score += score;
					j.get()->total++;
					break;
				}
			}
		}
	}

	for (auto i : rollout_step)
	{
		for (auto j : i)
		{
			delete[] j;
		}
	}
	rollout_step[0].clear();
	rollout_step[1].clear();
}

TicAMAFPlayer::TicAMAFPlayer(int id, std::string name, Chesspiece& cp, int64_t time_us, int iterations, int max_depth)
	:Player(id, name, cp), time_us(time_us), iterations(iterations), max_depth(max_depth)
{
	last_iterations = 0;
	last_time = 0;
	mct = nullptr;
	mcts = nullptr;
}

TicAMAFPlayer::~TicAMAFPlayer()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;
}

void TicAMAFPlayer::chess(const Chessjudge& cj, int* x, int* y)
{
	if (mct == nullptr)
	{
		Ticjudge* root_oj = new Ticjudge(*(dynamic_cast<const Ticjudge*>(&cj)));
		mct = new MCT((void*)root_oj, [](void* data) {Ticjudge* oj = (Ticjudge*)data; delete oj; });
		mcts = new MCTSAMAFTic(*mct);
	}
	else
	{
		auto ptr = mct->find(&cj, [](const void* data1, const void* data2) {const Chessjudge* cj1 = (const Chessjudge*)data1; const Chessjudge* cj2 = (const Chessjudge*)data2; return (cj1->getBorad() == cj2->getBorad()); }); //寻找节点，继承已经迭代的数据
		if (ptr.valid())
		{
			mct->changeRoot(ptr);
		}
		else // 没有节点 重新建立树
		{
			init();
			Ticjudge* root_oj = new Ticjudge(*(dynamic_cast<const Ticjudge*>(&cj)));
			mct = new MCT((void*)root_oj, [](void* data) {Ticjudge* oj = (Ticjudge*)data; delete oj; });
			mcts = new MCTSAMAFTic(*mct);
		}
	}
	auto ptr_ = mcts->search(time_us, iterations, max_depth, &last_time, &last_iterations);
	mct->changeRoot(ptr_);
	Ticjudge* temp_oj = (Ticjudge*)ptr_->data;
	*x = temp_oj->last_chess[0];
	*y = temp_oj->last_chess[1];
}

void TicAMAFPlayer::init()
{
	if (mct != nullptr)
		delete mct;
	if (mcts != nullptr)
		delete mcts;

	mct = nullptr;
	mcts = nullptr;
}
