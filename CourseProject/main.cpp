#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <deque>
#include <fstream>
#include <bitset>

template <class T>
void printVec(std::vector<T> &vec) {
    std::cout << "vec: ----\n";
    for (int i = 0; i < vec.size(); ++i)
        std::cout << vec[i] << ' ';
    std::cout << '\n';
}

void printMap(std::map<char, std::string> a) {
    for (auto &[key, value] : a) {
        std::cout << key << ' ' << value << '\n';
    }
}

namespace NSuffixTree {
const char SENTINEL = '$';

class End {
    int end;
    End(int i): end(i) {}
    friend class Node;
    friend class SuffixTree;
};

class Node {
private:
    int start;
    End *end;
    std::map<char, Node*> children;
    
    //Store index where to start this suffix
    int indexOfSuffix;

    Node *suffixLink;

public:
    Node (int start, End *end) : start(start), end(end), indexOfSuffix(0) {}
    
    friend class SuffixTree;
};

class SuffixTree {
private:
    // Global end for leaf
    End *end = new End(-1);
    Node *root = new Node(1, new End(0));
    Node *activeNode = root;
    int activeEdge = -1;
    int activeLength = 0;

    // Store how many suffix we have to create
    int remainingSuffixCount = 0;
    std::string text;
    std::string doubleText;
    int sizeText;
    int firstSize;

public:
    SuffixTree(std::string &str) {
        root->indexOfSuffix = -1;
        text = str;
        text += SENTINEL;
        sizeText = text.length();
    }

    SuffixTree() {
        root->indexOfSuffix = -1;
    }

    Node *selectNode() {
        return activeNode->children[text[activeEdge]];
    }

    Node *selectNode(int index) {
        return activeNode->children[text[index]];
    }

    bool existEdge(int i) {
        Node *node = selectNode();
        return node->children.count(text[i]);
    }

    bool isLeaf(Node *node) {
        return node->children.empty();
    }

    int diff(Node *node) {
        return node->end->end - node->start;
    }

    bool atTheMiddle(int i) {
        // This function check where is the next character
        // true - if one at edge
        // else - another (happens when we have to add new leaf at already
        // exist internal node
        Node *node = selectNode();
        if (diff(node) >= activeLength) {
            return true;
        }
        if (diff(node) + 1 == activeLength) {
            if (node->children[text[i]] != nullptr) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            activeNode = node;
            activeLength = activeLength - diff(node) - 1;
            activeEdge = activeEdge + diff(node) + 1;
            return atTheMiddle(i);
        }
    }

    char nextChar(int i) {
        Node *node = selectNode();
        if (diff(node) >= activeLength) {
            return text[activeNode->children[text[activeEdge]]->start + activeLength];
        }
        if (diff(node) + 1 == activeLength) {
            return text[i];
        }
        else {
            activeNode = node;
            activeLength = activeLength - diff(node) - 1;
            activeEdge = activeEdge + diff(node) + 1;
            return atTheMiddle(i);
        }
    }

    void walkDown(int i) {
        Node *node = selectNode();
        if (diff(node) < activeLength) {
            activeNode = node;
            activeLength = activeLength - diff(node);
            activeEdge = node->children[text[i]]->start;
        }
        else {
            ++activeLength;
        }
    }

