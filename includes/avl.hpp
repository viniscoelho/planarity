struct node {
    int key;
    unsigned char height;
    node* left;
    node* right;
    node(int k) {
        key = k; left = right = 0; height = 1;
    }
};

unsigned char height(node* p)
{
    return p ? p->height : 0;
}

int b_factor(node* p)
{
    return height(p->right) - height(p->left);
}

void fix_height(node* p)
{
    unsigned char hl = height(p->left);
    unsigned char hr = height(p->right);
    p->height = ((hl > hr) ? hl : hr) + 1;
}

node* rotate_right(node* p)
{
    node* q = p->left;
    p->left = q->right;
    q->right = p;
    fix_height(p);
    fix_height(q);
    return q;
}

node* rotate_left(node* q)
{
    node* p = q->right;
    q->right = p->left;
    p->left = q;
    fix_height(q);
    fix_height(p);
    return p;
}

// balance balances a p node.
node* balance(node* p)
{
    fix_height(p);
    if (b_factor(p) == 2) {
        if (b_factor(p->right) < 0)
            p->right = rotate_right(p->right);
        return rotate_left(p);
    }
    if (b_factor(p) == -2) {
        if (b_factor(p->left) > 0)
            p->left = rotate_left(p->left);
        return rotate_right(p);
    }
    // balancing is not required
    return p;
}

// insert inserts k key in a tree with p root.
node* insert(node* p, int k)
{
    if (!p) return new node(k);
    if (k < p->key)
        p->left = insert(p->left, k);
    else
        p->right = insert(p->right, k);
    return balance(p);
}

// find_min finds a node with minimal key in a p tree.
node* find_min(node* p)
{
    return p->left ? find_min(p->left) : p;
}

bool find(node *p, int key)
{
    if (!p) return false;
    if (key == p->key) return true;
    if (key < p->key) return find(p->left, key);
    else return find(p->right, key);
}