#include <iostream>
#include <string>
#include<map>
#include<queue>
#include<vector>
#include<algorithm>
#include<fstream>

using namespace std;

struct Binnode {//node
    int data; int name;//dataは入力内での出現回数、nameは文字char+128 
                       //"data" is the times it appeared, "name" is (char) + 128
    Binnode* parent;
    Binnode* lc;//left-child
    Binnode* rc;//right-child
    string code = {};//左に進むと１追加、右に進むと0追加 
                     //add "1" if go left, add "0" if go right
};

class BinTree {
private:
    Binnode* _root;
public:
    void makeBintree(multimap<int, int> T);//二分木の作成
                                           //make binary tree
    Binnode* add2node(Binnode* l, Binnode* r);//既成の2つのノードを子ノード、新しいノードを親ノード
                                              //existed two nodes as children, new node as parent
    Binnode* makenode(int data, int name);//ノード作成
                                          //make new node
    int haffman(int n, int totalsize);//ハフマン符号の記録
                       //record Haffman codes
    void visitnode(Binnode* current, vector<tuple<int, int, string>>& vc);//ノード探索
                                                                          //search nodes
};

Binnode* BinTree::makenode(int data, int name) {
    Binnode* newnode = new Binnode;

    newnode->data = data;
    newnode->name = name;
    newnode->parent = nullptr;
    newnode->lc = nullptr;
    newnode->rc = nullptr;

    return newnode;
}

Binnode* BinTree::add2node(Binnode* l, Binnode* r) {
    Binnode* newnodep = new Binnode;

    newnodep->data = l->data + r->data;
    newnodep->name = -1;//後付けの内部接点のnameは-1(NULLはOJが通らなかった)
                                                            //"name" of parents which have been made later is -1
    newnodep->parent = nullptr;
    newnodep->lc = l;
    newnodep->rc = r;

    l->parent = newnodep;
    r->parent = newnodep;

    return newnodep;
}


void BinTree::visitnode(Binnode* current, vector<tuple<int, int, string>>& vc) {
    if (!current) return;
    if (current->lc)current->lc->code = current->code + "1";
    visitnode(current->lc, vc);

    if (current->name != -1)vc.emplace_back(current->data, current->name, current->code);

    if (current->rc)current->rc->code = current->code + "0";
    visitnode(current->rc, vc);
}

int BinTree::haffman(int n, int totalsize) {


    vector<tuple<int, int, string>> vc;//sample.cppのcodeとname入れ替え
                                       //reverse the "code" and "name" in order to sort by "code"
    visitnode(_root, vc);//make "code" by visiting every nodes
    sort(vc.begin(), vc.end()); //sortがなぜかOJを通らなかった。

    char tmp;
    int tmp_;

    ifstream in{ "text.txt", ios::in };
    ofstream out{ "encoded.txt.", ios::out | ios::trunc };

    int outnum = 0;//the number of output

    while (!in.eof()) {//encode
        in.get(tmp);
        tmp_ = int(tmp) + 128;

        for (int i = 0; i < (int)vc.size(); i++) {
            if (tmp_ == get<1>(vc[i])) {
                out << get<2>(vc[i]);
                outnum++;
                break;
            }
        }
        if (outnum == totalsize) break;//output the all characters
    }

    in.close();
    out.close();

    reverse(begin(vc), end(vc));//make it faster to compare

    ofstream out2{ "haffman_key.txt", ios::out | ios::trunc };
    out2 << vc.size() << '\n' << totalsize << endl;

    for (auto x : vc) {
        out2 << (char)(get<1>(x) - 128) << " " << get<2>(x) << endl;
    }

    out2.close();

    return totalsize;
}


void BinTree::makeBintree(multimap<int, int> T) {
    priority_queue<pair<int, Binnode*>, vector<pair<int, Binnode*> >, greater<pair<int, Binnode*> > > pque;
    /*multimap<int, int>::iterator it;
    it = T.begin();
    pair<int, int> item = *it;*/ //can use these instead of "range-based for statement"

    for (auto itr : T) {
        pque.push(make_pair(itr.first, makenode(itr.first, itr.second)));
    }

    pair<int, Binnode*> templ;//temporary left child
    pair<int, Binnode*> tempr;//temporary right child

    while (pque.size() != 1) {
        templ = pque.top();
        pque.pop();
        tempr = pque.top();
        pque.pop();
        pque.push(make_pair(templ.first + tempr.first, add2node(templ.second, tempr.second)));
    }

    pair<int, Binnode*> temp = pque.top();
    _root = temp.second;
    pque.pop();

}

void decode() {
    vector<pair<char, string>> U;

    ifstream in{ "haffman_key.txt", ios::in };

    int n;//number of types of characters
    int totalsize;//number of characters
    in >> n;
    in >> totalsize;

    char ch;
    string str;

    in.get();

    for (int i = 0; i < n; i++) {//make decoding table
        getline(in, str);
        ch = str[0];
        str.erase(0, 2);
        U.push_back(make_pair(ch, str));
    }

    in.close();

    ifstream in2{ "encoded.txt", ios::in };
    ofstream out{ "decoded.txt", ios::out | ios::trunc };

    char tmp;
    string key;

    key.clear();
    int outnum = 0;//the number of output

    while (!in2.eof()) {//decode
        in2.get(tmp);
        key += tmp;

        for (int i = 0; i < (int)U.size(); i++) {
            if (key == U[i].second) {
                out << U[i].first;
                outnum++;
                key.clear();
                break;
            }
        }
        if (totalsize == outnum) break;//finish decodeing
    }

    in2.close();
    out.close();

}

int main()
{
    int option = -1;
    int totalsize;//the number of characters in "text.txt"

    while (option != 0 && option != 1) {
        cout << "Want to Encode or Decode?" << endl;
        cout << "Encode: 0, Decode: 1\n" << endl;
        cout << "--------------------------------------------------------------------------------------" << endl;
        cout << " Attention!\n 1. Put [text.txt] in the same folder with this cpp file.\n 2. This encoder CANNOT detect new lines.\n 3. Make sure there are NO characters except ASCII.\n 4. [text.txt] must contain at least 2 types of characters." << endl;
        cout << "--------------------------------------------------------------------------------------\n" << endl;
        cin >> option;
    }
    if (option == 0) {
        totalsize = 0;
        string input;
        input.clear();

        ifstream in{ "text.txt", ios::in };
        int freq[256];
        for (int i = 0; i < 256; i++) freq[i] = 0;
        while (!in.eof()) {
            getline(in, input);
            int length = (int)input.length();
            totalsize += length;
            for (int i = 0; i < length; i++) freq[int(input[i]) + 128]++;
        }

        in.close();

        multimap<int, int> S;//record that how many types of characters exist and how many times they appeared

        for (int i = 0; i < 256; i++)
            if (freq[i] != 0)
                S.insert(make_pair(freq[i], i));

        if (S.size() == 1) { cout << "Error: less character!" << endl; return 0; }//Need at least 2 characters for Haffman coding

        BinTree Tree;
        Tree.makeBintree(S);//make binary tree
        Tree.haffman(S.size(), totalsize);//Haffman coding

        cout << "Encode complited! Check [encoded.txt] and [haffman_key.txt]" << endl;

    }
    else {
        decode();
        cout << "Decode complited! Check [decoded.txt]" << endl;
    }
    return 0;
}