    void extensionOfTree(int i) {

        // Set lastCreatedInternalNode to nullptr before start of every phase
        Node *lastCreatedInternalNode = nullptr;
        
        // Fast extension all leafs
        ++end->end;
        
        ++remainingSuffixCount;
        while (remainingSuffixCount) {

            // If activeLength == 0 then look for current character from root
            if (activeLength == 0) {
                // If current character from root exist then increase 
                // activeLength by one and break out of while loop
                if (selectNode(i) != nullptr) {
                    activeEdge = activeNode->children[text[i]]->start;
                    ++activeLength;
                    break;
                }
                // Create a new leaf node with current character from root
                else { 
                    root->children[text[i]] = new Node(i, end);
                    --remainingSuffixCount;
                }
            }
            else {
                // If activeLength != 0 means we are in middle in edge
                if (atTheMiddle(i)) {
                    char ch = nextChar(i);
                    if (ch == text[i]) {
                        if (lastCreatedInternalNode != nullptr) {
                            lastCreatedInternalNode->suffixLink = selectNode();
                        }
                        walkDown(i);
                        break;
                    }
                    else {
                        // Here we create a new internal node
                        Node *node = selectNode();
                        int oldStart = node->start;
                        node->start = node->start + activeLength;
                        Node *newInternalNode = new Node(oldStart, new End(oldStart + activeLength - 1));

                        // create new leaf node
                        Node *newLeafNode = new Node(i, this->end);

                        // set internal nodes child as old node and new leaf node
                        newInternalNode->children[text[newInternalNode->start+activeLength]] = node;
                        newInternalNode->children[text[i]] = newLeafNode;
                        newInternalNode->indexOfSuffix = -1;
                        activeNode->children[text[newInternalNode->start]] = newInternalNode;

                        // if another internal node was created in last extension of this phase then suffix link of that
                        // node will be this node
                        if (lastCreatedInternalNode != nullptr) {
                            lastCreatedInternalNode->suffixLink = newInternalNode;
                        }

                        lastCreatedInternalNode = newInternalNode;
                        newInternalNode->suffixLink = root;

                        // if active node is not root then follow suffix link
                        if (activeNode != root) {
                            activeNode = activeNode->suffixLink;
                        }
                        // if active node is not root then increase active index by one 
                        // and decrease activeLength by 1
                        else {
                            activeEdge = activeEdge + 1;
                            --activeLength;
                        }
                        --remainingSuffixCount;
                    }
                }
                else {
                    Node *node = selectNode();
                    node->children[text[i]] = new Node(i, end);
                    if (lastCreatedInternalNode != nullptr) {
                        lastCreatedInternalNode->suffixLink = node;
                    }
                    lastCreatedInternalNode = node;

                    // if activeNode != root then follow suffixLink
                    if (activeNode != root) {
                        activeNode = activeNode->suffixLink;
                    }
                    else {
                        activeEdge = activeEdge + 1;
                        --activeLength;
                    }
                    --remainingSuffixCount;
                }
            }
        }
    }

    void setIndex(Node *node, int val, int size) {
        if (node == nullptr) {
            return;
        }

        val += node->end->end - node->start + 1;
        if (node->indexOfSuffix != -1) {
            node->indexOfSuffix = size - val;
            return;
        }

        if (node->children.size() == 0)
            return;
        for (auto it : node->children) {
            setIndex(it.second, val, size);
        }
    }

    void buildBasicSuffixTree(std::string str) {
        text = str;
        text += SENTINEL;
        sizeText = text.length();
        buildSuffixTree();
    }
    
    void buildSuffixTree() {
        for (int i = 0; i < sizeText; ++i) {
            extensionOfTree(i);
        }

        setIndex(root, 0, sizeText);
    }

    void buildDoubleSuffixTree(std::string str) {
        text = str;
        firstSize = text.length();
        text += text;
        text += SENTINEL;
        sizeText = text.length();
        buildSuffixTree();
    }

    void specificTreveling(Node *node, std::vector<int> &vec, int &j, int len) {
        int currentLen = diff(node) + 1 + len;
        if (diff(node) == 0 && currentLen < firstSize + 2 && node->indexOfSuffix != -1)
            return;
        if (currentLen > firstSize  && node->indexOfSuffix != -1) {
            if (node->indexOfSuffix == 0)
                vec.insert(vec.begin(), j);
            vec.push_back(node->indexOfSuffix);
            ++j;
            return;
        }
        for (auto it : node->children) {
            specificTreveling(it.second, vec, j, currentLen);
        }
    }

    std::string BWTString(int &number) {
        std::vector<int> indexs;
        int j = 0;
        specificTreveling(root, indexs, j, 0);
        std::string res = "";
        number = indexs[0];
        for (int i = 1; i < indexs.size(); ++i)
            res += text[indexs[i] + firstSize - 1];
        return res;
    }

    void lexicographicalSort(Node *node, std::string str) {
        if (node->indexOfSuffix != -1) {
            std::cout << str << ' ' << node->indexOfSuffix << '\n';
        }

        for (auto it : node->children) {
            std::string strEdge = text.substr(it.second->start, diff(it.second) + 1);
            lexicographicalSort(it.second, str+strEdge);
        }
    }

    void printTree() {
        lexicographicalSort(root, ""); 
    }
};
};


// It'll be using in bitset
const int NUMBER_OF_BITS = 8;

// This const variable store number of bits for each liter 
const int SIZE_OF_NUMBER = 16;

