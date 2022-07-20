#include"Othello.h"
#include"MCTS.h"
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



	Chessborad cb(borad_size[0], borad_size[1]);
	Chesspiece black_p(1, string("black")),white_p(2,string("white"));
	//HumanPlayer player1(1, string("player1"), black_p, borad_left_top, borad_right_bottom, borad_size);
	RandomPlayer player1(1, string("player1"), black_p);
	//HumanPlayer player2(2, string("player2"), white_p, borad_left_top, borad_right_bottom, borad_size);
	//RandomPlayer player2(2, string("player2"), white_p);
	AIPlayer player2(2, string("player2"), white_p, 2000, NO_LIMITS, 5);
	Othellojudge oj(cb, player1, player2);

	//display
	Window window(window_size[0], window_size[1], EW_SHOWCONSOLE);
	setbkcolor(RGB(195, 170, 135));
	setlinecolor(RGB(50, 50, 50));
	cleardevice();
	Displayothello dsp_o(oj, borad_left_top[0], borad_left_top[1], borad_right_bottom[0], borad_right_bottom[1], string("borad"));
	TextBar black_msg(black_msg_left_top[0], black_msg_left_top[1], black_msg_left_bottom[0], black_msg_left_bottom[1], string("Black: 2"), BLACK, string("black msg"));
	TextBar white_msg(white_msg_left_top[0], white_msg_left_top[1], white_msg_right_bottom[0], white_msg_right_bottom[1], string("White: 2"), WHITE, string("white msg"));
	window.draw(dsp_o);
	window.draw(black_msg);
	window.draw(white_msg);

	int statistics_total = 0, statistics_b_win = 0, statistics_w_win = 0, statistics_draw = 0;
	//int& player2_iterations = player2.last_iterations;
	while (1)
	{
		Player* win_player = nullptr;
		Player& p = oj.whoisNext();
		int b_x = -1, b_y = -1;
		p.chess(oj, &b_x, &b_y);
		oj.chess(b_x, b_y);
		
		black_msg.set(string("Black: ") + to_string(oj.getPiecesNum(BLACKPIECE)));
		white_msg.set(string("White: ") + to_string(oj.getPiecesNum(WHITEPIECE)));
		//cout << player2_iterations << endl;
		window.reflash();
		if (oj.gameover(&win_player))
		{
			statistics_total++;
			const TCHAR* tc = _T("");
			if (win_player == nullptr)
			{
				tc = _T("Draw");
				statistics_draw++;
			}
			else
			{
				if (win_player->id() == player1.id())
				{
					tc = _T("Black Win!");
					statistics_b_win++;
				}
				if (win_player->id() == player2.id())
				{
					tc = _T("White Win!");
					statistics_w_win++;
				}
			}
			MessageBox(window.gethandle(), tc, _T("Game Over"), MB_OK);
			oj.initborad();
			black_msg.set(string("Black: 2"));
			white_msg.set(string("White: 2"));
			window.reflash();
		}
		
	}
	return 0;
}
