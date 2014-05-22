/*===========================================================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t uchar;
typedef uint32_t weightlen_t;

/*===========================================================================*/
struct Node {
    weightlen_t weight;
    Node* left;
    Node* right;
};
/*===========================================================================*/

void create_weight_table(weightlen_t *des_buf, uchar *src_buf, size_t src_len) {
    for (size_t i = 0; i != src_len; ++i) {
        printf("src_buf[%lu] =%d\n", i, src_buf[i]);
        printf("des_buf[%lu] =%d\n", i, des_buf[i]);

        ++des_buf[(size_t)src_buf[i]];

        printf("++des_buf[%lu] =%d\n", (size_t)src_buf[i], des_buf[(size_t)src_buf[i]]);
        printf("--------------------------------------------\n");
    }
}

int main(void) {

    char src_buf[] = "ABBCCD";
    weightlen_t weigth_table[256];
    memset(weigth_table, 0, 254*sizeof(weightlen_t));

    create_weight_table(weigth_table, (uchar*)src_buf, strlen(src_buf));

    for(int i = 0; i < 256; ++i) {
        printf("weigth_table[%d] = %u\n", i, weigth_table[i]);
    }

    return 0;
}