void printAlphabet(std::vector<std::pair<char, int>> &vec) {
    std::cout << "\n----\n";
    for (int i = 0; i < vec.size(); ++i) {
        std::cout << vec[i].first << ' ' << vec[i].second << '\n';
    }
    std::cout << "----\n";
}

void printMap(std::map<std::string, char> a) {
    for (auto &[key, value] : a) {
        std::cout << key << ' ' << value << '\n';
    }
}

struct Node {
    std::pair<char, int> symbol;
    Node *left, *right;
    int commonFrequency;

    Node () {
        left = nullptr;
        right = nullptr;
    }

    Node (std::pair<char, int> p) {
        symbol = p;
        left = nullptr;
        right = nullptr;
    }

    Node (std::pair<char, int> l, std::pair<char, int> r) {
        left = new Node(l);
        right = new Node(r);
        commonFrequency = l.second + r.second;
    }

    Node (Node *l, std::pair<char, int> r) {
        left = l;
        right = new Node(r);
        commonFrequency = l->commonFrequency + r.second;
    }

    Node (Node *l, Node *r) {
        left = l;
        right = r;
        commonFrequency = l->commonFrequency + r->commonFrequency;
    }
};

bool sortByNumbers(std::pair<char, int> a, std::pair<char, int> b) {
    return a.second < b.second;
}

std::string readAll(std::ifstream &istrm) {
    std::string tmp;
    std::string res = "";
    while (std::getline(istrm, tmp)) {
        res += tmp;
        res += '\n';
    }
    //res += tmp;
    return res;
}

std::string readOutText() {
    std::string fileName = "text.txt";
    std::ifstream istrm(fileName, std::ios::binary);
    if (!istrm.is_open()) {
        std::cout << "Error\n";
        return "";
    }
    std::string text;
    std::getline(istrm,text);
    return text;
}

std::string runLengthEncoding(std::string &str) {
    std::string res = "";
    int i = 0;
    while (i < str.length()) {
        if (i == str.length() - 1) {
            res += str[i];
            break;
        }
        if (str[i] != str[i + 1]) {
            res += str[i];
            ++i;
        }
        else {
            int k = 1;
            res += str[i];
            while (i < str.length() && str[i] == str[i + 1]) {
                ++k;
                ++i;
            } 
            res += std::to_string(k);
            ++i;
        }
    }
    return res;
}

Node* buildHaffmanTree(std::string &str, std::vector<std::pair<char, int>> &alphabetVec) {
    std::deque<std::pair<char, int>> firstQueue;
    std::deque<Node *> secondQueue;
    for (int i = 0; i < alphabetVec.size(); ++i) 
        firstQueue.push_back(alphabetVec[i]);
    while (!(firstQueue.size() == 0 && secondQueue.size() == 1)) {
        if (secondQueue.empty()) {
            if (firstQueue.size() > 1) {
                secondQueue.push_back(new Node(firstQueue[0], firstQueue[1]));
                firstQueue.pop_front();
                firstQueue.pop_front();
            }
            else {
                secondQueue.push_back(new Node(firstQueue[0]));
                firstQueue.pop_front();
            }
        }
        else if (firstQueue.size() > 1 && secondQueue.size() > 1) {
            std::vector<int> combinations(3);
            combinations[0] = firstQueue[0].second + firstQueue[1].second;
            combinations[1] = firstQueue[0].second + secondQueue[0]->commonFrequency;
            combinations[2] = secondQueue[0]->commonFrequency + secondQueue[1]->commonFrequency;
            int minIndex = std::distance(combinations.begin(), std::min_element(combinations.begin(), 
                        combinations.end()));
            if (minIndex == 0) {
                secondQueue.push_back(new Node(firstQueue[0], firstQueue[1]));
                firstQueue.pop_front();
                firstQueue.pop_front();
            }
            else if (minIndex == 1) {
                secondQueue.push_back(new Node(secondQueue[0], firstQueue[0]));
                firstQueue.pop_front();
                secondQueue.pop_front();
            }
            else {
                secondQueue.push_back(new Node(secondQueue[0], secondQueue[1]));
                secondQueue.pop_front();
                secondQueue.pop_front();
            }
        }
        else if (firstQueue.size() == 1 && secondQueue.size() == 1) {
                secondQueue.push_back(new Node(secondQueue[0], firstQueue[0]));
                firstQueue.pop_front();
                secondQueue.pop_front();
        }
        else if (firstQueue.size() == 1) {
            std::vector<int> combinations(2);
            combinations[0] = firstQueue[0].second + secondQueue[0]->commonFrequency;
            combinations[1] = secondQueue[0]->commonFrequency + secondQueue[1]->commonFrequency;
            int minIndex = std::distance(combinations.begin(), std::min_element(combinations.begin(), 
                        combinations.end()));
            if (minIndex == 0) {
                secondQueue.push_back(new Node(secondQueue[0], firstQueue[0]));
                firstQueue.pop_front();
                secondQueue.pop_front();
            }
            else {
                secondQueue.push_back(new Node(secondQueue[0], secondQueue[1]));
                secondQueue.pop_front();
                secondQueue.pop_front();
            }
        }
        else if (firstQueue.size() == 0) {
            if (secondQueue.size() == 0) {
                std::cout << "Error" << '\n';
                break;
            }
            else {
               secondQueue.push_back(new Node(secondQueue[0], secondQueue[1]));
                secondQueue.pop_front();
                secondQueue.pop_front();
            }
        }
        else if (secondQueue.size() == 1) {
            std::vector<int> combinations(2);
            combinations[0] = firstQueue[0].second + firstQueue[1].second;
            combinations[1] = firstQueue[0].second + secondQueue[0]->commonFrequency;
            int minIndex = std::distance(combinations.begin(), std::min_element(combinations.begin(), 
                        combinations.end()));
            if (minIndex == 0) {
                secondQueue.push_back(new Node(firstQueue[0], firstQueue[1]));
                firstQueue.pop_front();
                firstQueue.pop_front();
            }
            else {
                secondQueue.push_back(new Node(secondQueue[0], firstQueue[0]));
                firstQueue.pop_front();
                secondQueue.pop_front();
            }
        }
        else {
            std::cout << "XZ" << '\n';
            break;
        }
    }
    return secondQueue[0];
}

