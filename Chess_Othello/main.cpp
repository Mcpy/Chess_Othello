#include"Othello.h"
#include"MCTS.h"
#include"tic_tac_toe.h"
#include<iostream>
using namespace std;

int main()
{
	const int window_size[] = { 1280,720 };

	const int borad_left_top[] = { 320,40 };
	const int borad_right_bottom[] = { 960,680 };

	const int black_msg_left_top[] = { 60,335 };
	const int black_msg_left_bottom[] = { 260,385 };

	const int white_msg_left_top[] = { 1020,335 };
	const int white_msg_right_bottom[] = { 1220,385 };

	const int borad_size[] = { 8,8 };

	ThreadPool tp(20, 30, 10, 60);

	Chessborad cb(borad_size[0], borad_size[1]);
	Chesspiece black_p(1, string("black")),white_p(2,string("white"));
	//HumanPlayer player1(1, string("player1"), black_p, borad_left_top, borad_right_bottom, borad_size);
	//RandomPlayer player1(1, string("player1"), black_p);
	AIPlayer player1(1, string("player1"), black_p, 500, NO_LIMITS, 5);
	//HumanPlayer player2(2, string("player2"), white_p, borad_left_top, borad_right_bottom, borad_size);
	//RandomPlayer player2(2, string("player2"), white_p);
	AIPlayer player2(2, string("player2"), white_p, 500, NO_LIMITS, 5);
	//AIPlayer_Thread player2(2, string("player2"), white_p, 1000, NO_LIMITS, 5, tp, 10);
	//AIplayer_Progressivebias player2(2, string("player2"), white_p, NO_LIMITS, 1000, 5);
	Othellojudge oj(cb, player1, player2);

	//display
	Window window(window_size[0], window_size[1]);
	setbkcolor(RGB(195, 170, 135));
	setlinecolor(RGB(50, 50, 50));
	cleardevice();
	Displayothello dsp_o(oj, borad_left_top[0], borad_left_top[1], borad_right_bottom[0], borad_right_bottom[1], string("borad"));
	TextBar black_msg(black_msg_left_top[0], black_msg_left_top[1], black_msg_left_bottom[0], black_msg_left_bottom[1], string("Black: 2"), BLACK, string("black msg"));
	TextBar white_msg(white_msg_left_top[0], white_msg_left_top[1], white_msg_right_bottom[0], white_msg_right_bottom[1], string("White: 2"), WHITE, string("white msg"));
	window.draw(dsp_o);
	window.draw(black_msg);
	window.draw(white_msg);


	while (1)
	{
		Player* win_player = nullptr;
		Player& p = oj.whoisNext();
		window.reflash();
		int b_x = -1, b_y = -1;
		p.chess(oj, &b_x, &b_y);
		oj.chess(b_x, b_y);
		
		black_msg.set(string("Black: ") + to_string(oj.getPiecesNum(BLACKPIECE)));
		white_msg.set(string("White: ") + to_string(oj.getPiecesNum(WHITEPIECE)));
		window.reflash();
		if (oj.gameover(&win_player))
		{
			const TCHAR* tc = _T("");
			if (win_player == nullptr)
			{
				tc = _T("Draw");
			}
			else
			{
				if (win_player->id() == player1.id())
				{
					tc = _T("Black Win!");
				}
				if (win_player->id() == player2.id())
				{
					tc = _T("White Win!");
				}
			}
			window.messageBox(tc, _T("Game Over"), MB_OK);
			oj.initborad();
			player1.init();
			player2.init();
			black_msg.set(string("Black: 2"));
			white_msg.set(string("White: 2"));
			window.reflash();
		}
	}
	return 0;
}

int experiment(Player& player_black, Player& player_white);
void runExperiment(Player& player_black, Player& player_white, int times);
void ticRunEx(Player& p_x, Player& p_o, int times);

