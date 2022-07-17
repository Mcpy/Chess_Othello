#include "MCTS.h"

//MCT
template<class Data> MCT<Data>::Ptr::Iterator::Iterator(const MCT& mct, MCT::Node* n, int layer) : mct(mct), node(n), layer(layer) {}

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
		if (p.layer + 1 > depth)
			depth = p.layer + 1;
		return Ptr(*this, node, p.layer + 1);
	}
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