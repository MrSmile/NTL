#include <iostream>
#include "literal.h"
#include "pointer.h"
#include "tree.h"

using namespace std;


void test_string()
{
    cout << "\n=== Testing Strings ===\n";

    String str = "~~~";
    str = "Zzz: " + str + '\n';
    str += Literal("Next line") + '\n';
    str += "0 " + Literal("1 ") + "2 " + "3 " + "3 " + "5 " + "6 " + "7 " + "8 " + "9 " + "10 " + '\n';
    str += '\n';  cout << str.data();
}


struct Node : public TreeNode<Node>, public SimpleKey<int>
{
    static int count;

    Node(int n = 777) : SimpleKey<int>(n)
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

void test_tree()
{
    cout << "\n=== Testing Trees ===\n";

    OwningTree<Node> tree;
    for(int i = 0; i < 10; i++)tree.get(i);
    {
        OwningTree<Node> copy;
        copy.copy(tree);
        delete copy.find(1005)->next();
        swap(tree, copy);
    }
}


Node *test_pointer()
{
    cout << "\n=== Testing Pointers ===\n";

    Pointer<Node> del(new_nt Node(555));
    Pointer<Node> node(new_nt Node(666));
    Array<Node> array(10);
    del = new_nt Node(888);
    return node.detach();
}


int main()
{
    test_string();
    test_tree();
    delete test_pointer();
    return 0;
}
