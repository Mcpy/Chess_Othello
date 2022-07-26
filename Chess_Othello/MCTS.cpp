#include "MCTS.h"

//Timer
Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
}

void Timer::reset()
{
	start = std::chrono::high_resolution_clock::now();
}

int64_t Timer::ms() const
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

int64_t Timer::s() const
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count();
}

int64_t Timer::us() const
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

int64_t Timer::ns() const
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
}


//MCT
MCT::Ptr::Iterator::Iterator(const MCT& mct, MCT::Node* n, int layer) : mct(mct), node(n), layer(layer) {}

MCT::Ptr::Iterator::Iterator(const MCT::Ptr::Iterator& i) : mct(i.mct), node(i.node), layer(i.layer) {}

MCT::Ptr MCT::Ptr::Iterator::get() const
{
	return Ptr(mct, node, layer);
}

void MCT::Ptr::Iterator::next()
{
	if (node != nullptr)
		node = node->bro;
}

bool MCT::Ptr::Iterator::end() const
{
	if (node == nullptr)
		return 1;
	else
		return 0;
}


MCT::Ptr::Ptr(const MCT& m, Node* n, int layer) :mct(m), n(n), layer(layer) {}

MCT::Ptr::Ptr(const MCT& m) : mct(m)
{
	layer = 0;
	n = nullptr;
}

MCT::Ptr::Ptr(const MCT::Ptr& p) : mct(p.mct), n(p.n), layer(p.layer) {}

bool MCT::Ptr::valid() const
{
	if (n == nullptr)
		return 0;
	else
		return 1;
}

MCT::Ptr MCT::Ptr::parent() const
{
	return Ptr(mct, n->parent, layer - 1);
}

int MCT::Ptr::childNum() const
{
	return n->child_num;
}

int MCT::Ptr::getLayer() const
{
	return layer;
}

MCT::Ptr::Iterator MCT::Ptr::childIterator() const
{
	return Iterator(mct, n->first_child, layer + 1);
}

void MCT::Ptr::operator=(const Ptr& p)
{
	if (mct.root != p.mct.root)
		throw("MCT::Ptr does not match!");
	else
	{
		n = p.n;
		layer = p.layer;
	}
}

MCT::Node::_MCTData* MCT::Ptr::operator->()
{
	return &(n->MCTdata);
}

bool MCT::Ptr::operator==(const Ptr& p) const
{
	if (mct.root == p.mct.root && p.n == n)
		return 1;
	else
		return 0;
}

MCT::MCT(void* root_data, void(*delData)(void*))
{
	root = new Node;
	root->MCTdata.data = root_data;
	node_num = 1;
	depth = 1;
	this->delData = delData;
}

MCT::~MCT()
{
	del(root);
}

void MCT::del(Node* n)
{
	while (n->first_child != nullptr)
	{
		del(n->first_child);
	}

	if (n->parent != nullptr)
	{
		if (n->parent->first_child == n)
		{
			n->parent->first_child = n->bro;
		}
		else
		{
			Node* i = n->parent->first_child;
			for (Node* j = i->bro; j != n; i = i->bro, j = j->bro);
			i->bro = n->bro;
		}
	}
	if (delData != nullptr)
		delData(n->MCTdata.data);
	delete n;
	node_num--;
}

int MCT::getNodeNum() const
{
	return node_num;
}


int MCT::getDepth() const
{
	return depth;
}

MCT::Ptr MCT::getRoot() const
{
	return Ptr(*this, root, 1);
}

MCT::Ptr MCT::addChild(const MCT::Ptr& p, void* data, bool termination_flag)
{
	if (p.mct.root != root)
		throw("MCT::Ptr does not match!");
	else
	{
		Node* temp=p.n->first_child;
		Node* node = new Node;
		node->MCTdata.data = data;
		node->MCTdata.termination_flag = termination_flag;
		node->parent = p.n;
		p.n->first_child = node;
		node->bro = temp;
		node_num++;
		p.n->child_num++;
		if (p.layer + 1 > depth)
			depth = p.layer + 1;
		return Ptr(*this, node, p.layer + 1);
	}
}

MCT::Ptr MCT::find(const void* data, bool(*findFunc)(const void*, const void*)) const
{
	std::queue<Ptr> ptr_queue;
	ptr_queue.push(getRoot());
	Ptr ptr(*this);
	while (!ptr_queue.empty())
	{
		Ptr p = ptr_queue.front();
		ptr_queue.pop();
		if (findFunc(data, p->data))
		{
			ptr = p;
			break;
		}
		for (auto i = p.childIterator(); !i.end(); i.next())
		{
			ptr_queue.push(i.get());
		}
	}
	return ptr;
}

