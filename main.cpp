/*===========================================================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <deque>
#include <assert.h>

#include <algorithm>
struct Node;

typedef uint8_t uchar;
typedef uint32_t weightlen_t;
typedef std::deque<Node*> forest_t;
typedef int16_t index_t;

/*===========================================================================*/
struct Node {
    index_t index;
    weightlen_t weight;
    Node* left;
    Node* right;
};
/*===========================================================================*/

void create_weight_table(weightlen_t *des_buf, uchar *src_buf, size_t src_len) {
    for (size_t i = 0; i != src_len; ++i) {
        // printf("src_buf[%lu] =%d\n", i, src_buf[i]);
        // printf("des_buf[%lu] =%d\n", i, des_buf[i]);

        ++des_buf[(size_t)src_buf[i]];
        //
        //         printf("++des_buf[%lu] =%d\n", (size_t)src_buf[i], des_buf[(size_t)src_buf[i]]);
        //         printf("--------------------------------------------\n");
    }
}

void init_forest_by_weight_table(forest_t& forest, const weightlen_t* w_table) {

    forest.clear();
    for(size_t i = 0; i !=256; ++i) {
        if (!w_table[i]) {
            continue;
        }
        Node* pnode = new Node();
        assert(pnode);

        pnode->weight = w_table[i];
        pnode->left = pnode->right = NULL;
        pnode->index = i;

        forest.push_back(pnode);

    }

}

bool node_compare(Node* n1, Node* n2) {
    return n1->weight < n2->weight;
}

Node* setup_huffman_tree(forest_t& forest) {
    for (; forest.size()>=2;) {
        std::sort(forest.begin(), forest.end(), node_compare);
        Node* pnode = new Node();

        pnode->left = forest.at(0);
        pnode->right = forest.at(1);
        pnode->weight = pnode->left->weight + pnode->right->weight;
        pnode->index = -1;

        forest.pop_front();
        forest.pop_front();
        forest.push_back(pnode);
    }

    return forest.front();

}

/*===========================================================================*/
struct encoding_t {
    uint8_t len;
    uint32_t data;
};
/*===========================================================================*/
void print_postfix(Node* node) {
    if (node->left) {
        //        printf("[L]\n");
        print_postfix(node->left);
    } else {
        //        printf("[Le]\n");
    }

    if (node->right) {
        //       printf("[R]\n");
        print_postfix(node->right);
    } else {
        //        printf("[Re]\n");
    }

    //printf("[%u, %d]\n", node->weight, node->index);

    printf("[%u]\n", node->weight);
}
/*===========================================================================*/
void ff(Node *node, encoding_t* encoding_data, encoding_t* e_table) {

    if (node->left) {
        ++encoding_data->len;
        ff(node->left, encoding_data, e_table);
        //        encoding_data->data |=  (0 << encoding_data->len);
        --encoding_data->len;
        //        encoding_data->data &= ~(0 << encoding_data->len);
    }

    if (node->right) {
        encoding_data->data |=  (1 << encoding_data->len);
        ++encoding_data->len;
        ff(node->right, encoding_data, e_table);
        encoding_data->data &= ~(1 << encoding_data->len);
        --encoding_data->len;
    }

    if (!(node->right || node->left)) {
        //e_table[node->index].len = encoding_data->len;
        //e_table[node->index].len = encoding_data->len;
        e_table[node->index] = *encoding_data;
        // for(int j = 0; j < e_table[node->index].len; ++j) {
        //     printf("%u-", (e_table[node->index].data >> j) & 0x1);
        // }
        // printf("\n");
        //        e_table[node->index] = *encoding_data;
    }

}

int bit_add(uchar* des, const uchar* src, int start_bit, int num_bit) {
    unsigned int bit_cur= 0;
    for (int i = 0; i <  num_bit; ++i) {
        bit_cur = (start_bit + i) % 8;
//        des[(i + start_bit)>>3] |= (src[i >> 8] & (0x1 << (i % 8))) << bit_cur;
        des[(i + start_bit)>>3] |= ((src[i >> 8] >> (i % 8)) & 0x1 ) << bit_cur;
//        printf("((%d))\n", (i+start_bit)>>3);
        printf("bit_cur = %d\n", bit_cur);
        //printf("--%d--\n",(src[i >> 8] & (0x1 << i)) << bit_cur);
        printf("--%d--\n", (src[i >> 8] >> (i % 8)) & 0x1 );
    }

    return start_bit + num_bit;
}

/*===========================================================================*/
int main(void) {

    char src_buf[] = "ABBCCDEEE";
    weightlen_t weight_table[256];
    forest_t forest;
    memset(weight_table, 0, sizeof(weight_table));

    ///create a weigth table
    create_weight_table(weight_table, (uchar*)src_buf, strlen(src_buf));

    // for(int i = 0; i < 256; ++i) {
    //     printf("weight_table[%d] = %u\n", i, weight_table[i]);
    // }

    ///init_forest
    init_forest_by_weight_table(forest, weight_table);

    // for(size_t i = 0; i != forest.size(); ++i) {
    //     if (forest[i]->weight) {
    //         printf("i = %lu, w = %u\n",i, forest[i]->weight );
    //     }
    // }
    //

    Node * p = setup_huffman_tree(forest);
    //print_postfix(p);


    encoding_t encoding_table[256];
    memset(encoding_table, 0, sizeof(encoding_table));
    encoding_t e_data = {0,0};
    ff(p, &e_data, encoding_table);

    // for(int i = 0; i < 256; ++i) {
    //     if (encoding_table[i].len) {
    //         printf("char[%c], weight[%d]  ", i, weight_table[i]);
    //         printf("coding[%d] = [%u]", i, encoding_table[i].len);
    //         for(int j = 0; j < encoding_table[i].len; ++j) {
    //             printf("%u-", (encoding_table[i].data >> j) & 0x1);
    //             //                printf("[%u-]", (encoding_table[i].data >> encoding_table[i].len));
    //         }
    //         printf("\n");
    //
    //     }
    // }

    uint32_t table_len = 0;
    for (int i = 0; i < 256; ++i) {
        if (!encoding_table[i].len)
            continue;

        table_len += 16 + encoding_table[i].len;
    }
    table_len += 32;

    printf("table_len (bit) = %d\n", table_len);
    printf("table_len (byte) = %d\n", table_len/8 + 1);

    /*===========================================================================*/
    uchar * header_table = new uchar[table_len/8+1];

    delete [] header_table;
    header_table = NULL;
    /*===========================================================================*/

    /*===========================================================================*/
    uchar kk[1024];
    memset(kk, 0, sizeof(kk));
    int l = 257;
    bit_add(kk, (uchar*)&l, 1, 8);

    for(int j = 0; j < 8; ++j) {
        printf("[%u]", (kk[0] >> j) & 0x1);
    }
    printf("\n");

    /*===========================================================================*/
    return 0;

}
