//
// Created by matze on 04/12/2023.
//

#ifndef AOCC_FENWICK_H
#define AOCC_FENWICK_H


#include <stdio.h>

long fenwick_get_sum(const long* tree, int i){
    long sum=0;
    i+=1;
    while(i){
        sum+=tree[i];
        i-=i&(-i);
    }
    return sum;
}

long fenwick_get_element(const long* tree,int i){
    return fenwick_get_sum(tree,i)- fenwick_get_sum(tree,i-1);
}

void fenwick_add_zero_range(long* tree,int tree_size,int i,int val){
    i+=1;
    while(i<=tree_size)
    {
        tree[i]+=val;
        i+=i&(-i);
    }
}

void fenwick_add_range(long* tree,int tree_size,int lower,int upper,long val){
    fenwick_add_zero_range(tree,tree_size,lower,val);
    fenwick_add_zero_range(tree,tree_size,upper,-val);
}

void test_fenwick(){
#define fenwick_test_tree_size 20
    long tree[fenwick_test_tree_size]={0};
    const int tree_size= fenwick_test_tree_size;
    fenwick_add_range(tree,tree_size,5,9,10);

    for(int i=0;i<tree_size;i++){
        printf("tree val at %d: %ld\n",i, fenwick_get_sum(tree,i));
    }

}

#endif //AOCC_FENWICK_H
