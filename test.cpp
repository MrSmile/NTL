#include <iostream>
#include "literal.h"
#include "pointer.h"
#include "tree.h"
#include "list.h"

using namespace std;
using namespace NTL;


void test_string()
{
    cout << "\n=== Testing Strings ===\n";

    String str = "~~~";
    str = "Zzz: " + str + '\n';
    str += Literal("Next line") + '\n';
    str += "0 " + Literal("1 ") + "2 " + "3 " + "3 " + "5 " + "6 " + "7 " + "8 " + "9 " + "10 " + '\n';
    str += '\n';  cout << str.data();
}


struct Node : public SimpleKey<int>
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

    void *operator new(size_t size, const std::nothrow_t &) throw()
    {
        const int max = 100;
        if(count >= max)return 0;  return new_nt char[size];
    }
};

int Node::count = 0;

struct Node1 : public TreeNode<Node1>, public Node
{
    Node1(int n) : Node(n)
    {
    }

    Node1(const Node &node) : Node(node)
    {
    }
};

void test_tree()
{
    cout << "\n=== Testing Trees ===\n";

    OwningTree<Node1> tree;
    for(int i = 0; i < 10; i++)tree.get(i);
    {
        OwningTree<Node1> copy;
        if(!copy.copy(tree))cout << "Copy failed!\n";
        else delete copy.find(1005)->next();
        swap(tree, copy);
    }
    for(Node1 *node = tree.first(); node; node = node->next())
        cout << node->id() << ' ';  cout << endl;
    for(Node1 *node = tree.last(); node; node = node->prev())
        cout << node->id() << ' ';  cout << endl;
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


struct Node2 : public StackNode<Node2>, public Node
{
    Node2(int n) : Node(n)
    {
    }

    Node2(const Node &node) : Node(node)
    {
    }
};

struct Node3 : public SimpleListNode<Node3>, public Node
{
    Node3(int n) : Node(n)
    {
    }

    Node3(const Node &node) : Node(node)
    {
    }
};

struct Node4 : public ListNode<Node4>, public Node
{
    Node4(int n) : Node(n)
    {
    }

    Node4(const Node &node) : Node(node)
    {
    }
};

void test_list()
{
    cout << "\n=== Testing Lists, Stage 1 ===\n";
    {
        OwningStack<Node2> list;
        for(int i = 0; i < 10; i++)list.prepend(new_nt Node2(i));
        list.first()->next()->next()->insert_next(new_nt Node2(777));
        delete list.first()->next()->take_next();
        OwningStack<Node2> move, copy;
        if(!copy.copy(list))cout << "Copy failed!\n";
        for(;;)
        {
            Node2 *node = list.take_first();  if(!node)break;
            cout << node->id() << ' ';  move.prepend(node);
        }
        cout << endl;  swap(list, move);
        for(Node2 *node = list.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << "\n=== Testing Lists, Stage 2 ===\n";
    {
        OwningSimpleList<Node3> list;
        for(int i = 0; i < 10; i++)list.append(new_nt Node3(i));
        list.insert(new_nt Node3(777), list.first()->next()->next());
        delete list.take(list.first());  list.prepend(new_nt Node3(666));
        OwningSimpleList<Node3> move, copy;
        if(!copy.copy(list))cout << "Copy failed!\n";
        for(;;)
        {
            Node3 *node = list.take_first();  if(!node)break;
            cout << node->id() << ' ';  move.prepend(node);
        }
        cout << endl;  swap(list, move);
        if(!list.prepend_copy(copy))cout << "Merge copy failed!\n";
        list.append(copy);
        for(Node3 *node = list.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << "\n=== Testing Lists, Stage 3 ===\n";
    {
        OwningList<Node4> list;
        for(int i = 0; i < 10; i++)list.prepend(new_nt Node4(i));
        list.first()->next()->next()->insert_next(new_nt Node4(777));
        list.first()->next()->next()->insert_prev(new_nt Node4(666));
        delete list.first()->next();
        OwningList<Node4> move, copy;
        if(!copy.copy(list))cout << "Copy failed!\n";
        for(;;)
        {
            Node4 *node = list.take_first();  if(!node)break;
            cout << node->id() << ' ';  move.prepend(node);
        }
        cout << endl;  swap(list, move);
        for(Node4 *node = list.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
    }
}


void time_tree();

int main(int n, char **arg)
{
    if(n == 1)
    {
        test_string();
        test_tree();
        delete test_pointer();
        test_list();
        return 0;
    }

    for(int i = 1; i < n; i++)
        if(arg[i] == Literal("tree"))time_tree();

    return 0;

}