int main_ex()
{	
	Chesspiece p_x(1, string("x")), p_o(2, string("o"));


	{
		//TicRandomPlayer p2(2, "0", p_o);
		TicAMAFPlayer p1(1, "x", p_x, 1, NO_LIMITS, NO_LIMITS);
		TicAIPlayer p2(2, "o", p_o, 1, NO_LIMITS, NO_LIMITS);
		std::cout << "AMAF 1ms vs MCTS 1ms" << std::endl;
		ticRunEx(p1, p2, 100);
	}
	{
		//TicRandomPlayer p2(2, "0", p_o);
		TicAMAFPlayer p1(1, "x", p_x, 2, NO_LIMITS, NO_LIMITS);
		TicAIPlayer p2(2, "o", p_o, 2, NO_LIMITS, NO_LIMITS);
		std::cout << "AMAF 2ms vs MCTS 2ms" << std::endl;
		ticRunEx(p1, p2, 100);
	}
	{
		//TicRandomPlayer p2(2, "0", p_o);
		TicAMAFPlayer p1(1, "x", p_x, 5, NO_LIMITS, NO_LIMITS);
		TicAIPlayer p2(2, "o", p_o, 5, NO_LIMITS, NO_LIMITS);
		std::cout << "AMAF 5ms vs MCTS 5ms" << std::endl;
		ticRunEx(p1, p2, 100);
	}
	{
		//TicRandomPlayer p2(2, "0", p_o);
		TicAMAFPlayer p1(1, "x", p_x, 10, NO_LIMITS, NO_LIMITS);
		TicAIPlayer p2(2, "o", p_o, 10, NO_LIMITS, NO_LIMITS);
		std::cout << "AMAF 10ms vs MCTS 10ms" << std::endl;
		ticRunEx(p1, p2, 100);
	}
	{
		//TicRandomPlayer p2(2, "0", p_o);
		TicAMAFPlayer p1(1, "x", p_x, 50, NO_LIMITS, NO_LIMITS);
		TicAIPlayer p2(2, "o", p_o, 50, NO_LIMITS, NO_LIMITS);
		std::cout << "AMAF 50ms vs MCTS 50ms" << std::endl;
		ticRunEx(p1, p2, 100);
	}
	{
		//TicRandomPlayer p2(2, "0", p_o);
		TicAMAFPlayer p1(1, "x", p_x, 100, NO_LIMITS, NO_LIMITS);
		TicAIPlayer p2(2, "o", p_o, 100, NO_LIMITS, NO_LIMITS);
		std::cout << "AMAF 100ms vs MCTS 100ms" << std::endl;
		ticRunEx(p1, p2, 100);
	}

	Chesspiece black_p(1, string("black")), white_p(2, string("white"));

	//verify
	{
		//MCTS(0.1) vs MCTS(1)
		std::cout << "MCTS(0.1) vs MCTS(1)" << std::endl;
		AIPlayer p1(1, "p1", black_p, 100, NO_LIMITS, 5);
		AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
		runExperiment(p1, p2, 20);
	}
	{
		//MCTS(0.5) vs MCTS(1)
		std::cout << "MCTS(0.5) vs MCTS(1)" << std::endl;
		AIPlayer p1(1, "p1", black_p, 500, NO_LIMITS, 5);
		AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
		runExperiment(p1, p2, 20);
	}
	{
		//MCTS(1) vs MCTS(2)
		std::cout << "MCTS(1) vs MCTS(2)" << std::endl;
		AIPlayer p1(1, "p1", black_p, 1000, NO_LIMITS, 5);
		AIPlayer p2(2, "p2", white_p, 2000, NO_LIMITS, 5);
		runExperiment(p1, p2, 20);
	}
	{
		//MCTS(2) vs MCTS(4)
		std::cout << "MCTS(2) vs MCTS(4)" << std::endl;
		AIPlayer p1(1, "p1", black_p, 2000, NO_LIMITS, 5);
		AIPlayer p2(2, "p2", white_p, 4000, NO_LIMITS, 5);
		runExperiment(p1, p2, 20);
	}
	{
		//MCTS(4) vs MCTS(8)
		std::cout << "MCTS(4) vs MCTS(8)" << std::endl;
		AIPlayer p1(1, "p1", black_p, 4000, NO_LIMITS, 5);
		AIPlayer p2(2, "p2", white_p, 8000, NO_LIMITS, 5);
		runExperiment(p1, p2, 20);
	}
	// Thread
	{
		ThreadPool tp(20, 30, 10, 60);
		// 5,10,15,20//leaf/root
		{
			//MCTS_root(5,1) vs MCTS(1)
			std::cout << "MCTS_root(5,1) vs MCTS(1)" << std::endl;
			//AIPlayer p1(1, "p1", black_p, 1000, NO_LIMITS, 5);
			AIPlayer_Root_Para p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 5);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 10);
		}
		{
			//MCTS_root(10,1) vs MCTS(1)
			std::cout << "MCTS_root(10,1) vs MCTS(1)" << std::endl;
			AIPlayer_Root_Para p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 10);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 20);
		}
		{
			//MCTS_root(15,1) vs MCTS(1)
			std::cout << "MCTS_root(15,1) vs MCTS(1)" << std::endl;
			AIPlayer_Root_Para p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 15);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 20);
		}
		{
			//MCTS_root(20,1) vs MCTS(1)
			std::cout << "MCTS_root(20,1) vs MCTS(1)" << std::endl;
			AIPlayer_Root_Para p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 20);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 20);
		}
		{
			//MCTS_leaf(5,1) vs MCTS(1)
			std::cout << "MCTS_leaf(5,1) vs MCTS(1)" << std::endl;
			AIPlayer_Thread p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 5);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 100);
		}
		{
			//MCTS_leaf(10,1) vs MCTS(1)
			std::cout << "MCTS_leaf(10,1) vs MCTS(1)" << std::endl;
			AIPlayer_Thread p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 10);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 100);
		}
		{
			//MCTS_leaf(15,1) vs MCTS(1)
			std::cout << "MCTS_leaf(15,1) vs MCTS(1)" << std::endl;
			AIPlayer_Thread p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 15);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 100);
		}
		{
			//MCTS_leaf(20,1) vs MCTS(1)
			std::cout << "MCTS_leaf(20,1) vs MCTS(1)" << std::endl;
			AIPlayer_Thread p1(1, "p1", black_p, 1000, NO_LIMITS, 5, tp, 20);
			AIPlayer p2(2, "p2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 100);
		}
	}

	{
		// AMAF vs MCTS
		cout << "AMAF vs MCTS" << endl;
		AMAFPlayer p1(1, "player1", black_p, 1000, NO_LIMITS, 5);
		AIPlayer p2(2, "player2", white_p, 1000, NO_LIMITS, 5);
		runExperiment(p1, p2, 100);
	}

	//para
	{
		ThreadPool tp(20, 30, 25, 60);
		{
			cout << "MCTS PARALLEL(5) vs MCTS(5)" << endl;
			AIPlayer_Thread p1(1, "player1", black_p, 1000, NO_LIMITS, 5, tp, 5);
			AIPlayer p2(2, "player2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 10);
		}
		{
			cout << "MCTS PARALLEL(10) vs MCTS(10)" << endl;
			AIPlayer_Thread p1(1, "player1", black_p, 1000, NO_LIMITS, 5, tp, 10);
			AIPlayer p2(2, "player2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 10);
		}
		{
			cout << "MCTS PARALLEL(15) vs MCTS(15)" << endl;
			AIPlayer_Thread p1(1, "player1", black_p, 1000, NO_LIMITS, 5, tp, 15);
			AIPlayer p2(2, "player2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 10);
		}
		{
			cout << "MCTS PARALLEL(20) vs MCTS(20)" << endl;
			AIPlayer_Thread p1(1, "player1", black_p, 1000, NO_LIMITS, 5, tp, 20);
			AIPlayer p2(2, "player2", white_p, 1000, NO_LIMITS, 5);
			runExperiment(p1, p2, 10);
		}
	}

	// ramdom vs MCTS_Progressivebias
	cout << "ramdom vs MCTS_Progressivebias:" << endl;
	RandomPlayer player_ramdom_1(1, string("player1"), black_p);
	//AIPlayer player_ai_1(2, string("player2"), white_p, 1000, NO_LIMITS, 5);
	AIplayer_Progressivebias player_ai_1(2, string("player2"), white_p, 1000, NO_LIMITS, 5);
	runExperiment(player_ramdom_1, player_ai_1, 0);

	//MCTS vs MCTS_Progressivebias
	cout << "MCTS vs MCTS_Progressivebias:" << endl;
	AIPlayer player_ai_2(1, string("player1"), black_p, 1000, NO_LIMITS, NO_LIMITS);
	//RandomPlayer player_ramdom_2(2, string("player2"), white_p);
	AIplayer_Progressivebias player_ai_p_2(2, string("player2"), white_p, 1000, NO_LIMITS, NO_LIMITS);
	runExperiment(player_ai_2, player_ai_p_2, 20);

	//MCTS 0.1
	cout << "MCTS 0.2:" << endl;
	AIPlayer player1_200ms(1, string("player1"), black_p, 200, NO_LIMITS, 5);
	AIPlayer player2_200ms(2, string("player2"), white_p, 200, NO_LIMITS, 5);
	runExperiment(player1_200ms, player2_200ms, 100);

	//cout << "MCTS 0.5:" << endl;
	AIPlayer player1_500ms(1, string("player1"), black_p, 500, NO_LIMITS, 5);
	AIPlayer player2_500ms(2, string("player2"), white_p, 500, NO_LIMITS, 5);
	runExperiment(player1_500ms, player2_500ms, 100);

	//MCTS 1
	cout << "MCTS 5:" << endl;
	AIPlayer player1_5(1, string("player1"), black_p, 5000, NO_LIMITS, 5);
	AIPlayer player2_5(2, string("player2"), white_p, 5000, NO_LIMITS, 5);
	runExperiment(player1_5, player2_5, 100);

	//MCTS 10
	cout << "MCTS 10:" << endl;
	AIPlayer player1_10(1, string("player1"), black_p, 10000, NO_LIMITS, 5);
	AIPlayer player2_10(2, string("player2"), white_p, 10000, NO_LIMITS, 5);
	runExperiment(player1_10, player2_10, 100);

	return 0;
}

int experiment(Player& player_black, Player& player_white)
{
	const int borad_size[] = { 8,8 };
	Chessborad cb(borad_size[0], borad_size[1]);
	Othellojudge oj(cb, player_black, player_white);

	while (1)
	{
		Player* win_player = nullptr;
		Player& p = oj.whoisNext();
		int b_x = -1, b_y = -1;
		p.chess(oj, &b_x, &b_y);
		oj.chess(b_x, b_y);

		if (oj.gameover(&win_player))
		{
			if (win_player == nullptr)
			{
				return 0;
			}
			else
			{
				if (win_player->id() == 1)
					int a = 1;////////////////////////
				return win_player->id();
			}
		}
	}
}

void runExperiment(Player& player_black, Player& player_white, int times)
{
	int statistics_b_win = 0, statistics_w_win = 0, statistics_draw = 0;
	for (int i = 0; i < times;)
	{
		player_black.init();
		player_white.init();
		int win_id = -1;
		try
		{
			win_id = experiment(player_black, player_white);
		}
		catch (const char* msg)
		{
			cout << msg << endl;
			continue;
		}
		catch (exception& e)
		{
			cout << e.what() << endl;
			continue;
		}
		catch (...)
		{
			cout << "unknown exception" << endl;
			continue;
		}

		if (win_id == 0)
			statistics_draw++;
		if (win_id == player_black.id())
			statistics_b_win++;
		if (win_id == player_white.id())
			statistics_w_win++;
		i++;
	}
	cout << "Black Win: " << statistics_b_win << " White Win: " << statistics_w_win << " Draw: " << statistics_draw << endl;
}

void ticRunEx(Player& p_x, Player& p_o, int times)
{
	int statistics_o_win = 0, statistics_x_win = 0, statistics_draw = 0;
	for (int i = 0; i < times; i++)
	{
		Player* win_p = ticgame(p_x, p_o);
		if (win_p == nullptr)
		{
			statistics_draw++;
		}
		else
		{
			if (win_p->id() == p_x.id())
			{
				statistics_x_win++;
			}
			else
			{
				statistics_o_win++;
			}
		}
	}
	std::cout << "x: " << statistics_x_win << " o: " << statistics_o_win << " draw: " << statistics_draw << std::endl;
}
