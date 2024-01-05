#include "InternalNode.hpp"

// creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr)
{
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

// max element from tree rooted at this node
Key InternalNode::max()
{
    Key max_key = DELETE_MARKER;
    TreeNode *last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

// if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr()
{
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

// inserts <key, record_ptr> into subtree rooted at this node.
// returns pointer to split node if exists
// TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr)
{
    TreePtr new_tree_ptr = NULL_PTR;

    ///////////TEAM_6///////////////

    TreePtr next_tree_ptr = NULL_PTR;
    int current = -1;
    int j = 0;

    for (; j < this->keys.size(); j++)
    {
        if (key <= this->keys[j])
        {
            current = j;
            break;
        }
        else
            continue;
    }

    if (j == this->keys.size())
    {
        current = j;
    }

    next_tree_ptr = this->tree_pointers[current];
    TreeNode *next_child_node = TreeNode::tree_node_factory(next_tree_ptr);

    if (next_tree_ptr == NULL_PTR)
    {
        cout << "NULL CHILD POINTER" << endl;
        return new_tree_ptr;
    }
    else
    {
    }
    TreePtr potential_split_node_ptr = next_child_node->insert_key(key, record_ptr);

    if (!is_null(potential_split_node_ptr))
    {

        this->keys.insert(this->keys.begin() + current, next_child_node->max());
        this->tree_pointers.insert(this->tree_pointers.begin() + current + 1, potential_split_node_ptr);
        this->size++;

        // if overflow happens
        if (this->overflows())
        {
            vector<TreePtr> vec_ptr;
            vector<Key> vec_keys;

            // pushing to vectors of keys and pointers after overflow
            for (int i = 0; i < this->size - 1; i++)
            {
                vec_ptr.push_back(this->tree_pointers[i]);
                vec_keys.push_back(this->keys[i]);
            }

            vec_ptr.push_back(this->tree_pointers[this->size - 1]);
            this->size = 0;
            this->tree_pointers.clear();
            this->keys.clear();
            // made new internal node due to overflow
            auto new_internal_node = new InternalNode();
            int i = 0;

            // setting keys and pointers to this node according to min occupancy
            for (; i < MIN_OCCUPANCY - 1; i++)
            {
                this->size++;
                this->tree_pointers.push_back(vec_ptr[i]);
                this->keys.push_back(vec_keys[i]);
            }

            this->size++;
            this->tree_pointers.push_back(vec_ptr[i]);
            i++;

            // setting remaining values to new node
            for (; i < vec_keys.size(); i++)
            {
                new_internal_node->size++;
                new_internal_node->tree_pointers.push_back(vec_ptr[i]);
                new_internal_node->keys.push_back(vec_keys[i]);
            }

            new_internal_node->size++;
            new_internal_node->tree_pointers.push_back(vec_ptr[i]);

            new_internal_node->dump();
            new_tree_ptr = new_internal_node->tree_ptr;
        }
    }

    delete next_child_node;

    //////////////TEAM_6///////////////
    cout << "InternalNode::insert_key" << endl;
    this->dump();
    return new_tree_ptr;
}

void InternalNode::redistributeLeft(TreeNode *right_sibling_tree_node)
{

    InternalNode *siblingNodeRight = dynamic_cast<InternalNode *>(right_sibling_tree_node);

    siblingNodeRight->size++;
    siblingNodeRight->tree_pointers.push_back(NULL_PTR);
    siblingNodeRight->keys.push_back(DELETE_MARKER);

    for (int i = siblingNodeRight->size - 1; i >= 1; i--)
    {
        siblingNodeRight->tree_pointers[i] = siblingNodeRight->tree_pointers[i - 1];
    }

    for (int i = siblingNodeRight->size - 2; i >= 1; i--)
    {
        siblingNodeRight->keys[i] = siblingNodeRight->keys[i - 1];
    }

    siblingNodeRight->tree_pointers[0] = this->tree_pointers[this->size - 1];
    auto right_sibling_first_child_node = TreeNode::tree_node_factory(siblingNodeRight->tree_pointers[0]);
    siblingNodeRight->keys[0] = right_sibling_first_child_node->max();

    delete right_sibling_first_child_node;

    this->tree_pointers.pop_back();
    this->keys.pop_back();
    this->size--;

    this->dump();
    siblingNodeRight->dump();
}
void InternalNode::mergeNode(TreeNode *sibling_tree_node)
{
    auto selfLastChildNode = TreeNode::tree_node_factory(this->tree_pointers.back());
    this->keys.push_back(selfLastChildNode->max());

    InternalNode *siblingNode = dynamic_cast<InternalNode *>(sibling_tree_node);

    delete selfLastChildNode;

    for (int i = 0; i < siblingNode->size - 1; i++)
    {
        this->keys.push_back(siblingNode->keys[i]);
    }
    for (int i = 0; i < siblingNode->size; i++)
    {
        this->tree_pointers.push_back(siblingNode->tree_pointers[i]);
    }
    this->size += siblingNode->size;

    siblingNode->delete_node();
    this->dump();
}
void InternalNode::redistributeRight(TreeNode *left_sibling_tree_node)
{
    InternalNode *siblingNodeLeft = dynamic_cast<InternalNode *>(left_sibling_tree_node);

    auto left_sibling_last_child_node = TreeNode::tree_node_factory(siblingNodeLeft->tree_pointers.back());
    siblingNodeLeft->keys.push_back(left_sibling_last_child_node->max());
    siblingNodeLeft->tree_pointers.push_back(this->tree_pointers[0]);
    siblingNodeLeft->size++;
    this->size--;

    delete left_sibling_last_child_node;

    for (int i = 0; i < this->size - 1; i++)
    {
        this->keys[i] = this->keys[i + 1];
    }

    for (int i = 0; i < this->size; i++)
    {
        this->tree_pointers[i] = this->tree_pointers[i + 1];
    }

    this->tree_pointers.pop_back();
    this->keys.pop_back();

    this->dump();
    siblingNodeLeft->dump();
}

// deletes key from subtree rooted at this if exists
// TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key)
{
    int pos = 0;
    while (pos < this->size - 1 && this->keys[pos] != DELETE_MARKER && key > this->keys[pos])
    {
        pos++;
    }

    auto childNode = TreeNode::tree_node_factory(this->tree_pointers[pos]);

    childNode->delete_key(key);

    if (pos < this->size - 1)
    {
        this->keys[pos] = childNode->max();
    }

    if (!childNode->underflows())
    {
        delete childNode;
        this->dump();
        return;
    }

    if (pos > 0)
    {
        auto siblingNodeLeft = TreeNode::tree_node_factory(this->tree_pointers[pos - 1]);
        if (siblingNodeLeft->size + childNode->size >= 2 * MIN_OCCUPANCY)
        {
            siblingNodeLeft->redistributeLeft(childNode);
            this->keys[pos - 1] = siblingNodeLeft->max();
        }
        else
        {
            siblingNodeLeft->mergeNode(childNode);
            for (int i = pos; i < this->size - 1; i++)
            {
                this->tree_pointers[i] = this->tree_pointers[i + 1];
            }
            for (int i = pos - 1; i < this->size - 2; i++)
            {
                this->keys[i] = this->keys[i + 1];
            }
            this->tree_pointers.pop_back();
            this->size--;
            this->keys.pop_back();
        }
        delete siblingNodeLeft;
    }
    else
    {
        auto siblingNodeRight = TreeNode::tree_node_factory(this->tree_pointers[pos + 1]);
        if (siblingNodeRight->size + childNode->size >= 2 * MIN_OCCUPANCY)
        {
            siblingNodeRight->redistributeRight(childNode);
            this->keys[pos] = childNode->max();
        }
        else
        {
            childNode->mergeNode(siblingNodeRight);
            for (int i = pos; i < this->size - 2; i++)
            {
                this->keys[i] = this->keys[i + 1];
            }
            for (int i = pos + 1; i < this->size - 1; i++)
            {
                this->tree_pointers[i] = this->tree_pointers[i + 1];
            }
            this->keys.pop_back();
            this->tree_pointers.pop_back();
            this->size--;
        }
        delete siblingNodeRight;
    }
    delete childNode;
    this->dump();
}

// runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const
{
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++)
    {
        if (min_key <= this->keys[i])
        {
            auto *childNode = TreeNode::tree_node_factory(this->tree_pointers[i]);
            childNode->range(os, min_key, max_key);
            delete childNode;
            return;
        }
    }
    auto *childNode = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    childNode->range(os, min_key, max_key);
    delete childNode;
}

// exports node - used for grading
void InternalNode::export_node(ostream &os)
{
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++)
    {
        auto childNode = TreeNode::tree_node_factory(this->tree_pointers[i]);
        childNode->export_node(os);
        delete childNode;
    }
}

// writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os)
{
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++)
    {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1)
        {
            link += to_string(this->keys[i - 1]) + " < x";
        }
        else
        {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream &InternalNode::write(ostream &os) const
{
    TreeNode::write(os);
    for (int i = 0; i < this->size - 1; i++)
    {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream &InternalNode::read(istream &is)
{
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++)
    {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
