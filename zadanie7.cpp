/*
* Narysuj przyk³adowy graf o min 8 wierzcho³kach i min 10 krawêdziach i opisz dla tego grafu dzia³anie algorytmów DFS i BFS.
* Przy DFS napisz równie¿ wszystkie odwiedzane wierzcho³ki w drodze powrotnej, nie tylko te, które odwiedzasz po raz pierwszy.
* Przy BFS napisz równie¿ jak wygl¹da kolejka wierzcho³ków.
*/

#include "iostream"
#include <vector>
#include <sstream>
#include <iomanip>
#include <queue>

/*
* Graf przyje³am ¿e ma 12 wierzcho³ków z czego posiada trzy grupy:
* - jedna grupa sk³ada siê z 9 po³aczonych ze sob¹ wierzcho³ków
* - druga grupa sk³ada siê z 1 wiercho³ka, bez po³¹czeñ
* - trzecia grupa sk³ada siê z 2 po³¹czonych ze sob¹ wierzcho³ków
*/

#define LICZBA_WIERZCHO£KÓW 12
#define ILOSC_POLACZEN 12
#define POLACZENIA {{0,1},{0,4},{0,8},{1,2},{2,3},{2,4},{3,7},{7,8},{7,4},{4,5},{5,6},{10,11}}

class Node;
class Connection;
class Grapth;

enum ConnectioType {
	one,
	two
};

class Connection {
public:
	Node* start;
	Node* end;
	int weight;
	Connection(Node* s, Node* e);
	Connection(Node* s, Node* e, int w);
};

Connection::Connection(Node* s, Node* e) : start(s), end(e), weight(1) {}
Connection::Connection(Node* s, Node* e, int w) : start(s), end(e), weight(w) {}

class Node {
public:
	int id;
	std::vector<Connection*> connections;
	Node(int id);
	void AddOneDirectionConnection(Node* node);
	void AddTwoDirectionConnection(Node* node);
	void printAsConnection(std::ostringstream& os);
};

Node::Node(int id) :id(id) {}

void Node::AddOneDirectionConnection(Node* node) {
	auto connection = new Connection(this, node);
	connections.push_back(connection);
}

void Node::AddTwoDirectionConnection(Node* node) {
	auto connection = new Connection(this, node);
	node->AddOneDirectionConnection(this);
	connections.push_back(connection);
}

void Node::printAsConnection(std::ostringstream& os) {
	os << "[" << id << "] -> ";
	if (connections.size() == 0) {
		os << "no connections";
		return;
	}
	for (const auto connection : connections)
	{
		os << "[" << connection->end->id << "]";
	}
}

class Graph {
private:
	std::vector<Node*> GraphNodes;
public:
	Graph(int nodeCounts, std::vector<std::pair<int, int>> connections, ConnectioType type);
	size_t size();
	const Node* TakeNode(int x);
	void printAsConnection();
};

Graph::Graph(int nodeCounts, std::vector<std::pair<int, int>> connections, ConnectioType type) {
	for (size_t i = 0; i < nodeCounts; i++)
	{
		GraphNodes.push_back(new Node(i));
	}
	for (const auto& connection : connections) {
		Node* start = GraphNodes[connection.first];
		Node* end = GraphNodes[connection.second];
		if (type == ConnectioType::one) {
			start->AddOneDirectionConnection(end);
		}
		else {
			start->AddTwoDirectionConnection(end);
		}
	}
}

size_t Graph::size() {
	return GraphNodes.size();
}

const Node* Graph::TakeNode(int x) {
	return GraphNodes[x];
}

void Graph::printAsConnection() {
	std::ostringstream os;
	for (const auto& node : GraphNodes)
	{
		os << "* ";
		node->printAsConnection(os);
		os << std::endl;
	}
	std::cout << os.str();
}

class ViewNode {
public:
	const Node* node = nullptr;
	bool checked = false;
	ViewNode(const Node* node);
};
ViewNode::ViewNode(const Node* node) :node(node) {}

class DFSAlgo {
private:
	std::vector<ViewNode*> Nodes;
public:
	DFSAlgo(Graph* graph);
	void StartProcess();
	void StepIn(ViewNode* dfsNode, std::ostringstream& processScript, int lvl = 1);
	void AddIndent(std::ostringstream& os, int lvl);
};
DFSAlgo::DFSAlgo(Graph* graph) {
	int size = graph->size();
	for (size_t i = 0; i < size; i++)
	{
		ViewNode* dfsNode = new ViewNode(graph->TakeNode(i));
		Nodes.push_back(dfsNode);
	}
}

void DFSAlgo::StartProcess() {
	std::ostringstream processScript;
	processScript << "Start DFS Process\n";
	ViewNode* dfsNode = Nodes[0];
	StepIn(dfsNode, processScript);
	processScript << "Check if all nodes are checked in first run\n";
	for (size_t i = 1; i < Nodes.size(); i++)
	{
		ViewNode* dfsNode = Nodes[i];
		processScript << "Check if Node " << dfsNode->node->id << " was checked => ";
		if (dfsNode->checked) {
			processScript << "checked\n";
		}
		else {
			processScript << "unchecked, startDFS for this node\n";
			StepIn(dfsNode, processScript);
		}
	}
	processScript << "DFS Algorithm check all existing nodes. Algortihm End.\n";
	std::cout << processScript.str();
}

