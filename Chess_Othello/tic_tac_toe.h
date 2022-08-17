#pragma once
#include"Chess.h"
#include"MCTS.h"
#include<vector>
#include<ctime>
class Ticjudge:public Chessjudge
{
public:
	int last_chess[2];
	int last_player_id;
	Ticjudge(const Chessborad& cb, Player& player1, Player& player2);
	Ticjudge(const Ticjudge& cj);
	void initborad();
	bool chess(int x, int y);
	bool gameover(Player** p = nullptr);
	Player& whoisNext();
};



class TicRandomPlayer :public Player
{
public:
	TicRandomPlayer(int id, std::string name, Chesspiece& cp);
	void chess(const Chessjudge& tj, int* x, int* y);
};

class MCTSTic :public MCTS
{
protected:
	TicRandomPlayer* player1, * player2;
public:
	MCTSTic(MCT& mct);
	~MCTSTic();
	double selectFunction(MCT::Ptr& p) const;
	bool expansion(MCT::Ptr& p) const;
	double rollout(MCT::Ptr& p);
};

class MCTSAMAFTic :public MCTS
{
protected:
	TicRandomPlayer* player1, * player2;
	std::list<int*> rollout_step[2];
public:
	MCTSAMAFTic(MCT& mct);
	~MCTSAMAFTic();
	double selectFunction(MCT::Ptr& p) const;
	bool expansion(MCT::Ptr& p) const;
	double rollout(MCT::Ptr& p);
	void backup(MCT::Ptr& p, double score);
};

class TicAMAFPlayer :public Player
{
protected:
	int64_t time_us;
	int iterations;
	int max_depth;
	MCT* mct;
	MCTSAMAFTic* mcts;
public:
	int64_t last_time;
	int last_iterations;
	TicAMAFPlayer(int id, std::string name, Chesspiece& cp, int64_t time_us, int iterations, int max_depth);
	~TicAMAFPlayer();
	void chess(const Chessjudge& cj, int* x, int* y);
	void init();
};

class TicAIPlayer :public Player
{
protected:
	int64_t time_ms;
	int iterations;
	int max_depth;
	MCT* mct;
	MCTSTic* mcts;

public:
	int64_t last_time;
	int last_iterations;
	TicAIPlayer(int id, std::string name, Chesspiece& cp, int64_t time_us, int iterations, int max_depth);
	~TicAIPlayer();
	void chess(const Chessjudge& cj, int* x, int* y);
	void init();

};

Player* ticgame(Player& p1, Player& p2);