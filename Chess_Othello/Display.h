#pragma once
//#include <graphics.h>
#include<easyx.h>
#include <string>

class Display
{
protected:
	std::string tags;
public:
	std::string tag() const;
	virtual void display() = 0;
	virtual void clear() = 0;
};

class Displaylist
{
public:
	struct Node
	{
		Display* dsp;
		Node* next;
	};

public:
	class Ptr
	{
	private:
		Node* ptr;
	public:
		Ptr(Node* head);
		Ptr(const Ptr& p);
		void next();
		bool end();
		Display* get();
	};
protected:
	Node* head, * tail;
	int length;

public:
	Displaylist();
	~Displaylist();
	void add(Display* dsp);
	Display* pop();
	Display* pop(std::string tags);
	void remove(std::string tags);
	void clear();
	int len();
	Ptr begin();
	Display* get(Ptr& p);
	Display* get(std::string tags);
	Display* operator[](const int n);
};



class Window
{
protected:
	int width;
	int height;
	HWND handle;
	Displaylist dis_list;


public:
	Window(int width, int height, int flag = 0);
	void clearall();
	~Window();
	void draw(Display& dsp);
	void clear(std::string tags);// maybe clear other things
	void reflash();//clear all and darw from list
	void del(std::string tags);// del the Display from list but not clear;
	Display* get(std::string tags);
	HWND gethandle();
	void messageBox(LPCWSTR msg, LPCWSTR title, UINT flags);
};

class TextBar :public Display
{
protected:
	std::string str;
	TCHAR* tc;
	int left_top[2], right_bottom[2];
	void char2tchar();
	COLORREF color;
public:
	TextBar(int x1, int y1, int x2, int y2, std::string str, COLORREF text_color, std::string tags);
	~TextBar();
	void set(std::string str);
	void display();
	void clear();
};

class Button : public TextBar
{
protected:
	void (*callback)();
public:
	Button(int x1, int y1, int x2, int y2, void (*callback)(), std::string text, std::string tags);
	void display();
	void clear();
};