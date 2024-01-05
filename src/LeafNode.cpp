#include "RecordPtr.hpp"
#include "LeafNode.hpp"

LeafNode::LeafNode(const TreePtr &tree_ptr) : TreeNode(LEAF, tree_ptr)
{
    this->data_pointers.clear();
    this->next_leaf_ptr = NULL_PTR;
    if (!is_null(tree_ptr))
        this->load();
}

// returns max key within this leaf
Key LeafNode::max()
{
    auto it = this->data_pointers.rbegin();
    return it->first;
}

// inserts <key, record_ptr> to leaf. If overflow occurs, leaf is split
// split node is returned
// TODO: LeafNode::insert_key to be implemented
TreePtr LeafNode::insert_key(const Key &key, const RecordPtr &record_ptr)
{
    TreePtr new_leaf = NULL_PTR; // if leaf is split, new_leaf = ptr to new split node ptr

    /////////TEAM_6///////////////

    //checking if it is required to split the node or not.
    if (this->data_pointers.size() < FANOUT) //not required to split, just adding into the leafnode
    {
        this->data_pointers[key] = record_ptr;
        this->size = this->data_pointers.size();
    }
    else //Required to Split
    {
        vector<RecordPtr> vr(FANOUT + 1); // Vector of record pointers 
        vector<Key> vk(FANOUT + 1); // vector of keys

        int index = 0;
        auto spliting_node = new LeafNode();
        this->data_pointers.insert({key, record_ptr});


        // loop for storing each pointers so that we can arrange keys and their pointers after splitting
        for (auto &ptr : this->data_pointers) 
        {
            vr[index] = ptr.second;
            vk[index] = ptr.first;
            index++;
        }

        //setting split node next pointer
        spliting_node->next_leaf_ptr = this->next_leaf_ptr;
        this->next_leaf_ptr = spliting_node->tree_ptr;


        // empty the pointer vector
        this->data_pointers.clear();
        this->size = 0;

        //assigning pointer to current leaf node as per min occupancy
        for (int i = 0; i < MIN_OCCUPANCY; i++)
        {
            this->size++;
            this->data_pointers[vk[i]] = vr[i];
        }

        //assignming rest other pointer to splitted leaf node
        for (int i = MIN_OCCUPANCY; i < vk.size(); i++)
        {
            spliting_node->size++;
            spliting_node->data_pointers.insert({vk[i], vr[i]});
        }

        // need to tell that new node is splitted node add further data to that
        new_leaf = spliting_node->tree_ptr;

        spliting_node->dump();
    }
    /////////TEAM_6///////////////

    cout << "LeafNode::Inserted" << endl;
    this->dump();
    return new_leaf;
}

// key is deleted from leaf if exists
// TODO: LeafNode::delete_key to be implemented
void LeafNode::delete_key(const Key &key)
{

    ////////////TEAM_6////////////////

    
    if(this->data_pointers.find(key)!= this->data_pointers.end()){
        
        this->size--;
        this->data_pointers.erase(key);
        cout<<"LeafNode::Deleted";
        cout<<endl;
        
    }
    else{
        cout<<"LeafNode::Not found";
        cout<<endl;
    }

    ////////////TEAM_6////////////////


    // cout << "LeafNode::delete_key not implemented" << endl;
    this->dump();
}

// runs range query on leaf
void LeafNode::range(ostream &os, const Key &min_key, const Key &max_key) const
{
    BLOCK_ACCESSES++;
    for (const auto &data_pointer : this->data_pointers)
    {
        if (data_pointer.first >= min_key && data_pointer.first <= max_key)
            data_pointer.second.write_data(os);
        if (data_pointer.first > max_key)
            return;
    }
    if (!is_null(this->next_leaf_ptr))
    {
        auto next_leaf_node = new LeafNode(this->next_leaf_ptr);
        next_leaf_node->range(os, min_key, max_key);
        delete next_leaf_node;
    }
}

// exports node - used for grading
void LeafNode::export_node(ostream &os)
{
    TreeNode::export_node(os);
    for (const auto &data_pointer : this->data_pointers)
    {
        os << data_pointer.first << " ";
    }
    os << endl;
}

// writes leaf as a mermaid chart
void LeafNode::chart(ostream &os)
{
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    for (const auto &data_pointer : this->data_pointers)
    {
        chart_node += to_string(data_pointer.first) + " ";
    }
    chart_node += "]";
    os << chart_node << endl;
}

ostream &LeafNode::write(ostream &os) const
{
    TreeNode::write(os);
    for (const auto &data_pointer : this->data_pointers)
    {
        if (&os == &cout)
            os << "\n"
               << data_pointer.first << ": ";
        else
            os << "\n"
               << data_pointer.first << " ";
        os << data_pointer.second;
    }
    os << endl;
    os << this->next_leaf_ptr << endl;
    return os;
}

istream &LeafNode::read(istream &is)
{
    TreeNode::read(is);
    this->data_pointers.clear();
    for (int i = 0; i < this->size; i++)
    {
        Key key = DELETE_MARKER;
        RecordPtr record_ptr;
        if (&is == &cin)
            cout << "K: ";
        is >> key;
        if (&is == &cin)
            cout << "P: ";
        is >> record_ptr;
        this->data_pointers.insert(pair<Key, RecordPtr>(key, record_ptr));
    }
    is >> this->next_leaf_ptr;
    return is;
}