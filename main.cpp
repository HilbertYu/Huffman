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
/**
 *  @brief conut the freqence.
 *  @param des_buf output
 *  @param src_buf source data.
 *  @param src_len the size of the source data (bytes)
 *
 *
 */
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
        --encoding_data->len;
        encoding_data->data &= ~(1 << encoding_data->len);
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

int bit_add(void* arg_des, const void* arg_src, int start_bit, int num_bit, int src_offset = 0) {
    uchar *des = (uchar*) arg_des;
    const uchar* src = (uchar*) arg_src;

    unsigned int bit_cur= 0;
    for (int i = 0; i <  num_bit; ++i) {
        bit_cur = (start_bit + i) % 8;
//        des[(i + start_bit)>>3] |= (src[i >> 8] & (0x1 << (i % 8))) << bit_cur;
        des[(i + start_bit)>>3] |= ((src[(i + src_offset) >> 3] >> ((i + src_offset) % 8)) & 0x1 ) << bit_cur;
//        printf("((%d))\n", (i+start_bit)>>3);
//        printf("bit_cur = %d\n", bit_cur);
        //printf("--%d--\n",(src[i >> 8] & (0x1 << i)) << bit_cur);
//        printf("i = %d, -%d--\n", i, (src[i >> 3] >> (i % 8)) & 0x1 );
//        printf("i >> 3 = %d\n", i >> 3);
//        printf("i mod  8 = %u\n", i % 8);
    }

    return start_bit + num_bit;
}


void huffman_decoding_raw(Node* node, uchar* bit_stream) {
    printf("huffman_deconding_now\n");
    printf("bit_stream[0] = %u\n", bit_stream[17]);
    // for (int i = 0 ; i < 20; ++i) {
    //     uint8_t ch = (bit_stream[i >> 3] >> (i%8)) & 0x01;
    //     printf("[%u]", ch);
    // }
    //

    Node* root = node;
    for (int i = 0 ; i < 20; ++i) {
        uint8_t ch = (bit_stream[i >> 3] >> (i%8)) & 0x01;
        if (!(node->left || node->right)) {
            printf("[%c]", node->index);
            node = root;
        }
        if (!ch) {
            node = node->left;
        }else {
            node = node->right;
        }
        //printf("[%u]", ch);
    }
    printf("\n");
}
/*===========================================================================*/
int main(void) {

    char src_buf[] = "ABBCCDEEE";
    size_t src_len = strlen(src_buf);
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
    print_postfix(p);
//    return 0;

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
    // return 0;

    uint32_t table_len = 0;
    uint32_t encoding_bit_stream_size = 0;
    for (int i = 0; i < 256; ++i) {
        if (!encoding_table[i].len)
            continue;

        table_len += 16 + encoding_table[i].len;
        encoding_bit_stream_size += encoding_table[i].len*weight_table[i];
    }
    table_len += 32;

    printf("table_len (bit) = %d\n", table_len);
    printf("table_len (byte) = %d\n", table_len/8 + 1);
    printf("encoding_size = %d\n", encoding_bit_stream_size);

    /*===========================================================================*/
    uint32_t header_len = table_len/8 + 1;
    uchar * header_table = new uchar[header_len + src_len];

    {
        memset(header_table, 0, header_len);
        int  bit_cur = 0;
        bit_add(header_table, &table_len, bit_cur, 32);
        bit_cur += 32;



        /*===========================================================================*/
        // printf("table_len = %d\n", table_len);
        // printf("header_len = %d\n", header_len);
        // printf("\n\n");
        // exit(-1);
        /*===========================================================================*/


        for(size_t i = 0; i < 256; ++i) {
            if (!weight_table[i])
                continue;

            bit_add(header_table, &i, bit_cur, 8);
            bit_cur += 8;

            bit_add(header_table, &encoding_table[i].len, bit_cur, 8);
            bit_cur += 8;

            bit_add(header_table, &encoding_table[i].data, bit_cur, encoding_table[i].len);
            bit_cur += encoding_table[i].len;

        }



        bit_add(header_table, &encoding_bit_stream_size, bit_cur, 32);
        bit_cur += 32;

        printf("raw encoding...\n");
        int encoding_raw_size = 0;
        for (size_t i = 0; i < src_len; i++) {
            bit_add(header_table, &encoding_table[(size_t)src_buf[i]].data, bit_cur, encoding_table[(size_t)src_buf[i]].len);
            printf("bit_cur = %d, len = %d ", bit_cur, encoding_table[(size_t)src_buf[i]].len);
            /*===========================================================================*/
                for (int j = 0; j < encoding_table[(size_t)src_buf[i]].len; ++j) {
                    printf("[%u]", (encoding_table[(size_t)src_buf[i]].data >> j) & 0x01);
                }
                printf("\n");
            /*===========================================================================*/
            encoding_raw_size += encoding_table[(size_t)src_buf[i]].len;
            bit_cur += encoding_table[(size_t)src_buf[i]].len;
        }

        printf("bit_cur = %d\n", bit_cur);
        printf("ecoding_raw_size = %d\n", encoding_raw_size);
    }


    uint32_t total_len = table_len + 8 + encoding_bit_stream_size;
    printf("total len = %u\n", total_len);
    printf("table = %u, raw = %u\n", table_len, encoding_bit_stream_size);
    printf("table = ##%u, raw = %u\n", header_table[0], header_table[4]);


    ///reconstruct the huffman tree
    Node * htree = NULL;
    {
        uchar rec_table[1024];
        memset(rec_table, 0, sizeof(rec_table));
        bit_add(rec_table, header_table, 0, header_table[0], 32);

        printf("xxx %c\n", header_table[4]);
        printf("xxx %c\n", rec_table[0]);

        htree = new Node();
        htree->index = htree->weight = 0;
        htree->left = htree->right = NULL;
        uint32_t bit_cur = 0;
        for (;;) {
            uchar ch = 0;
            uchar len = 0;
            uint32_t data = 0;

            bit_add(&ch, rec_table, 0, 8, bit_cur);
            bit_cur += 8;

            bit_add(&len, rec_table, 0, 8, bit_cur);
            bit_cur += 8;

            bit_add(&data, rec_table, 0, len, bit_cur);
            bit_cur += len;


            printf("ch = %c, ", ch);
            printf("len = %u", len);
            printf("bit_cur = %u/%u", bit_cur, table_len-32);
            printf("\n");

            if (bit_cur >= table_len-32)
                break;

        }

        return 0;
    }

    ///decoding
    {
        uchar bit_raw_data[1024];
        memset(bit_raw_data, 0, sizeof(bit_raw_data));

        //parser
        uint32_t header_len = 0;
        bit_add(&header_len, header_table, 0, 32);

        for(int i = 0; i < 32; ++i) {
            uint8_t k = (header_len >> i) & 0x01;
            printf("-%u-", k );
        }
        printf("header_len = %u\n", header_len);
        printf("header_len___ = %u\n", header_table[0]);

        printf("table_len = %d\n", table_len);
        bit_add(bit_raw_data, header_table, 0, 20, table_len + 32);

        printf("header_table[0] = %u\n", header_table[0]);
        printf("bit_raw_data[0] = %u\n", bit_raw_data[0]);
        huffman_decoding_raw(p, bit_raw_data);
    }


    delete [] header_table;
    header_table = NULL;
    return 0;

}