void serializeRealisation(Node *root, std::string &str) {
    if (root->left == nullptr && root->right == nullptr) {
        str += '1';
        str += char(root->symbol.first);
        return;
    }
    str += '0';
    serializeRealisation(root->left, str);
    serializeRealisation(root->right, str);
}

std::string serializeTree(Node *root) {
    std::string str = "";
    serializeRealisation(root, str);
    return str;
}

std::string modificationOfText(std::string str, int &BWTNumber) {
    NSuffixTree::SuffixTree *tree = new NSuffixTree::SuffixTree();
    tree->buildDoubleSuffixTree(str);
    std::string BWTString = tree->BWTString(BWTNumber);
    //std::cout << "before BWTString = " << BWTString << '\n';
    std::string RLEString = runLengthEncoding(BWTString);
    //std::cout << "before RLEString = " << RLEString << '\n';
    return RLEString;
}

void fullUpContaiber(std::string &container, const int &size) {
    for (int i = container.length(); i < size; ++i) {
        container += '0';
    }
}
void writeDownTreeInFile(Node *root, std::string fileName, int size) {
    std::string serializationOfTree = serializeTree(root);
    //std::cout << "serializationOfTree = " << serializationOfTree << '\n';
    std::ofstream fout(fileName, std::ios::binary);
    std::string container = "";
    std::string res = "";
    std::string q = std::bitset<NUMBER_OF_BITS>(size).to_string();
    for (int i = 0; i < NUMBER_OF_BITS; ++i) {
        fout.put(q[i]);
    }
    for (int i = 0; i < serializationOfTree.length(); ++i) {
        fout.put(serializationOfTree[i]);
    }
    fout.close();
}

void writeDownTextInFile(std::string text, std::string fileName, 
    std::map<char, std::string> alphabetCodes, int BWTNumber) {
    std::ofstream fout(fileName, std::ios::binary | std::ios::app); 
    std::string container = "";
    int numberOfLiters = 0;
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] < '0' || text[i] > '9') {
            ++numberOfLiters;
        }
    }
    //std::cout << "NUmber of Liters = " << numberOfLiters << '\n';
    container += std::bitset<32>(numberOfLiters).to_string();
    container += std::bitset<32>(BWTNumber).to_string();
    int i = 0;
    while (i < text.length()) {
        container += alphabetCodes[text[i]];
        if (i != text.length() - 1) {
            ++i;
            if (text[i] >= '1' && text[i] <= '9') {
                int number = 0;
                while (i < text.length() && text[i] >= '0' && text[i] <= '9') {
                    number = number*10 + (text[i] - '0');
                    ++i;
                }
                container += '1';
                container += std::bitset<NUMBER_OF_BITS>(number).to_string();
            }
            else {
                container += '0';
            }
        }
        else {
            ++i;
            container += '0';
        }
    }
    while (container.length() >= NUMBER_OF_BITS) {
        fout.put(char(std::bitset<NUMBER_OF_BITS>(container.substr(0, NUMBER_OF_BITS)).to_ulong()));
        container.erase(0, NUMBER_OF_BITS);
    }
    if (container.length() != 0) {
        fullUpContaiber(container, NUMBER_OF_BITS);
        fout.put(char(std::bitset<NUMBER_OF_BITS>(container.substr(0,
                            NUMBER_OF_BITS)).to_ulong()));
       
    }
    fout.close();
}
            
