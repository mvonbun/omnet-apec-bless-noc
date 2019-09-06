# Codes

In this simulator, two classes of erasure codes have been implemented.

The first class of codes includes the *FEC* and *BLOCKXOR* codes and use simple
decoders that just count the number of arriving flits.  These codes are the ones
that have been used for the *APEC* evaluations.

The second class of codes includes the *heuristic* labeled codes.  These codes
rely on syndrome decoding.  In the simulator, the codes implement a look up to
syndrome files generated externally and return failure or success.
**IMPORTANT** These codes are not for use and the syndrome files in this folder
are mostly dummy / mock-up syndromes.  However, these codes are not used in any
of the provided simulation configurations.


## A Remark on Syndrome Representation
Let's assume we have only 32 bit to represent a word.  Then, for very long
packets, say 512 = 16 * 32 flits, we need can use a vector of 16 integers to
represent the syndromes.

The integers are in the range [0, 4294967295] in this case.

However, for the time being, the heuristic codes are more brute force, i.e. only
syndromes for small packets have been calculated so far (see `single` and
`burst` folders, where 16 flit packets are the maximum).


