/*===========================================================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <deque>
#include <assert.h>

struct Node;

typedef uint8_t uchar;
typedef uint32_t weightlen_t;
typedef std::deque<Node*> forest_t;

/*===========================================================================*/
struct Node {
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

        forest.push_back(pnode);

    }

}

int main(void) {

    char src_buf[] = "ABBCCD";
    weightlen_t weigth_table[256];
    forest_t forest;
    memset(weigth_table, 0, 256*sizeof(weightlen_t));

    ///create a weigth table
    create_weight_table(weigth_table, (uchar*)src_buf, strlen(src_buf));

    // for(int i = 0; i < 256; ++i) {
    //     printf("weigth_table[%d] = %u\n", i, weigth_table[i]);
    // }

    ///init_forest
    init_forest_by_weight_table(forest, weigth_table);

    for(size_t i = 0; i != forest.size(); ++i) {
        if (forest[i]->weight) {
            printf("i = %lu, w = %u\n",i, forest[i]->weight );
        }
    }
    return 0;
}