void DFSAlgo::StepIn(ViewNode* dfsNode, std::ostringstream& processScript, int lvl) {
	AddIndent(processScript, lvl);
	processScript << "Entered Node[" << dfsNode->node->id << "], set checked as true.";
	dfsNode->checked = true;
	if (dfsNode->node->connections.size() == 0) {
		processScript << " No Connection Occur, Step Back.\n";
		return;
	}
	processScript << "\n";
	for (const auto& connection : dfsNode->node->connections)
	{
		AddIndent(processScript, lvl);
		processScript << "Node[" << dfsNode->node->id << "] has connection to Node[" << connection->end->id << "] => ";
		ViewNode* node = Nodes[connection->end->id];
		if (node->checked == true) {
			processScript << " is checked.\n";
		}
		else {
			processScript << " isn't checked. Step In\n";
			StepIn(node, processScript, lvl + 1);
		}
	}
}

void DFSAlgo::AddIndent(std::ostringstream& os, int lvl) {
	os << std::setfill('-') << std::setw(lvl) << "";
}

class BFSAlgo {
private:
	std::vector<ViewNode*> Nodes;
public:
	BFSAlgo(Graph* graph);
	void StartProcess();
	void StartQueue(int startIndex, std::ostringstream& processScript);
	void AddIndent(std::ostringstream& os, int lvl);
};
BFSAlgo::BFSAlgo(Graph* graph) {
	int size = graph->size();
	for (size_t i = 0; i < size; i++)
	{
		ViewNode* dfsNode = new ViewNode(graph->TakeNode(i));
		Nodes.push_back(dfsNode);
	}
}

void BFSAlgo::StartProcess() {
	std::ostringstream processScript;
	processScript << "Start BFS Process\n";
	StartQueue(0, processScript);
	processScript << "Check if all nodes are checked in first run\n";
	for (size_t i = 1; i < Nodes.size(); i++)
	{
		processScript << "Check if Node " << Nodes[i]->node->id << " was checked => ";
		if (Nodes[i]->checked) {
			processScript << "checked\n";
			continue;
		}
		else {
			processScript << "unchecked, startBFS from this Node\n";
			StartQueue(i, processScript);
		}
	}
	std::cout << processScript.str();
}

void BFSAlgo::StartQueue(int startIndex, std::ostringstream& processScript) {
	//Dla celów mo¿liwoœci wypisywania kolejki zast¹pi³am std::queue std::vector-em
	std::vector<int> nodesToView;
	nodesToView.push_back(startIndex);
	do {
		auto first = nodesToView.begin();
		int idCheck = *first;
		AddIndent(processScript, 1);
		processScript << "Check Node[" << idCheck << "], remove first element from queue\n";
		nodesToView.erase(first);
		ViewNode* node = Nodes[idCheck];
		node->checked = true;
		for (const auto& connection : node->node->connections) {
			AddIndent(processScript, 2);
			auto viewNode = Nodes[connection->end->id];
			processScript << "Node[" << viewNode->node->id << "]";
			if (viewNode->checked) {
				processScript << " is checked\n";
			}
			else {
				processScript << " is unchecked, set ass checked and add to Queue\n";
				viewNode->checked = true;
				nodesToView.push_back(viewNode->node->id);
			}
		}
		AddIndent(processScript, 2);
		processScript << "Current Queue Created =>{";
		for (size_t i = 0; i < nodesToView.size(); i++)
		{
			processScript << nodesToView[i];
			if (i != nodesToView.size() - 1) {
				processScript << ',';
			}
		}
		processScript << "}\n";
	} while (!nodesToView.empty());
}

void BFSAlgo::AddIndent(std::ostringstream& os, int lvl) {
	os << std::setfill('-') << std::setw(lvl) << "";
}

int main() {
	std::vector<std::pair<int, int>> connections;
	int  table[ILOSC_POLACZEN][2] = POLACZENIA;
	for (size_t i = 0; i < ILOSC_POLACZEN; i++)
	{
		std::pair<int, int> connection(table[i][0], table[i][1]);
		connections.push_back(connection);
	}
	Graph* graph = new Graph(LICZBA_WIERZCHO£KÓW, connections, ConnectioType::two);
	std::cout << "Grapth as connection list:\n";
	graph->printAsConnection();
	std::cout << "=================DFS Algorithm===================\n";
	DFSAlgo* dfsAlgo = new DFSAlgo(graph);
	dfsAlgo->StartProcess();
	std::cout << "=================BFS Algorithm===================\n";
	BFSAlgo* bfsAlgo = new BFSAlgo(graph);
	bfsAlgo->StartProcess();
}