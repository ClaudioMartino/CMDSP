#include <cstdlib> 
#include <iostream> 
#include <vector>

// Thanks to https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/

struct HuffmanNode { 
  HuffmanNode (char d, float f)
    : data { d }, freq { f }, left { NULL }, right { NULL } { 
  }

  HuffmanNode (float f)
    : data { '*' }, freq { f }, left { NULL }, right { NULL } { 
  }

  char data; 
  float freq; 
  struct HuffmanNode* left; 
  struct HuffmanNode* right; 
};

void swapNode(struct HuffmanNode** a, struct HuffmanNode** b) {
  struct HuffmanNode* tmp = *a;
  *a = *b;
  *b = tmp;
} 

struct Huffman { 
  struct HuffmanNode* extractMin() {
    struct HuffmanNode* min_ptr = array[0]; 
    array[0] = array[size - 1]; 
    size--; 
    minHeapify(0); 
    return min_ptr;
  }

  void insert(struct HuffmanNode* minHeapNode) { 
    size_t i = size; 
    while (i && minHeapNode->freq < array[(i - 1) / 2]->freq) { 
      array[i] = array[(i - 1) / 2]; 
      i = (i - 1) / 2;
    }
    array[i] = minHeapNode; 
    size++;
  } 

  void minHeapify(size_t idx) {
    // Min-Heap: The value of the root node must be the smallest among all its descendant nodes and the same thing must be done for its left and right sub-tree
    int smallest = idx; 
    int left = 2 * idx + 1; 
    int right = 2 * idx + 2; 

    if (left < size && array[left]->freq < array[smallest]->freq) 
      smallest = left; 

    if (right < size && array[right]->freq < array[smallest]->freq) 
      smallest = right; 

    if (smallest != idx) {
      swapNode(&array[smallest], &array[idx]); 
      minHeapify(smallest); 
    } 
  }

  size_t size; 
  size_t capacity; 
  struct HuffmanNode** array; 
}; 

struct HuffmanNode* buildHuffmanTree(std::vector<char> data, std::vector<float> freq) {
  size_t size = freq.size();

  // Create a min heap of capacity equal to size and nodes equal to size. 
  struct Huffman* minHeap = (struct Huffman*)malloc(sizeof(struct Huffman));
  minHeap->capacity = size;
  minHeap->size = size;
  minHeap->array = (struct HuffmanNode **)malloc(minHeap->capacity * sizeof(struct HuffmanNode *)); 
  for (size_t i = 0; i < size; ++i) 
    minHeap->array[i] = new HuffmanNode(data[i], freq[i]);

  for (int i = (minHeap->size - 2) / 2; i >= 0; --i) 
    minHeap->minHeapify(i); 

  // Iterate while size of heap doesn't become 1
  struct HuffmanNode *left;
  struct HuffmanNode *right;
  struct HuffmanNode *top;
  while (minHeap->size != 1) { 
    // Extract the two minimum freq items
    left  = minHeap->extractMin(); 
    right = minHeap->extractMin(); 

    // Create a new internal node with frequency equal to the sum of the two frequencies 
    top = new HuffmanNode(left->freq + right->freq);
    top->left  = left; 
    top->right = right; 
    minHeap->insert(top); 
  } 

  // The remaining node is the root node and the tree is complete
  return minHeap->extractMin();
} 

void printCodes(struct HuffmanNode* root, int* arr, int i) {
  // Go down left edge
  if (root->left) { 
    arr[i] = 0; 
    printCodes(root->left, arr, i + 1); 
  }

  // Go down right edge
  if (root->right) { 
    arr[i] = 1;
    printCodes(root->right, arr, i + 1); 
  } 

  // Leaf node, print the code
  if (!(root->left) && !(root->right)) {
    std::cout << root->data << ": "; 
    for (size_t k=0; k<i; ++k) 
      std::cout << arr[k]; 
    std::cout << std::endl;
  }
} 

int main() {
  std::vector<char> arr = { 'a', 'b', 'c', 'd', 'e', 'f' }; 
  std::vector<float> freq = { 0.05, 0.09, 0.12, 0.13, 0.16, 0.45 }; 

  // Construct Huffman tree 
  struct HuffmanNode* root = buildHuffmanTree(arr, freq); 

  // Print Huffman codes
  int arr2[100];
  printCodes(root, arr2, 0); 

  return 0; 
}

