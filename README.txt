My program is structured according to the following:

1) Parse input
	- #cats #dogs #voters
	  keep_pet cut_pet
	  keep_pet cut_pet
2) Create bipartite graph
	- Dog lovers on right half, cat lovers on left
	- Create source on left with a single edge to each cat lover
		- edge weights of 1
	- Create source on right with a single edge from each dog lover
		- edge weights of 1
	- Create one "conflict" edge between each pair of cat lover and dog lover
	  that cannot both be satisfied
		- edge weights of 1
3) Use conflict graph and its residual graph to find answer
	- Answer is two-fold:
		1) Maximum possible number of satisfied voters
			- this number = size of "maximum matching" for the graph = size of the "minimum vertex cover" for the graph
			- find minimum vertex cover:
				1) Find maximum flow
					- run Edmonds-Karp
				2) consider residual flow network for this maximum flow
				3) vertex cover will consist of the union of the set of cat lovers not reachable from s
				   with the set of dog lovers that are reachable from s
		2) Which cats and dogs that will remain on the show
		   and, consequently, which voters will be happy
			- there is no correct standard for this

Uniqueness of implementation:
  - All interesting modifications are outlined in comments in the code

Correctness:
  - I was unable to get my code to run within the Kattis time limit so I can only
    say that it is correct with the five examples we were provided.

