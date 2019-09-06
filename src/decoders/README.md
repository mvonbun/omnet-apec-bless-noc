# Heuristic Syndorme Based Code Matching

We use syndrome decoding and store the precalculated syndromes, which are merely
bit-patterns, in external files loaded by the decoders.

The structure of the syndrome files is straight forward, there is ONE syndrome
per line in the file.  The syndromes themselves are splitted into (possible)
multiple unsigned integers, therefore we could also have packets with say 1024
symbols / flits

If at the end of the decoding process the state matches anyone of these
syndromes, then we can not decode the packet.

In the end it depends on the inner structure of the decoding state class what
the base entity of the syndrome is.
