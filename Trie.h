#include <vector>
#include <map>
template <typename char_t>
struct trie_node
{
	std::vector<char_t> prefix;
	std::map<std::vector<char_t>, trie_node*> children;
	trie_node* parent;
	int offset;
	
};

template <typename char_t>
class Trie
{
	public:
	Trie();
	~Trie();
	insert(
	
	private:
		trie_node* root;
	
};