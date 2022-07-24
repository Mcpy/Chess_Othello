#pragma once
#include"Chess.h"
#include"Display.h"
#include"MCTS.h"
#include<vector>
#include<ctime>
#include<cmath>

#define BLACKPIECE false
#define WHITEPIECE true


//ºÚÆåÏÈÂä×Ó
//°× ºÚ
//ºÚ °×
class Othellojudge :public Chessjudge
{

protected:
	void availability();
	int index(const Player& p);
	int player_chess_num[2];
	std::vector<Chessborad::Ptr> availability_point;
	bool turn(int x, int y, const int dir[2]); //·­×ªÆå×Ó

public:
	int last_chess[2];
	int last_player_id;
	Othellojudge(const Chessborad& cb, Player& player_black, Player& player_white); //player1 ºÚ£» player2 °×
	Othellojudge(const Othellojudge& oj);
	void initborad();
	bool chess(int x, int y); // 0 invalid chess; 1 valid chess;
	bool gameover(Player** p = nullptr); // 0 not over; 1 gameover; p - who win, if gameover and p is nullptr is draw
	Player& whoisNext();
	Chesspiece& getPiece(bool player_flag) const;
	const std::vector<Chessborad::Ptr>& getAvailability() const;
	int getPiecesNum(bool player_flag) const; // 0 black 1 white;
};


class Displayothello :public Display
{
protected:
	const Othellojudge* cj;
	int left_top[2], right_bottom[2];
	int x_path, y_path;
	Chesspiece& black, & white;
	void b2p(int b_x, int b_y, int* p_x, int* p_y);
public:
	Displayothello(const Othellojudge& cj, int x1, int y1, int x2, int y2, std::string tags);
	void display();
	void clear();
};

class HumanPlayer :public Player
{
protected:
	int left_top[2], right_bottom[2], size[2], x_path, y_path;
	void p2b(int p_x, int p_y, int* b_x, int* b_y) const;

public: 
	HumanPlayer(int id, std::string name, Chesspiece& cp,const int left_top[2],const int right_bottom[2],const int size[2]);
	void chess(Chessborad cb, int* x, int* y);
	void chess(const Chessjudge& oj, int* x, int* y);
};

class RandomPlayer :public Player
{
public:
	RandomPlayer(int id, std::string name, Chesspiece& cp);
	void chess(const Chessjudge& oj, int* x, int* y);
};

class MCTSOthello :public MCTS
{
protected:
	RandomPlayer *player1, *player2;
public:
	MCTSOthello(MCT& mct);
	~MCTSOthello();
	double selectFunction(MCT::Ptr& p) const;
	bool expansion(MCT::Ptr& p) const;
	double rollout(MCT::Ptr& p) const;
};


class AIPlayer :public Player
{
protected:
	int64_t time_ms;
	int iterations;
	int max_depth;
	MCT *mct;
	MCTSOthello *mcts;
public:
	int64_t last_time;
	int last_iterations;
	AIPlayer(int id, std::string name, Chesspiece& cp, int64_t time_ms, int iterations, int max_depth);
	~AIPlayer();
	void chess(const Chessjudge& cj, int* x, int* y);
	void init();
};

void deldata(void* data);