void MCT::del(MCT::Ptr& p)
{
	if (p.mct.root != root)
		throw("MCT::Ptr does not match!");
	if (p.n == root)
		throw("cannot del root node!");
	if (p.n->parent->first_child == p.n)
	{
		p.n->parent->first_child = p.n->bro;
	}
	else
	{
		Node* i = p.n->parent->first_child;
		for (Node* j = i->bro; j != p.n; i = i->bro, j = j->bro);
		i->bro = p.n->bro;
	}
	p.n->parent = nullptr;
	del(p.n);
	p.n = nullptr;
	depth = 0;
	calculateDepth(root, &depth);
}

void MCT::changeRoot(const MCT::Ptr& p)
{
	if (p.mct.root != root)
		throw("MCT::Ptr does not match!");
	if (p.n == root)
		return;
	else
	{
		Node* temp = root;
		if (p.n->parent->first_child == p.n)
		{
			p.n->parent->first_child = p.n->bro;
		}
		else
		{
			Node* i = p.n->parent->first_child;
			for (Node* j = i->bro; j != p.n; i = i->bro, j = j->bro);
			i->bro = p.n->bro;
		}
		p.n->parent = nullptr;
		p.n->bro = nullptr;
		root = p.n;
		del(temp);
		depth = 0;
		calculateDepth(root, &depth);
	}
}

void MCT::calculateDepth(Node* n, int* max_layer, int layer)
{
	if (layer == 1)
	{
		if (n->first_child != nullptr)
		{
			calculateDepth(n->first_child, max_layer, layer + 1);
		}
	}
	else
	{
		if (n->first_child != nullptr)
		{
			calculateDepth(n->first_child, max_layer, layer + 1);
		}
		if (n->bro != nullptr)
		{
			calculateDepth(n->bro, max_layer, layer);
		}
	}
	if (layer > *max_layer)
		*max_layer = layer;
}

int MCT::calculateDepth(const Ptr& p)
{
	int max_layer = 0;
	calculateDepth(p.n, &max_layer);
	return max_layer;
}



//MCTS
MCTS::MCTS(MCT& mct):mct(mct) {}

MCT::Ptr MCTS::select(MCT::Ptr& p) const
{
	double max_s = -DBL_MAX;
	MCT::Ptr max_p = p;
	for (auto i = p.childIterator(); !i.end(); i.next())
	{
		auto child = i.get();
		double s = selectFunction(child);
		if (s > max_s)
		{
			max_s = s;
			max_p = child;
		}
	}
	return max_p;
}



void MCTS::backup(MCT::Ptr& p, double score) const
{
	for (MCT::Ptr i = p; i.valid(); i = i.parent())
	{
		i->score += score;
		i->total++;
	}
}

MCT::Ptr MCTS::search(int time_ms, int max_iterations, int max_step, int64_t* use_time, int* iterations)
{
	int max_depth = max_step + 1;
	if (time_ms == NO_LIMITS && max_iterations == NO_LIMITS)
	{
		throw ("MCTS need limits");
	}
	int it_ = 1;
	int64_t	t = 0;
	timer.reset();
	for (; it_ <= max_iterations || max_iterations == NO_LIMITS; it_++)
	{
		t = timer.ms();
		if (t > time_ms && time_ms != NO_LIMITS)
			break;

		MCT::Ptr i = mct.getRoot();
		for (; i.childNum() != 0; i = select(i)); //select
		if (!i->termination_flag)
		{
			if (max_depth != NO_LIMITS)
			{
				if (i.getLayer() < max_depth)
				{
					if (expansion(i)) //expansion
						i = i.childIterator().get();
				}
			}
			else
			{
				if (expansion(i)) //expansion
					i = i.childIterator().get();
			}
		}
		backup(i, rollout(i)); //rollout and backup
	}
	if (use_time != nullptr)
		*use_time = t;
	if (iterations != nullptr)
		*iterations = it_;
	
	int max_total = -1;
	auto max_p = MCT::Ptr(mct);
	for (auto i = mct.getRoot().childIterator(); !i.end(); i.next())
	{
		auto ptr = i.get();
		if (ptr->total > max_total)
		{
			max_total = ptr->total;
			max_p = ptr;
		}
	}
	return max_p;
}