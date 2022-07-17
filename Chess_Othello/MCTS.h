#pragma once

//template <class Data>
class MCT
{
public:
	struct Node
	{
		Node* parent;
		Node* first_child;
		int child_num;
		Node* bro;
		struct _MCTData
		{
			void* data;
			int total;
			float score;
			bool termination_flag;
			_MCTData()
			{
				data = nullptr;
				total = 0;
				score = 0;
				termination_flag = 0;
			}
		} MCTdata;

		Node()
		{
			parent = nullptr;
			first_child = nullptr;
			child_num = 0;
			bro = nullptr;
		}
	};
public:
	class Ptr
	{
	public:
		class Iterator
		{
		private:
			Node* node;
			const MCT& mct;
			int layer;
		public:
			Iterator(const MCT& mct ,Node* n, int layer);
			Iterator(const Iterator& i);
			Ptr get() const;
			void next();
			bool end() const;
		};
	private:
		Node* n;
		const MCT& mct;
		int layer;
	public:
		Ptr(const MCT& m, Node* n, int layer);
		Ptr(const Ptr& p);
		bool valid() const;
		Ptr parent() const;
		int childNum() const;
		Iterator childIterator() const;
		void operator=(const Ptr& p);
		Node::_MCTData* operator->();
		friend class MCT;
	};
protected:
	Node* root;
	int node_num;
	int depth;
	void (*delData)(void*);
	void del(Node* n);
	void calculateDepth(Node* n, int* max_layer, int layer = 1);
public:
	MCT(void* root_data, void(*delData)(void*) = nullptr);
	~MCT();
	int getNodeNum() const;
	int getDepth() const;
	Ptr getRoot() const;
	Ptr addChild(const Ptr& p, void* data, bool termination_flag = 0);
	void del(Ptr& p);
	void changeRoot(const Ptr& p);
	int calculateDepth(const Ptr& p);
};

class MCTS
{
public:
	MCT::Ptr select(MCT::Ptr& p);
	float selectFunction(void* data);

};