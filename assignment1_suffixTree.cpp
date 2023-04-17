#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;


const int N = 27; // N is the size of alphabet (note it is 27 not 26 because we are counting the $ sign)
class Node // this is a node hold the information the suffix tree needs.
{

public:

    int startIndex ; //The starting index (in the original string) of the characters on the edge going to this node.
    int suffixIndex; //The starting index (in the original string) of the suffix ending at this node (used for leaf nodes only, otherwise it will be -1).
    Node *children[N]; //Array of pointers to children nodes.

    Node()
    {
        // this is a constructor of Node class it assign startIndex and suffixIndex to -1
        this->startIndex = -1;
        this->suffixIndex = -1;

        // the constructor assign the children array of pointers cells to null.
        for (int i = 0; i < N; ++i)
        {
            this->children[i] = nullptr;
        }
    }
    // constructor that assign the startIndex and suffixIndex and make children array cells to null
    Node(int startIndex, int suffixIndex)
    {
        this->startIndex = startIndex;
        this->suffixIndex = suffixIndex;

        for (int i = 0; i < N; ++i)
        {
            this->children[i] = nullptr;
        }
    }
};
class SuffixTree
        {
private:

    char *str; // this is the original string the suffix tree builds
    int n; // this represents the size of the original string


    int
    getCharacterIndex(char chr) // this is a method that map a character to an index in the children array of pointers
    {
        int characterIndex = chr - 'a'; // this will return 0,1,2,3,... for a,b,c,d,...

        // if it returned a number < 0 then this means it was a $ sign, so we map this character to index 26
        if (characterIndex < 0)
            characterIndex = 26;

        return characterIndex;
    }

    void insert(Node *&currentNode, int i, int suffixIndexP) // this is the insert function that is used to build the suffix tree
    {
        /* 3 arguments is taken
         * currentNode: represents the current node that i stand on, it will start from root->children[firstCharacter of suffix]
         * i: which represent the starting of the suffix that i am currently at
         * suffixIndexP: represent the index of the suffix that i am currently processing
         * */


        if (currentNode == nullptr) // if currentNode is null then just assign a new node to it.
        {
            currentNode = new Node(i, suffixIndexP); // the node take startIndex: i and suffixIndex: suffixIndexP
        }
        else
        {
            int length = getLength(currentNode); // this will get the length of characters in the edge.
            int startIndex = currentNode->startIndex;
            int suffixIndex = currentNode->suffixIndex;

            /*
             if current Node is not equal to null then:-
             get length of current node and compare the characters on the edge with characters of the suffix.
             when not equal split at this point make d = startIndex of edge characters + splitPoint and c = i + splitPoint

             if no splitting point exist that mean we need to go one level down in the tree and we start comparing from i + length
             so we call insert(nextNode,i + length,suffixIndexP) recursively.

             if a splitting point is found then:-

             now two cases will be applied
             case 1 currentNode has no Children.
             - first make a new Node(d,suffixEnding of the edgeCharacters) and Node(c,suffixEndingP (the suffix ending of the processed suffix))
             - make the current node point to this two nodes at the index of the str[c] and str[d]
             case 2 currentNode has children
             -  first make a new Node(d,-1) and Node(c,suffixEndingP (the suffix ending of the processed suffix))
             -  make the children of Node(d,-1) equal to children of the currentNode then delete the pointer of all the children of currentNode
             -  then make the current node point to this two nodes at the index of the str[c] and str[d]
             */
            int d, c;
            bool breakPoint = false; // this a boolean that determine if a split point has been found

            // loop that determine the split point
            for (int k = 0; k < length; ++k)
            {
                if (str[startIndex + k] != str[i + k])
                {
                    d = startIndex + k;
                    c = i + k;
                    breakPoint = true;
                    break;
                }
            }
            // if no split point found then go one level down and do the same steps again
            if (!breakPoint)
            {
                insert(currentNode->children[getCharacterIndex(str[i + length])], i + length, suffixIndexP);
                return;
            }

            bool case1 = true; // this determines the case we are in case 1 true means currentNode has no children else then currentNode has children

            for (int k = 0; k < N; ++k)
            {
                if (currentNode->children[k] != nullptr)
                {
                    case1 = false;
                    break;
                }
            }

            // this handle case 1
            if (case1)
            {
                Node *newNode1 = new Node(d, suffixIndex);
                Node *newNode2 = new Node(c, suffixIndexP);
                int characterIndex1 = getCharacterIndex(str[d]);
                int characterIndex2 = getCharacterIndex(str[c]);
                currentNode->children[characterIndex1] = newNode1;
                currentNode->children[characterIndex2] = newNode2;
                currentNode->suffixIndex = -1;
            }
                // this handle case 2
            else
            {
                Node *newNode1 = new Node(c, suffixIndexP);
                Node *newNode2 = new Node(d, -1);

                for (int k = 0; k < N; ++k)
                {
                    newNode2->children[k] = currentNode->children[k];
                }
                for (int k = 0; k < N; ++k)
                {
                    currentNode->children[k] = nullptr;
                }
                int characterIndex1 = getCharacterIndex(str[c]);
                int characterIndex2 = getCharacterIndex(str[d]);
                currentNode->children[characterIndex1] = newNode1;
                currentNode->children[characterIndex2] = newNode2;
            }
        }
    }


    int getLength(Node *currentNode) // this a function that get the length of characters on the edge that is going to the node
    {

        // to get the length we get the minimum startingIndex of the children
        // after getting the minimum we return minStartingIndexFound - currentNode.startIndex
        // if no children are there then we return n - currentNode.startIndex + 1(length of suffix)
        int minChild = INT32_MAX;
        for (int i = 0; i < N; ++i)
        {
            if (currentNode->children[i] != nullptr)
            {
                minChild = min(minChild, currentNode->children[i]->startIndex);
            }
        }
        // minChild is INT32_MAX meaning there is no children
        if (minChild == INT32_MAX)
            return n - currentNode->startIndex + 1;
        return abs(minChild - currentNode->startIndex);
    }

public:
    Node *root; // this is the root node

    // constructor that build the suffix tree.
    SuffixTree(char *str)
    {
        this->str = str; // assign the original string
        root = new Node(); // assign the root node
        this->n = strlen(str); // assign n to the length of str

        // for every suffix call insert on it, and we start the insert from the root.children[str[i]]
        for (int i = 0; i < n; ++i)
        {
            Node *&currentNode = root->children[getCharacterIndex(str[i])];
            insert(currentNode, i, i);
        }
    }

    void
    Search(char *pattern) // method that search for a pattern and print the starting indices of the pattern in the string
    {
        int characterIndex = getCharacterIndex(pattern[0]);

        // we start from traversing from root.children[pattern[0]]
        Node *currentNode = root->children[characterIndex];
        int i = 0; // the i determine the starting of the pattern which is initialized to 0
        int j = strlen(pattern) - 1; // the j determine the ending of the pattern index
        searchPattern(pattern, i, j, currentNode); // we call searchPattern.
        cout << endl;


//        Case 1: if one character doesn't exist then print -1

//  case 2: if all pattern matching at an edge print all suffix endings of the leafs of the edge;


// case3: if all characters on edge finished before finishing the pattern got to next child if exist if not print -1;

    }

    void searchPattern(char *pattern, int i, int j, Node *currentNode)
    {
        // if the current character of pattern doesn't exist then print -1;
        if (currentNode == nullptr)
        {
            cout << -1;
            return;
        }
        int length = getLength(currentNode); // length of edge characters

        // we compare the edge characters with the pattern
        for (int k = 0; k < length; ++k)
        {
            if (pattern[i + k] != str[currentNode->startIndex + k])
            {
                // if one character on edge doesn't equal to pattern then pattern doesn't exist, and we return -1;
                cout << -1;
                return;

            }
            if (i + k == j)
            {
                // if we compared all characters of patterns and all match then we call printDfs on this Node which print all leaf values of this Node
                printDfs(currentNode);
                return;
            }

        }
        // if we reached here then this means that edge characters are finished and pattern is not so should go one level down
        // to this we call searchPattern recursively on the next node the pattern starting index will be i + length since previous
        // characters are already compared.

        int characterIndex = getCharacterIndex(pattern[i + length]);
        Node *newNode = currentNode->children[characterIndex];
        searchPattern(pattern, i + length, j, newNode);
    }

    void printDfs(Node *currentNode)
    {

        bool isLeaf = true; // this determines if current Node is leaf or not
        for (int i = 0; i < N; ++i)
        {
            if (currentNode->children[i] != nullptr)
            {
                isLeaf = false;
                printDfs(currentNode->children[i]);
                // if at least one child is not null then this is not a leaf node, and we call printDfs recursively on this child.
            }

        }
        // if it is a leaf then print the suffixIndex of this leaf
        if (isLeaf)
        {
            cout << currentNode->suffixIndex << " ";
        }

    }
};
int main() {

    // Add test cases here.

//    test case 1
    cout << "TEST CASE 1" << endl;
    SuffixTree t("bananabanaba$");
    t.Search("ana"); // Prints: 1 3 7
    t.Search("naba"); // Prints: 4 8
    cout << "----------------------------" << endl;

    // test case 2;
    cout << "TEST CASE 2" << endl;
    SuffixTree t2("abcdabcdabcdabxdabtd$");
    t2.Search("b");
    t2.Search("d");
    t2.Search("abx");
    t2.Search("abt");
    t2.Search("xd");
    t2.Search("dab");
    t2.Search("dtb");
    t2.Search("z");
    cout << "----------------------------" << endl;

    // test case 3;
    cout << "TEST CASE 3" << endl;
    SuffixTree t3("aaaaaa$");
    t3.Search("a");
    t3.Search("aa");
    t3.Search("aaa");
    t3.Search("aaaa");
    t3.Search("aaaaa");
    t3.Search("aaaaaa");
    t3.Search("aaaaaaa");
    t3.Search("b");
    t3.Search("$");
    cout << "----------------------------" << endl;


    // test case 4;
    cout << "TEST CASE 4" << endl;
    SuffixTree t4("xabxac$");
    t4.Search("a");
    t4.Search("xa");
    t4.Search("xba");
    t4.Search("xac");
    cout << "----------------------------" << endl;

    // test case 5;
    cout << "TEST CASE 5" << endl;
    SuffixTree t5("banana$");
    t5.Search("a");
    t5.Search("ba");
    t5.Search("na");
    t5.Search("banana");

    cout << "----------------------------" << endl;

    // test case 6;
    cout << "TEST CASE 6" << endl;
    SuffixTree t6("cabcabban$");
    t6.Search("cab");
    t6.Search("ab");
    t6.Search("ban");
    t6.Search("b");
    cout << "----------------------------" << endl;

    // test case 7;
    cout << "TEST CASE 7" << endl;
    SuffixTree t7("rtyxxrtyxxcc$");
    t7.Search("c");
    t7.Search("yxx");
    t7.Search("xx");
    t7.Search("rt");
    t7.Search("rty");
    cout << "----------------------------" << endl;

    // test case 8;
    cout << "TEST CASE 8" << endl;
    SuffixTree t8("doudoud$");
    t8.Search("dou");
    t8.Search("ud");
    t8.Search("udo");
    t8.Search("du");
    cout << "----------------------------" << endl;

    // test case 9;
    cout << "TEST CASE 9" << endl;
    SuffixTree t9("acxacxaxcaxcaxccax$");
    t9.Search("xc");
    t9.Search("xa");
    t9.Search("cx");
    t9.Search("acx");
    cout << "----------------------------" << endl;

    // test case 10;
    cout << "TEST CASE 10" << endl;
    SuffixTree t10("caracarbarcar$");
    t10.Search("car");
    t10.Search("ac");
    t10.Search("bar");
    t10.Search("c");
    cout << "----------------------------" << endl;

    return 0;
}