void buildCodes(Node *root, std::map<char, std::string> &alphabetCodes, 
        std::string str) {
    if (root->left == nullptr && root->right == nullptr) {
        alphabetCodes[root->symbol.first] = str;
        return;
    }
    buildCodes(root->left, alphabetCodes, str+"0");
    buildCodes(root->right, alphabetCodes, str+"1");
}

void restoreCodes(Node *root, std::map<std::string, char> &alphabetCodes, 
        std::string str) {
    if (root->left == nullptr && root->right == nullptr) {
        alphabetCodes[str] = root->symbol.first;
        return;
    }
    restoreCodes(root->left, alphabetCodes, str+"0");
    restoreCodes(root->right, alphabetCodes, str+"1");
}

void writeDown(std::string text, std::string fileName) {
    std::map<char, int> alphabetMap;
    std::vector<std::pair<char, int>> alphabetVec;
    std::map<char, std::string> alphabetCodes;
    for (int i = 0; i < text.size(); ++i) {
        if (alphabetMap.count(text[i]) == 0) 
            alphabetMap[text[i]] = 1;
        else
            ++alphabetMap[text[i]];
    }
    alphabetVec = std::vector<std::pair<char, int>>(alphabetMap.size());
    std::map<char, int>::iterator mapInterator = alphabetMap.begin();
    for (int i = 0; i < alphabetMap.size(); ++i) {
        alphabetVec[i] = *mapInterator;
        ++mapInterator;
    }
    std::sort(alphabetVec.begin(), alphabetVec.end(), sortByNumbers);
    Node *root = buildHaffmanTree(text, alphabetVec);
    buildCodes(root, alphabetCodes, "");
    //printMap(alphabetCodes);
    int BWTNumber;
    std::string modifierString = modificationOfText(text, BWTNumber);
    //std::cout << "BWTNumber = " << BWTNumber << '\n';
    writeDownTreeInFile(root, fileName, alphabetVec.size());
    writeDownTextInFile(modifierString, fileName, alphabetCodes, BWTNumber);
}

std::string readOutFromFileTree(std::string fileName, int &index) {
    std::ifstream fin(fileName, std::ios::binary);
    std::string codingTree;
    std::getline(fin, codingTree);
    std::string container = "";
    std::string res = "";
    int numberOfLiters = 
        std::bitset<NUMBER_OF_BITS>(codingTree.substr(0, NUMBER_OF_BITS)).to_ulong();
    int i = 8;
    int counter = 0;
    index += i;
    while (counter != numberOfLiters) {
        res += char(std::bitset<NUMBER_OF_BITS>(codingTree[i]).to_ulong());
        if (codingTree[i] != '0' && codingTree[i] != '1') {
            ++counter;
        }
        ++i;
        ++index;
    }
    return res;
}

