#include <iostream>
#include <queue>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
using namespace std;

class HuffmanCodec {
public:
    explicit HuffmanCodec(const string& text) {
        for (char ch : text) {
            ++frequencies_[ch];
        }

        root_ = buildTree();
        generateCodes(root_, "");

        for (char ch : text) {
            encodedText_ += codes_.at(ch);
        }
    }

    const map<char, int>& frequencies() const { return frequencies_; }
    const map<char, string>& codes() const { return codes_; }
    const string& encodedText() const { return encodedText_; }

    string decode(const string& encoded) const {
        if (root_ == nullptr) {
            return "";
        }

        if (root_->isLeaf()) {
            return string(encoded.size(), root_->ch);
        }

        string decoded;
        const Node* current = root_;

        for (char bit : encoded) {
            if (bit == '0') {
                current = current->left;
            } else if (bit == '1') {
                current = current->right;
            } else {
                throw invalid_argument("Invalid encoded bit");
            }

            if (current == nullptr) {
                throw invalid_argument("Invalid Huffman code");
            }

            if (current->isLeaf()) {
                decoded += current->ch;
                current = root_;
            }
        }

        if (current != root_) {
            throw invalid_argument("Incomplete encoded data");
        }

        return decoded;
    }

private:
    struct Node {
        char ch;
        int freq;
        const Node* left;
        const Node* right;

        Node(char ch, int freq, const Node* left, const Node* right)
            : ch(ch), freq(freq), left(left), right(right) {}

        bool isLeaf() const {
            return left == nullptr && right == nullptr;
        }
    };

    struct Compare {
        bool operator()(const Node* a, const Node* b) const {
            return a->freq > b->freq;
        }
    };

    const Node* addNode(
        char ch, int freq,
        const Node* left = nullptr, const Node* right = nullptr
    ) {
        nodes_.push_back(make_unique<Node>(ch, freq, left, right));
        return nodes_.back().get();
    }

    const Node* buildTree() {
        priority_queue<const Node*, vector<const Node*>, Compare> minHeap;

        for (const auto& entry : frequencies_) {
            minHeap.push(addNode(entry.first, entry.second));
        }

        if (minHeap.empty()) {
            return nullptr;
        }

        while (minHeap.size() > 1) {
            const Node* left = minHeap.top();
            minHeap.pop();

            const Node* right = minHeap.top();
            minHeap.pop();

            minHeap.push(addNode('\0', left->freq + right->freq, left, right));
        }

        return minHeap.top();
    }

    void generateCodes(const Node* node, const string& currentCode) {
        if (node == nullptr) {
            return;
        }

        if (node->isLeaf()) {
            codes_[node->ch] = currentCode.empty() ? "0" : currentCode;
            return;
        }

        generateCodes(node->left, currentCode + "0");
        generateCodes(node->right, currentCode + "1");
    }

    map<char, int> frequencies_;
    map<char, string> codes_;
    string encodedText_;
    // Owns the nodes. Tree links and heap entries point to these nodes.
    vector<unique_ptr<Node>> nodes_;
    const Node* root_ = nullptr;
};

// Main Function
int main() {

    string text;

    cout << "===== HUFFMAN COMPRESSION =====\n";

    cout << "\nEnter text to compress: ";
    getline(cin, text);

    if (text.empty()) {
        cout << "\nInput text is empty.\n";
        return 0;
    }

    HuffmanCodec codec(text);
    const auto& freqMap = codec.frequencies();
    const auto& codeTable = codec.codes();

    // Display Huffman Codes
    cout << "\nCharacter Frequencies and Huffman Codes:\n";
    cout << "----------------------------------------\n";

    for (auto pair : freqMap) {

        char ch = pair.first;

        if (ch == ' ')
            cout << "Space";
        else if (ch == '\n')
            cout << "Newline";
        else
            cout << ch;

        cout << "\t" << pair.second
             << "\t" << codeTable.at(ch) << endl;
    }

    // Encode Text
    string encoded = codec.encodedText();

    cout << "\nEncoded Binary String:\n";
    cout << encoded << endl;

    // Calculate Compression
    int originalBits = text.size() * 8;
    int compressedBits = encoded.size();

    double ratio =
        (double)compressedBits / originalBits * 100;

    double saved =
        100 - ratio;

    cout << "\n===== COMPRESSION RESULTS =====\n";

    cout << "Original Size: "
         << originalBits << " bits\n";

    cout << "Compressed Size: "
         << compressedBits << " bits\n";

    cout << fixed << setprecision(2);

    cout << "Compression Ratio: "
         << ratio << "%\n";

    cout << "Space Saved: "
         << saved << "%\n";

    // Decode Text
    string decoded = codec.decode(encoded);

    cout << "\n===== DECOMPRESSION =====\n";

    cout << "Decoded Text: "
         << decoded << endl;

    // Verify Original Text
    if (decoded == text) {
        cout << "\nVerification: SUCCESS!\n";
        cout << "Original and decoded text match.\n";
    }
    else {
        cout << "\nVerification: FAILED!\n";
    }

    return 0;
}
