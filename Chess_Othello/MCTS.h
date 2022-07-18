#pragma once
#include<float.h>
#include<chrono>

#define NO_LIMITS -1

class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
public:
	Timer();
	void reset();
	int64_t ms() const;
	int64_t s() const;
	int64_t us() const;
	int64_t ns() const;
};


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
			double score;
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
		Ptr(const MCT& m);
		Ptr(const Ptr& p);
		bool valid() const;
		Ptr parent() const;
		int childNum() const;
		Iterator childIterator() const;
		void operator=(const Ptr& p);
		Node::_MCTData* operator->();
		bool operator==(const Ptr& p) const;
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
protected:
	Timer timer;
	MCT& mct;
public:
	MCTS(MCT& mct);
	MCT::Ptr select(MCT::Ptr& p) const;
	virtual double selectFunction(MCT::Ptr& p) const = 0;
	virtual bool expansion(MCT::Ptr& p) const = 0; //可以扩展扩展后返回1；不可扩展 设置终止标志返回0；
	virtual double rollout(MCT::Ptr& p) const = 0; // 会有终止标记的数据
	void backup(MCT::Ptr& p, double score) const;
	MCT::Ptr search(int time_ms, int max_iterations, int64_t* use_time = nullptr, int* iterations = nullptr);
};