std::string readOutFromFileText(std::string fileName, 
        std::map<std::string, char> alphabetCodes, int index, int &BWTNumber) {
    std::string res = "";
    std::ifstream fin(fileName, std::ios::binary);
    std::string text = readAll(fin);
    //std::getline(fin, text);
    // Get numberOfLiters
    for (int i = 0; i < 4; ++i) {
        res += std::bitset<NUMBER_OF_BITS>(text[index + i]).to_string();
    }
    int numberOfLiters = std::bitset<32>(res).to_ulong();
    //std::cout << "numberOfLiters = " << numberOfLiters << '\n';
    res = "";
    index += 4;
    // Get BWTNumber
    for (int i = 0; i < 4; ++i) {
        //res += std::to_string(std::bitset<NUMBER_OF_BITS>(text[index + i]).to_ulong());
        res += std::bitset<NUMBER_OF_BITS>(text[index + i]).to_string();
    }
    index += sizeof(int);
    BWTNumber = std::bitset<32>(res).to_ulong();
    //std::cout << "BWTNumber = " << BWTNumber << '\n';
    res = "";
    std::string container = "";
    int gettingLiters = 0;
    res = "";
    //std::cout << "INDE = " << std::bitset<NUMBER_OF_BITS>(text[index+1]).to_string() << " TEX = " << std::bitset<56>(text).to_string() << '\n';
    while (gettingLiters != numberOfLiters) {
        if (container.length() < 3*NUMBER_OF_BITS) {
            container += std::bitset<NUMBER_OF_BITS>(text[index]).to_string();
            ++index;
        }
        else {
            int j = 1;
            while (alphabetCodes.count(container.substr(0, j)) == 0) {
                ++j;
            }
            res += alphabetCodes[container.substr(0, j)];
            ++gettingLiters;
            container.erase(0, j);
            if (container[0] == '0')
                container.erase(0, 1);
            else {
                res += std::to_string(std::bitset<NUMBER_OF_BITS>(container.substr(1, NUMBER_OF_BITS)).to_ulong());
                container.erase(0, NUMBER_OF_BITS + 1);
            }
        }
    }
    return res;
}

void buildTreeRealisation(Node *root, std::string str, int &i) {
    if (i == str.length())
        return;
    if (str[i] == '0') {
        root->left = new Node();
        ++i;
        buildTreeRealisation(root->left, str, i);
        root->right = new Node();
        ++i;
        buildTreeRealisation(root->right, str, i);
    }
    else {
        ++i;
        root->symbol = std::make_pair(str[i], 0);
        return;
    }
}

void buildTree(Node *root, std::string str) {
    int i = 0;
    buildTreeRealisation(root, str, i);
}

std::string reverseRunLengthEncoding(std::string str) {
    std::string res = "";
    int i = 0;
    while (i < str.length()) {
        if (str[i] < '0' || str[i] > '9') {
            res += str[i];
            ++i;
        }
        else {
            int k = 0;
            char symbol = str[i-1];
            while (i < str.length() && str[i] >= '0' && str[i] <= '9') {
                k = k*10 + (str[i] - '0');
                ++i;
            }
            for (int j = 1; j < k; ++j) {
                res += symbol;
            }
        }
    }
    return res;
}

void reverseBWTString(std::string str, int BWTNumber) {
    std::map<char, int> firstOccurrence;
    std::vector<int> text(256);
    for (int i = 0; i < str.length(); ++i) {
        ++text[int(str[i])];
    }
    std::string newText = "";
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < text[i]; ++j)
            newText += char(i);
    }
    //std::cout << "New text = "<< newText << '\n';
    for (int i = 0; i < str.length(); ++i) {
        if (firstOccurrence.count(newText[i]) == 0) {
            firstOccurrence[newText[i]] = i;
        }
    }
    std::vector<int> forBWTStrin(str.length());
    for (int i = 0; i < str.length(); ++i) {
        forBWTStrin[i] = firstOccurrence[str[i]];
        ++firstOccurrence[str[i]];
    }
    std::vector<int> transition(str.length());
    for (int i = 0; i < str.length(); ++i) {
        transition[forBWTStrin[i]] = i;
    }
    int index = BWTNumber;
    std::string res = "";
    for (int i = 0; i < str.length(); ++i) {
        res += newText[index];
        index = transition[index];
    }
    std::cout << res << '\n';
}

std::string readOut(std::string fileName) {
    std::map<std::string, char> alphabetCodes;
    int index = 0;
    std::string serializationOfTree = readOutFromFileTree(fileName, index);
    //std::cout << "serializationOfTree = " << serializationOfTree << '\n';
    Node *root = new Node();
    buildTree(root, serializationOfTree);
    restoreCodes(root, alphabetCodes, "");
    //printMap(alphabetCodes);
    int BWTNumber;
    std::string RLEString = readOutFromFileText(fileName, alphabetCodes, index, 
            BWTNumber);
    //std::cout << "after RLEString = " << RLEString << '\n';
    std::string BWTString = reverseRunLengthEncoding(RLEString);
    //std::cout << "after BWTString = " << BWTString << '\n';
    reverseBWTString(BWTString, BWTNumber);
    return "a";
}

int main() {
    std::string text = readOutText();
    writeDown(text, "code.b");
    readOut("code.b");
}
