/* -*- mode: C -*-  */
/* 
   IGraph library.
   Copyright (C) 2003, 2004, 2005  Gabor Csardi <csardi@rmki.kfki.hu>
   MTA RMKI, Konkoly-Thege Miklos st. 29-33, Budapest 1121, Hungary
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "igraph.h"
#include "memory.h"

int igraph_clusters_weak(igraph_t *graph, vector_t *membership,
			 vector_t *csize);

int igraph_clusters_strong(igraph_t *graph, vector_t *membership,
			   vector_t *csize);

int igraph_clusters(igraph_t *graph, vector_t *membership, vector_t *csize, 
		    integer_t mode) {
  if (mode==1 || !igraph_is_directed(graph)) {
    return igraph_clusters_weak(graph, membership, csize);
  } else if (mode==2) {
    return igraph_clusters_strong(graph, membership, csize);
  }
  
  return 1;
}
  
int igraph_clusters_weak(igraph_t *graph, vector_t *membership,
			 vector_t *csize) {

  long int no_of_nodes=igraph_vcount(graph);
  char *already_added;
  long int first_node, act_cluster_size=0, no_of_clusters=1;
  
  dqueue_t q;
  
  long int i, j;
  vector_t neis;

  already_added=Calloc(no_of_nodes,char);

  /* Memory for result, csize is dynamically allocated */
  vector_resize(membership, no_of_nodes);
  vector_clear(csize);

  dqueue_init(&q, no_of_nodes > 100000 ? 10000 : no_of_nodes/10);
  vector_init(&neis, 0);

  /* The algorithm */

  for (first_node=0; first_node < no_of_nodes; ++first_node) {
    if (already_added[first_node]==1) continue;

    already_added[first_node]=1;
    act_cluster_size=1;
    VECTOR(*membership)[first_node]=no_of_clusters-1;
    dqueue_push(&q, first_node);
    
    while ( !dqueue_empty(&q) ) {
      long int act_node=dqueue_pop(&q);
      igraph_neighbors(graph, &neis, act_node, 3);
      for (i=0; i<vector_size(&neis); i++) {
	long int neighbor=VECTOR(neis)[i];
	if (already_added[neighbor]==1) { continue; }
	dqueue_push(&q, neighbor);
	already_added[neighbor]=1;
	act_cluster_size++;
	VECTOR(*membership)[neighbor]=no_of_clusters-1;
      }
    }
    no_of_clusters++;
    vector_push_back(csize, act_cluster_size);
  }
  
  /* Cleaning up */
  
  Free(already_added);
  dqueue_destroy(&q);
  vector_destroy(&neis);
  
  return 0;
}

int igraph_clusters_strong(igraph_t *graph, vector_t *membership,
			   vector_t *csize) {

  long int no_of_nodes=igraph_vcount(graph);
  long int *next_nei;
  
  long int i;
  dqueue_t q;
  
  long int no_of_clusters=1;
  long int act_cluster_size;

  vector_t out;
  vector_t tmp;

  printf("OK\n");

  /* The result */
  
  vector_resize(membership, no_of_nodes);
  vector_clear(csize);
  
  vector_init(&out, 0);
  vector_reserve(&out, no_of_nodes);
  vector_null(&out);

  next_nei=Calloc(no_of_nodes, long int);

  dqueue_init(&q, 100);
  vector_init(&tmp, 0);

  for (i=0; i<no_of_nodes; i++) {
    igraph_neighbors(graph, &tmp, i, 1);
    if (next_nei[i] > vector_size(&tmp)) { continue; }
    
    dqueue_push(&q, i);
    while (!dqueue_empty(&q)) {
      long int act_node=dqueue_back(&q);
      igraph_neighbors(graph, &tmp, act_node, 1);
      if (next_nei[act_node]==0) {
	/* this is the first time we've met this vertex */
	next_nei[act_node]++;
      } else if (next_nei[act_node] <= vector_size(&tmp)) {
	/* we've already met this vertex but it has more children */
	long int neighbor=VECTOR(tmp)[next_nei[act_node]-1];
	if (next_nei[neighbor] == 0) {
	  dqueue_push(&q, neighbor);
	}
	next_nei[act_node]++;
      } else {
	/* we've met this vertex and it has no more children */
	vector_push_back(&out, act_node);
	dqueue_pop_back(&q);
      }
    } /* while q */
  }  /* for */

  /* OK, we've the 'out' values for the nodes, let's use them in
     descreasing order with the help of a heap */

  memset(next_nei, 0, no_of_nodes*sizeof(long int)); /* mark already
							added vertices */
  while (!vector_empty(&out)) {
    long int grandfather=vector_pop_back(&out);
    if (next_nei[grandfather] != 0) { continue; }
    next_nei[grandfather]=1;
    act_cluster_size=1;
    VECTOR(*membership)[grandfather]=no_of_clusters-1;
    dqueue_push(&q, grandfather);
    
    while (!dqueue_empty(&q)) {
      long int act_node=dqueue_pop_back(&q);
      igraph_neighbors(graph, &tmp, act_node, 2);
      for (i=0; i<vector_size(&tmp); i++) {
	long int neighbor=VECTOR(tmp)[i];
	if (next_nei[neighbor] != 0) { continue; }
	dqueue_push(&q, neighbor);
	next_nei[neighbor]=1;
	act_cluster_size++;
	VECTOR(*membership)[neighbor]=no_of_clusters-1;
      }
    }
    no_of_clusters++;
    vector_push_back(csize, act_cluster_size);
  }
  
  /* Clean up, return */

  vector_destroy(&out);
  vector_destroy(&tmp);
  dqueue_destroy(&q);
  Free(next_nei);

  return 0;
}

int igraph_is_connected_weak(igraph_t *graph, bool_t *res);

int igraph_is_connected(igraph_t *graph, bool_t *res, integer_t mode) {
  if (mode==1 || !igraph_is_directed(graph)) {
    return igraph_is_connected_weak(graph, res);
  } else if (mode==2) {
    vector_t membership;
    vector_t csize;
    int retval;
    vector_init(&membership, 0);
    vector_init(&csize, 0);
    retval = igraph_clusters_strong(graph, &membership, &csize);
    *res = (vector_size(&csize)==1);
    vector_destroy(&membership);
    vector_destroy(&csize);
    return retval;
  }
}

int igraph_is_connected_weak(igraph_t *graph, bool_t *res) {

  long int no_of_nodes=igraph_vcount(graph);
  char *already_added;
  vector_t neis;
  dqueue_t q;
  
  long int i, j;

  already_added=Calloc(no_of_nodes, char);
  vector_init(&neis, 0);
  
  /* Try to find at least two clusters */
  already_added[0]=1;
  dqueue_push(&q, 0);
  
  j=1;
  while ( !dqueue_empty(&q)) {
    long int actnode=dqueue_pop(&q);
    igraph_neighbors(graph, &neis, actnode, 3);
    for (i=0; i <vector_size(&neis); i++) {
      long int neighbor=VECTOR(neis)[i];
      if (already_added[neighbor] != 0) { continue; }
      dqueue_push(&q, neighbor);
      j++;
      already_added[neighbor]++;
    }
  }
  
  /* Connected? */
  *res = (j == no_of_nodes);

  Free(already_added);
  dqueue_destroy(&q);
  vector_destroy(&neis);

  /* TODO */
  return 0;
}
