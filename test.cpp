#include <iostream>
#include "literal.h"
#include "tree.h"

using namespace std;


struct Node : public TreeNode<Node>, public SimpleKey<int>
{
    static int count;

    Node(int n) : SimpleKey<int>(n)
    {
        cout << "Created " << id() << ", total " << ++count << endl;
    }

    Node(const Node &node) : SimpleKey<int>(node.id() + 1000)
    {
        cout << "Copied " << node.id() << " -> " << id() << ", total " << ++count << endl;
    }

    ~Node()
    {
        cout << "Deleted " << id() << ", left " << --count << endl;
    }
};

int Node::count = 0;


int main()
{
    String str = "~~~";
    str = "Zzz: " + str + '\n';
    str += Literal("Next line") + '\n';
    str += "0 " + Literal("1 ") + "2 " + "3 " + "3 " + "5 " + "6 " + "7 " + "8 " + "9 " + "10 " + '\n';
    str += '\n';  cout << str.data();

    OwningTree<Node> tree;
    for(int i = 0; i < 10; i++)tree.get(i);
    {
        OwningTree<Node> copy;
        copy.copy(tree);
        delete copy.find(1005)->next();
        swap(tree, copy);
    }
    return 0;
}
