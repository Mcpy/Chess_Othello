#include "Display.h"

//Displaylisy

	//Displaylisy::Ptr
Displaylist::Ptr::Ptr(Node* head)
{
	ptr = head;
}

Displaylist::Ptr::Ptr(const Ptr& p)
{
	ptr = p.ptr;
}

void Displaylist::Ptr::next()
{
	if (ptr != nullptr)
		ptr = ptr->next;
}

bool Displaylist::Ptr::end()
{
	return (ptr == nullptr);
}

Display* Displaylist::Ptr::get()
{
	Display* dsp = nullptr;
	if (ptr != nullptr)
		dsp = ptr->dsp;
	return dsp;
}

//Displaylisy
Displaylist::Displaylist() :head(nullptr), tail(nullptr), length(0) {}

Displaylist::~Displaylist()
{
	clear();
}

void Displaylist::add(Display* dsp)
{
	Node* ptr = new Node;
	ptr->dsp = dsp;
	ptr->next = nullptr;
	if (head == nullptr)
	{
		head = ptr;
		tail = ptr;
	}
	else
	{
		tail->next = ptr;
		tail = ptr;
	}
	length++;
}

Display* Displaylist::pop()
{
	Display* dsp = nullptr;
	if (head != nullptr)
	{
		Node* ptr = head;
		dsp = head->dsp;
		head = head->next;
		delete ptr;
		length--;
		if (head == nullptr)
			tail = nullptr;
	}
	return dsp;
}

Display* Displaylist::pop(std::string tags)
{
	Display* dsp = nullptr;
	if (head != nullptr)
	{
		if (head->dsp->tag() == tags)
		{
			Node* n = head;
			dsp = head->dsp;
			head = head->next;
			delete n;
			length--;
			if (head == nullptr)
			{
				tail = nullptr;
			}
		}
		else
		{
			for (Node* i = head, *j = head->next; j != nullptr; i = i->next, j = j->next)
			{
				if (j->dsp->tag() == tags)
				{
					i->next = j->next;
					if (j == tail)
						tail = i;
					dsp = j->dsp;
					delete j;
					length--;
					break;
				}
			}
		}
	}
	return dsp;
}

void Displaylist::remove(std::string tags)
{
	while (pop(tags) != nullptr);
}

void Displaylist::clear()
{
	if (head != nullptr)
	{
		for (Node* i = head; i != nullptr; i = head)
		{
			head = head->next;
			delete i;
		}
		head = nullptr;
		tail = nullptr;
		length = 0;
	}
}

int Displaylist::len()
{
	return length;
}

Displaylist::Ptr Displaylist::begin()
{
	return Ptr(head);
}

Display* Displaylist::get(Displaylist::Ptr& p)
{
	return p.get();
}

Display* Displaylist::operator[](const int n)
{
	Display* dsp = nullptr;
	Node* ptr = head;
	for (int i = 0; i < n; i++)
	{
		if (ptr != nullptr)
			ptr = ptr->next;
		else
			break;
	}
	if (ptr != nullptr)
	{
		dsp = ptr->dsp;
	}
	return dsp;
}




//Window
Window::Window(int width, int height, int flag) :width(width), height(height)
{
	handle = initgraph(width, height, flag);
	BeginBatchDraw();
}

void Window::clearall()
{
	cleardevice();
	FlushBatchDraw();
 }

Window::~Window()
{
	EndBatchDraw();
	closegraph();
}

void Window::draw(Display& dsp)
{
	dsp.display();
	dis_list.add(&dsp);
	FlushBatchDraw();
}

void Window::clear(std::string tags)
{
	Display* dsp = dis_list.pop(tags);
	if (dsp != nullptr)
	{
		dsp->clear();
	}
	FlushBatchDraw();
}

void Window::reflash()
{
	cleardevice();
	for (auto i = dis_list.begin(); !i.end(); i.next())
	{
		Display* dsp = dis_list.get(i);
		dsp->display();
	}
	FlushBatchDraw();
}

void Window::del(std::string tags)
{
	dis_list.pop(tags);
}

HWND Window::gethandle()
{
	return handle;
}

//Display
std::string Display::tag() const
{
	return tags;
}


//TextBar
TextBar::TextBar(int x1, int y1, int x2, int y2, std::string str, COLORREF text_color, std::string tags)
{
	this->tags = tags;
	this->str = str;
	color = text_color;
	left_top[0] = x1;
	left_top[1] = y1;
	right_bottom[0] = x2;
	right_bottom[1] = y2;
	tc = nullptr;
	char2tchar();
}

TextBar::~TextBar()
{
	if (tc != nullptr)
	{
		delete[] tc;
		tc = nullptr;
	}
}

void TextBar::set(std::string str)
{
	this->str = str;
	char2tchar();
}

void TextBar::display()
{
	//rectangle(left_top[0], left_top[1], right_bottom[0], right_bottom[1]);
	int h = right_bottom[1] - left_top[1], w = right_bottom[0] - left_top[0];
	settextstyle(0.8 * h, 0, L"Arial");
	int t_h = textheight(tc), t_w = textwidth(tc);
	settextcolor(color);
	outtextxy(left_top[0] + w / 2 - t_w / 2, left_top[1] + h / 2 - t_h / 2, tc);
}

void TextBar::clear()
{

}

void TextBar::char2tchar()
{
	if (tc != nullptr)
	{
		delete[] tc;
		tc = nullptr;
	}
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	tc = new TCHAR[sizeof(TCHAR) * size];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, tc, size);
}

// Button

Button::Button(int x1, int y1, int x2, int y2, void(*callback)(), std::string text, std::string tags) :TextBar(x1, y1, x2, y2, text, BLACK, tags)
{
	this->callback = callback;
};

void Button::display()
{
	ExMessage msg;
	if (peekmessage(&msg, EM_MOUSE))
	{
		if (msg.message == WM_LBUTTONUP)
		{
			if (msg.x > left_top[0] && msg.x<right_bottom[0] && msg.y>left_top[1] && msg.y < right_bottom[1])
			{
				callback();
			}
		}
	}
	rectangle(left_top[0], left_top[1], right_bottom[0], right_bottom[1]);
	TextBar::display();
}

void Button::clear()
{

}