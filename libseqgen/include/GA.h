#pragma once

#include <vector>

// Function to generate random numbers in given range  
int random_num(int start, int end);
  
// Create random genes for mutation 
char mutated_genes();
  
// create chromosome or string of genes 
std::vector<int> create_gnome(const std::vector<int>& foo);

void permutation(std::vector<int>& arr, int start, int end, std::vector<std::vector<int>>& perms);
void findPermutations(int a[], int n, std::vector<std::vector<int>>& perms);