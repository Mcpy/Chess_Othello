#pragma once
#include<string>
#include<algorithm>
#include<iostream>
#define PLAYER1 0
#define PLAYER2 1
class Chessborad
{
public:
	class Ptr
	{
	private:
		int ptr_x;
		int ptr_y;
		int num;
		int num_max;
		int size[2];
	public:
		Ptr(const int(&size)[2]);
		Ptr(const Ptr& p);
		bool end();
		void coord(int* x, int* y);
		Ptr operator++(int);
		void next();
		Ptr copy();
		void move(const int direction[2], int path = 1);
		bool valid();
	};

private:
	void malloc();
	void del();
	void copy(int** b);
protected:
	int** borad;
	int size[2];
	//int total_chess;
	//int num_of_chess1;
	//int num_of_chess2;
public:
	Chessborad(int size_x, int size_y);
	Chessborad(const Chessborad& cb);
	~Chessborad();
	void init();
	int get(int x, int y) const;
	int get(Ptr& p) const;
	void set(int x, int y, int chess_id);
	void set(Ptr& p, int chess_id);
	int getSize(bool dim = 0); // 0 - x; 1 - y
	void print() const;
	Ptr begin();
	void operator=(const Chessborad& cb);
	int* operator[](const int x);
	bool operator==(const Chessborad& cb);
	bool operator!=(const Chessborad& cb);
};


class Chesspiece
{
protected:
	int chess_id;
	std::string chess_name;
	void (*dsp)(int, int); //display
public:
	Chesspiece(int id, std::string name, void (*dsp)(int, int) = nullptr);
	int id() const;
	std::string name() const;
};


class Chessjudge;

class Player
{
protected:
	int player_id;
	std::string player_name;
	Chesspiece* cp;
public:
	Player(int id, std::string name, Chesspiece& cp); // need bind piece
	int id() const;
	std::string name() const;
	Chesspiece& piece() const;
	virtual void chess(Chessborad cb, int* x, int* y);
	virtual void chess(const Chessjudge& cj, int* x, int* y); //cj转化为子类 dynamic_cast<const 子类名*>
	virtual void init();
	bool operator==(const Player& p) const;
};


class Chessjudge
{
protected:
	Chessborad cb;
	Player* player_list[2];
	bool bell_flag;
public:
	Chessjudge(const Chessborad& cb, Player& player1, Player& player2);
	Chessjudge(const Chessjudge& cj);
	void changePlayer(bool player_flag, Player& player);
	Chessborad getBorad() const;
	virtual void initborad() = 0;
	virtual bool chess(int x, int y) = 0; // 0 invalid chess; 1 valid chess;
	virtual bool gameover(Player** p) = 0; // 0 not over; 1 gameover; p - who win, if gameover and p is nullptr is draw
	virtual Player& whoisNext() = 0;// which player can chess
	Player& getCurrentPlayer() const;
	Player& getPlayer(bool player_flag) const;
};